#include "FitnessCalculator.h"

#include "../Data/DataCenter.h"
#include "../Util/Utils.h"

FitnessCalculator::FitnessCalculator(const vector<NodeData>& InGraphData, uint32_t PathNum)
	: GraphData(InGraphData)
	, NumberOfPath(PathNum)
{
	// convert vector<NodeData> to common graph data using link, if need.
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

		// get passage time of shortest path k1, k2.
		vector<float> PassageTimeList;
		for (auto ShortestPath : ShortestPathList)
			PassageTimeList.emplace_back(CalculatePassageTime(ShortestPath));

		if (PassageTimeList.size() == 1)
		{
			// set single path
		}
		else if (PassageTimeList.size() == 2)
		{
			float Compare = PassageTimeList.at(0) - PassageTimeList.at(1);
			if (-5.f <= Compare && Compare <= 5.f)		// ´ÜÀ§ ¸ÂÃçÁà¾ß ÇÔ..
			{
				// add K2 in PassageDataList with MNL Model.
				// P = exp(U) / sum(exp(U'))
				// U = -0.0176IVTT - 0.0296OVTT - 3.8418CTPI + 3.1469RELI - 0.3896CIRC
			}
		}
		else
		{
			// todo. handling k is greater than 2
		}
	}

	CalculateFitness();
}

float FitnessCalculator::CalculatePassageTime(ShortestPathData& PathData)
{
	if (PathData.Path.size() == 0)
	{
		return 0.f;
	}

	float PassageTime = 0.f;
	for (uint32_t i = 0; i < PathData.Path.size() - 1; ++i)
	{
		auto FromNode = PathData.Path[i];
		auto ToNode = PathData.Path[i + 1];
		// get next node from path, find time data from DataCenter
	}

	return 0.f;
}

float FitnessCalculator::CalculateFitness()
{


	return 0;
}