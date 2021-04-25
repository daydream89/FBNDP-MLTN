#include "Population.h"
#include "../util/Utils.h"
#include "../Data/DataCenter.h"
#include <cassert>
#include <random>
#include <fstream>
#include <list>
#include <set>

Population::Population(uint64_t MemberNum)
{
	MaxChromosomeNum = MemberNum;
	SetNodes();
	if (auto DataCenterInstance = DataCenter::GetInstance())
	{
		UserInputMaxRouteLength = DataCenterInstance->GetUserInputData().MaxRouteLength / 2;
		if (CheckMaxRouteLengthLimit() == false)
		{
			printf("Some Route longer than User Input Max Length - Program Ends\n");
			getchar();
			exit(-1);
		}
		for (uint64_t i = 0; i < MemberNum; ++i)
		{
			ChromosomeArray.emplace_back(Chromosome(RailNode, BusNode, TownBusNode));
			DataCenterInstance->AddTownBusRouteData(ChromosomeArray.at(i).GetRoute());
		}
	}
	assert(GetCurrentChromosomeNum() == MemberNum);
	printf("All Initial Population Created\n");
//	PrintCurrentPopulationData();
}
bool Population::CheckMaxRouteLengthLimit()
{
	float MaxRouteLength = 0.0f;
	
	for (const auto& TownBusNodeIter : TownBusNode)
	{
		float ShortestRouteLength = INFINITY;
		for (vector<NodeData>::iterator RailNodeIter = RailNode.begin(); RailNodeIter != RailNode.end(); RailNodeIter++)
		{
			vector<NodeData> InputGraph;
			InputGraph.assign(BusNode.begin(), BusNode.end());
			InputGraph.emplace_back(*RailNodeIter);
			vector<ShortestPathData> ShortestRoute;
			PathFinderData ShortestPathData(InputGraph, TownBusNodeIter.Num, RailNodeIter->Num, EPathFinderCostType::Length, 1);
			if (Util::PathFinder::FindShortestPath(ShortestPathData, ShortestRoute) == 0)
			{
				printf("No Route from BusNode(%llu) to RailNode(%llu)\n", TownBusNodeIter.Num, RailNodeIter->Num);
				continue;
			}
			if (ShortestRouteLength > ShortestRoute.at(0).Cost)
			{
				ShortestRouteLength = ShortestRoute.at(0).Cost;
				/*
				for (const auto& RouteNodeIter : ShortestRoute.at(0).Path)
				{
					printf("%llu ", RouteNodeIter.Num);
				}
				printf(" Current Shortest route(length: %f)\n", ShortestRoute.at(0).Cost);
				*/
			}
		}
		if (MaxRouteLength < ShortestRouteLength)
		{
			MaxRouteLength = ShortestRouteLength;
		}
	}
	printf(" Current MaxRouteLegnth: %f\n\n", MaxRouteLength);
	if (MaxRouteLength <= UserInputMaxRouteLength)
		return true;
	else
		return false;
}
void Population::GetNextGeneration()
{
	
	if (auto DataCenterInstance = DataCenter::GetInstance())
	{
		Selection();

		for (uint64_t i = 0; i < SelectionCompair.size(); ++i)
		{
			Crossover(SelectionCompair.at(i).first, SelectionCompair.at(i).second);
		}

		for (uint64_t i = DataCenterInstance->GetUserInputData().NoCrossoverNum; i < ChildrenChromosomeArray.size(); ++i)
		{
			Mutation(ChildrenChromosomeArray.at(i));
		}

		/*Print Parent Chromosomes and make F1 to Parents, F1 must be cleared*/
		ChromosomeArray.clear();
		ChromosomeArray.assign(ChildrenChromosomeArray.begin(), ChildrenChromosomeArray.end());
		ChildrenChromosomeArray.clear();
		DataCenterInstance->ClearTownBusRouteData();
		for (uint64_t i = 0; i < MaxChromosomeNum; ++i)
		{
			DataCenterInstance->AddTownBusRouteData(ChromosomeArray.at(i).GetRoute());
		}
	}
}
void Population::Selection()
{
	uint64_t CrossoverExceptionNum = 0;
	if (auto DataCenterInstance = DataCenter::GetInstance())
	{
		for (uint64_t i = 0; i < MaxChromosomeNum; ++i)
		{
			ChromosomeArray.at(i).GetRouteRef().clear();
			ChromosomeArray.at(i).GetRouteRef().assign(DataCenterInstance->GetChromosomeRoutesDataRef(i).begin(), DataCenterInstance->GetChromosomeRoutesDataRef(i).end());
			ChromosomeArray.at(i).SetRouteNum();
		}
		CrossoverExceptionNum = DataCenterInstance->GetUserInputData().NoCrossoverNum;
	}

	/*Find Crossover Exceptions..Large Fitness*/
	vector<uint64_t> CrossoverExceptPositions;
	for(uint64_t i = 0; i < CrossoverExceptionNum; ++i)
	{
		double MaxFitness = -INFINITY;
		uint64_t MaxChromosomePos = 0;
		for (uint64_t i = 0; i < ChromosomeArray.size(); ++i)
		{
			bool ContinueFlag = false;
			for (const auto& ExceptPositionIter : CrossoverExceptPositions)
			{
				if (ExceptPositionIter == i)
				{
					ContinueFlag = true;
					break;
				}
			}
			if (ContinueFlag)
				continue;
			if (ChromosomeArray.at(i).GetFitnessValue() > MaxFitness)
			{
				MaxFitness = ChromosomeArray.at(i).GetFitnessValue();
				MaxChromosomePos = i;
			}
		}
		CrossoverExceptPositions.emplace_back(MaxChromosomePos);
		ChildrenChromosomeArray.emplace_back(ChromosomeArray.at(MaxChromosomePos));
	}

	vector<Chromosome> SelectionChromosomeArray;
	for (uint64_t i = 0; i < ChromosomeArray.size(); ++i)
	{
		bool ExceptionFounded = false;
		for (const auto& ExceptPositionIter : CrossoverExceptPositions)
		{
			if (ExceptPositionIter == i)
			{
				ExceptionFounded = true;
				break;
			}
		}
		if (ExceptionFounded)
			continue;
		SelectionChromosomeArray.emplace_back(ChromosomeArray.at(i));
		
	}
	random_device rd;
	mt19937 gen(rd());

	vector<uint64_t> RandomOrder;
	for (uint64_t i = 0; i < ChromosomeArray.size() - CrossoverExceptionNum; ++i)
	{
		RandomOrder.emplace_back(i);
	}

	/* Make Random Order By Swapping vector*/
	uint64_t CurrentPos = 0;
	uint64_t LastMemberSwappedPosition;
	for (uint64_t i = 0; i < RandomOrder.size()-1; ++i)
	{
		uniform_int_distribution<int64_t> dis(CurrentPos + 1, static_cast<int64_t>(RandomOrder.size() - 1));
		int64_t RandomNum = dis(gen);
		if (RandomNum == RandomOrder.size() - 1)
			LastMemberSwappedPosition = i;
		uint64_t TempNum = RandomOrder.at(RandomNum);
		RandomOrder.at(RandomNum) = RandomOrder.at(i);
		RandomOrder.at(i) = TempNum;
		++CurrentPos;
	}

	SelectionCompair.clear();
	/*Make Chromosome Pair(Selection)*/
	for (uint64_t i = 0; i < SelectionChromosomeArray.size(); ++i)
	{
		SelectionCompair.emplace_back(make_pair(SelectionChromosomeArray.at(i), SelectionChromosomeArray.at(RandomOrder.at(i))));
	}
}


