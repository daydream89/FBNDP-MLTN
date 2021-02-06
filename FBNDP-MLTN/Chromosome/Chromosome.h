#pragma once
#include "../Data/DataTypes.h"
#include <map>

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
	//vector<ShortestPathData> NewPath); /*For Children*/
	Chromosome(const vector<NodeData>& RailNode, const vector<NodeData>& BusNode, const vector<NodeData>& TownBusNodes, vector<ShortestPathData> NewPath);
	Chromosome(const vector<NodeData>& RailNode, const vector<NodeData>& BusNode, const vector<NodeData>&TownBusNodes);
	uint64_t GetRouteNum(){ return RouteDataList.size(); }
	vector<ShortestPathData> GetRoute(){ return RouteDataList; }
	vector<pair<NodeData, bool>> GetChromosome() { return ChromosomeNodeList; }
	void RemoveOverlapedRoute(void);
	
private:
	NodeData SelectRailNode();
	SelectedBusNodeData SelectBusNode(const NodeData& SelectedRailNode);

	vector<NodeData> BusNode;	//flowchart B
	vector<NodeData> TownBusNode;	//flowchart B
	vector<NodeData> RailNode;	//Rail Station Nodes

	map<uint64_t, bool> RailStationSelected;
	bool bAllRailStationHaveRoute;

	vector<NodeData> CopiedBusNode;	//flowchart B'

	uint64_t BusRouteNum; //flowchart k
	vector<ShortestPathData> RouteDataList;
	vector<pair<NodeData, bool>> ChromosomeNodeList;
};
