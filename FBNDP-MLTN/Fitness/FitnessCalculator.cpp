#include "FitnessCalculator.h"

#include "../Util/Utils.h"

#include <math.h>

FitnessCalculator::FitnessCalculator(const vector<NodeData>& InGraphData, uint64_t PathNum)
	: GraphData(InGraphData)
	, NumberOfPath(PathNum)
{
	if (auto DataCenterInstance = DataCenter::GetInstance())
	{
		PassageTimeDiff = DataCenterInstance->GetUserInputData().PassageTimeDiff;
		PassageTimeDiff = Util::Converter::ConvertMinuteToHour(PassageTimeDiff);

		RouteDataMap = DataCenterInstance->GetRouteData();
		// RouteData�� GraphData�� �ִ� ��� �߰� �ʿ�.
		AddRouteDataMapFromGraphData();
		
		// GraphData�� RouteData�� �ִ� �뼱 �߰� �ʿ�.
		AddGraphDataFromRouteDataMap(DataCenterInstance->GetRouteData(), DataCenterInstance->GetNodeData());
	}
}

float FitnessCalculator::Calculate()
{
	float NetworkCost = PassageAssignment();

	return CalculateFitness(NetworkCost);
}

float FitnessCalculator::PassageAssignment()
{
	// get OD Matrix
	vector<TrafficVolumeData> TrafficVolumeDataList;
	if (auto DataCenterInstance = DataCenter::GetInstance())
		TrafficVolumeDataList = DataCenterInstance->GetTrafficVolumeData();

	float SumofCustomerCost = 0.f;
	for (const TrafficVolumeData& ODData : TrafficVolumeDataList)
	{
		if (ODData.FromNodeNum == ODData.ToNodeNum)
			continue;

		if (!FindNodeNumberFromGraphData(ODData.FromNodeNum, ODData.ToNodeNum))
			continue;

		// find the shortest path based on the OD Matrix from network graph data.
		vector<ShortestPathData> ShortestPathList;
		PathFinderData PathFinder(GraphData, ODData.FromNodeNum, ODData.ToNodeNum, EPathFinderCostType::Duration, NumberOfPath);
		if (Util::PathFinder::FindShortestPath(PathFinder, ShortestPathList) == 0)
			continue;	// if not exist ShortestPath, ignore.

		SetPassageAssignmentForMNLModel(ShortestPathList, ODData.TrafficVolume);
		
		CalculateCustomerCost(ShortestPathList, SumofCustomerCost);
	}

	return CalculateNetworkCost(SumofCustomerCost);
}

void FitnessCalculator::SetPassageAssignmentForMNLModel(vector<ShortestPathData>& InOutPathList, uint64_t TrafficVolume)
{
	if (InOutPathList.size() == 1)
	{
		InOutPathList.at(0).TrafficVolumeForPath = static_cast<uint32_t>(TrafficVolume);
	}
	else if (InOutPathList.size() == 2)
	{
		vector<LinkData> LinkDataList;
		vector<DistanceData> DistanceDataList;
		if (auto DataCenterInstance = DataCenter::GetInstance())
		{
			LinkDataList = DataCenterInstance->GetLinkData();
			DistanceDataList = DataCenterInstance->GetDistanceData();
		}

		float Compare = InOutPathList.at(0).Cost - InOutPathList.at(1).Cost;
		if (PassageTimeDiff < fabsf(Compare))
		{
			InOutPathList.at(0).TrafficVolumeForPath = static_cast<uint32_t>(TrafficVolume);
		}

		auto PathData = InOutPathList.at(0);
		vector<float> UnityFunctionValue;
		float ActualDistance = 0.f, DirectDistance = 0.f;
		float TrainTravelTime = 0.f;
		for (uint64_t i = 0; i < PathData.Path.size() - 1; ++i)
		{
			NodeData CurNodeData = PathData.Path.at(i);
			NodeData NextNodeData = PathData.Path.at(i + 1);
			if (CurNodeData.Type == NodeType::Station)
			{
				for (const auto& Link : LinkDataList)
				{
					if (Link.FromNodeNum == CurNodeData.Num && Link.ToNodeNum == NextNodeData.Num)
					{
						float Distance = 0.f;
						TrainTravelTime += Util::Calculator::CalculateIVTT(Link, Distance);
						ActualDistance += Distance;
						break;
					}
				}
			}

			for (const auto& Data : DistanceDataList)
			{
				if (Data.FromNodeNum == CurNodeData.Num && Data.ToNodeNum == NextNodeData.Num)
				{
					DirectDistance += Data.Distance;
					break;
				}
			}
		}

		float TravelTimeInVechicle = PathData.IVTT;		// IVTT
		float TravelTimeOutVechicle = PathData.OVTT;	// OVTT
		float CumulativeTransferPanaltyIndex = 1.f - expf(-static_cast<float>(PathData.CTPI));	// CTPI
		float TrainTravelTimeRatio = TrainTravelTime / PathData.IVTT;	// RELI
		float Curve = ActualDistance / DirectDistance;	// CIRC

		UnityFunctionValue.emplace_back((MNLCoefData.IVTTCoef * TravelTimeInVechicle) + (MNLCoefData.OVTTCoef * TravelTimeOutVechicle) + (MNLCoefData.CTPICoef * CumulativeTransferPanaltyIndex)
			+ (MNLCoefData.RELICoef * TrainTravelTimeRatio) + (MNLCoefData.CIRCCoef * Curve));

		float SumOfUnityFunctionValue = 0.f;
		for (auto Value : UnityFunctionValue)
			SumOfUnityFunctionValue += Value;

		float PassageRate = expf(UnityFunctionValue[0]) / SumOfUnityFunctionValue;
		InOutPathList.at(0).TrafficVolumeForPath = static_cast<uint32_t>(TrafficVolume * PassageRate);
		InOutPathList.at(1).TrafficVolumeForPath = static_cast<uint32_t>(TrafficVolume) - InOutPathList.at(0).TrafficVolumeForPath;
	}
	else
	{
		// todo. handling k is greater than 2
	}
}

