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
	const NodeData& GetNodeData(uint32_t NodeNum, const vector<NodeData>& Graph)
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

	typedef pair<float, uint32_t> CostNodeNumPair;
	float DijkstraAlgorithm(const PathFinderData& InData, Coordinate RemovedLink, vector<NodeData>& OutPath)
	{
		priority_queue<CostNodeNumPair, vector<CostNodeNumPair>, greater<CostNodeNumPair> > PriorityQueue;

		vector<float> Dist(InData.Graph.size() + 1, INFINITY);
		vector<uint32_t> Path;
		for (uint32_t i = 0; i < InData.Graph.size() + 1; ++i)
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
			int NodeNum = PriorityQueue.top().second;
			PriorityQueue.pop();

			vector<uint32_t> AdjNodeList;
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
							Cost = Link.Length / Link.Speed;
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

		stack<uint32_t> Stack;
		uint32_t Next = Path.at(InData.EndNodeNum);
		while (Next != 0)
		{
			Stack.push(Next);
			Next = Path[Next];
		}

		while (!Stack.empty())
		{
			uint32_t NodeNum = Stack.top();
			OutPath.emplace_back(GetNodeData(NodeNum, InData.Graph));
			Stack.pop();
		}
		OutPath.emplace_back(GetNodeData(InData.EndNodeNum, InData.Graph));

		return Dist.at(InData.EndNodeNum);
	}
}

// todo. need NumberOfPath > 2 case handling
size_t Util::PathFinder::FindShortestPath(const PathFinderData& InData, vector<ShortestPathData>& OutPath)
{
	// 1. find shortest path with Dijkstra Algorithm.
	Coordinate RemovedLink(InData.StartNodeNum, InData.StartNodeNum);
	ShortestPathData FirstPathData;
	FirstPathData.Cost = PathFinderPrivate::DijkstraAlgorithm(InData, RemovedLink, FirstPathData.Path);
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
	for (uint32_t i = 0; i < FirstPathData.Path.size() - 1; ++i)
	{
		FinderData.StartNodeNum = FirstPathData.Path.at(i).Num;
		FinderData.EndNodeNum = FirstPathData.Path.at(i + 1).Num;

		Coordinate RemovedLink(FinderData.StartNodeNum, FinderData.EndNodeNum);
		vector<NodeData> NewPathList;
		float NewPathCost = PathFinderPrivate::DijkstraAlgorithm(FinderData, RemovedLink, NewPathList);

		float RemovedLinkCost = 0.f;
		for (auto Link : LinkDataList)
		{
			if (Link.FromNodeNum == FinderData.StartNodeNum && Link.ToNodeNum == FinderData.EndNodeNum)
			{
				if (InData.CostType == EPathFinderCostType::Duration)
					RemovedLinkCost = Link.Length / Link.Speed;
				else if (InData.CostType == EPathFinderCostType::Length)
					RemovedLinkCost = Link.Length;
			}
		}
		NewPathCost += (FirstPathData.Cost - RemovedLinkCost);
		
		vector<NodeData> CompletePath;
		for (uint32_t j = 0; j < i; ++j)	// add previous path
		{
			CompletePath.emplace_back(FirstPathData.Path.at(j));
		}
		
		for (auto NewPath : NewPathList)	// add new path
		{
			CompletePath.emplace_back(NewPath);
		}

		if (i + 2 < FirstPathData.Path.size())	// add post path
		{
			for (uint32_t j = i + 2; j < FirstPathData.Path.size(); ++j)
			{
				CompletePath.emplace_back(FirstPathData.Path.at(j));
			}
		}

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