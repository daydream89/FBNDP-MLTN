#include "Utils.h"

#include <algorithm>
#include <map>
#include <queue>
#include <stack>

#include "../Data/DataTypes.h"
#include "../Data/DataCenter.h"

namespace PathFinderPrivate
{
	void GetRouteNameFromNodesNum(const RouteMap& InRouteDataMap, uint64_t InFromNodeNum, uint64_t InToNodeNum, string& OutRouteName)
	{
		for (auto RoutePair : InRouteDataMap)
		{
			for (const auto& RouteOrderPair : RoutePair.second)
			{
				auto NextOrderPair = RoutePair.second.find(RouteOrderPair.first + 1);
				if (NextOrderPair == RoutePair.second.end())
					break;

				if (RouteOrderPair.second.Node == InFromNodeNum && NextOrderPair->second.Node == InToNodeNum)
				{
					OutRouteName = RoutePair.first;
				}
			}
		}
	}

	typedef pair<float, uint64_t> CostNodeNumPair;
	float DijkstraAlgorithm(const PathFinderData& InData, Coordinate RemovedLink, ShortestPathData& OutPathData)
	{
		priority_queue<CostNodeNumPair, vector<CostNodeNumPair>, greater<CostNodeNumPair> > PriorityQueue;

		uint64_t NodeCount = DataCenter::GetInstance() ? DataCenter::GetInstance()->GetNodeData().size() : InData.Graph.size();
		vector<float> Dist(NodeCount + 1, INFINITY);
		vector<uint64_t> Path;
		for (uint64_t i = 0; i < NodeCount + 1; ++i)
		{
			Path.emplace_back(0);
		}

		PriorityQueue.push(make_pair(0.f, InData.StartNodeNum));
		Dist.at(InData.StartNodeNum) = 0.f;

		while (!PriorityQueue.empty())
		{
			uint64_t NodeNum = PriorityQueue.top().second;
			PriorityQueue.pop();

			vector<uint64_t> AdjNodeList;
			for (const auto& Link : InData.GraphLink)
			{
				if (Link.FromNodeNum == NodeNum)
				{
					if (Util::PathFinder::GetNodeData(Link.ToNodeNum, InData.Graph).Num == -1)
						continue;

					if (Path.at(NodeNum) == Link.ToNodeNum)
						continue;

					bool bExist = false;
					for (auto Data : AdjNodeList)
					{
						if (Data == Link.ToNodeNum)
						{
							bExist = true;
							break;
						}
					}

					if (!bExist)
						AdjNodeList.emplace_back(Link.ToNodeNum);
				}
			}

			for (auto AdjNodeNum : AdjNodeList)
			{
				float Cost = -1.f;
				for (const auto& Link : InData.GraphLink)
				{
					if (Link.FromNodeNum == RemovedLink.X && Link.ToNodeNum == RemovedLink.Y)
						continue;

					if (Link.FromNodeNum == NodeNum && Link.ToNodeNum == AdjNodeNum)
					{
						float Distance = 0.f; string RouteName = "";// not use;
						if (InData.CostType == EPathFinderCostType::Duration)
							Cost = Util::Calculator::CalcIVTT(Link, InData.RouteDataMap, Distance, RouteName);
						else if (InData.CostType == EPathFinderCostType::Length)
							Cost = Link.Length;

						break;
					}
				}

				if (Cost < 0.f)
					continue;

				if (Dist.at(AdjNodeNum) > Dist.at(NodeNum) + Cost)
				{
					Dist.at(AdjNodeNum) = Dist.at(NodeNum) + Cost;
					Path.at(AdjNodeNum) = NodeNum;
					PriorityQueue.push(make_pair(Dist.at(AdjNodeNum), AdjNodeNum));
				}
			}
		}

		stack<uint64_t> Stack;
		uint64_t Next = Path.at(InData.EndNodeNum);
		while (Next != 0)
		{
			Stack.push(Next);
			Next = Path[Next];
		}

		while (!Stack.empty())
		{
			uint64_t NodeNum = Stack.top();
			OutPathData.Path.emplace_back(Util::PathFinder::GetNodeData(NodeNum, InData.Graph));
			Stack.pop();
		}
		OutPathData.Path.emplace_back(Util::PathFinder::GetNodeData(InData.EndNodeNum, InData.Graph));

		if (InData.CostType == EPathFinderCostType::Duration)
		{
			OutPathData.IVTT = Dist.at(InData.EndNodeNum);
			OutPathData.Transfer = Util::Calculator::CalcOVTTData(OutPathData.Path, InData.RouteDataMap);
			return OutPathData.IVTT + OutPathData.Transfer.OVTT;
		}

		return Dist.at(InData.EndNodeNum);
	}
}

