#pragma once

#include "../Data/DataTypes.h"

using namespace std;

struct NodeData;

namespace Util
{
	namespace PathFinder
	{
		size_t FindShortestPath(const PathFinderData& InData, vector<vector<LinkData>>& OutPath);
	}

	namespace Compare
	{
		bool CompareLinkDataLength(const LinkData& LValue, const LinkData& RValue);
		bool CompareLinkDataDuration(const LinkData& LValue, const LinkData& RValue);
	}
}