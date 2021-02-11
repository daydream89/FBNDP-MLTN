#pragma once

#include "../Data/DataTypes.h"

using namespace std;

class CSVWriter
{
public:
	void WriteCSVFile(int GenerationNumber, int ChromosomeIndex, const vector<ShortestPathData>& InPathDataList);
	void WriteCSVFile(int GenerationNumber, const vector<vector<ShortestPathData>>& InTownBusRouteDataList);
};