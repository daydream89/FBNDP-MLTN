#include "FitnessCalculator.h"

#include "../Util/Utils.h"

#include <math.h>

FitnessCalculator::FitnessCalculator(int InChromosomeIndex, uint64_t PathNum)
	: ChromosomeIndex(InChromosomeIndex)
	, NumberOfPath(PathNum)
{
	// set graph data & link data

	if (auto DataCenterInstance = DataCenter::GetInstance())
	{
		DataCenterInstance->ClearShortestPathDataList();

		PassageTimeDiff = DataCenterInstance->GetUserInputData().PassageTimeDiff;
		PassageTimeDiff = Util::Converter::ConvertMinuteToHour(PassageTimeDiff);

		auto& ChromosomeData = DataCenterInstance->GetChromosomeRoutesDataRef(ChromosomeIndex);
		SetGraphData(ChromosomeData);
		SetLinkDataList(ChromosomeData, DataCenterInstance->GetLinkData());

		RouteDataMap = DataCenterInstance->GetRouteData();
		AddLinkDataFromDefaultRouteData(RouteDataMap, DataCenterInstance->GetOperatingData());
		AddGraphDataToRouteDataMap(ChromosomeData);
		
		AddRouteDataMapToGraphData(DataCenterInstance->GetRouteData(), DataCenterInstance->GetNodeData());
		AddRouteDataMapToLinkData(RouteDataMap, DataCenterInstance->GetLinkData());
	}
}

FitnessCalculator::FitnessCalculator(const vector<uint64_t>& InPathList, const vector<uint64_t>& InBusStopList)
{
	vector<ShortestPathData> PathDataList;
	if (auto DataCenterInstance = DataCenter::GetInstance())
	{
		int i = 0;
		// Make ShortestPathData.Path
		while (i < InPathList.size())
		{
			ShortestPathData PathData;
			for (; i < InPathList.size(); ++i)
			{
				auto Node = Util::PathFinder::GetNodeData(InPathList.at(i), DataCenterInstance->GetNodeData());
				if (Node.Type == NodeType::Station)
				{
					PathData.Path.emplace_back(Node);
					PathDataList.emplace_back(PathData);
					++i;
					break;
				}
				else
					PathData.Path.emplace_back(Node);
			}
		}

		// Make ShortestPathData.TownBusData.TownBusStopCheck
		for (auto& PathData : PathDataList)
		{
			for (auto& Node : PathData.Path)
			{
				bool bBusStop = false;
				if (Node.Type == NodeType::Station)
					bBusStop = true;
				else
				{
					for (auto& BusStop : InBusStopList)
					{
						if (BusStop == Node.Num)
						{
							bBusStop = true;
							break;
						}
					}
				}

				PathData.TownBusData.TownBusStopCheck.emplace_back(make_pair(Node, bBusStop));
			}
		}
		DataCenterInstance->AddTownBusRouteData(PathDataList);

		SetGraphData(PathDataList);
		SetLinkDataList(PathDataList, DataCenterInstance->GetLinkData());

		RouteDataMap = DataCenterInstance->GetRouteData();
		AddLinkDataFromDefaultRouteData(RouteDataMap, DataCenterInstance->GetOperatingData());
		AddGraphDataToRouteDataMap(PathDataList);

		AddRouteDataMapToGraphData(DataCenterInstance->GetRouteData(), DataCenterInstance->GetNodeData());
		AddRouteDataMapToLinkData(RouteDataMap, DataCenterInstance->GetLinkData());
	}
}

FitnessResultData FitnessCalculator::Calculate()
{
	ResultData.ObjectFunctionValue = PassageAssignment();
	ResultData.FitnessValue = CalcFitness(ResultData.ObjectFunctionValue);

	return ResultData;
}