vector<uint64_t> Population::GetOverlappedNodeNum(Chromosome Parent)
{
	map<uint64_t, uint64_t> NodeCountMap;
	vector<uint64_t> OverlappedNodeNum;

	for (const auto& ChromosomeNodeIter : Parent.GetChromosome())
	{
		map<uint64_t, uint64_t>::iterator it;
		it = NodeCountMap.find(ChromosomeNodeIter.first.Num);
		if (it == NodeCountMap.end())
		{
			/*First Node Num - (not founded)*/
			NodeCountMap.insert(make_pair(ChromosomeNodeIter.first.Num, 1));
		}
		else
		{
			/*Alread Exist Node Num */
			++(it->second);
		}
	}
	for (const auto& ChromosomeNodeIter : Parent.GetChromosome())
	{
		uint64_t CurrentNodeNum = ChromosomeNodeIter.first.Num;
		map<uint64_t, uint64_t>::iterator it;
		it = NodeCountMap.find(CurrentNodeNum);
		if (it == NodeCountMap.end())
		{
			/*Node Num not exist - error*/
			assert(0);
		}
		else
		{
			OverlappedNodeNum.emplace_back(it->second);
		}
	}

	return OverlappedNodeNum;
}
void Population::Crossover(Chromosome P1, Chromosome P2)
{
	//vector<uint64_t> P1OverlappedNodeNum = GetOverlappedNodeNum(P1);
	//vector<uint64_t> P2OverlappedNodeNum = GetOverlappedNodeNum(P2);
	bool CrossoverOccured = false;
	if (auto* DataCenter = DataCenter::GetInstance())
	{
		uint64_t CrossoverOccurPercent = DataCenter->GetUserInputData().CrossoverPercent;
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<uint64_t> dis(1, 100);
		uint64_t RandomNum = dis(gen);
		if (RandomNum <= CrossoverOccurPercent)
			CrossoverOccured = true;
	}
	if (CrossoverOccured == false)
	{
		ChildrenChromosomeArray.emplace_back(P1);
		return;
	}

	uint64_t P1RouteNum = P1.GetRouteNum();
	uint64_t P2RouteNum = P2.GetRouteNum();

	vector<ShortestPathData> P1Routes = P1.GetRoute();
	vector<ShortestPathData> P2Routes = P2.GetRoute();

	uint64_t P1CurPosition = 0; 
	vector<ShortestPathData> NewPathData;
	ShortestPathData ShortestPath;
	while (P1CurPosition < P1.GetChromosome().size())//P1RouteNum != 0)
	{
		NodeData FirstNode = P1.GetChromosome().at(P1CurPosition).first;
		bool IsTownBusStop = false;
		for (const auto& TBNodesIter : TownBusNode)
		{
			if (TBNodesIter.Num == FirstNode.Num)
			{
				IsTownBusStop = true;
				break;
			}
		}
		if (FirstNode.Type == NodeType::Station || !IsTownBusStop)
		{
			++P1CurPosition;
			continue;
		}
		else /* Find the shortest path include first node and add to F1*/
		{
			/* Add FirstNode To F1 */
			ShortestPath.Path.emplace_back(FirstNode);

			set<uint64_t> P1FoundedNodePosSet;
			set<uint64_t> P2FoundedNodePosSet;
			set<uint64_t> TotalFoundedNodePosSet;
			uint64_t FoundedRouteNum = 0;
			uint64_t LastFoundedRouteNum = 0;
			vector<NodeData> FoundedNextNode;
			do {
				double MinRouteCost = INFINITY;
				uint64_t P1RouteStartPos = 0;
				for (const auto& PathDataIter : P1Routes) /*Find First Node in P1*/
				{
					bool NodeFoundFlag = false;
					bool FoundShortestPathFlag = false;
					uint64_t P1FoundedNodePos = P1RouteStartPos;
					for (const auto& NodeIter : PathDataIter.TownBusData.TownBusStopCheck)
					{
						if (FoundShortestPathFlag && NodeFoundFlag) /*Found Shortest Path include FirstNode */
						{
							FoundedNextNode.emplace_back(NodeIter.first);
							if (NodeIter.second == true)
							{
								if ((TotalFoundedNodePosSet.find(P1FoundedNodePos) != TotalFoundedNodePosSet.end() &&
									LastFoundedRouteNum == FoundedRouteNum)) /*Exist*/
								{
									++P1FoundedNodePos;
									continue;
								}
								else
								{
									P1FoundedNodePosSet.insert(P1FoundedNodePos);
									break;
								}
							}
							else
							{
								continue;
							}
						}
						if (!FoundShortestPathFlag && NodeFoundFlag) /*Found Including First Node Path, But Not Shortest Path*/
							break;
						if (NodeIter.first.Num == FirstNode.Num) //&& P1OverlappedNodeNum.at(P1FoundedNodePos) > 0)
						{
							NodeFoundFlag = true;
							if (MinRouteCost > PathDataIter.TownBusData.RouteCostPerPerson)
							{
								/*Find Route Include First Node*/
								MinRouteCost = PathDataIter.TownBusData.RouteCostPerPerson;
								//ShortestPath = PathDataIter;
								FoundShortestPathFlag = true;
								FoundedNextNode.clear();
								P1FoundedNodePosSet.clear();
								FoundedRouteNum = 1;
							}
						}
						++P1FoundedNodePos;
					}
					P1RouteStartPos += PathDataIter.Path.size();
				}
				uint64_t P2RouteStartPos = 0;
				for (const auto& PathDataIter : P2Routes) /*Find First Node in P2*/
				{
					bool NodeFoundFlag = false;
					bool FoundShortestPathFlag = false;
					uint64_t P2FoundedNodePos = P2RouteStartPos;
					for (const auto& NodeIter : PathDataIter.TownBusData.TownBusStopCheck)
					{
						if (FoundShortestPathFlag && NodeFoundFlag) /*Found Shortest Path include FirstNode */
						{
							FoundedNextNode.emplace_back(NodeIter.first);
							if (NodeIter.second == true)
							{
								if ((TotalFoundedNodePosSet.find(P2FoundedNodePos) != TotalFoundedNodePosSet.end() &&
									LastFoundedRouteNum == FoundedRouteNum)) /*Exist*/
								{
									++P2FoundedNodePos;
									continue;
								}
								else
								{
									P2FoundedNodePosSet.insert(P2FoundedNodePos);
									break;
								}
							}
							else
							{
								continue;
							}
						}
						if (!FoundShortestPathFlag && NodeFoundFlag) /*Found Including First Node Path, But Not Shortest Path*/
							break;
						if (NodeIter.first.Num == FirstNode.Num) //&& P2OverlappedNodeNum.at(P2FoundedNodePos) > 0)
						{
							NodeFoundFlag = true;
							if (MinRouteCost > PathDataIter.TownBusData.RouteCostPerPerson)
							{
								/*Find Route Include First Node*/
								MinRouteCost = PathDataIter.TownBusData.RouteCostPerPerson;
								//ShortestPath = PathDataIter;
								FoundShortestPathFlag = true;
								FoundedNextNode.clear();
								P2FoundedNodePosSet.clear();
								FoundedRouteNum = 2;
							}
						}
						++P2FoundedNodePos;
					}
					P2RouteStartPos += PathDataIter.Path.size();
				}
#if DEBUG_MODE
				printf("Founded Next Node num: %llu\n", FoundedNextNode.Num);
#endif

				/* add FoundedNextNode to F1 or F1's Routes Vector */
				//ShortestPath.Path.emplace_back(FoundedNextNode);
				ShortestPath.Path.insert(ShortestPath.Path.end(), FoundedNextNode.begin(), FoundedNextNode.end());

				FirstNode = FoundedNextNode.at(FoundedNextNode.size()-1);

				/*If FoundedNextNode is Rail Station, add RoutesData*/
				if (FoundedNextNode.at(FoundedNextNode.size()-1).Type == NodeType::Station)
				{
					NewPathData.emplace_back(ShortestPath);
					ShortestPath.Path.clear();
				}

				if (FoundedRouteNum == 1)
				{
					for (const auto& NodePosSet : P1FoundedNodePosSet)
					{
						TotalFoundedNodePosSet.insert(NodePosSet);
					}
					LastFoundedRouteNum = FoundedRouteNum;

				}
				else if (FoundedRouteNum == 2)
				{
					for (const auto& NodePosSet : P2FoundedNodePosSet)
					{
						TotalFoundedNodePosSet.insert(NodePosSet);
					}

					LastFoundedRouteNum = FoundedRouteNum;
				}
			} while (FoundedNextNode.at(FoundedNextNode.size()-1).Type != NodeType::Station);
			++P1CurPosition;
			P1FoundedNodePosSet.clear();
			P2FoundedNodePosSet.clear();
			TotalFoundedNodePosSet.clear();
		}
	}
	Chromosome F1(RailNode, BusNode, TownBusNode, NewPathData);
	ChildrenChromosomeArray.emplace_back(F1);
}

