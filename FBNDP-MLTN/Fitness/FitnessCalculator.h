#pragma once

#include "../Data/DataCenter.h"

#include <vector>

using namespace std;

class FitnessCalculator
{
public:
	FitnessCalculator(const vector<NodeData>& InGraphData, uint64_t PathNum);

	void Calculate();

private:
	void PassageAssignment();

	void SetPassageAssignmentForMNLModel(vector<ShortestPathData>& InOutPathList, uint64_t TrafficVolume);

	float CalculateCustomerCost();

	float CalculateNetworkCost();

private:
	vector<NodeData> GraphData;
	uint64_t NumberOfPath = 2;
	
	float PassageTimeDiff = 5.f;
	MNLCoefficientData MNLCoefData;
};