double FitnessCalculator::PassageAssignment()
{
	// get OD Matrix
	vector<TrafficVolumeData> TrafficVolumeDataList;
	if (auto DataCenterInstance = DataCenter::GetInstance())
		TrafficVolumeDataList = DataCenterInstance->GetTrafficVolumeData();

	map<string, RouteCostData> RouteCostMap;
	double SumofCustomerCost = 0.f;
	for (const TrafficVolumeData& ODData : TrafficVolumeDataList)
	{
		if (ODData.FromNodeNum == ODData.ToNodeNum)
			continue;

		if (!FindNodeNumberFromGraphData(ODData.FromNodeNum, ODData.ToNodeNum))
			continue;

		// find the shortest path based on the OD Matrix from network graph data.
		vector<ShortestPathData> ShortestPathList;
		PathFinderData PathFinder(GraphData, LinkDataList, RouteDataMap, ODData.FromNodeNum, ODData.ToNodeNum, EPathFinderCostType::Duration, NumberOfPath);
		if (Util::PathFinder::FindShortestPath(PathFinder, ShortestPathList) == 0)
			continue;	// if not exist ShortestPath, ignore.

		SetPassageAssignmentForMNLModel(ShortestPathList, ODData.TrafficVolume);
		
		CalcCustomerCost(ShortestPathList, SumofCustomerCost, RouteCostMap);

		Util::Calculator::CalcNumOfPassengerPerRoute(ShortestPathList, RouteDataMap, RouteCostMap);

		if (auto DataCenterInstance = DataCenter::GetInstance())
			DataCenterInstance->AddShortestPathDataList(ShortestPathList);
	}

	// apply customer cost per person in ChromosomeData.
	if (auto DataCenterInstance = DataCenter::GetInstance())
	{
		for (auto& PathData : DataCenterInstance->GetChromosomeRoutesDataRef(ChromosomeIndex))
		{
			auto Iter = RouteCostMap.find(PathData.TownBusData.RouteName);
			PathData.TownBusData.RouteCostPerPerson = 0;
			if (Iter != RouteCostMap.end() && Iter->second.NumberOfUsers != 0)
				PathData.TownBusData.RouteCostPerPerson = Iter->second.CostPerRoute / static_cast<double>(Iter->second.NumberOfUsers);
		}
	}

	ResultData.TotalCustomerCost = SumofCustomerCost;

	return CalcNetworkCost(SumofCustomerCost);
}

void FitnessCalculator::SetPassageAssignmentForMNLModel(vector<ShortestPathData>& InOutPathList, uint64_t TrafficVolume)
{
	vector<float> UnityFunctionValue;
	if (InOutPathList.size() == 1)
	{
		InOutPathList.at(0).TrafficVolumeForPath = static_cast<uint32_t>(TrafficVolume);
		CalcCurveNTransportationIVTT(InOutPathList.at(0));
		return;
	}
	else if (InOutPathList.size() == 2)
	{
		vector<LinkData> LinkDataList;
		vector<DistanceData> DistanceDataList;
		if (auto DataCenterInstance = DataCenter::GetInstance())
			LinkDataList = DataCenterInstance->GetLinkData();

		float Compare = InOutPathList.at(0).Cost - InOutPathList.at(1).Cost;
		if (PassageTimeDiff < fabsf(Compare))
		{
			InOutPathList.at(0).TrafficVolumeForPath = static_cast<uint32_t>(TrafficVolume);
			InOutPathList.at(1).TrafficVolumeForPath = 0;
			CalcCurveNTransportationIVTT(InOutPathList.at(0));
			CalcCurveNTransportationIVTT(InOutPathList.at(1));
			return;
		}

		auto& PathData = InOutPathList.at(0);
		for (auto& PathData : InOutPathList)
		{
			float TravelTimeInVechicle = PathData.IVTT;		// IVTT
			float TravelTimeOutVechicle = PathData.Transfer.OVTT;	// OVTT
			float CumulativeTransferPanaltyIndex = 1.f - expf(-static_cast<float>(PathData.Transfer.CTPI));	// CTPI
			float Curve = CalcCurveNTransportationIVTT(PathData);	// CIRC
			float TrainTravelTimeRatio = PathData.TrainIVTT / PathData.IVTT;	// RELI

			UnityFunctionValue.emplace_back((MNLCoefData.IVTTCoef * TravelTimeInVechicle) + (MNLCoefData.OVTTCoef * TravelTimeOutVechicle) + (MNLCoefData.CTPICoef * CumulativeTransferPanaltyIndex)
				+ (MNLCoefData.RELICoef * TrainTravelTimeRatio) + (MNLCoefData.CIRCCoef * Curve));
		}
	}
	else
	{
		// todo. handling k is greater than 2
	}

	float SumOfUnityFunctionValue = 0.f;
	for (auto Value : UnityFunctionValue)
		SumOfUnityFunctionValue += expf(Value);

	float PassageRate = expf(UnityFunctionValue[0]) / SumOfUnityFunctionValue;
	InOutPathList.at(0).TrafficVolumeForPath = static_cast<uint32_t>(TrafficVolume * PassageRate);
	InOutPathList.at(1).TrafficVolumeForPath = static_cast<uint32_t>(TrafficVolume) - InOutPathList.at(0).TrafficVolumeForPath;

	AddTrafficVolumeByTownBusRoute(InOutPathList.at(0).PathRouteList, InOutPathList.at(0).TrafficVolumeForPath);
	AddTrafficVolumeByTownBusRoute(InOutPathList.at(1).PathRouteList, InOutPathList.at(1).TrafficVolumeForPath);
}

