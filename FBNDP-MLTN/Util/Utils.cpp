#include "Utils.h"
#include <xtree>

size_t Util::PathFinder::FindShortestPath(const vector<uint32_t>& InGraph, vector<vector<LinkData>>& OutPath)
{
	//using Yen's Algorithm (based on Dijkstra Algorithm)

	// 1. find shortest path with Dijkstra Algorithm.

	// 2. remove each link of shortest path, find the path between the nodes that have removed links.
	//	  perform all link in the shortest path.

	// 3. save above result into map. (total cost & path pair)

	// 4. convert k-th result to vector<vector<LinkData>>.

	return 0;
}