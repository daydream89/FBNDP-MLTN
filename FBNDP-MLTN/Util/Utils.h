#pragma once

#include "../Data/DataTypes.h"

using namespace std;

namespace Util
{
	namespace PathFinder
	{
		size_t FindShortestPath(const vector<uint32_t>& InGraph, vector<vector<LinkData>>& OutPath);
	}

	namespace Compare
	{

	}
}