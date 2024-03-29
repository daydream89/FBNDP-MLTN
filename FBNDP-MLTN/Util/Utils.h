#pragma once

#include "../Data/DataTypes.h"

using namespace std;

class Population;
class Chromosome;
struct NodeData;
struct TransferData;

namespace Util
{
	namespace PathFinder
	{
		//using Yen's Algorithm (based on Dijkstra Algorithm)
		size_t FindShortestPath(PathFinderData& InData, vector<ShortestPathData>& OutPath);

		const NodeData& GetNodeData(uint64_t NodeNum, const vector<NodeData>& Graph);
		OperatingData* GetOperatingData(const string& InRouteName);
	}

	namespace Calculator
	{
		float CalcIVTT(const LinkData& InLink, const RouteMap& InRouteMap, float& OutDistance, string& OutRouteName);
		OVTTData CalcOVTTData(const vector<NodeData>& InPath, const RouteMap& InRouteMap);
		void CalcNumOfPassengerPerRoute(vector<ShortestPathData>& InPath, const RouteMap& InRouteMap, map<string, RouteCostData>& OutRouteCost);
	}

	namespace Compare
	{
		bool IsFloatEqual(float Value1, float Value2);
	}

	namespace Converter
	{
		float ConvertMinuteToHour(float Minute);
		int64_t ConvertDoubleToIntegerRoundUp(double Value);
	}

	bool FindBestChromosome(const Population& InPopulation, uint64_t& OutChromosomeNumber);
	bool FindWorstChromosome(const Population& InPopulation, uint64_t& OutChromosomeNumber);
}