float FitnessCalculator::CalcCurveNTransportationIVTT(ShortestPathData& PathData)
{
	UserInputData UserInput;
	vector<DistanceData> DistanceDataList;
	if (auto DataCenterInstance = DataCenter::GetInstance())
	{
		DistanceDataList = DataCenterInstance->GetDistanceData();
		UserInput = DataCenterInstance->GetUserInputData();
	}

	float ActualDistance = 0.f, DirectDistance = 0.f;
	for (uint64_t i = 0; i < PathData.Path.size() - 1; ++i)
	{
		float TrainTravelTime = 0.f, TownBusTravelTime = 0.f, BusTravelTime = 0.f;
		NodeData CurNodeData = PathData.Path.at(i);
		NodeData NextNodeData = PathData.Path.at(i + 1);
		for (const auto& Link : LinkDataList)
		{
			if (Link.FromNodeNum == CurNodeData.Num && Link.ToNodeNum == NextNodeData.Num)
			{
				string RouteName = "";
				float Distance = 0.f;
				float TravelTime = Util::Calculator::CalcIVTT(Link, RouteDataMap, Distance, RouteName);
				ActualDistance += Distance;
				if (CurNodeData.Type == NodeType::Station && NextNodeData.Type == NodeType::Station)
					TrainTravelTime += TravelTime;
				else if (CurNodeData.Type == NodeType::BusStop || NextNodeData.Type == NodeType::BusStop)
				{
					if (RouteName.substr(0, 7) == TownBusStr)
						TownBusTravelTime += TravelTime;
					else
						BusTravelTime += TravelTime;
				}

				bool bExist = false;
				for (auto PathRouteName : PathData.PathRouteList)
					if (PathRouteName == RouteName)
						bExist = true;
				
				if (!bExist)
					PathData.PathRouteList.emplace_back(RouteName);
				
				break;
			}
		}

		PathData.BusIVTT += UserInput.BusTimeCost * BusTravelTime;
		PathData.TownBusIVTT += UserInput.TownBusTimeCost * TownBusTravelTime;
		PathData.TrainIVTT += UserInput.TrainTimeCost * TrainTravelTime;

		for (const auto& Data : DistanceDataList)
		{
			if (Data.FromNodeNum == CurNodeData.Num && Data.ToNodeNum == NextNodeData.Num)
			{
				DirectDistance += Data.Distance;
				break;
			}
		}
	}

	return ActualDistance / DirectDistance;
}

