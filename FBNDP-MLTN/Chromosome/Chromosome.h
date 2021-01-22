#pragma once
#include "../Data/DataTypes.h"
#include <map>

struct NodeSelectPercent
{
	float Percentage;
	uint64_t BusNodeNum;
};
struct BusRouteData
{
	uint64_t BusRouteNum;	//flowchart k
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
	Chromosome(const vector<NodeData>& RailNode, const vector<NodeData>& BusNode);
	uint64_t GetRouteNum(){ return RouteDataList.size(); }
	vector<NodeData> GetChromosome() { return ChromosomeNodeList; }
	
private:
	NodeData SelectRailNode();
	SelectedBusNodeData SelectBusNode(const NodeData& SelectedRailNode);


	void CalculateNetworkCost();
	void SelectMinNetworkCost();


	vector<NodeData> BusNode;	//flowchart B
	vector<NodeData> RailNode;	//Rail Station Nodes

	map<uint64_t, bool> RailStationSelected;
	bool bAllRailStationHaveRoute;

	vector<NodeData> CopiedBusNode;	//flowchart B'

	uint64_t BusRouteNum; //flowchart k
	vector<ShortestPathData> RouteDataList;
	vector<NodeData> ChromosomeNodeList;
};
