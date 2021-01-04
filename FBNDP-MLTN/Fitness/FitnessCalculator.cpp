#include "FitnessCalculator.h"

#include "../Data/DataCenter.h"
#include "../Util/Utils.h"

void FitnessCalculator::Calculate()
{
	PassageAssignment();
}

void FitnessCalculator::PassageAssignment()
{
	// get network graph data

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
		vector<uint32_t> tempData;
		vector<vector<LinkData>> ShortestPathList;
		Util::PathFinder::FindShortestPath(tempData, ShortestPathList);

		// get passage time of shortest path k1, k2.
		float PassageTimeK1 = CalculatePassageTime(ShortestPathList.at(0));
		float PassageTimeK2 = CalculatePassageTime(ShortestPathList.at(1));

		// check passage time of k1 and k2
		bool bDoublePassage = false;
		// 이거 비교하는 함수 따로 만들어서 Util에 넣자. offset도 세팅할 수 있게. ex) CompareFloat(float value1, float value2, float offset)
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

float FitnessCalculator::CalculatePassageTime(const vector<LinkData>& Path)
{
	if (Path.size() == 0)
	{
		return 0.f;
	}

	float PassageTime = 0.f;
	for (vector<LinkData>::const_iterator CIter = Path.begin(); CIter != Path.end(); ++CIter)
	{
		CIter->FromNodeNum;
		CIter->ToNodeNum;
		// get next node from path, find time data from DataCenter
	}

	return 0.f;
}

float FitnessCalculator::CalculateFitness()
{


	return 0;
}