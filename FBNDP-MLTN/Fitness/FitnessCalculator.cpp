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
	// get OD Matrix & Link Data List
	vector<LinkData> LinkDataList;
	vector<TrafficVolumeData> TrafficVolumeDataList;
	if (auto DataCenterInstance = DataCenter::GetInstance())
	{
		TrafficVolumeDataList = DataCenterInstance->GetTrafficVolumeData();
		LinkDataList = DataCenterInstance->GetLinkData();
	}

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

		SetPassageAssignmentForMNLModel(ShortestPathList, LinkDataList, 0, ODData.TrafficVolume);
		SetPassageAssignmentForMNLModel(ShortestPathList, LinkDataList, 1, ODData.TrafficVolume);
		
		CalculateCustomerCost();
	}

	CalculateNetworkCost();
}

float FitnessCalculator::SetPassageAssignmentForMNLModel(const vector<ShortestPathData>& InPathList, const vector<LinkData>& InLinkList, uint64_t SelectedPathNum, uint64_t TrafficVolume)
{
	if (InPathList.size() == 1)
	{
		// set single path
	}
	else if (InPathList.size() == 2)
	{
		float Compare = InPathList.at(0).Cost - InPathList.at(1).Cost;
		if (-PassageTimeDiff <= Compare && Compare <= PassageTimeDiff)
		{
			vector<float> UnityFunctionValue;
			for (const ShortestPathData& PathData : InPathList)
			{
				float TrainTravelTime = 0.f;
				for (uint64_t i = 0; i < PathData.Path.size() - 1; ++i)
				{
					NodeData CurNodeData = PathData.Path.at(i);
					NodeData NextNodeData = PathData.Path.at(i + 1);
					if (CurNodeData.Type == NodeType::Station)
						for (const auto& Link : InLinkList)
							if (Link.FromNodeNum == CurNodeData.Num && Link.ToNodeNum == NextNodeData.Num)
								TrainTravelTime += Util::Calculator::CalculateIVTT(Link);
				}

				float TravelTimeInVechicle = PathData.IVTT;		// IVTT
				float TravelTimeOutVechicle = PathData.OVTT;	// OVTT
				float CumulativeTransferPanaltyIndex = 1.f - expf(-static_cast<float>(PathData.CTPI));	// CTPI
				float TrainTravelTimeRatio = TrainTravelTime / PathData.IVTT;	// RELI
				float Curve = 0.f;	// CIRC

				UnityFunctionValue.emplace_back((MNLCoefData.IVTTCoef * TravelTimeInVechicle) + (MNLCoefData.OVTTCoef * TravelTimeOutVechicle) + (MNLCoefData.CTPICoef * CumulativeTransferPanaltyIndex)
					+ (MNLCoefData.RELICoef * TrainTravelTimeRatio) + (MNLCoefData.CIRCCoef * Curve));
			}

			float SumOfUnityFunctionValue = 0.f;
			for (auto Value : UnityFunctionValue)
				SumOfUnityFunctionValue += Value;

			float PassageRate = expf(UnityFunctionValue[SelectedPathNum]) / SumOfUnityFunctionValue;
			float TrafficForPath1 = static_cast<float>(TrafficVolume) * UnityFunctionValue[SelectedPathNum];
		}
		else
		{

		}
	}
	else
	{
		// todo. handling k is greater than 2
	}

	return 0.f;
}

float FitnessCalculator::CalculateCustomerCost()
{


	return 0.f;
}

float FitnessCalculator::CalculateNetworkCost()
{


	return 0.f;
}