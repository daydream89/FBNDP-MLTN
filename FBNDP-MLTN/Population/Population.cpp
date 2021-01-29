#include "Population.h"
#include "../Data/DataCenter.h"
#include <cassert>
#include <random>
#include <fstream>
#include <list>

Population::Population(uint64_t MemberNum)
{
	MaxChromosomeNum = MemberNum;
	SetNodes();
	for (uint64_t i = 0; i < MemberNum; ++i)
	{
		ChromosomeArray.emplace_back(Chromosome(RailNode, BusNode));
	}
	assert(GetCurrentChromosomeNum() == MemberNum);
	printf("All Initial Population Created\n");
	PrintCurrentPopulationData();
}
void Population::Selection()
{
	random_device rd;
	mt19937 gen(rd());

	vector<uint64_t> RandomOrder;
	for (uint64_t i = 0; i < ChromosomeArray.size(); ++i)
	{
		RandomOrder.emplace_back(i);
	}

	/* Make Random Order By Swapping vector*/
	uint64_t CurrentPos = 0;
	uint64_t LastMemberSwappedPosition;
	for (uint64_t i = 0; i < RandomOrder.size()-1; ++i)
	{
		uniform_int_distribution<int64_t> dis(CurrentPos + 1, static_cast<int64_t>(RandomOrder.size() - 1));
		int64_t RandomNum = dis(gen);
		if (RandomNum == RandomOrder.size() - 1)
			LastMemberSwappedPosition = i;
		uint64_t TempNum = RandomOrder.at(RandomNum);
		RandomOrder.at(RandomNum) = RandomOrder.at(i);
		RandomOrder.at(i) = TempNum;
		++CurrentPos;
	}

	SelectionCompair.clear();
	/*Make Chromosome Pair(Selection)*/
	for (uint64_t i = 0; i < ChromosomeArray.size(); ++i)
	{
		SelectionCompair.emplace_back(make_pair(ChromosomeArray.at(i), ChromosomeArray.at(RandomOrder.at(i))));
	}
}

void Population::SetNodes()
{
	if (auto* DataCenter = DataCenter::GetInstance())
	{
		for (auto NodeIter : DataCenter->GetNodeData())
		{
			if (NodeIter.Type == NodeType::BusStop)
				BusNode.emplace_back(NodeIter);
			else if (NodeIter.Type == NodeType::Station)
				RailNode.emplace_back(NodeIter);
			else
				fprintf(stderr, "Unkonwn Node Type..?: %d\n", NodeIter.Type);

		}
	}
}

void Population::PrintCurrentPopulationData()
{
	string FilePath = "PopulationData.txt";
	ofstream WriteFile(FilePath.data());
	if (WriteFile.is_open())
	{
		uint64_t i = 0;
		for (auto& ChromosomeArrayIter : ChromosomeArray)
		{
			WriteFile << ++i << "th Chromosome: ";
			for (const auto& NodeArrayIter : ChromosomeArrayIter.GetChromosome())
			{
				WriteFile << NodeArrayIter.Num << " ";
			}
			WriteFile << "\n";
		}
	}
}
