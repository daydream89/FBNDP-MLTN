#pragma once

#include <vector>

using namespace std;

class FitnessCalculator
{
public:
	void Calculate();

private:
	void PassageAssignment();

	void FindShortestPath(vector<uint32_t>& OutShortestPathK1, vector<uint32_t>& OutShortestPathK2);

	float CalculatePassageTime(const vector<uint32_t>& Path);

	float CalculateFitness();
};