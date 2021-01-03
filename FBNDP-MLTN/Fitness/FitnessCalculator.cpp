#include "FitnessCalculator.h"
#include "../Data/DataCenter.h"

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
		vector<uint32_t> K1, K2;
		FindShortestPath(K1, K2);

		// get passage time of shortest path k1, k2.
		float PassageTimeK1 = CalculatePassageTime(K1);
		float PassageTimeK2 = CalculatePassageTime(K2);

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
			// add K2 in PassageDataList
		}
	}

	CalculateFitness();
}

void FitnessCalculator::FindShortestPath(vector<uint32_t>& OutShortestPathK1, vector<uint32_t>& OutShortestPathK2)
{

}

float FitnessCalculator::CalculatePassageTime(const vector<uint32_t>& Path)
{
	if (Path.size() == 0)
	{
		return 0.f;
	}

	float PassageTime = 0.f;
	for (vector<uint32_t>::const_iterator CIter = Path.begin(); CIter != Path.end(); ++CIter)
	{
		uint32_t NodeNum = (*CIter);
		// get next node from path, find time data from DataCenter
	}

	return 0.f;
}

float FitnessCalculator::CalculateFitness()
{


	return 0;
}