void FitnessCalculator::CalcCustomerCost(vector<ShortestPathData>& InPathList, double& OutCostSum, map<string, RouteCostData>& OutRouteCostMap)
{
	UserInputData UserInput;
	map<string, OperatingData> OperatingDataMap;
	if (auto DataCenterInstance = DataCenter::GetInstance())
	{
		UserInput = DataCenterInstance->GetUserInputData();
		OperatingDataMap = DataCenterInstance->GetOperatingData();
	}

	for (ShortestPathData& Path : InPathList)
	{
		float InitialDispatchesPerHour = 0.f;
		const auto& FirstNode = Path.Path.at(0);
		for (const auto& Link : LinkDataList)
		{
			if (Link.FromNodeNum == FirstNode.Num && Link.ToNodeNum == Path.Path.at(1).Num)
			{
				string RouteName = "";
				float Dist = 0.f;
				float Cost = Util::Calculator::CalcIVTT(Link, RouteDataMap, Dist, RouteName);
				if (RouteName.substr(0, 7) == TownBusStr)
					InitialDispatchesPerHour = static_cast<float>(UserInput.TownBusDispatchesPerHour);
				else
				{
					auto Iter = OperatingDataMap.find(RouteName);
					if (Iter != OperatingDataMap.end())
						InitialDispatchesPerHour = static_cast<float>(Iter->second.Dispatch);
				}

				auto Found = OutRouteCostMap.find(RouteName);
				if (Found != OutRouteCostMap.end())
				{
					Found->second.CostPerRoute += Cost;
				}
				else
				{
					RouteCostData Data;
					Data.CostPerRoute = Cost;
					OutRouteCostMap.insert(make_pair(RouteName, Data));
				}
				
				break;
			}
		}

		float TransferWaitTimeCost = 0.f;
		float TransferTime = 0.f;
		for (const auto& TransferTimeData : Path.Transfer.TransferList)
		{
			TransferTime += TransferTimeData.TransferTime;
			TransferWaitTimeCost += UserInput.WaitTimeCost / (2 * TransferTimeData.DispatchesPerHour);
		}

		Path.Transfer.OVTT = (UserInput.WaitTimeCost / (2 * InitialDispatchesPerHour)) + (UserInput.TransferTimeCost * (TransferTime / 60.f)) + TransferWaitTimeCost;

		double CustomerCost = 0;
		CustomerCost += Path.TownBusIVTT;
		CustomerCost += Path.BusIVTT;
		CustomerCost += Path.TrainIVTT;
		CustomerCost += Path.Transfer.OVTT;
		CustomerCost *= static_cast<double>(Path.TrafficVolumeForPath);

		OutCostSum += CustomerCost;
	}
}

double FitnessCalculator::CalcNetworkCost(double SumofCustomerCost)
{
	UserInputData UserInput;
	if (auto DataCenterInstance = DataCenter::GetInstance())
	{
		UserInput = DataCenterInstance->GetUserInputData();
	}

	int32_t NumberOfTownBusLine = 0;
	double NumberOfBusesSum = 0;
	double TotalCost = SumofCustomerCost;
	for (const auto& RoutePair : RouteDataMap)
	{
		if (RoutePair.first.substr(0, 7) == TownBusStr)
		{
			NumberOfTownBusLine++;

			auto RIter = RoutePair.second.rbegin();
			if (RIter != RoutePair.second.rend())
			{
				TotalLengthOfTownBusLine += RIter->second.CumDistance;
				double NumberOfBuses = (UserInput.TownBusDispatchesPerHour * RIter->second.CumDistance) / UserInput.TownBusSpeed;
				NumberOfBusesSum += Util::Converter::ConvertDoubleToIntegerRoundUp(NumberOfBuses);
			}
		}
	}

	//double NumberOfBuses = UserInput.TownBusDispatchesPerHour * TotalLengthOfTownBusLine / UserInput.TownBusSpeed;
	//NumberOfBuses = static_cast<double>(Util::Converter::ConvertDoubleToIntegerRoundUp(NumberOfBuses));
	ResultData.NumberOfBuses = NumberOfBusesSum;
	ResultData.NumberOfTownBusRoutes = NumberOfTownBusLine;

	//double TownBusOperatorCost = static_cast<double>(UserInput.TownBusOperationCost) * NumberOfBuses * static_cast<double>(TotalLengthOfTownBusLine / 2) * 2;
	//TownBusOperatorCost += (static_cast<double>(UserInput.RouteFixCost) * static_cast<double>(NumberOfTownBusLine));
	double TownBusOperatorCost1 = static_cast<double>(UserInput.TownBusOperationCost) * static_cast<double>(UserInput.OperatingHoursPerDay) * static_cast<double>(UserInput.TownBusSpeed) * NumberOfBusesSum;
	double TownBusOperatorCost2 = static_cast<double>(UserInput.TownBusOperationCost) * static_cast<double>(UserInput.TownBusDispatchesPerHour) * static_cast<double>(TotalLengthOfTownBusLine);
	double TownBusOperatorCost = (UserInput.OperatorCostOption1 * TownBusOperatorCost1) + (UserInput.OperatorCostOption2 * TownBusOperatorCost2);

	TotalCost += TownBusOperatorCost;
	ResultData.TownBusOperatorCost += TownBusOperatorCost;

	ResultData.TotalRouteDistance = TotalLengthOfTownBusLine;

	printf("Calculated NetworkCost : %lf\n", TotalCost);

	return TotalCost;
}