void FitnessCalculator::CalculateCustomerCost(const vector<ShortestPathData>& InPathList, float& OutCostSum)
{
	// �̰� ����Ϸ��� �ô��� �ʱ⿡ IVTT, OVTT ����� ������ ������ �Ұ� ������...?
	// IVTT, OVTT�� �뼱���� ���ҵǾ� �����صξ�� ��������/����/��ö�� ����� ���� ���� ����� �� ����.

	UserInputData UserInput;
	if (auto DataCenterInstance = DataCenter::GetInstance())
	{
		UserInput = DataCenterInstance->GetUserInputData();
	}

	for (auto& Path : InPathList)
	{
		float CustomerCost = 0.f;
		CustomerCost += UserInput.TownBusTimeCost;// *IVTTForTownBus;
		CustomerCost += UserInput.BusTimeCost;// *IVTTForBus;
		CustomerCost += UserInput.TrainTimeCost;// *IVTTForTrain;
		CustomerCost += UserInput.WaitTimeCost;// / (2 * InitialDispatchesPerHour);
		CustomerCost;	// ȯ�� �ð� ���
		CustomerCost += UserInput.WaitTimeCost;// / (2 * DispatchesPerHour);	// �̰� ȯ���Ҷ����� ���ҵǾ� �־�� �� �� ������...
		CustomerCost *= static_cast<float>(Path.TrafficVolumeForPath);

		OutCostSum += CustomerCost;
	}
}

float FitnessCalculator::CalculateNetworkCost(float SumofCustomerCost)
{
	UserInputData UserInput;
	if (auto DataCenterInstance = DataCenter::GetInstance())
	{
		UserInput = DataCenterInstance->GetUserInputData();
	}

	float TotalCost = SumofCustomerCost;
	//for(auto Path : ���������뼱list)
		TotalCost += 2 * UserInput.TownBusOperationCost * UserInput.TownBusDispatchesPerHour;// *���������� �� �뼱 ����

	return TotalCost;
}

float FitnessCalculator::CalculateFitness(float NetworkCost)
{
	UserInputData UserInput;
	if (auto DataCenterInstance = DataCenter::GetInstance())
	{
		UserInput = DataCenterInstance->GetUserInputData();
	}

	float Fitness = 1 / (UserInput.PanaltyFactor * NetworkCost);
	// �߰��� �ٸ� ���� �� ���� ��..

	return Fitness;
}

bool FitnessCalculator::FindNodeNumberFromGraphData(uint64_t FromNodeNum, uint64_t ToNodeNum)
{
	bool bExistFromNodeNum = false;
	for (const auto& Node : GraphData)
	{
		if (FromNodeNum == Node.Num)
		{
			bExistFromNodeNum = true;
			break;
		}
	}

	bool bExistToNodeNum = false;
	for (const auto& Node : GraphData)
	{
		if (ToNodeNum == Node.Num)
		{
			bExistToNodeNum = true;
			break;
		}
	}

	return bExistFromNodeNum && bExistToNodeNum;
}

void FitnessCalculator::AddRouteDataMapFromGraphData()
{
	int RouteCount = 1;
	vector<NodeData> NewRouteList;
	for (const auto& Node : GraphData)
	{
		if (Node.Type == NodeType::Station)
		{
			uint64_t Order = 0;
			map<uint64_t, RouteData> NewRouteMap;
			for (vector<NodeData>::iterator Iter = NewRouteList.begin(); Iter != NewRouteList.end(); ++Iter)
			{
				RouteData Route;
				Route.Node = Iter->Num;
				Route.CumDistance = 0.f;
				NewRouteMap.insert(make_pair(++Order, Route));
			}

			RouteData Route;
			Route.Node = Node.Num;
			Route.CumDistance = 0.f;
			NewRouteMap.insert(make_pair(++Order, Route));

			for (vector<NodeData>::reverse_iterator RIter = NewRouteList.rbegin(); RIter != NewRouteList.rend(); ++RIter)
			{
				RouteData Route;
				Route.Node = RIter->Num;
				Route.CumDistance = 0.f;
				NewRouteMap.insert(make_pair(++Order, Route));
			}

			string RouteName = "TownBus";
			RouteName.append(to_string(RouteCount++));
			RouteDataMap.insert(make_pair(RouteName, NewRouteMap));
			NewRouteMap.clear();
		}
		else
			NewRouteList.emplace_back(Node);
	}
}

void FitnessCalculator::AddGraphDataFromRouteDataMap(const RouteMap& RouteDataMap, const vector<NodeData>& FullGraphData)
{
	for (const auto& RouteMapPair : RouteDataMap)
	{
		size_t RouteOneWayCount = RouteMapPair.second.size() / 2;
		size_t CurCount = 0;
		for (const auto& RoutePair : RouteMapPair.second)
		{
			GraphData.push_back(Util::PathFinder::GetNodeData(RoutePair.second.Node, FullGraphData));
			if (RouteOneWayCount < ++CurCount)
				break;
		}
	}
}