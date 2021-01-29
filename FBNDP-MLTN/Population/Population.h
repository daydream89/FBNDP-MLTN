#pragma once

#include "../Data/DataTypes.h"
#include "../Chromosome/Chromosome.h"

using namespace std;

class Population
{
public:
	Population(uint64_t MemberNum);
	uint64_t GetCurrentChromosomeNum() { return ChromosomeArray.size(); }
	Chromosome GetChromosome(uint64_t ChromosomeNum) { return ChromosomeArray.at(ChromosomeNum); }
	vector<NodeData> GetChromosomeData(uint64_t ChromosomeNum)	/*ChromosomeNum��°�� ChromosomeData�� ����*/
	{
		return ChromosomeArray.at(ChromosomeNum).GetChromosome();
	}

	void Selection(void);

	/*TODO
	*  Crossover, Mutation, GetNextGeneration - TODO: Selection, Crossover and Mutation should be private. Ther are used in GetNextGeneration.
	*/
private:
	void SetNodes(void);

	uint64_t MaxChromosomeNum;
	vector<Chromosome> ChromosomeArray;
	vector<Chromosome> ChildChromosomeArray; /* for Crossover..?*/
	vector<pair<Chromosome, Chromosome>> SelectionCompair;
	vector<NodeData> BusNode;
	vector<NodeData> RailNode;

	void PrintCurrentPopulationData();
};
