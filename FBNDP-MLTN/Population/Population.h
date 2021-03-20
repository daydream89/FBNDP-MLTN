#pragma once

#include "../Data/DataTypes.h"
#include "../Chromosome/Chromosome.h"

using namespace std;

class Population
{
public:
	Population(uint64_t MemberNum);
	uint64_t GetCurrentChromosomeNum() const { return ChromosomeArray.size(); }
	Chromosome GetChromosome(uint64_t ChromosomeNum) const { return ChromosomeArray.at(ChromosomeNum); }
	Chromosome& GetChromosomeRef(uint64_t ChromosomeNum) { return ChromosomeArray.at(ChromosomeNum); }
	vector<pair<NodeData,bool>> GetChromosomeData(uint64_t ChromosomeNum)	/*ChromosomeNum번째의 ChromosomeData를 얻음*/
	{
		return ChromosomeArray.at(ChromosomeNum).GetChromosome();
	}
	vector<pair<NodeData,bool>>& GetChromosomeDataRef(uint64_t ChromosomeNum)	/*ChromosomeNum번째의 ChromosomeData를 얻음*/
	{
		return ChromosomeArray.at(ChromosomeNum).GetChromosomeRef();
	}

	void GetNextGeneration(void);
	//pair<Chromosome, Chromosome> GetSelectedChromosomePair(int n) { return SelectionCompair.at(n); }

	/*TODO
	*  Crossover, Mutation, GetNextGeneration - TODO: Selection, Crossover and Mutation should be private. Ther are used in GetNextGeneration.
	*/
private:
	void SetNodes(void);
	void Selection(void);
	void Crossover(Chromosome P1, Chromosome P2);
	void Mutation(Chromosome& MutantCh);

	uint64_t MaxChromosomeNum;
	vector<Chromosome> ChromosomeArray;
	vector<Chromosome> ChildrenChromosomeArray; /* for Crossover..?*/
	vector<pair<Chromosome, Chromosome>> SelectionCompair;
	vector<NodeData> BusNode;
	vector<NodeData> TownBusNode;
	vector<NodeData> RailNode;

	vector<uint64_t> GetOverlappedNodeNum(Chromosome Parent);
	void ConnectExchangedRoute(Chromosome& MutantCh, uint64_t RouteNum, uint64_t RoutePos, uint64_t ChromosomePos);
	void PrintCurrentPopulationData();
};