double FitnessCalculator::CalcFitness(double NetworkCost)
{
	UserInputData UserInput;
	if (auto DataCenterInstance = DataCenter::GetInstance())
	{
		UserInput = DataCenterInstance->GetUserInputData();
	}

	double Value3 = 0;
	for (const auto& RoutePair : RouteDataMap)
	{
		if (RoutePair.first.substr(0, 7) == TownBusStr)
		{
			auto RIter = RoutePair.second.rbegin();
			if (RIter != RoutePair.second.rend())
			{
				// Value3 += D - Lk < 0 ? D - Lk : 0;
				if (UserInput.MaxRouteLength - RIter->second.CumDistance < 0)
					Value3 += UserInput.MaxRouteLength - RIter->second.CumDistance;
			}
		}
	}
	
	double Value4 = 0;
	double ReferenceValue = static_cast<double>(UserInput.TownBusDispatchesPerHour) * static_cast<double>(UserInput.TownBusSeat) * UserInput.LoadFactor;
	for (const auto& NumOfUsers : NumOfUsersPerTownBusRoute)
	{
		if (NumOfUsers.first.substr(0, 7) == TownBusStr)
		{
			// Value4 += fCp - Pk < 0 ? fCp - Pk : 0;
			if (ReferenceValue - static_cast<double>(NumOfUsers.second) < 0)
				Value4 += ReferenceValue - static_cast<double>(NumOfUsers.second);
		}
	}

	ResultData.P2Value = Value3 * UserInput.PanaltyFactor3;
	ResultData.P3Value = Value4 * UserInput.PanaltyFactor4;

	double Fitness = 1 / (UserInput.PanaltyFactor * NetworkCost);
	Fitness += ResultData.P2Value;
	Fitness += ResultData.P3Value;

	printf("Calculated Fitness : %lf\n", Fitness);

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

void FitnessCalculator::AddGraphDataToRouteDataMap(vector<ShortestPathData>& InOutPathDataList)
{
	int RouteCount = 1;
	for (auto& PathData : InOutPathDataList)
	{
		uint64_t Order = 0;
		map<uint64_t, RouteData> NewRouteMap;
		vector<pair<NodeData, bool>>& TownBusDataList = PathData.TownBusData.TownBusStopCheck;
		for (vector<pair<NodeData, bool>>::iterator Iter = TownBusDataList.begin(); Iter != TownBusDataList.end(); ++Iter)
		{
			if (!Iter->second)
				continue;

			uint64_t PrevNodeNum = NewRouteMap.find(Order) != NewRouteMap.end() ? NewRouteMap.find(Order)->second.Node : Iter->first.Num;
			float PrevCumDistance = NewRouteMap.find(Order) != NewRouteMap.end() ? NewRouteMap.find(Order)->second.CumDistance : 0.f;

			RouteData Route;
			Route.Node = Iter->first.Num;
			Route.CumDistance = PrevCumDistance + FindLinkLength(LinkDataList, PrevNodeNum, Iter->first.Num);
			NewRouteMap.insert(make_pair(++Order, Route));
		}

		vector<pair<NodeData, bool>>::reverse_iterator RIter = TownBusDataList.rbegin();
		for (++RIter; RIter != TownBusDataList.rend(); ++RIter)
		{
			if (!RIter->second)
				continue;

			RouteData Route;
			Route.Node = RIter->first.Num;
			Route.CumDistance = NewRouteMap.find(Order)->second.CumDistance + FindLinkLength(LinkDataList, NewRouteMap.find(Order)->second.Node, RIter->first.Num);
			NewRouteMap.insert(make_pair(++Order, Route));
		}

		string RouteName = TownBusStr;
		RouteName.append(to_string(RouteCount++));

		RouteDataMap.insert(make_pair(RouteName, NewRouteMap));
		PathData.TownBusData.RouteName = RouteName;
	}
}

void FitnessCalculator::AddRouteDataMapToGraphData(const RouteMap& RouteDataMap, const vector<NodeData>& FullGraphData)
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

void FitnessCalculator::AddRouteDataMapToLinkData(const RouteMap& RouteDataMap, const vector<LinkData>& FullLinkData)
{
	for (const auto& RouteMapPair : RouteDataMap)
	{
		auto Iter = RouteMapPair.second.begin();
		auto RIter = RouteMapPair.second.rbegin();
		if (RIter == RouteMapPair.second.rend() || Iter == RouteMapPair.second.end())
			continue;

		uint64_t FirstOrder = Iter->first;
		uint64_t LastOrder = RIter->first;
		for (uint64_t i = FirstOrder; i < LastOrder; ++i)
		{
			auto FromRouteNode = RouteMapPair.second.find(i)->second;
			auto ToRouteNode = RouteMapPair.second.find(i + 1)->second;
			for (const auto& Link : FullLinkData)
			{
				if (Link.FromNodeNum == FromRouteNode.Node && Link.ToNodeNum == ToRouteNode.Node)
				{
					bool bExist = false;
					for (const auto& ExistLink : LinkDataList)
					{
						if (ExistLink.FromNodeNum == FromRouteNode.Node && ExistLink.ToNodeNum == ToRouteNode.Node)
						{
							bExist = true;
							break;
						}
					}

					if (!bExist)
						LinkDataList.emplace_back(Link);

					break;
				}
			}
		}
	}
}

void FitnessCalculator::AddTrafficVolumeByTownBusRoute(const vector<string>& RouteNameList, uint64_t TrafficVolume)
{
	for (const auto& RouteName : RouteNameList)
	{
		if (RouteName.substr(0, 7) == TownBusStr)
		{
			auto FoundIter = NumOfUsersPerTownBusRoute.find(RouteName);
			if (FoundIter != NumOfUsersPerTownBusRoute.end())
				FoundIter->second += TrafficVolume;
			else
				NumOfUsersPerTownBusRoute.insert(make_pair(RouteName, TrafficVolume));
		}
	}
}

void FitnessCalculator::SetGraphData(const vector<ShortestPathData>& InPathData)
{
	for (const auto& PathData : InPathData)
		for (const auto& TownBusPair : PathData.TownBusData.TownBusStopCheck)
			if(TownBusPair.second)
				GraphData.push_back(TownBusPair.first);
}

void FitnessCalculator::SetLinkDataList(const vector<ShortestPathData>& InPathData, const vector<LinkData>& InFullLinkDataList)
{
	if (InPathData.size() < 1 || InFullLinkDataList.size() < 1)
		return;

	for (const auto& Path : InPathData)
	{
		auto TownBusRoute = Path.TownBusData.TownBusStopCheck;

		vector<LinkData> LinkListToReverseIntegrated;
		vector<LinkData> LinkListToIntegrated;
		uint64_t StartNodeNum = TownBusRoute.at(0).first.Num;
		for (size_t i = 0; i < TownBusRoute.size() - 1; ++i)
		{
			uint64_t NextNodeNum = TownBusRoute.at(i + 1).first.Num;
			for (const auto& FullLink : InFullLinkDataList)
			{
				if (FullLink.FromNodeNum == TownBusRoute.at(i).first.Num && FullLink.ToNodeNum == NextNodeNum)
					LinkListToIntegrated.push_back(FullLink);
				if (FullLink.FromNodeNum == NextNodeNum && FullLink.ToNodeNum == TownBusRoute.at(i).first.Num)
					LinkListToReverseIntegrated.push_back(FullLink);
			}

			if (!TownBusRoute.at(i + 1).second)
				continue;

			if (LinkListToIntegrated.size() != 0)
			{
				LinkData NewLinkData;
				NewLinkData.FromNodeNum = StartNodeNum;
				NewLinkData.ToNodeNum = NextNodeNum;
				NewLinkData.Type = LinkListToIntegrated.at(0).Type;
				for (const auto& IntegratedLink : LinkListToIntegrated)
				{
					NewLinkData.Length += IntegratedLink.Length;
					NewLinkData.Speed += IntegratedLink.Speed;
				}
				NewLinkData.Speed /= LinkListToIntegrated.size();
				LinkDataList.push_back(NewLinkData);
			}

			if (LinkListToReverseIntegrated.size() != 0)
			{
				LinkData NewLinkData;
				NewLinkData.FromNodeNum = NextNodeNum;
				NewLinkData.ToNodeNum = StartNodeNum;
				NewLinkData.Type = LinkListToReverseIntegrated.at(0).Type;
				for (const auto& IntegratedLink : LinkListToReverseIntegrated)
				{
					NewLinkData.Length += IntegratedLink.Length;
					NewLinkData.Speed += IntegratedLink.Speed;
				}
				NewLinkData.Speed /= LinkListToReverseIntegrated.size();
				LinkDataList.push_back(NewLinkData);
			}

			StartNodeNum = NextNodeNum;
			LinkListToIntegrated.clear();
			LinkListToReverseIntegrated.clear();
		}
	}
}

void FitnessCalculator::AddLinkDataFromDefaultRouteData(const RouteMap& InRouteDataMap, const map<string, OperatingData>& InOperatingData)
{
	for (const auto& RouteMapPair : InRouteDataMap)
	{
		auto Iter = RouteMapPair.second.begin();
		auto RIter = RouteMapPair.second.rbegin();
		if (RIter == RouteMapPair.second.rend() || Iter == RouteMapPair.second.end())
			continue;

		uint64_t FirstOrder = Iter->first;
		uint64_t LastOrder = RIter->first;
		for (uint64_t i = FirstOrder; i < LastOrder; ++i)
		{
			auto FromRouteNode = RouteMapPair.second.find(i)->second;
			auto ToRouteNode = RouteMapPair.second.find(i + 1)->second;

			bool bExist = false;
			for (const auto& Link : LinkDataList)
			{
				if (Link.FromNodeNum == FromRouteNode.Node && Link.ToNodeNum == ToRouteNode.Node)
				{
					bExist = true;
					break;
				}
			}

			if (bExist)
				continue;

			LinkData Link;
			Link.FromNodeNum = FromRouteNode.Node;
			Link.ToNodeNum = ToRouteNode.Node;
			Link.Length = ToRouteNode.CumDistance - FromRouteNode.CumDistance;

			if (RouteMapPair.first.substr(0, 5) == "metro" || RouteMapPair.first.substr(0, 5) == "Metro")
				Link.Type = LinkType::Rail;

			auto OperatingIter = InOperatingData.find(RouteMapPair.first);
			if (OperatingIter != InOperatingData.end())
				Link.Speed = OperatingIter->second.Speed;

			LinkDataList.emplace_back(Link);
		}
	}
}

float FitnessCalculator::FindLinkLength(const vector<LinkData>& InFullLinkDataList, uint64_t FromNodeNum, uint64_t ToNodeNum)
{
	for (const auto& Link : InFullLinkDataList)
		if (Link.FromNodeNum == FromNodeNum && Link.ToNodeNum == ToNodeNum)
			return Link.Length;

	return 0.f;
}