#pragma once

#include "../Data/DataTypes.h"

using namespace std;

struct NodeData;

class CSVWriter
{
public:
	CSVWriter();

	void WriteCSVFile(int GenerationNumber, const vector<vector<ShortestPathData>>& InTownBusRouteDataList);
	void WriteCSVFile(int GenerationNumber, int ChromosomeIndex, const vector<ShortestPathData>& InPathDataList);
	void WriteCSVFile(const vector<NodeData>& InFinalResultData);
};