// todo. need NumberOfPath > 2 case handling
size_t Util::PathFinder::FindShortestPath(PathFinderData& InData, vector<ShortestPathData>& OutPath)
{
	if (InData.GraphLink.size() == 0)
	{
		if (auto DataCenterInstance = DataCenter::GetInstance())
			InData.GraphLink = DataCenterInstance->GetLinkData();
	}

	// 1. find shortest path with Dijkstra Algorithm.
	Coordinate RemovedLink(InData.StartNodeNum, InData.StartNodeNum);
	ShortestPathData FirstPathData;
	FirstPathData.Order = 1;
	FirstPathData.Cost = PathFinderPrivate::DijkstraAlgorithm(InData, RemovedLink, FirstPathData);
	if (INFINITY <= FirstPathData.Cost)
	{
		return 0;
	}

	OutPath.emplace_back(FirstPathData);

	// if function caller wants only one shortest path, don't need to perform Yen's Algorithm.
	if (InData.NumberOfPath == 1)
	{
		return 1;
	}

	// 2. remove each link of shortest path, find the path between the nodes that have removed links.
	//	  perform all link in the shortest path.
	map<float, ShortestPathData> SecondPathCandidateMap;
	PathFinderData FinderData(InData);
	for (uint64_t i = 0; i < FirstPathData.Path.size() - 1; ++i)
	{
		FinderData.StartNodeNum = FirstPathData.Path.at(i).Num;
		FinderData.EndNodeNum = FirstPathData.Path.at(i + 1).Num;

		Coordinate RemovedLink(FinderData.StartNodeNum, FinderData.EndNodeNum);
		ShortestPathData NewPathData;
		float NewPathCost = PathFinderPrivate::DijkstraAlgorithm(FinderData, RemovedLink, NewPathData);
		if (INFINITY <= NewPathCost)
			continue;
		
		vector<NodeData> CompletePath;
		for (uint64_t j = 0; j < i; ++j)	// add previous path
		{
			CompletePath.emplace_back(FirstPathData.Path.at(j));
		}
		
		for (auto NewPath : NewPathData.Path)	// add new path
		{
			CompletePath.emplace_back(NewPath);
		}

		if (i + 2 < FirstPathData.Path.size())	// add post path
		{
			for (uint64_t j = i + 2; j < FirstPathData.Path.size(); ++j)
			{
				CompletePath.emplace_back(FirstPathData.Path.at(j));
			}
		}

		// recalculate Cost
		NewPathData.Path = CompletePath;
		NewPathData.IVTT = 0.f;
		NewPathData.Transfer.Clear();
		for (int i = 0; i < NewPathData.Path.size() - 1; ++i)
		{
			NodeData& CurNode = NewPathData.Path.at(i);
			NodeData& NextNode = NewPathData.Path.at(i + 1);
			for (const auto& Link : InData.GraphLink)
			{
				if (Link.FromNodeNum == CurNode.Num && Link.ToNodeNum == NextNode.Num)
				{
					float Distance = 0.f; string RouteName = ""; // not use;
					NewPathData.IVTT += Calculator::CalcIVTT(Link, InData.RouteDataMap, Distance, RouteName);
					break;
				}
			}
		}
		NewPathData.Transfer = Calculator::CalcOVTTData(NewPathData.Path, InData.RouteDataMap);
		NewPathData.Cost = NewPathData.IVTT + NewPathData.Transfer.OVTT;
		SecondPathCandidateMap.emplace(make_pair(NewPathCost, NewPathData));
	}

	// 3. save above result into map. (total cost & path pair)
	if (SecondPathCandidateMap.begin() != SecondPathCandidateMap.end())
	{
		SecondPathCandidateMap.begin()->second.Order = 2;
		OutPath.emplace_back(SecondPathCandidateMap.begin()->second);
	}

	return OutPath.size();
}

