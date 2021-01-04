#pragma once

#include <vector>

using namespace std;

struct LinkData;

class FitnessCalculator
{
public:
	void Calculate();

private:
	void PassageAssignment();

	float CalculatePassageTime(const vector<LinkData>& Path);

	float CalculateFitness();
};