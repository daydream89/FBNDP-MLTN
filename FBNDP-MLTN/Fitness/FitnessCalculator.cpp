#include "FitnessCalculator.h"

#include "../Util/Utils.h"

#include <math.h>

FitnessCalculator::FitnessCalculator(const vector<NodeData>& InGraphData, uint64_t PathNum)
	: GraphData(InGraphData)
	, NumberOfPath(PathNum)
{
	
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
	{
		TrafficVolumeDataList = DataCenterInstance->GetTrafficVolumeData();
	}

	//vector<PassageData> PassageDataList;
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

float FitnessCalculator::SetPassageAssignmentForMNLModel(const vector<ShortestPathData>& PathList, uint64_t TrafficVolume)
{
	if (PathList.size() == 1)
	{
		// set single path
	}
	else if (PathList.size() == 2)
	{
		float Compare = PathList.at(0).Cost - PathList.at(1).Cost;
		if (-PassageTimeDiff <= Compare && Compare <= PassageTimeDiff)		// 단위 확인 후 맞춰줘야 함.. shortest path로부터 도출된 시간이 hour인지 min인지 모름.
		{
			// add K2 in PassageDataList with MNL Model.
			// P = exp(U) / sum(exp(U'))
			// U = -0.0176IVTT - 0.0296OVTT - 3.8418CTPI + 3.1469RELI - 0.3896CIRC
		}
		else
		{

		}
	}
	else
	{
		// todo. handling k is greater than 2
	}

	float TravelTimeInVechicle = 0.f; // IVTT
	float TravelTimeOutVechicle = 0.f;	// OVTT
	float CumulativeTransferPanaltyIndex = 0.f;	// CTPI
	float TrainTravelTimeRatio = 0.f;	// RELI
	float Curve = 0.f;	// CIRC

	float UnityFunctionValue = (MNLCoefData.IVTTCoef * TravelTimeInVechicle) + (MNLCoefData.OVTTCoef * TravelTimeOutVechicle) + (MNLCoefData.CTPICoef * CumulativeTransferPanaltyIndex)
								+ (MNLCoefData.RELICoef * TrainTravelTimeRatio) + (MNLCoefData.CIRCCoef * Curve);

	float PassageRate = expf(UnityFunctionValue);// / sum of UnityFunctionValue;
	float TrafficForPath = static_cast<float>(TrafficVolume) * UnityFunctionValue;

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