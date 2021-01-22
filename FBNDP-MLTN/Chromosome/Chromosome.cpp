#include "Chromosome.h"
#include "../util/Utils.h"
#include <random>
#include <map>
#include <cassert>

Chromosome::Chromosome(const vector<NodeData>& RailNode, const vector<NodeData>& BusNode):bAllRailStationHaveRoute(false),BusRouteNum(0)
{
	this->RailNode.assign(RailNode.begin(), RailNode.end());
	this->BusNode.assign(BusNode.begin(), BusNode.end());
	CopiedBusNode.assign(BusNode.begin(), BusNode.end());
	for (const auto& CheckRailNode : RailNode)
		RailStationSelected[CheckRailNode.Num] = false;

#if 1
	while (CopiedBusNode.size() > 0) 
	{
		NodeData SelectedRailNode = SelectRailNode();
		/* TODO */
		SelectedBusNodeData SelectedBus = SelectBusNode(SelectedRailNode);
		if (!bAllRailStationHaveRoute)
		{
			if (bAllRailStationHaveRoute == false)
			{
				bool StationCheckFlag = true;
				if (RailStationSelected.find(SelectedRailNode.Num)->second == false)
					RailStationSelected[SelectedRailNode.Num] = true;
				for (const auto& StationSelectedCheck : RailStationSelected)
				{
					if (StationSelectedCheck.second == false)
					{
						StationCheckFlag = false;
						break;
					}
				}
				if (StationCheckFlag)
					bAllRailStationHaveRoute = true;
			}
			++BusRouteNum; //k = k+1
			ShortestPathData RoutePathData;
			RoutePathData.Path.assign(SelectedBus.BusRouteData.Path.begin(), SelectedBus.BusRouteData.Path.end());
			RoutePathData.Cost = SelectedBus.BusRouteData.Cost;
			RouteDataList.emplace_back(RoutePathData);
		}
		else
		{
			/*TODO : Find Shortest Path Routes...*/
			ShortestPathData FoundedShortestRoute;
			ShortestPathData ExistRoute;
			for (auto RouteDataIter : RouteDataList)
			{
				static float MinRouteLength = INFINITY;
				//printf("Rail Node Num: %ld\n", (int)RouteDataIter.Path.at(RouteDataIter.Path.size() - 1).Num);
				if (RouteDataIter.Path.at(RouteDataIter.Path.size() - 1).Num == SelectedRailNode.Num) //Current Selected Rail Node Routes...
				{
					float RouteLength = 0;
					vector<NodeData> InputGraph;
					vector<ShortestPathData> ShortestRoute;
					InputGraph.assign(CopiedBusNode.begin(), CopiedBusNode.end());
					InputGraph.emplace_back(RouteDataIter.Path.at(0));
					PathFinderData ShortestPathData(InputGraph, SelectedBus.SelectedBusNode.Num, RouteDataIter.Path.begin()->Num, EPathFinderCostType::Length, 1);
					Util::PathFinder::FindShortestPath(ShortestPathData, ShortestRoute);
					RouteLength = ShortestRoute.at(0).Cost;
					if (RouteLength <= MinRouteLength)
					{
						MinRouteLength = RouteLength;
						FoundedShortestRoute.Path.clear();
						FoundedShortestRoute.Path.assign(ShortestRoute.begin()->Path.begin(), ShortestRoute.begin()->Path.end());
						FoundedShortestRoute.Cost = RouteLength + RouteDataIter.Cost;
						ExistRoute = RouteDataIter;
					}
				}
			}
			/*TODO: SelectedBus.BusRouteData.Cost vs Shortest Cost include exist route */
			if (FoundedShortestRoute.Cost > SelectedBus.BusRouteData.Cost || FoundedShortestRoute.Cost >= INFINITY) /* Find Lesat Cost Path */
			{
				for (auto& RouteIter : RouteDataList)
				{
					if (RouteIter.Path.begin()->Num == ExistRoute.Path.begin()->Num)
					{
						RouteIter.Path.insert(RouteIter.Path.begin(), FoundedShortestRoute.Path.begin(), FoundedShortestRoute.Path.end()-1);
					}
				}
			}
			else
			{
				++BusRouteNum; //k = k+1
				ShortestPathData RoutePathData;
				RoutePathData.Path.assign(SelectedBus.BusRouteData.Path.begin(), SelectedBus.BusRouteData.Path.end());
				RoutePathData.Cost = SelectedBus.BusRouteData.Cost;
				RouteDataList.emplace_back(RoutePathData);
			}

		}

		for (const NodeData& BusRouteNodeNum : SelectedBus.BusRouteData.Path)
		{
			for (vector<NodeData>::const_iterator NodeIter = CopiedBusNode.begin(); NodeIter != CopiedBusNode.end();) //B' = \i
			{
				if (NodeIter->Num == BusRouteNodeNum.Num)
				{
					CopiedBusNode.erase(NodeIter);
					break;
				}
				else
				{
					++NodeIter;
				}
			}
		}
	}
	
	for (const auto& RouteIter : RouteDataList)
	{
		ChromosomeNodeList.insert(ChromosomeNodeList.end(), RouteIter.Path.begin(), RouteIter.Path.end());
	}
	
#endif

}
NodeData Chromosome::SelectRailNode()
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int64_t> dis(0, static_cast<int64_t>(RailNode.size()-1));
	int64_t RandomNum = dis(gen);
	/*
	if (bAllRailStationHaveRoute == false) 
	{
		bool StationCheckFlag = true;
		if (RailStationSelected.at(RandomNum) == false)
			RailStationSelected.at(RandomNum) = true;
		for (auto StationSelectedCheck : RailStationSelected)
		{
			if (StationSelectedCheck == false)
			{
				StationCheckFlag = false;
				break;
			}
		}
		if (StationCheckFlag)
			bAllRailStationHaveRoute = true;
	}
	*/
	return RailNode.at(RandomNum);
}

