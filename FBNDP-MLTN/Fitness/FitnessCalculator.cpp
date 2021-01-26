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
	}
}

void FitnessCalculator::Calculate()
{
	PassageAssignment();
}

void FitnessCalculator::PassageAssignment()
{
	// get OD Matrix
	vector<TrafficVolumeData> TrafficVolumeDataList;
	if (auto DataCenterInstance = DataCenter::GetInstance())
		TrafficVolumeDataList = DataCenterInstance->GetTrafficVolumeData();

	for (const TrafficVolumeData& ODData : TrafficVolumeDataList)
	{
		if (ODData.FromNodeNum == ODData.ToNodeNum)
		{
			continue;
		}

		// find the shortest path based on the OD Matrix from network graph data.
		vector<ShortestPathData> ShortestPathList;
		PathFinderData PathFinder(GraphData, ODData.FromNodeNum, ODData.ToNodeNum, EPathFinderCostType::Duration, NumberOfPath);
		if (Util::PathFinder::FindShortestPath(PathFinder, ShortestPathList) == 0)
			continue;	// if not exist ShortestPath, ignore.

		SetPassageAssignmentForMNLModel(ShortestPathList, ODData.TrafficVolume);
		
		CalculateCustomerCost();
	}

	CalculateNetworkCost();
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
		if (-PassageTimeDiff <= Compare && Compare <= PassageTimeDiff)
		{
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

				for (auto Data : DistanceDataList)
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
			InOutPathList.at(0).TrafficVolumeForPath = static_cast<uint32_t>(TrafficVolume);
		}
	}
	else
	{
		// todo. handling k is greater than 2
	}
}

float FitnessCalculator::CalculateCustomerCost()
{


	return 0.f;
}

float FitnessCalculator::CalculateNetworkCost()
{


	return 0.f;
}