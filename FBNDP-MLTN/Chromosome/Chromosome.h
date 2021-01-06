#pragma once
#include "../Data/DataTypes.h"

struct NodeSelectPercent
{
	float Percentage;
	uint32_t BusNodeNum;
};
struct BusRouteData
{
	uint32_t BusRouteNum;	//flowchart k
	vector<NodeData> RouteNodes;
};

class Chromosome
{
public:
	Chromosome(vector<NodeData> RailNode, vector<NodeData> BusNode);
	uint32_t GetRouteNum(){ return RouteList.size(); }
	vector<NodeData> GetChromosome() { return ChromosomeNodeList; }
	
private:
	NodeData SelectRailNode();
	vector<NodeSelectPercent> GenRouletteWheelPercent(const NodeData& SelectedRailNode);
	NodeData SelectBusNode(const vector<NodeSelectPercent>& NodePercent);


	void CalculateNetworkCost();
	void SelectMinNetworkCost();


	vector<NodeData> BusNode;	//flowchart B
	vector<NodeData> RailNode;	//Rail Station Nodes

	vector<bool> RailStationSelected;
	bool bAllRailStationHaveRoute;

	vector<NodeData> CopiedBusNode;	//flowchart B'

	vector<vector<NodeData>> RouteList;
	vector<NodeData> ChromosomeNodeList;
};