#if 1
SelectedBusNodeData Chromosome::SelectBusNode(const NodeData& SelectedRailNode)
{
	/* TODO */

	map<uint64_t, ShortestPathData> FoundBusRouteMap;
	map<uint64_t, float> CalculatePercentMap;
	float TotalRouteLength = 0.0f;
	float MinRouteLength = INFINITY;
	float MaxRouteLength = 0.0f;
	for (auto BusNodeIter : CopiedBusNode)
	{
		vector<NodeData> InputGraph;
		vector<ShortestPathData> ShortestRoute;
		InputGraph.assign(BusNode.begin(), BusNode.end());
		InputGraph.emplace_back(SelectedRailNode);
		PathFinderData ShortestPathData(InputGraph, BusNodeIter.Num, SelectedRailNode.Num, EPathFinderCostType::Length, 1);
		Util::PathFinder::FindShortestPath(ShortestPathData, ShortestRoute);
		if (ShortestRoute.at(0).Cost >= INFINITY)
		{
			printf("No Route\n");
			continue;
		}
		MaxRouteLength = max(MaxRouteLength, ShortestRoute.at(0).Cost);
		MinRouteLength = min(MinRouteLength, ShortestRoute.at(0).Cost);
		TotalRouteLength += ShortestRoute.at(0).Cost;
		FoundBusRouteMap.insert(make_pair(BusNodeIter.Num, ShortestRoute.at(0)));
		CalculatePercentMap.insert(make_pair(BusNodeIter.Num, ShortestRoute.at(0).Cost));
		printf("From %lu to %lu, Shortest Path Length : %lf\n", (int)BusNodeIter.Num, (int)SelectedRailNode.Num, ShortestRoute.at(0).Cost);
	}
	printf("Max Route Length: %lf\n", MaxRouteLength);
	printf("Min Route Length: %lf\n", MinRouteLength);
	float MaxPlusMinLength = MaxRouteLength + MinRouteLength;

	float TotalLengthDiff = 0.0f;
	for (auto& PercentMapIter : CalculatePercentMap)
	{
		PercentMapIter.second = MaxPlusMinLength - PercentMapIter.second;
		TotalLengthDiff += PercentMapIter.second;
	}

	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<float> dis(0.0f, 1.0f);
	float RandomNum = dis(gen);
	printf("Created Random Num(0.0 ~ 1.0): %f\n", RandomNum);

	float CumulativeProbability = 0.0f;
	uint64_t SelectedBusNodeNum = INFINITY;
	bool bBusSelected = false;
	for (const auto& PercentMapIter : CalculatePercentMap)
	{
		float LowRange = CumulativeProbability;
		float HighRange = CumulativeProbability + (PercentMapIter.second/TotalLengthDiff);
		if  (RandomNum >= LowRange && RandomNum <HighRange)
		{
			SelectedBusNodeNum = PercentMapIter.first;
			bBusSelected = true;
			break;
		}
		CumulativeProbability = HighRange;
	}

	assert(bBusSelected == true);

	if (bBusSelected == false) {
		printf("Bus doesn't selected..error\n");

	}

	SelectedBusNodeData SelectedData;
	for (const auto &BusNodeIter : CopiedBusNode)
	{
		if (BusNodeIter.Num == SelectedBusNodeNum) 
		{
			SelectedData.SelectedBusNode = BusNodeIter;
			break;
		}
	}
	SelectedData.BusRouteData = FoundBusRouteMap.find(SelectedBusNodeNum)->second;
	printf("Selected Bus Node Num: %d\n", (int)SelectedBusNodeNum);
	return SelectedData;
}
#endif
