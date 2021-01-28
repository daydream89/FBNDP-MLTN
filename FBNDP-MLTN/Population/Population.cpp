#include "Population.h"
#include "../Data/DataCenter.h"
#include <cassert>

#include <fstream>

Population::Population(int MemberNum)
{
	MaxChromosomeNum = MemberNum;
	SetNodes();
	for (int i = 0; i < MemberNum; ++i)
	{
		ChromosomeArray.emplace_back(Chromosome(RailNode, BusNode));
	}
	assert(GetCurrentChromosomeNum() == MemberNum);
	printf("All Initial Population Created\n");
	PrintCurrentPopulationData();
}
void Population::Selection()
{
	CopiedChromosomeArray.clear();
	CopiedChromosomeArray.assign(ChromosomeArray.begin(), ChromosomeArray.end());
	/* TODO - select two another Chromosomes and make pair */

	SelectionCompair.clear();
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
		for (auto& ChromosomeArrayIter : ChromosomeArray)
		{
			static int i = 0;
			WriteFile << ++i << "th Chromosome: ";
			for (const auto& NodeArrayIter : ChromosomeArrayIter.GetChromosome())
			{
				WriteFile << NodeArrayIter.Num << " ";
			}
			WriteFile << "\n";
		}
	}
}
