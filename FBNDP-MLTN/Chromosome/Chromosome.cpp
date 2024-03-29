#include "Chromosome.h"
#include "../Data/DataCenter.h"
#include "../util/Utils.h"
#include <random>
#include <map>
#include <set>
#include <cassert>
#include <math.h>

#define DEBUG_MODE 0

Chromosome::Chromosome(const vector<NodeData>& RailNode, const vector<NodeData>& BusNode, const vector<NodeData>& TownBusNodes)
	:bAllRailStationHaveRoute(false),BusRouteNum(0)
{
	this->RailNode.assign(RailNode.begin(), RailNode.end());
	this->BusNode.assign(BusNode.begin(), BusNode.end());
	this->TownBusNode.assign(TownBusNodes.begin(), TownBusNodes.end());
	CopiedBusNode.assign(TownBusNode.begin(), TownBusNode.end());
	for (const auto& CheckRailNode : RailNode)
		RailStationSelected[CheckRailNode.Num] = false;
	if (auto DataCenterInstance = DataCenter::GetInstance()) {
		if (DataCenterInstance->GetUserInputData().PopulationGenerationMethod == 1)
			UserInputMaxRouteLength = DataCenterInstance->GetUserInputData().MaxRouteLength / 2;
		else
			UserInputMaxRouteLength = INFINITY;

		while (CopiedBusNode.size() > 0)
		{
			NodeData SelectedRailNode = SelectRailNode();
			SelectedBusNodeData SelectedBus = SelectBusNode(SelectedRailNode);
			if (!bAllRailStationHaveRoute)
			{
				if (DataCenterInstance->GetUserInputData().PopulationGenerationMethod == 1)
				{
					if (DataCenterInstance->GetUserInputData().PopulationGenerationMethod == 1)
					{
						if (SelectedBus.BusRouteData.Cost > UserInputMaxRouteLength)
						{
							printf("Current Route's Length is longer than User Input max Route Length\n");
							continue;
						}
					}
				}
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
				ShortestPathData FoundedShortestRoute;
				ShortestPathData ExistRoute;
				float MinRouteLength = INFINITY;
				for (auto RouteDataIter : RouteDataList)
				{
#if DEBUG_MODE
					printf("Rail Node Num: %llu\n", RouteDataIter.Path.at(RouteDataIter.Path.size() - 1).Num);
#endif
					if (RouteDataIter.Path.at(RouteDataIter.Path.size() - 1).Num == SelectedRailNode.Num) //Current Selected Rail Node Routes...
					{
						float RouteLength = 0;
						vector<NodeData> InputGraph;
						vector<ShortestPathData> ShortestRoute;
						InputGraph.assign(CopiedBusNode.begin(), CopiedBusNode.end());
						//InputGraph.assign(CopiedBusNode.begin(), CopiedBusNode.end());
						InputGraph.emplace_back(RouteDataIter.Path.at(0));
						PathFinderData ShortestPathData(InputGraph, SelectedBus.SelectedBusNode.Num, RouteDataIter.Path.begin()->Num, EPathFinderCostType::Length, 1);
						if (Util::PathFinder::FindShortestPath(ShortestPathData, ShortestRoute) != 0)
						{
							RouteLength = ShortestRoute.at(0).Cost;
							if (RouteLength <= MinRouteLength)
							{
								MinRouteLength = RouteLength;
								FoundedShortestRoute.Path.clear();
								FoundedShortestRoute.Path.assign(ShortestRoute.begin()->Path.begin(), ShortestRoute.begin()->Path.end());
								FoundedShortestRoute.Cost = RouteLength;
								ExistRoute = RouteDataIter;
							}
						}
					}
				}
				/* SelectedBus.BusRouteData.Cost vs Shortest Cost include exist route */
				if ((FoundedShortestRoute.Cost < SelectedBus.BusRouteData.Cost) && FoundedShortestRoute.Cost != 0)// || FoundedShortestRoute.Cost >= INFINITY) /* Find Lesat Cost Path */
				{
					bool MaxLengthCheckFlag = false;
					for (auto& RouteIter : RouteDataList)
					{
						if (RouteIter.Path.begin()->Num == ExistRoute.Path.begin()->Num)
						{
							if (DataCenterInstance->GetUserInputData().PopulationGenerationMethod == 1)
							{
								if ((static_cast<double>(RouteIter.Cost) + static_cast<double>(FoundedShortestRoute.Cost)) > UserInputMaxRouteLength)
								{
									printf("Route %llu - %llu -%llu is too long(%f)\n", FoundedShortestRoute.Path.front().Num,
										FoundedShortestRoute.Path.back().Num, RouteIter.Path.back().Num,
										(static_cast<double>(RouteIter.Cost) + static_cast<double>(FoundedShortestRoute.Cost)));
									MaxLengthCheckFlag = true;
									break;
								}
							}
							RouteIter.Path.insert(RouteIter.Path.begin(), FoundedShortestRoute.Path.begin(), FoundedShortestRoute.Path.end() - 1);
							RouteIter.Cost += FoundedShortestRoute.Cost;
						}
					}
					if (MaxLengthCheckFlag)
						continue;
				}
				else
				{
					if (DataCenterInstance->GetUserInputData().PopulationGenerationMethod == 1)
					{
						if (SelectedBus.BusRouteData.Cost > UserInputMaxRouteLength)
						{
							printf("Route from %llu to %llu is too long(Length: %f)\n", SelectedBus.BusRouteData.Path.front().Num,
								SelectedBus.BusRouteData.Path.back().Num, SelectedBus.BusRouteData.Cost);
							continue;
						}
					}
					++BusRouteNum; //k = k+1
					ShortestPathData RoutePathData;
					RoutePathData.Path.assign(SelectedBus.BusRouteData.Path.begin(), SelectedBus.BusRouteData.Path.end());
					RoutePathData.Cost = SelectedBus.BusRouteData.Cost;
					RouteDataList.emplace_back(RoutePathData);
				}

			}

			/*Delete Selecte Bus Nodes*/
			bool break_flag = false;
			uint64_t SelectedBusNodeNum = SelectedBus.BusRouteData.Path.at(0).Num;
			for (vector<NodeData>::const_iterator NodeIter = CopiedBusNode.begin(); NodeIter != CopiedBusNode.end();) //B' = \i
			{
				if (NodeIter->Num == SelectedBusNodeNum)
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

	RemoveOverlapedRoute();
	SetChromosomeFromRoute();
}

Chromosome::Chromosome(const vector<NodeData>& RailNode, const vector<NodeData>& BusNode, const vector<NodeData>& TownBusNodes, vector<ShortestPathData> NewPath)
{
	this->RailNode.assign(RailNode.begin(), RailNode.end());
	this->BusNode.assign(BusNode.begin(), BusNode.end());
	this->TownBusNode.assign(TownBusNodes.begin(), TownBusNodes.end());
	RouteDataList.clear();
	RouteDataList.assign(NewPath.begin(), NewPath.end());

	RemoveOverlapedRoute();
	ConnectDisconnectedNodes();
	SetChromosomeFromRoute();
}
void Chromosome::ConnectDisconnectedNodes(void)
{
	vector<NodeData> InputGraph;
	InputGraph.assign(BusNode.begin(), BusNode.end());
	InputGraph.insert(InputGraph.end(), RailNode.begin(), RailNode.end());

	for (auto& RouteIter : RouteDataList) {
		for (uint64_t NodePos = 0; NodePos < RouteIter.Path.size() - 1; ++NodePos)
		{
			if (NodePos == (RouteIter.Path.size() - 1))
				break;

			vector<ShortestPathData> ShortestRoute;
			PathFinderData ShortestPathData(InputGraph, RouteIter.Path.at(NodePos).Num, RouteIter.Path.at(NodePos+1).Num, EPathFinderCostType::Length, 1);
			if (Util::PathFinder::FindShortestPath(ShortestPathData, ShortestRoute) == 0)
			{
				printf("No Route from Node %llu to %llu\n", RouteIter.Path.at(NodePos).Num, RouteIter.Path.at(NodePos+1).Num);
			}
			if (ShortestRoute.at(0).Path.size() > 2)
			{
				RouteIter.Path.erase(RouteIter.Path.begin() + NodePos, RouteIter.Path.begin() + NodePos + 2);
				RouteIter.Path.insert(RouteIter.Path.begin() + NodePos, ShortestRoute.at(0).Path.begin(), ShortestRoute.at(0).Path.end());
			}
		}
	}
}

void Chromosome::SetChromosomeFromRoute(void)
{
	ChromosomeNodeList.clear();
	for (auto& RouteIter : RouteDataList)
	{
		set<uint64_t> CheckDuplicatedNode;
		RouteIter.TownBusData.TownBusStopCheck.clear();

		for (const auto& PathNodes : RouteIter.Path)
		{
			bool IsTownBusStop = false;
			if (PathNodes.Type == NodeType::BusStop)
			{
				for (const auto& TBNodesIter : TownBusNode)
				{
					if (TBNodesIter.Num == PathNodes.Num)
					{
						if (CheckDuplicatedNode.find(PathNodes.Num) == CheckDuplicatedNode.end()) /* Not Exist in Set */
						{
							CheckDuplicatedNode.insert(PathNodes.Num);
							IsTownBusStop = true;
						}
						else
						{
							IsTownBusStop = false;
						}
						break;
					}
				}
				if (IsTownBusStop)
				{
					ChromosomeNodeList.emplace_back(make_pair(PathNodes, true));
					RouteIter.TownBusData.TownBusStopCheck.emplace_back(make_pair(PathNodes, true));
				}
				else
				{
					ChromosomeNodeList.emplace_back(make_pair(PathNodes, false));
					RouteIter.TownBusData.TownBusStopCheck.emplace_back(make_pair(PathNodes, false));
				}
			}
			else if (PathNodes.Type == NodeType::Station)
			{
				ChromosomeNodeList.emplace_back(make_pair(PathNodes, true));
				RouteIter.TownBusData.TownBusStopCheck.emplace_back(make_pair(PathNodes, true));
			}
		}
	}
}

void Chromosome::RemoveSameRoute(void)
{
	for (int i = 0; i < RouteDataList.size() - 1; ++i)
	{
		vector<NodeData> CheckingRoute = RouteDataList.at(i).Path;
		for (int j = i + 1; j < RouteDataList.size(); ++j)
		{
			bool SameRouteFlag = true;
			vector<NodeData> CurrentRoute = RouteDataList.at(j).Path;
			if (CheckingRoute.size() != CurrentRoute.size())
			{
#if DEBUG_MODE
				printf("Route length is different, Not same Route\n");
#endif
				continue;
			}
			for (int RoutePos = 0; RoutePos < CheckingRoute.size(); ++RoutePos)
			{
				if (CheckingRoute.at(RoutePos).Num != CurrentRoute.at(RoutePos).Num)
				{
#if DEBUG_MODE
				printf("Not same Route\n");
#endif
					SameRouteFlag = false;
					break;
				}
			}
			if (SameRouteFlag) {
				RouteDataList.erase(RouteDataList.begin() + j);
				j = i;
			}
		}
	}
}

void Chromosome::RemoveOverlapedRoute(void)
{
	RemoveSameRoute();
	vector<ShortestPathData> OverlapRemovedRoute;

	int Count1 = 0;
	int Count2 = 0;
	for (const auto& CheckOverlapRoute : RouteDataList)
	{
		bool RouteInputFlag = true;
		++Count1;
		map<uint64_t, bool> NodeCheckMap;
		NodeCheckMap.clear();
		for (const auto& MapInsert : CheckOverlapRoute.Path)
		{
			NodeCheckMap.insert(make_pair(MapInsert.Num, false));
		}
		for (const auto& CurrentCheckRoute : RouteDataList)
		{
			Count2++;
			if (Count1 == Count2)
			{
#if DEBUG_MODE
				printf("same Route\n");
#endif
				continue;
			}
			else if (CheckOverlapRoute.Cost > CurrentCheckRoute.Cost)
			{
#if DEBUG_MODE
				printf("Current Route is shorter than CheckOverlapRoute\n");
#endif
				continue;
			}
			else if (CheckOverlapRoute.Path.at(CheckOverlapRoute.Path.size() - 1).Num != CurrentCheckRoute.Path.at(CurrentCheckRoute.Path.size() - 1).Num)
			{
#if DEBUG_MODE
				printf("Route's Rail Station is not same - no overlap\n");
#endif
				continue;
			}
			/* check Overlapped Path...*/
			for (const auto& PathIter : CurrentCheckRoute.Path)
			{
				map<uint64_t, bool>::iterator it;
				it = NodeCheckMap.find(PathIter.Num);
				if (it != NodeCheckMap.end())
				{
#if DEBUG_MODE
					printf("NodeNum is founded: %llu\n", it->first);
#endif
					it->second = true;
				}
			}
			bool AllNodeInFlag = true;
			for (const auto& MapIter : NodeCheckMap)
			{
				if (MapIter.second == false)
				{
					AllNodeInFlag = false;
					break;
				}
			}
			if (AllNodeInFlag) //Exist Route..Not insert to vector
			{
				RouteInputFlag = false;
				break;
			}

		}
		Count2 = 0;
		if (RouteInputFlag)
		{
			OverlapRemovedRoute.emplace_back(CheckOverlapRoute);
		}
	}

	RouteDataList.clear();
	RouteDataList.assign(OverlapRemovedRoute.begin(), OverlapRemovedRoute.end());
	BusRouteNum = RouteDataList.size();

}


NodeData Chromosome::SelectRailNode()
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int64_t> dis(0, static_cast<int64_t>(RailNode.size()-1));
	int64_t RandomNum = dis(gen);

	return RailNode.at(RandomNum);
}

#if 1
SelectedBusNodeData Chromosome::SelectBusNode(const NodeData& SelectedRailNode)
{
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
		if(Util::PathFinder::FindShortestPath(ShortestPathData, ShortestRoute) == 0)
		{
#if DEBUG_MODE
			printf("No Route\n");
#endif
			continue;
		}
		MaxRouteLength = max(MaxRouteLength, ShortestRoute.at(0).Cost);
		MinRouteLength = min(MinRouteLength, ShortestRoute.at(0).Cost);
		TotalRouteLength += ShortestRoute.at(0).Cost;
		FoundBusRouteMap.insert(make_pair(BusNodeIter.Num, ShortestRoute.at(0)));
		CalculatePercentMap.insert(make_pair(BusNodeIter.Num, ShortestRoute.at(0).Cost));
#if DEBUG_MODE
		printf("From %llu to %llu, Shortest Path Length : %lf\n", BusNodeIter.Num, SelectedRailNode.Num, ShortestRoute.at(0).Cost);
#endif
	}
#if DEBUG_MODE
	printf("Max Route Length: %lf\n", MaxRouteLength);
	printf("Min Route Length: %lf\n", MinRouteLength);
#endif
	float MaxPlusMinLength = MaxRouteLength + MinRouteLength;

	float TotalLengthDiff = 0.0f;
	for (auto& PercentMapIter : CalculatePercentMap)
	{
		PercentMapIter.second = (powf(2.7f, MaxPlusMinLength - PercentMapIter.second) * 0.0001f);
		TotalLengthDiff += PercentMapIter.second;
	}

	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<float> dis(0.0f, 1.0f);
	float RandomNum = dis(gen);
#if DEBUG_MODE
	printf("Created Random Num(0.0 ~ 1.0): %f\n", RandomNum);
#endif

	float CumulativeProbability = 0.0f;
	uint64_t SelectedBusNodeNum = UINT64_MAX;
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
#if DEBUG_MODE
		printf("Bus doesn't selected..error\n");
#endif

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
#if DEBUG_MODE
	printf("Selected Bus Node Num: %llu\n", SelectedBusNodeNum);
#endif
	return SelectedData;
}
#endif