static NodeData TempNodeData;
const NodeData& Util::PathFinder::GetNodeData(uint64_t NodeNum, const vector<NodeData>& Graph)
{
	for (const auto& Node : Graph)
	{
		if (Node.Num == NodeNum)
		{
			return Node;
		}
	}

	return TempNodeData;
}

OperatingData* Util::PathFinder::GetOperatingData(const string& InRouteName)
{
	if (auto DataCenterInst = DataCenter::GetInstance())
	{
		auto OperatingData = DataCenterInst->GetOperatingData();
		auto Iter = OperatingData.find(InRouteName);
		if (Iter != OperatingData.end())
			return &Iter->second;
	}

	return nullptr;
}

float Util::Calculator::CalcIVTT(const LinkData& InLink, const RouteMap& InRouteMap, float& OutDistance, string& OutRouteName)
{
	float IVTT = InLink.Length;
	string RouteName = "";
	for (auto RoutePair : InRouteMap)
	{
		RouteName = "";
		float PreCumDistance = -1.f, PostCumDistance = -1.f;
		for (auto RouteOrderPair : RoutePair.second)
		{
			if (RouteOrderPair.second.Node == InLink.FromNodeNum)
			{
				RouteName = RoutePair.first;
				PreCumDistance = RouteOrderPair.second.CumDistance;
			}
			else
			{
				if (!RouteName.empty() && RouteOrderPair.second.Node == InLink.ToNodeNum)
				{
					PostCumDistance = RouteOrderPair.second.CumDistance;
					break;
				}
				else
				{
					RouteName = "";
					PreCumDistance = -1.f;
				}
			}
		}

		if (0.f <= PreCumDistance && 0.f <= PostCumDistance)
		{
			IVTT = PostCumDistance - PreCumDistance;
			break;
		}
	}

	OutRouteName = RouteName;
	OutDistance = IVTT;
	if (auto DataCenterInst = DataCenter::GetInstance())
	{
		auto OperatingDataMap = DataCenterInst->GetOperatingData();
		auto FoundData = OperatingDataMap.find(RouteName);
		if (FoundData != OperatingDataMap.end())
			IVTT /= FoundData->second.Speed;
		else if (RouteName.substr(0, 7) == TownBusStr)
			IVTT /= DataCenterInst->GetUserInputData().TownBusSpeed;
		else
			IVTT /= InLink.Speed;
	}

	return IVTT;
}

OVTTData Util::Calculator::CalcOVTTData(const vector<NodeData>& InPath, const RouteMap& InRouteMap)
{
	OVTTData ReturnData;

	auto DataCenterInst = DataCenter::GetInstance();
	if (!DataCenterInst)
	{
		return ReturnData;
	}

	if (2 <= InPath.size())	// add first wait time
	{
		string RouteName = "";
		PathFinderPrivate::GetRouteNameFromNodesNum(InRouteMap, InPath.at(0).Num, InPath.at(1).Num, RouteName);
		if (RouteName.substr(0, 7) == TownBusStr)
			ReturnData.InitialDispatchesPerHour = static_cast<float>(DataCenterInst->GetUserInputData().TownBusDispatchesPerHour);
		else
		{
			auto OperatingData = PathFinder::GetOperatingData(RouteName);
			ReturnData.InitialDispatchesPerHour = OperatingData ? static_cast<float>(OperatingData->Dispatch) : 0.f;
		}

		ReturnData.OVTT = Util::Converter::ConvertMinuteToHour(60.f / ReturnData.InitialDispatchesPerHour);
	}

	if (InPath.size() < 2)
	{
		return ReturnData;
	}

	string PreRouteName = "";
	string CurRouteName = "";
	PathFinderPrivate::GetRouteNameFromNodesNum(InRouteMap, InPath.at(0).Num, InPath.at(1).Num, PreRouteName);
	for (int i = 1; i < InPath.size() - 1; ++i)
	{
		uint64_t CurNodeNum = InPath.at(i).Num;
		uint64_t NextNodeNum = InPath.at(i + 1).Num;
		PathFinderPrivate::GetRouteNameFromNodesNum(InRouteMap, CurNodeNum, NextNodeNum, CurRouteName);
		if (PreRouteName == "")
			PreRouteName = CurRouteName;

		if (PreRouteName != CurRouteName)	// route changed!
		{
			uint64_t Dispatch = 0;
			if (CurRouteName.substr(0, 7) == TownBusStr)
				Dispatch = DataCenterInst->GetUserInputData().TownBusDispatchesPerHour;
			else
			{
				auto OperatingData = PathFinder::GetOperatingData(CurRouteName);
				Dispatch = OperatingData ? OperatingData->Dispatch : 0;
			}

			float WaitTime = Util::Converter::ConvertMinuteToHour(60.f / static_cast<float>(Dispatch));
			ReturnData.TransferList.emplace_back(TransferData(ETransportationType::Train, InPath.at(i).TransferTime, Dispatch));

			ReturnData.OVTT += WaitTime + Util::Converter::ConvertMinuteToHour(InPath.at(i).TransferTime);
			++ReturnData.CTPI;
			
			PreRouteName = CurRouteName;
			CurRouteName = "";
		}
	}

	return ReturnData;
}

