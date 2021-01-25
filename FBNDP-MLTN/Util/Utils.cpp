#include "Utils.h"

#include <algorithm>
#include <map>
#include <queue>
#include <stack>

#include "../Data/DataTypes.h"
#include "../Data/DataCenter.h"

namespace PathFinderPrivate
{
	static NodeData TempNodeData;
	const NodeData& GetNodeData(uint64_t NodeNum, const vector<NodeData>& Graph)
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

	void GetRouteNameFromNodesNum(const RouteMap& InRouteDataMap, uint64_t InFromNodeNum, uint64_t InToNodeNum, string& OutRouteName)
	{
		for (auto RoutePair : InRouteDataMap)
		{
			for (auto RouteOrderPair : RoutePair.second)
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

		vector<LinkData> LinkDataList;
		if (auto* DataCenterInstance = DataCenter::GetInstance())
			LinkDataList = DataCenterInstance->GetLinkData();

		while (!PriorityQueue.empty())
		{
			uint64_t NodeNum = PriorityQueue.top().second;
			PriorityQueue.pop();

			vector<uint64_t> AdjNodeList;
			for (const auto& Link : LinkDataList)
			{
				if (Link.FromNodeNum == NodeNum)
				{
					if (GetNodeData(Link.ToNodeNum, InData.Graph).Num == -1)
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
				for (const auto& Link : LinkDataList)
				{
					if (Link.FromNodeNum == RemovedLink.X && Link.ToNodeNum == RemovedLink.Y)
						continue;

					if (Link.FromNodeNum == NodeNum && Link.ToNodeNum == AdjNodeNum)
					{
						if (InData.CostType == EPathFinderCostType::Duration)
							Cost = Util::Calculator::CalculateIVTT(Link);
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
			OutPathData.Path.emplace_back(GetNodeData(NodeNum, InData.Graph));
			Stack.pop();
		}
		OutPathData.Path.emplace_back(GetNodeData(InData.EndNodeNum, InData.Graph));

		if (InData.CostType == EPathFinderCostType::Duration)
		{
			OutPathData.IVTT = Dist.at(InData.EndNodeNum);
			OutPathData.OVTT = Util::Calculator::CalculateOVTT(OutPathData.Path, OutPathData.CTPI);
			return OutPathData.IVTT + OutPathData.OVTT;
		}

		return Dist.at(InData.EndNodeNum);
	}
}

// todo. need NumberOfPath > 2 case handling
size_t Util::PathFinder::FindShortestPath(const PathFinderData& InData, vector<ShortestPathData>& OutPath)
{
	// 1. find shortest path with Dijkstra Algorithm.
	Coordinate RemovedLink(InData.StartNodeNum, InData.StartNodeNum);
	ShortestPathData FirstPathData;
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
	auto LinkDataList = DataCenter::GetInstance()->GetLinkData();
	map<float, vector<NodeData>> SecondPathCandidateMap;
	PathFinderData FinderData(InData);
	for (uint64_t i = 0; i < FirstPathData.Path.size() - 1; ++i)
	{
		FinderData.StartNodeNum = FirstPathData.Path.at(i).Num;
		FinderData.EndNodeNum = FirstPathData.Path.at(i + 1).Num;

		Coordinate RemovedLink(FinderData.StartNodeNum, FinderData.EndNodeNum);
		ShortestPathData NewPathData;
		float NewPathCost = PathFinderPrivate::DijkstraAlgorithm(FinderData, RemovedLink, NewPathData);

		float RemovedLinkCost = 0.f;
		for (auto Link : LinkDataList)
		{
			if (Link.FromNodeNum == FinderData.StartNodeNum && Link.ToNodeNum == FinderData.EndNodeNum)
			{
				if (InData.CostType == EPathFinderCostType::Duration)
					RemovedLinkCost = Calculator::CalculateIVTT(Link);
				else if (InData.CostType == EPathFinderCostType::Length)
					RemovedLinkCost = Link.Length;
			}
		}
		NewPathCost += (FirstPathData.Cost - RemovedLinkCost);
		
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

		// NewPathCost 재계산 필요함.. 기존 Cost에 RemovedCost를 빼는 방식은 OVTT로 인해 문제 생길 가능성이 존재함.
		// 그냥 CompletePath를 기준으로 재계산 하는것이 정확함.
		SecondPathCandidateMap.emplace(make_pair(NewPathCost, CompletePath));
	}

	// 3. save above result into map. (total cost & path pair)
	if (SecondPathCandidateMap.begin() != SecondPathCandidateMap.end())
	{
		ShortestPathData SecondPathData;
		SecondPathData.Cost = SecondPathCandidateMap.begin()->first;
		SecondPathData.Path = SecondPathCandidateMap.begin()->second;
		OutPath.emplace_back(SecondPathData);
	}

	return OutPath.size();
}

float Util::Calculator::CalculateIVTT(const LinkData& Link)
{
	float IVTT = Link.Length / Link.Speed;
	if (auto DataCenterInst = DataCenter::GetInstance())
	{
		string RouteName = "";
		auto RouteDataMap = DataCenterInst->GetRouteData();
		for (auto RoutePair : RouteDataMap)
		{
			float PreCumDistance = -1.f, PostCumDistance = -1.f;
			for (auto RouteOrderPair : RoutePair.second)
			{
				if (RouteOrderPair.second.Node == Link.FromNodeNum)
				{
					RouteName = RoutePair.first;
					PreCumDistance = RouteOrderPair.second.CumDistance;
				}
				else
				{
					if (!RouteName.empty() && RouteOrderPair.second.Node == Link.ToNodeNum)
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

		auto OperatingDataMap = DataCenterInst->GetOperatingData();
		auto FoundData = OperatingDataMap.find(RouteName);
		if (FoundData != OperatingDataMap.end())
		{
			IVTT /= FoundData->second.Speed;
		}
		else if (!RouteName.empty())
			IVTT /= Link.Speed;
	}

	return IVTT;
}

float Util::Calculator::CalculateOVTT(const vector<NodeData>& InPath, uint16_t& OutTransferCount)
{
	auto DataCenterInst = DataCenter::GetInstance();
	if (!DataCenterInst)
	{
		return 0.f;
	}

	float OVTT = 0.f;
	if (1 < InPath.size())	// add first wait time
	{
		if (InPath.at(0).Type == NodeType::BusStop)
			OVTT = Util::Converter::ConvertMinuteToHour(1.f / static_cast<float>(DataCenterInst->GetUserInputData().BusDispatchesPerHour));
		else if (InPath.at(0).Type == NodeType::Station)
			OVTT = Util::Converter::ConvertMinuteToHour(1.f / static_cast<float>(DataCenterInst->GetUserInputData().TrainDispatchesPerHour));
	}

	string PreRouteName = "";
	string CurRouteName = "";
	for (int i = 1; i < InPath.size() - 1; ++i)
	{
		uint64_t CurNodeNum = InPath.at(i).Num;
		uint64_t NextNodeNum = InPath.at(i + 1).Num;
		PathFinderPrivate::GetRouteNameFromNodesNum(DataCenterInst->GetRouteData(), CurNodeNum, NextNodeNum, CurRouteName);
		if (PreRouteName == "")
			PreRouteName = CurRouteName;

		if (PreRouteName != CurRouteName)	// route changed!
		{
			float WaitTime = 0.f;
			if (InPath.at(i).Type == NodeType::BusStop)
				WaitTime = Util::Converter::ConvertMinuteToHour(1.f / static_cast<float>(DataCenterInst->GetUserInputData().BusDispatchesPerHour));
			else if (InPath.at(i).Type == NodeType::Station)
				WaitTime = Util::Converter::ConvertMinuteToHour(1.f / static_cast<float>(DataCenterInst->GetUserInputData().TrainDispatchesPerHour));

			OVTT += WaitTime + InPath.at(i).TransferTime;
			++OutTransferCount;
		}
	}

	return OVTT;
}

bool Util::Compare::IsFloatEqual(float Value1, float Value2)
{
	return fabs(Value1 - Value2) < FLT_EPSILON;
}

float Util::Converter::ConvertMinuteToHour(float Minute)
{
	return Minute / 60.f;
}