void Population::Mutation(Chromosome& MutantCh)
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<uint64_t> dis(0, 100);
	uint64_t RandomNum = dis(gen);
	if (auto* DataCenter = DataCenter::GetInstance())
	{
		UserInputData UserInput;
		UserInput = DataCenter->GetUserInputData();
		if (RandomNum <= UserInput.MutationPercent)
		{
			/*: find random TownBusNode(i) in Route a */
			bool FindRandomNode = false;
			uint64_t FoundedRandomNodeNum = 0;
			uint64_t FirstTownBusNodePos = 0;
			uniform_int_distribution<uint64_t> PosDistribute(0, MutantCh.GetChromosomeRef().size() - 1);
			uint64_t FirstTownBusNodeRouteNum = 0;
			do {
				uint64_t RandomPos = PosDistribute(gen);
				if ((MutantCh.GetChromosomeRef().at(RandomPos).second == true) &&
					(MutantCh.GetChromosomeRef().at(RandomPos).first.Type == NodeType::BusStop))
				{
					FindRandomNode = true;
					FirstTownBusNodePos = RandomPos;
					FoundedRandomNodeNum = MutantCh.GetChromosomeRef().at(RandomPos).first.Num;
#if DEBUG_MODE
					printf("Founded First Random Node: %llu\n", FoundedRandomNodeNum);
#endif
					uint64_t ChromosomePos = 0;
					for (const auto& RouteIter : MutantCh.GetRouteRef())
					{
						ChromosomePos += RouteIter.Path.size();
						if (ChromosomePos > RandomPos) {
							break;
						}
						++FirstTownBusNodeRouteNum; /*Route a*/
					}
				}
			} while (!FindRandomNode);
#if DEBUG_MODE
			printf("FirstNode RouteNum: %llu\n", FirstTownBusNodeRouteNum);
#endif
			/* find nearest TownBusNode(j) from step 1 TownBusNode(i)*/
			map <uint64_t, ShortestPathData> BetweenTownBusShortestRoutes;
			for (auto BusNodeIter : TownBusNode)
			{
				if (BusNodeIter.Num == FoundedRandomNodeNum) /*same bus node, skip*/
					continue;

				vector<NodeData> InputGraph;
				vector<ShortestPathData> ShortestRoute;
				InputGraph.assign(BusNode.begin(), BusNode.end());
				PathFinderData ShortestPathData(InputGraph, FoundedRandomNodeNum, BusNodeIter.Num, EPathFinderCostType::Length, 1);
				if (Util::PathFinder::FindShortestPath(ShortestPathData, ShortestRoute) == 0)
				{
#if DEBUG_MODE
					printf("No Route\n");
#endif
					continue;
				}
				//printf("find shortest path from %llu to %llu\n", FoundedRandomNodeNum, BusNodeIter.Num);
				//printf("ShortestRoute.at(0).size(): %llu\n", ShortestRoute.at(0).Path.size());
				BetweenTownBusShortestRoutes.insert(make_pair(BusNodeIter.Num, ShortestRoute.at(0)));
				//printf("BetweenTownBusShortestRoutes size: %llu\n", BetweenTownBusShortestRoutes.size());
			}

			bool NearestNodeFounded = false;
			vector<uint64_t> NearestNodePosArray;
			map<uint64_t, uint64_t> NearestNodePosMap;
			uint64_t SecondTownBusNodePos;
			uint64_t SecondTownBusNodeRouteNum;
			do {
				if (BetweenTownBusShortestRoutes.size() == 0) {
					printf("Mutation fail..\n");
					return;
				}
					uint64_t NearestTownBusNodeNum = 0;
					SecondTownBusNodePos = 0;
					float MinCost = INFINITY;
					for (const auto& MapIter : BetweenTownBusShortestRoutes)
					{
						if (MapIter.second.Cost < MinCost)
						{
							NearestTownBusNodeNum = MapIter.first;
							MinCost = MapIter.second.Cost;
						}
					}

#if DEBUG_MODE
					printf("Founded Nearest TownBus Node: %llu\n", NearestTownBusNodeNum);
#endif
					uint64_t SecondSelectedNodeRouteNum = 0;
					for (const auto& RouteIter : MutantCh.GetRouteRef())
					{
						if (FirstTownBusNodeRouteNum == SecondSelectedNodeRouteNum) /*same route*/
						{
							SecondTownBusNodePos += RouteIter.Path.size();
							++SecondSelectedNodeRouteNum; /*Route b*/
							continue;
						}
						for (const auto& BusNodeIter : RouteIter.Path)
						{
							if (BusNodeIter.Num == NearestTownBusNodeNum)
							{
#if DEBUG_MODE
								printf("SecondPos NodeNum: %llu\n", MutantCh.GetChromosomeRef().at(SecondTownBusNodePos).first.Num);
								printf("SecondPos Route Num: %llu\n", SecondSelectedNodeRouteNum);
#endif
								NearestNodePosArray.emplace_back(SecondTownBusNodePos);
								NearestNodePosMap.insert(make_pair(SecondTownBusNodePos, SecondSelectedNodeRouteNum));
								NearestNodeFounded = true;
							}
							++SecondTownBusNodePos;
						}
						++SecondSelectedNodeRouteNum; /*Route b*/
					}

					if (NearestNodeFounded)
					{

						uniform_int_distribution<int64_t> NearestDis(0, NearestNodePosArray.size() - 1);
						int64_t RandomNodePos = NearestDis(gen);
						SecondTownBusNodePos = NearestNodePosArray.at(RandomNodePos);
						SecondTownBusNodeRouteNum = NearestNodePosMap.at(SecondTownBusNodePos);
					}

					if (NearestNodeFounded == false)
						BetweenTownBusShortestRoutes.erase(NearestTownBusNodeNum);
				} while (!NearestNodeFounded);
#if DEBUG_MODE
			printf("NodeNum(%llu) at NodePos(%llu) swapped NodeNum(%llu) at NodePos(%llu)\n",
				MutantCh.GetChromosomeRef().at(FirstTownBusNodePos).first.Num, FirstTownBusNodePos,
				MutantCh.GetChromosomeRef().at(SecondTownBusNodePos).first.Num, SecondTownBusNodePos);
			printf("First Node's RouteNum: %llu, Second Node's RouteNum: %llu\n", FirstTownBusNodeRouteNum, SecondTownBusNodeRouteNum);
#endif

			/* swap i and j in Chromosome*/
			pair<NodeData, bool> TempNodePair = MutantCh.GetChromosome().at(FirstTownBusNodePos);
			MutantCh.GetChromosomeRef().at(FirstTownBusNodePos) = MutantCh.GetChromosomeRef().at(SecondTownBusNodePos);
			MutantCh.GetChromosomeRef().at(SecondTownBusNodePos) = TempNodePair;

#if DEBUG_MODE
			printf("NodeNum(%llu) at NodePos(%llu) swapped NodeNum(%llu) at NodePos(%llu)\n",
				MutantCh.GetChromosomeRef().at(FirstTownBusNodePos).first.Num, FirstTownBusNodePos,
				MutantCh.GetChromosomeRef().at(SecondTownBusNodePos).first.Num, SecondTownBusNodePos);
#endif

			/* swap i and j in RouteData*/
			uint64_t FirstNodeRoutePos = FirstTownBusNodePos;
			uint64_t SecondNodeRoutePos = SecondTownBusNodePos;

			for (uint64_t i = 0; i < FirstTownBusNodeRouteNum; ++i)
			{
				FirstNodeRoutePos -= MutantCh.GetRouteRef().at(i).Path.size();
			}
			for (uint64_t i = 0; i < SecondTownBusNodeRouteNum; ++i)
			{
				SecondNodeRoutePos -= MutantCh.GetRouteRef().at(i).Path.size();
			}

			NodeData TempNode = MutantCh.GetRouteRef().at(FirstTownBusNodeRouteNum).Path.at(FirstNodeRoutePos);
			MutantCh.GetRouteRef().at(FirstTownBusNodeRouteNum).Path.at(FirstNodeRoutePos) = MutantCh.GetRouteRef().at(SecondTownBusNodeRouteNum).Path.at(SecondNodeRoutePos);
			MutantCh.GetRouteRef().at(SecondTownBusNodeRouteNum).Path.at(SecondNodeRoutePos) = TempNode;

			ConnectExchangedRoute(MutantCh, FirstTownBusNodeRouteNum, FirstNodeRoutePos, FirstTownBusNodePos);
			ConnectExchangedRoute(MutantCh, SecondTownBusNodeRouteNum, SecondNodeRoutePos, SecondTownBusNodePos);

			/*
			uint64_t SearchRouteNum = 0;
			for (const auto& RouteNodeIter : MutantCh.GetRouteRef())
			{
				set<uint64_t> NodeNumSet;
				uint64_t NodePos = 0;
				for (const auto NodeIter : RouteNodeIter.Path)
				{
					if (NodeNumSet.find(NodeIter.Num) != NodeNumSet.end())
					{
						printf("Set Founded\n");
						MutantCh.GetRouteRef().at(SearchRouteNum).Path.erase(MutantCh.GetRouteRef().at(SearchRouteNum).Path.begin() + NodePos);
						//RouteNodeIter.Path.erase(RouteNodeIter.Path.begin() + NodePos);
						//break;
					}
					else
					{
						NodeNumSet.insert(NodeIter.Num);
						NodePos++;
					}
				}
				SearchRouteNum++;

			}
			*/
			for (vector<ShortestPathData>::iterator RouteIter = MutantCh.GetRouteRef().begin(); RouteIter != MutantCh.GetRouteRef().end(); RouteIter++)
			{
				set<uint64_t> NodeNumSet;
				for (vector<NodeData>::iterator NodeIter = RouteIter->Path.begin(); NodeIter != RouteIter->Path.end();)
				{
					if (NodeNumSet.find(NodeIter->Num) != NodeNumSet.end())
					{
						//printf("Set Founded\n");
						NodeIter = RouteIter->Path.erase(NodeIter);
					}
					else
					{
						NodeNumSet.insert(NodeIter->Num);
						NodeIter++;
					}
				}
			}

			MutantCh.SetChromosomeFromRoute();



		}
		else
		{
			/* No Mutation*/
		}
	}
}
void Population::ConnectExchangedRoute(Chromosome& MutantCh, uint64_t RouteNum, uint64_t RoutePos, uint64_t ChromosomePos)
{
	uint64_t FirstNodeNum = MutantCh.GetRouteRef().at(RouteNum).Path.at(RoutePos).Num;
	uint64_t TempBeforeChromosomePos = ChromosomePos;
	if (RoutePos != 0)
	{
		NodeData BeforeNode;
		uint64_t TempNodePos = RoutePos;
		BeforeNode.Num = -1;
		while (TempNodePos)
		{
			--TempNodePos;
			--TempBeforeChromosomePos;
			if ((FirstNodeNum == MutantCh.GetRouteRef().at(RouteNum).Path.at(TempNodePos).Num))
			{
				MutantCh.GetRouteRef().at(RouteNum).Path.erase(MutantCh.GetRouteRef().at(RouteNum).Path.begin() + TempNodePos);
				RoutePos--;
			}
			else if ((MutantCh.GetRouteRef().at(RouteNum).Path.at(TempNodePos).Type == NodeType::BusStop) &&
				(MutantCh.GetChromosomeRef().at(TempBeforeChromosomePos).second == true) &&
				(FirstNodeNum != MutantCh.GetRouteRef().at(RouteNum).Path.at(TempNodePos).Num))
			{
				BeforeNode = MutantCh.GetRouteRef().at(RouteNum).Path.at(TempNodePos);
				break;
			}
		}

		if (BeforeNode.Num != -1)
		{
			vector<NodeData> InputGraph;
			vector<ShortestPathData> ShortestRoute;
			InputGraph.assign(BusNode.begin(), BusNode.end());
			PathFinderData ShortestPathData(InputGraph, BeforeNode.Num, FirstNodeNum, EPathFinderCostType::Length, 1);
			if (Util::PathFinder::FindShortestPath(ShortestPathData, ShortestRoute) != 0)
			{
				vector<NodeData> BeforeNodeRoute = ShortestRoute.at(0).Path;
				/*remove from TempNodePos to RoutePos*/
				/*
				vector<NodeData> CorrectRoute = MutantCh.GetRouteRef().at(RouteNum).Path;
				CorrectRoute.erase(CorrectRoute.begin() + TempNodePos, CorrectRoute.begin() + RoutePos + 1);
				CorrectRoute.insert(CorrectRoute.begin() + TempNodePos, ShortestRoute.at(0).Path.begin(), ShortestRoute.at(0).Path.end());
				*/
				MutantCh.GetRouteRef().at(RouteNum).Path.erase(MutantCh.GetRouteRef().at(RouteNum).Path.begin() + TempNodePos,
					MutantCh.GetRouteRef().at(RouteNum).Path.begin() + RoutePos + 1);
				//RoutePos -= (RoutePos - TempNodePos);
				//FirstTownBusNodePos -= (RoutePos - TempNodePos);
				MutantCh.GetRouteRef().at(RouteNum).Path.insert(MutantCh.GetRouteRef().at(RouteNum).Path.begin() + TempNodePos,
					BeforeNodeRoute.begin(), BeforeNodeRoute.end());
					//ShortestRoute.at(0).Path.begin(), ShortestRoute.at(0).Path.end());
				RoutePos += (ShortestRoute.at(0).Path.size() - (RoutePos - TempNodePos + 1));
			}
			else
			{
				fprintf(stderr, "No Route from %llu to %llud\n", BeforeNode.Num, FirstNodeNum);
			}

		}
	}

	NodeData NextNode;
	uint64_t TempNodePos = RoutePos;
	uint64_t TempAfterChromosomePos = ChromosomePos;
	/*
	uint64_t TempAfterChromosomePos2 = 0;
	for (int i = 0; i < RouteNum; ++i)
	{
		TempAfterChromosomePos2 += MutantCh.GetRouteRef().at(i).Path.size();
	}
	for (const auto& NodeIter : MutantCh.GetRouteRef().at(RouteNum).Path)
	{
		if (NodeIter.Num == FirstNodeNum) {
			break;
		}
		else {
		TempAfterChromosomePos2++;
		}
	}
	if (TempAfterChromosomePos != TempAfterChromosomePos2)
		printf("Test\n");
	*/
	NextNode.Num = -1;
	while (TempNodePos < MutantCh.GetRouteRef().at(RouteNum).Path.size() -1)
	{
		++TempNodePos;
		++TempAfterChromosomePos;
		//++TempAfterChromosomePos2;
		if ((FirstNodeNum == MutantCh.GetRouteRef().at(RouteNum).Path.at(TempNodePos).Num))
		{
			MutantCh.GetRouteRef().at(RouteNum).Path.erase(MutantCh.GetRouteRef().at(RouteNum).Path.begin() + TempNodePos);
		}
		else if ((MutantCh.GetChromosomeRef().at(TempAfterChromosomePos).second == true) &&
			(FirstNodeNum != MutantCh.GetRouteRef().at(RouteNum).Path.at(TempNodePos).Num))
		{
			NextNode = MutantCh.GetRouteRef().at(RouteNum).Path.at(TempNodePos);
			break;
		}
	}
	if (NextNode.Num == -1) {
		NextNode = MutantCh.GetRouteRef().at(RouteNum).Path.at(TempNodePos);
	}
	bool IsNextNodeRail = false;
	for (const auto& RailNodeIter : RailNode)
	{
		if (RailNodeIter.Num == NextNode.Num)
			IsNextNodeRail = true;
	}
	if (NextNode.Num != -1)
	{
		vector<NodeData> InputGraph;
		vector<ShortestPathData> ShortestRoute;
		InputGraph.assign(BusNode.begin(), BusNode.end());
		if (IsNextNodeRail)
			InputGraph.emplace_back(MutantCh.GetRouteRef().at(RouteNum).Path.at(MutantCh.GetRoute().at(RouteNum).Path.size() - 1));
		PathFinderData ShortestPathData(InputGraph, FirstNodeNum, NextNode.Num, EPathFinderCostType::Length, 1);
		if (Util::PathFinder::FindShortestPath(ShortestPathData, ShortestRoute) != 0)
		{
			vector<NodeData> AfterNodeRoute = ShortestRoute.at(0).Path;
			/*remove from TempNodePos to RoutePos*/
			/*
			vector<NodeData> CorrectRoute = MutantCh.GetRouteRef().at(RouteNum).Path;
			CorrectRoute.erase(CorrectRoute.begin() + TempNodePos, CorrectRoute.begin() + RoutePos + 1);
			CorrectRoute.insert(CorrectRoute.begin() + TempNodePos, ShortestRoute.at(0).Path.begin(), ShortestRoute.at(0).Path.end());
			*/
			MutantCh.GetRouteRef().at(RouteNum).Path.erase(MutantCh.GetRouteRef().at(RouteNum).Path.begin() + RoutePos,
				MutantCh.GetRouteRef().at(RouteNum).Path.begin() + TempNodePos + 1);
;
			//RoutePos -= (RoutePos - TempNodePos);
			//FirstTownBusNodePos -= (RoutePos - TempNodePos);
			MutantCh.GetRouteRef().at(RouteNum).Path.insert(MutantCh.GetRouteRef().at(RouteNum).Path.begin() + RoutePos,
				ShortestRoute.at(0).Path.begin(), ShortestRoute.at(0).Path.end());
			RoutePos += (ShortestRoute.at(0).Path.size() - (RoutePos - TempNodePos + 1));
		}
		else
		{
			fprintf(stderr, "No Route from %llu to %llud\n", NextNode.Num, FirstNodeNum);
		}

	}
}


