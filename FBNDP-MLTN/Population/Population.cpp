#include "Population.h"
#include "../Data/DataCenter.h"
#include <cassert>
#include <random>
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
	random_device rd;
	mt19937 gen(rd());
	vector<bool> PairCheck;
	PairCheck.assign(ChromosomeArray.size(), false);
	uniform_int_distribution<int64_t> dis(0, static_cast<int64_t>(ChromosomeArray.size() - 1));

	SelectionCompair.clear();
	int i = 0;
	for (const auto& ChromosomeIter : ChromosomeArray)
	{
		while (1)
		{
			int64_t RandomNum = dis(gen);
			if (RandomNum != i && PairCheck.at(RandomNum) != true)
			{
				PairCheck.at(RandomNum) = true;
				SelectionCompair.emplace_back(make_pair(ChromosomeIter, ChromosomeArray.at(RandomNum)));
				break;
			}
		}
		++i;
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
