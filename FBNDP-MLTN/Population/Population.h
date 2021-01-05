#pragma once

#include "../Data/DataTypes.h"
#include "../Chromosome/Chromosome.h"

using namespace std;

class Population
{
public:
	Population(int MemberNum);
	uint32_t GetCurrentChromosomeNum() { return ChromosomeArray.size(); }
	Chromosome GetChromosome(uint32_t ChromosomeNum) { ChromosomeArray.at(ChromosomeNum); }
	vector<NodeData> GetChromosomeData(uint32_t ChromosomeNum)	/*ChromosomeNum번째의 ChromosomeData를 얻음*/
	{
		return ChromosomeArray.at(ChromosomeNum).GetChromosome();
	}

	/*TODO
	* Selection, Crossover, Mutation
	*/
private:
	void SetNodes(void);

	uint32_t MaxChromosomeNum;
	vector<Chromosome> ChromosomeArray;
	vector<NodeData> BusNode;
	vector<NodeData> RailNode;

};