void Population::SetNodes()
{
	if (auto* DataCenter = DataCenter::GetInstance())
	{
		UserInputData UserInput;
		UserInput = DataCenter->GetUserInputData();

		for (auto NodeIter : DataCenter->GetNodeData())
		{
			if (NodeIter.Type == NodeType::BusStop)
			{
				BusNode.emplace_back(NodeIter);
				for (uint64_t TownBusNum : UserInput.TownBusNodesNum)
				{
					if (TownBusNum == NodeIter.Num)
						TownBusNode.emplace_back(NodeIter);
				}
			}
			else if (NodeIter.Type == NodeType::Station)
				RailNode.emplace_back(NodeIter);
			else
				fprintf(stderr, "Unkonwn Node Type..?: %d\n", NodeIter.Type);
		}
	}
}

void Population::PrintCurrentPopulationData()
{
	string FilePath = "PopulationData.txt";
	ofstream WriteFile(FilePath.data());
	if (WriteFile.is_open())
	{
		uint64_t i = 0;
		for (auto& ChromosomeArrayIter : ChromosomeArray)
		{
			WriteFile << ++i << "th Chromosome: ";
			for (const auto& NodeArrayIter : ChromosomeArrayIter.GetChromosome())
			{
				WriteFile << NodeArrayIter.first.Num << " ";
			}
			WriteFile << "\n";
		}
	}
}
