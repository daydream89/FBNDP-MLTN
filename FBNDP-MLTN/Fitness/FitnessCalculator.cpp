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
		PathFinderData PathFinder(GraphData, ODData.FromNodeNum, ODData.ToNodeNum, PathFinderCostType::Duration, NumberOfPath);
		Util::PathFinder::FindShortestPath(PathFinder, ShortestPathList);

		// get passage time of shortest path k1, k2.
		float PassageTimeK1 = CalculatePassageTime(ShortestPathList.at(0));
		float PassageTimeK2 = CalculatePassageTime(ShortestPathList.at(1));

		// check passage time of k1 and k2
		bool bDoublePassage = false;
		// �̰� ���ϴ� �Լ� ���� ���� Util�� ����. offset�� ������ �� �ְ�. ex) CompareFloat(float value1, float value2, float offset)
		float Compare = PassageTimeK1 - PassageTimeK2;
		if (-5.f <= Compare && Compare <= 5.f)
		{
			bDoublePassage = true;
		}

		// add K1 in PassageDataList
		if (bDoublePassage)
		{
			// add K2 in PassageDataList with MNL Model.
			// P = exp(U) / sum(exp(U'))
			// U = -0.0176IVTT - 0.0296OVTT - 3.8418CTPI + 3.1469RELI - 0.3896CIRC
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