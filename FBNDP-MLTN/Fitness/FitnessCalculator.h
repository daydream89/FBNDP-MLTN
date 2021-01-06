#pragma once

#include <vector>

using namespace std;

struct LinkData;
struct NodeData;

class FitnessCalculator
{
public:
	FitnessCalculator(const vector<NodeData>& InGraphData);

	void Calculate();

private:
	void PassageAssignment();

	float CalculatePassageTime(const vector<LinkData>& Path);

	float CalculateFitness();

private:
	vector<NodeData> GraphData;
};