void Util::Calculator::CalcNumOfPassengerPerRoute(vector<ShortestPathData>& InPath, const RouteMap& InRouteMap, map<string, uint32_t>& OutRouteCost)
{
	for (const auto& PathData : InPath)
	{
		auto Path = PathData.Path;
		if (Path.size() < 2)
			continue;

		map<string, uint32_t> RouteCostMap;
		for (int i = 0; i < Path.size() - 1; ++i)
		{
			string RouteName = "";
			uint64_t CurNodeNum = Path.at(i).Num;
			uint64_t NextNodeNum = Path.at(i + 1).Num;
			PathFinderPrivate::GetRouteNameFromNodesNum(InRouteMap, CurNodeNum, NextNodeNum, RouteName);
			if (RouteName.empty())
				continue;

			if (RouteCostMap.find(RouteName) == RouteCostMap.end())
				RouteCostMap.insert(make_pair(RouteName, PathData.TrafficVolumeForPath));
		}

		for (const auto& RouteCostPair : RouteCostMap)
		{
			auto FoundRoute = OutRouteCost.find(RouteCostPair.first);
			if (FoundRoute != OutRouteCost.end())
				FoundRoute->second += RouteCostPair.second;
			else
			{
				OutRouteCost.insert(RouteCostPair);
			}
		}
	}
}

bool Util::Compare::IsFloatEqual(float Value1, float Value2)
{
	return fabs(Value1 - Value2) < FLT_EPSILON;
}

float Util::Converter::ConvertMinuteToHour(float Minute)
{
	return Minute / 60.f;
}

int64_t Util::Converter::ConvertDoubleToIntegerRoundUp(double Value)
{
	auto IntValue = static_cast<int64_t>(Value);
	if (static_cast<double>(IntValue) < Value)
		IntValue++;

	return IntValue;
}

#include "../Chromosome/Chromosome.h"
#include "../Population/Population.h"
bool Util::FindBestChromosome(const Population& InPopulation, uint64_t& OutChromosomeNumber)
{
	uint64_t ChromosomeSize = InPopulation.GetCurrentChromosomeNum();
	if (ChromosomeSize < 1)
		return false;

	OutChromosomeNumber = 0;
	if (ChromosomeSize == 1)
		return true;

	for (uint64_t i = 1; i < ChromosomeSize; ++i)
	{
		auto ChromosomeData = InPopulation.GetChromosome(i);
		if (InPopulation.GetChromosome(OutChromosomeNumber).GetFitnessValue() < ChromosomeData.GetFitnessValue())
		{
			OutChromosomeNumber = i;
		}
	}

	return true;
}
bool Util::FindWorstChromosome(const Population& InPopulation, uint64_t& OutChromosomeNumber)
{
	uint64_t ChromosomeSize = InPopulation.GetCurrentChromosomeNum();
	if (ChromosomeSize < 1)
		return false;

	OutChromosomeNumber = 0;
	if (ChromosomeSize == 1)
		return true;

	for (uint64_t i = 1; i < ChromosomeSize; ++i)
	{
		auto ChromosomeData = InPopulation.GetChromosome(i);
		if (InPopulation.GetChromosome(OutChromosomeNumber).GetFitnessValue() > ChromosomeData.GetFitnessValue())
		{
			OutChromosomeNumber = i;
		}
	}

	return true;
}