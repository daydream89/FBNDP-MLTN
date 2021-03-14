#include "Population.h"
#include "../Data/DataCenter.h"
#include <cassert>
#include <random>
#include <fstream>
#include <list>

Population::Population(uint64_t MemberNum)
{
	MaxChromosomeNum = MemberNum;
	SetNodes();
	if (auto DataCenterInstance = DataCenter::GetInstance())
	{
		for (uint64_t i = 0; i < MemberNum; ++i)
		{
			ChromosomeArray.emplace_back(Chromosome(RailNode, BusNode, TownBusNode));
			DataCenterInstance->AddTownBusRouteData(ChromosomeArray.at(i).GetRoute());
		}
	}
	assert(GetCurrentChromosomeNum() == MemberNum);
	printf("All Initial Population Created\n");
	PrintCurrentPopulationData();
}
void Population::GetNextGeneration()
{
	
	Selection();

	for (uint64_t i = 0; i < SelectionCompair.size(); ++i)
	{
		Crossover(SelectionCompair.at(i).first, SelectionCompair.at(i).second);
	}

	Mutation();

	/*TODO: Print Parent Chromosomes and make F1 to Parents, F1 must be cleared*/
	ChromosomeArray.clear();
	ChromosomeArray.assign(ChildrenChromosomeArray.begin(), ChildrenChromosomeArray.end());
	ChildrenChromosomeArray.clear();
	if (auto DataCenterInstance = DataCenter::GetInstance())
	{
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
								break;
							else
								continue;
						}
						if (!FoundShortestPathFlag && NodeFoundFlag) /*Found Including First Node Path, But Not Shortest Path*/
							break;
						if (NodeIter.first.Num == FirstNode.Num) //&& P1OverlappedNodeNum.at(P1FoundedNodePos) > 0)
						{
							NodeFoundFlag = true;
							if (MinRouteCost >= PathDataIter.TownBusData.RouteCostPerPerson)
							{
								/*Find Route Include First Node*/
								MinRouteCost = PathDataIter.TownBusData.RouteCostPerPerson;
								//ShortestPath = PathDataIter;
								FoundShortestPathFlag = true;
								FoundedNextNode.clear();
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
								break;
							else
								continue;
						}
						if (!FoundShortestPathFlag && NodeFoundFlag) /*Found Including First Node Path, But Not Shortest Path*/
							break;
						if (NodeIter.first.Num == FirstNode.Num) //&& P2OverlappedNodeNum.at(P2FoundedNodePos) > 0)
						{
							NodeFoundFlag = true;
							if (MinRouteCost >= PathDataIter.TownBusData.RouteCostPerPerson)
							{
								/*Find Route Include First Node*/
								MinRouteCost = PathDataIter.TownBusData.RouteCostPerPerson;
								//ShortestPath = PathDataIter;
								FoundShortestPathFlag = true;
								FoundedNextNode.clear();
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
			} while (FoundedNextNode.at(FoundedNextNode.size()-1).Type != NodeType::Station);
			++P1CurPosition;
		}
	}
	Chromosome F1(RailNode, BusNode, TownBusNode, NewPathData);
	ChildrenChromosomeArray.emplace_back(F1);
}

void Population::Mutation()
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int64_t> dis(1, 100);
	int64_t RandomNum = dis(gen);
	if (RandomNum <= 5) /*TODO: Get Mutation percent from User..*/
	{
	}
	else
	{
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
