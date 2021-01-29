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


vector<uint64_t> Population::GetOverlappedNodeNum(Chromosome Parent)
{
	map<uint64_t, uint64_t> NodeCountMap;
	vector<uint64_t> OverlappedNodeNum;

	for (const auto& ChromosomeNodeIter : Parent.GetChromosome())
	{
		map<uint64_t, uint64_t>::iterator it;
		it = NodeCountMap.find(ChromosomeNodeIter.Num);
		if (it == NodeCountMap.end())
		{
			/*First Node Num - (not founded)*/
			NodeCountMap.insert(make_pair(ChromosomeNodeIter.Num, 1));
		}
		else
		{
			/*Alread Exist Node Num */
			++(it->second);
		}
	}
	for (const auto& ChromosomeNodeIter : Parent.GetChromosome())
	{
		uint64_t CurrentNodeNum = ChromosomeNodeIter.Num;
		map<uint64_t, uint64_t>::iterator it;
		it = NodeCountMap.find(CurrentNodeNum);
		if (it == NodeCountMap.end())
		{
			/*Node Num not exist - error*/
			assert(0);
		}
		else
		{
			OverlappedNodeNum.emplace_back(it->second);
		}
	}

	return OverlappedNodeNum;
}
void Population::Crossover(pair<Chromosome, Chromosome> Parents)
{
	struct CrossoverData {
		Chromosome ChromosomeData;
		vector<uint64_t> OverlappedNodeNum;
		CrossoverData(Chromosome Parent, vector<uint64_t> Overlapped) : ChromosomeData(Parent), OverlappedNodeNum(Overlapped) {};
	};

	Chromosome P1 = Parents.first;
	Chromosome P2 = Parents.second;

	vector<uint64_t> P1OverlappedNodeNum = GetOverlappedNodeNum(P1);
	vector<uint64_t> P2OverlappedNodeNum = GetOverlappedNodeNum(P2);

	uint64_t P1RouteNum = P1.GetRouteNum();
	uint64_t P2RouteNum = P2.GetRouteNum();

	vector<ShortestPathData> P1Routes = P1.GetRoute();
	vector<ShortestPathData> P2Routes = P2.GetRoute();

	CrossoverData C1(P1, P1OverlappedNodeNum);
	CrossoverData C2(P2, P2OverlappedNodeNum);

	uint64_t CurrentPosition = 0; // flowchart o
	while (P1RouteNum != 0)
	{
		NodeData FirstNode = C1.ChromosomeData.GetChromosome().at(CurrentPosition);
		if (FirstNode.Type == NodeType::Station)
		{
			P1.GetChromosome().erase(C1.ChromosomeData.GetChromosome().begin() + CurrentPosition);
			continue;
		}
		else
		{
			float MinRouteLength = INFINITY;
			ShortestPathData ShortestPath;
			for (const auto& PathDataIter : P1Routes)
			{
				for (const auto& NodeIter : PathDataIter.Path)
				{
					if (NodeIter.Num == FirstNode.Num)
					{
						if (MinRouteLength > PathDataIter.Cost)
						{
							/*Find Route Include First Node*/
							MinRouteLength = PathDataIter.Cost;
							ShortestPath = PathDataIter;
						}
					}
				}
			}
		}

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
