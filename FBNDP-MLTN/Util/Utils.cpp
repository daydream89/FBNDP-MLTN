#include "Utils.h"

#include <algorithm>
#include <queue>

#include "../Data/DataTypes.h"
#include "../Data/DataCenter.h"

namespace PathFinderPrivate
{
	typedef pair<float, uint32_t> CostNodeNumPair;
	void DijkstraAlgorithm(const vector<NodeData>& InGraph, uint32_t InStart, uint32_t InEnd, PathFinderCostType InCostType, vector<LinkData>& OutPath)
	{
		// 추가로 OutPath 채워줘야 함....
		priority_queue<CostNodeNumPair, vector<CostNodeNumPair>, greater<CostNodeNumPair> > PriorityQueue;

		vector<float> dist(InGraph.size(), INFINITY);

		PriorityQueue.push(make_pair(0.f, InStart));
		dist[InStart] = 0.f;

		vector<LinkData> LinkDataList;
		if (auto* DataCenterInstance = DataCenter::GetInstance())
		{
			LinkDataList = DataCenterInstance->GetLinkData();
		}

		while (!PriorityQueue.empty())
		{
			int NodeNum = PriorityQueue.top().second;
			PriorityQueue.pop();

			vector<uint32_t> AdjNodeList;
			for (const auto& Link : LinkDataList)
			{
				if (Link.FromNodeNum == NodeNum)
				{
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
				float Cost = 0.f;
				for (const auto& Link : LinkDataList)
				{
					if (Link.FromNodeNum == NodeNum && Link.ToNodeNum == AdjNodeNum)
					{
						if (InCostType == PathFinderCostType::Duration)
							Cost = Link.Length / Link.Speed;
						else if (InCostType == PathFinderCostType::Length)
							Cost = Link.Length;

						break;
					}
				}

				if (dist[AdjNodeNum] > dist.at(NodeNum) + Cost)
				{
					dist[AdjNodeNum] = dist.at(NodeNum) + Cost;
					PriorityQueue.push(make_pair(dist[AdjNodeNum], AdjNodeNum));
				}
			}
		}
	}
}

size_t Util::PathFinder::FindShortestPath(const PathFinderData& InData, vector<vector<LinkData>>& OutPath)
{
	//using Yen's Algorithm (based on Dijkstra Algorithm)

	// 1. find shortest path with Dijkstra Algorithm.
	vector<LinkData> ShortestPath;
	PathFinderPrivate::DijkstraAlgorithm(InData.Graph, InData.StartNodeNum, InData.EndNodeNum, InData.CostType, ShortestPath);

	// 2. remove each link of shortest path, find the path between the nodes that have removed links.
	//	  perform all link in the shortest path.

	// 3. save above result into map. (total cost & path pair)

	// 4. convert k-th result to vector<vector<LinkData>>.

	return 0;
}

bool Util::Compare::CompareLinkDataLength(const LinkData& LValue, const LinkData& RValue)
{
	return LValue.Length < RValue.Length;
}

bool Util::Compare::CompareLinkDataDuration(const LinkData& LValue, const LinkData& RValue)
{
	return (LValue.Length / LValue.Speed) < (RValue.Length / RValue.Speed);
}