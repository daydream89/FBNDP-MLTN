#pragma once

#include "../Data/DataTypes.h"

using namespace std;

struct NodeData;

namespace Util
{
	namespace PathFinder
	{
		//using Yen's Algorithm (based on Dijkstra Algorithm)
		size_t FindShortestPath(const PathFinderData& InData, vector<ShortestPathData>& OutPath);
	}

	namespace Compare
	{

	}
}