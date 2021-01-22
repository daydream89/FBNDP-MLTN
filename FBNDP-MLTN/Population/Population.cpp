#include "Population.h"
#include "../Data/DataCenter.h"
#include <cassert>

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

