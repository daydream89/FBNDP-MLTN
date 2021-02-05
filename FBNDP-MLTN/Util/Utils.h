#pragma once

#include "../Data/DataTypes.h"

using namespace std;

struct NodeData;
struct TransferData;

namespace Util
{
	namespace PathFinder
	{
		//using Yen's Algorithm (based on Dijkstra Algorithm)
		size_t FindShortestPath(PathFinderData& InData, vector<ShortestPathData>& OutPath);

		const NodeData& GetNodeData(uint64_t NodeNum, const vector<NodeData>& Graph);
	}

	namespace Calculator
	{
		float CalcIVTT(const LinkData& InLink, const RouteMap& InRouteMap, float& OutDistance, string& OutRouteName);
		TransferData CalcTransferData(const vector<NodeData>& InPath, const RouteMap& InRouteMap);
	}

	namespace Compare
	{
		bool IsFloatEqual(float Value1, float Value2);
	}

	namespace Converter
	{
		float ConvertMinuteToHour(float Minute);
	}
}