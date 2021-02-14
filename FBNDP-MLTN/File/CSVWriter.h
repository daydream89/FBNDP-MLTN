#pragma once

#include "../Data/DataTypes.h"

using namespace std;

struct NodeData;

class CSVWriter
{
public:
	CSVWriter();

	void WriteCSVFile(uint64_t GenerationNumber, const vector<vector<ShortestPathData>>& InTownBusRouteDataList);
	void WriteCSVFile(uint64_t GenerationNumber, int ChromosomeIndex, const vector<ShortestPathData>& InPathDataList);
	void WriteCSVFile(const vector<NodeData>& InFinalResultData);
};