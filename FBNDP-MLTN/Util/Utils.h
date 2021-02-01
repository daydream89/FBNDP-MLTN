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

		const NodeData& GetNodeData(uint64_t NodeNum, const vector<NodeData>& Graph);
	}

	namespace Calculator
	{
		float CalculateIVTT(const LinkData& InLink, float& OutDistance);
		float CalculateOVTT(const vector<NodeData>& InPath, uint16_t& OutTransferCount);
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