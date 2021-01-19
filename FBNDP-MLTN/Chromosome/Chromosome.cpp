#include "Chromosome.h"
#include "../util/Utils.h"
#include <random>
#include <map>

Chromosome::Chromosome(vector<NodeData> RailNode, vector<NodeData> BusNode):bAllRailStationHaveRoute(false),BusRouteNum(0)
{
	this->RailNode.assign(RailNode.begin(), RailNode.end());
	CopiedBusNode.assign(BusNode.begin(), BusNode.end());
	for (auto CheckRailNode : RailNode)
		RailStationSelected[CheckRailNode.Num] = false;

#if 0
	while (CopiedBusNode.size() > 0) 
	{
		NodeData SelectedRailNode = SelectRailNode();
		/* TODO */
		SelectedBusNodeData SelectedBus;
		bool bIsBusSelected = SelectBusNode(SelectedRailNode, SelectedBus);
		if (bIsBusSelected)
		{
			if (!bAllRailStationHaveRoute)
			{
				if (bAllRailStationHaveRoute == false)
				{
					bool StationCheckFlag = true;
					if (RailStationSelected.find(SelectedRailNode.Num)->second == false)
						RailStationSelected[SelectedRailNode.Num] = true;
					for (auto StationSelectedCheck : RailStationSelected)
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
				float MinRouteLength = INFINITY;
				for (auto RouteDataIter : RouteDataList)
				{
					printf("Route Node Data Num: %ld\n", RouteDataIter.Path.at(RouteDataIter.Path.size() - 1).Num);
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
				if (FoundedShortestRoute.Cost < SelectedBus.BusRouteData.Cost) /* Find Lesat Cost Path */
				{
					for (auto RouteIter : RouteDataList)
					{
						if (RouteIter.Path.begin()->Num == ExistRoute.Path.begin()->Num)
						{
							RouteIter.Path.insert(RouteIter.Path.begin(), FoundedShortestRoute.Path.begin(), FoundedShortestRoute.Path.end());
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
		}
		else /* Find Shortest Path from Selected Rail Node to CopiedBusNode(Left)*/
		{
			ShortestPathData FoundedShortestRoute;
			ShortestPathData ExistRoute;
			float MinRouteLength = INFINITY;
			for (auto CopiedBusNodeIter : CopiedBusNode)
			{
				float RouteLength = 0;
				printf("CopiedBusNode Iter Num: %ld\n", CopiedBusNodeIter.Num);
				vector<NodeData> InputGraph;
				vector<ShortestPathData> ShortestRoute;
				InputGraph.assign(CopiedBusNode.begin(), CopiedBusNode.end());
				PathFinderData ShortestPathData(InputGraph, SelectedBus.SelectedBusNode.Num, CopiedBusNodeIter.Num, EPathFinderCostType::Length, 1);
				Util::PathFinder::FindShortestPath(ShortestPathData, ShortestRoute);
				RouteLength = ShortestRoute.at(0).Cost;
				/*Find Shortest Path From Selected Bus From CopiedBusIter*/
				if (RouteLength <= MinRouteLength)
				{
					MinRouteLength = RouteLength;
					FoundedShortestRoute.Path.clear();
					FoundedShortestRoute.Path.assign(ShortestRoute.begin()->Path.begin(), ShortestRoute.begin()->Path.end());
					FoundedShortestRoute.Cost = RouteLength + RouteDataIter.Cost;
					ExistRoute = RouteDataIter;
				}
			}
			/*add Find Route to Route start from CopiedBusNode to SelectedRailNode*/
		}

		for (auto BusRouteNodeNum : SelectedBus.BusRouteData.Path)
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
bool Chromosome::SelectBusNode(const NodeData& SelectedRailNode, SelectedBusNodeData& OutputData)
{
	/* TODO */

	map<uint64_t, ShortestPathData> FoundBusRouteMap;
	float TotalRouteLength = 0;
	int NoRouteCount = 0;
	for (auto BusNodeIter : CopiedBusNode)
	{
		vector<NodeData> InputGraph;
		vector<ShortestPathData> ShortestRoute;
		InputGraph.assign(CopiedBusNode.begin(), CopiedBusNode.end());
		InputGraph.emplace_back(SelectedRailNode);
		PathFinderData ShortestPathData(InputGraph, BusNodeIter.Num, SelectedRailNode.Num, EPathFinderCostType::Length, 1);
		Util::PathFinder::FindShortestPath(ShortestPathData, ShortestRoute);
		if (ShortestRoute.at(0).Cost >= INFINITY)
		{
			printf("No Route\n");
			++NoRouteCount;
			if (NoRouteCount == CopiedBusNode.size())
			{
				OutputData.SelectedBusNode = CopiedBusNode.at(0);
				return false;
			}
			continue;
		}
		TotalRouteLength += ShortestRoute.at(0).Cost;
		FoundBusRouteMap.insert(make_pair(BusNodeIter.Num, ShortestRoute.at(0)));
		printf("From %lu to %lu, Shortest Path Length : %lf\n", BusNodeIter.Num, SelectedRailNode.Num, ShortestRoute.at(0).Cost);
	}

	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<float> dis(0.0f, 1.0f);
	float RandomNum = dis(gen);

	float CumulativeProbability = 0.0f;
	uint64_t SelectedBusNodeNum;
	bool bBusSelected = false;
	for (auto BusMapIter : FoundBusRouteMap)
	{
		float LowRange = CumulativeProbability;
		float HighRange = CumulativeProbability + (BusMapIter.second.Cost/TotalRouteLength);
		if  (RandomNum >= LowRange && RandomNum <HighRange)
		{
			SelectedBusNodeNum = BusMapIter.first;
			bBusSelected = true;
			break;
		}
		CumulativeProbability += (BusMapIter.second.Cost/TotalRouteLength);
	}

	SelectedBusNodeData SelectedData;
	if (bBusSelected)
	{
		for (auto BusNodeIter : CopiedBusNode)
		{
			if (BusNodeIter.Num == SelectedBusNodeNum)
			{
				OutputData.SelectedBusNode = BusNodeIter;
				break;
			}
		}

		OutputData.BusRouteData = FoundBusRouteMap.find(SelectedBusNodeNum)->second;

	}
		printf("Selected Bus Node Num: %d\n", SelectedBusNodeNum);
		return true;
}
#endif
