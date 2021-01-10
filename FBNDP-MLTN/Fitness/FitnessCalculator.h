#pragma once

#include <vector>

using namespace std;

struct LinkData;
struct NodeData;
struct ShortestPathData;

class FitnessCalculator
{
public:
	FitnessCalculator(const vector<NodeData>& InGraphData, uint32_t PathNum);

	void Calculate();

private:
	void PassageAssignment();

	float CalculatePassageTime(ShortestPathData& Path);

	float CalculateFitness();

private:
	vector<NodeData> GraphData;
	uint32_t NumberOfPath = 2;
};