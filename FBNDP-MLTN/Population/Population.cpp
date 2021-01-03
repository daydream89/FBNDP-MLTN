
#include "Population.h"
#include "../Data/DataCenter.h"
#include <random>

Population* Population::Instance = NULL;

PopulationMember::PopulationMember(const vector<NodeData>& BusNode,
	const vector<NodeData>& RailNode)
{
	CopyBusNode(BusNode);
	for (auto BusNodeIter : LeftBusNode)
	{
		printf("BusNode Num: %d\n", BusNodeIter.Num);
		printf("BusNode Coord.X: %d\n", BusNodeIter.Coord.X);
		printf("BusNode Coord.Y: %d\n", BusNodeIter.Coord.Y);
	}

	CopyRailNode(RailNode);
	for (auto RailNodeIter : MemberRailNode)
	{
		printf("RailNode Num: %d\n", RailNodeIter.Num);
		printf("RailNode Coord.X: %d\n", RailNodeIter.Coord.X);
		printf("RailNode Coord.Y: %d\n", RailNodeIter.Coord.Y);
	}
	for (int i = 0; i < 50; ++i)
	{
		NodeData tmp = SelectRailNode();
		printf("Selected RailNode Num: %d\n", tmp.Num);
	}
}

NodeData PopulationMember::SelectRailNode()
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> dis(0, MemberRailNode.size()-1);
	return MemberRailNode.at(dis(gen));
}
/*
NodeData SelectBusNode(vector<NodeSelectPercent> NodePercent)
{
	
}
*/

Population::Population(int MemberNum)
{
	SetNodes();
	for (int i = 0; i < MemberNum ; ++i)
	{
		PopulationMemberList.emplace_back(PopulationMember(BusNode, RailNode));
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
