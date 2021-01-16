#pragma once
#include "../Data/DataTypes.h"
#include <map>

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
struct SelectedBusNodeData
{
	NodeData SelectedBusNode;
	ShortestPathData BusRouteData;
};

class Chromosome
{
public:
	Chromosome(vector<NodeData> RailNode, vector<NodeData> BusNode);
	uint32_t GetRouteNum(){ return RouteDataList.size(); }
	vector<NodeData> GetChromosome() { return ChromosomeNodeList; }
	
private:
	NodeData SelectRailNode();
	SelectedBusNodeData SelectBusNode(const NodeData& SelectedRailNode);


	void CalculateNetworkCost();
	void SelectMinNetworkCost();


	vector<NodeData> BusNode;	//flowchart B
	vector<NodeData> RailNode;	//Rail Station Nodes

	map<uint32_t, bool> RailStationSelected;
	bool bAllRailStationHaveRoute;

	vector<NodeData> CopiedBusNode;	//flowchart B'

	uint32_t BusRouteNum; //flowchart k
	vector<ShortestPathData> RouteDataList;
	vector<NodeData> ChromosomeNodeList;
};
