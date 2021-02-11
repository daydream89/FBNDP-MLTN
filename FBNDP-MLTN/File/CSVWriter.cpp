#include "CSVWriter.h"

#include <iostream>
#include <fstream>

#include "../Data/DataCenter.h"

void CSVWriter::WriteCSVFile(int GenerationNumber, const vector<vector<ShortestPathData>>& InTownBusRouteDataList)
{
	string FileName = "./Output/";
	FileName.append(to_string(GenerationNumber) + "_");
	FileName.append("ChromosomeData.csv");
	ofstream FileStream;
	FileStream.open(FileName);
	if (FileStream.is_open())
	{
		// write data
	}

	FileStream.close();
}

void CSVWriter::WriteCSVFile(int GenerationNumber, int ChromosomeIndex, const vector<ShortestPathData>& InPathDataList)
{
	string FileName = "./Output/";
	FileName.append(to_string(GenerationNumber) + "_");
	FileName.append("ShortestPath");
	FileName.append("_" + to_string(ChromosomeIndex) + ".csv");

	ofstream FileStream;
	FileStream.open(FileName);
	if (FileStream.is_open())
	{
		FileStream << "test" << endl;
	}

	FileStream.close();
}