#include "Chromosome.h"
#include <random>

Chromosome::Chromosome(vector<NodeData> RailNode, vector<NodeData> BusNode)
{
	bAllRailStationHaveRoute = false;
	this->RailNode.assign(RailNode.begin(), RailNode.end());
	CopiedBusNode.assign(BusNode.begin(), BusNode.end());
	RailStationSelected.resize(RailNode.size());

	while (CopiedBusNode.size() > 0) {
		NodeData SelectedNode = SelectRailNode();
		/* TODO */
		//GenRouletteWheelPercent(SelectedNode);
	}

}
NodeData Chromosome::SelectRailNode()
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> dis(0, RailNode.size()-1);
	uint32_t RandomNum = dis(gen);
	if (bAllRailStationHaveRoute == false) {
		if (RailStationSelected.at(RandomNum) == false)
			RailStationSelected.at(RandomNum) = true;
		for (auto StationSelectedCheck : RailStationSelected)
		{
			if (StationSelectedCheck == false)
				break;
			else
				bAllRailStationHaveRoute = true;
		}
	}
	return RailNode.at(RandomNum);
}

#if 0
vector<NodeSelectPercent> Chromosome::GenRouletteWheelPercent(const NodeData& SelectedRailNode)
{
	/* TODO */

}

NodeData SelectBusNode(const vector<NodeSelectPercent>& NodePercent)
{
	/* TODO */
}
#endif
