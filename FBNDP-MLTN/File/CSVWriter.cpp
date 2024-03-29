#include "CSVWriter.h"

#include <iostream>
#include <fstream>
#include <filesystem>

#include "../Data/DataCenter.h"

CSVWriter::CSVWriter()
{
	_wmkdir(L"./Output");
}

void CSVWriter::WriteCSVFile(uint64_t GenerationNumber, const vector<vector<ShortestPathData>>& InTownBusRouteDataList)
{
	string FileNameRoutes = "./Output/";
	FileNameRoutes.append(to_string(GenerationNumber) + "_");
	FileNameRoutes.append("ChromosomeRoutesData.csv");

	string FileNameStops = "./Output/";
	FileNameStops.append(to_string(GenerationNumber) + "_");
	FileNameStops.append("ChromosomeStopsData.csv");

	ofstream RoutesFileStream;
	RoutesFileStream.open(FileNameRoutes);

	ofstream StopsFileStream;
	StopsFileStream.open(FileNameStops);
	if (RoutesFileStream.is_open() && StopsFileStream.is_open())
	{
		string IndexRow = "Chromosome Data, NodeNum\n";
		RoutesFileStream.write(IndexRow.c_str(), IndexRow.length());
		StopsFileStream.write(IndexRow.c_str(), IndexRow.length());
		for (uint64_t PopSize = 0; PopSize < InTownBusRouteDataList.size(); ++PopSize)
		{
			string NewLine = "\n";
			string ChromosomeRouteData = "";
			string ChromosomeStopData = "";
			ChromosomeRouteData.append(to_string(PopSize + 1) + "_ChromosomeTownBusRoutes,");
			ChromosomeStopData.append(to_string(PopSize + 1) + "_ChromosomeTownBusStops,");
			RoutesFileStream.write(ChromosomeRouteData.c_str(), ChromosomeRouteData.length());
			StopsFileStream.write(ChromosomeStopData.c_str(), ChromosomeStopData.length());
			for (const auto& ShortestPathDataIter : InTownBusRouteDataList.at(PopSize))
			{
				for (const auto& RouteIter : ShortestPathDataIter.TownBusData.TownBusStopCheck)
				{
					/*RoutesInfo*/
					string NodeNum = "";
					NodeNum.append(to_string(RouteIter.first.Num) + ",");
					RoutesFileStream.write(NodeNum.c_str(), NodeNum.length());
					if (RouteIter.second == true) /*TownBusStop*/
					{
						StopsFileStream.write(NodeNum.c_str(), NodeNum.length());
					}
				}
			}
			RoutesFileStream.write(NewLine.c_str(), NewLine.length());
			StopsFileStream.write(NewLine.c_str(), NewLine.length());
		}
		// write data
	}

	RoutesFileStream.close();
	StopsFileStream.close();
}

void CSVWriter::WriteCSVFile(uint64_t GenerationNumber, int ChromosomeIndex, const vector<ShortestPathData>& InPathDataList)
{
	string FileName = "./Output/";
	FileName.append(to_string(GenerationNumber) + "_");
	FileName.append("ShortestPath");
	FileName.append("_" + to_string(ChromosomeIndex) + ".csv");

	ofstream FileStream;
	FileStream.open(FileName);
	if (FileStream.is_open())
	{
		string NewLine = "\n";
		string IndexRow = "Order,Cost,TrainIVTT,BusIVTT,TownBusIVTT,OVTT,TrafficVolume,Path\n";
		FileStream.write(IndexRow.c_str(), IndexRow.length());
		for (const auto& PathData : InPathDataList)
		{
			string PathStr = "";
			for (const auto& Node : PathData.Path)
				PathStr.append(to_string(Node.Num) + " ");

			string Row = "";
			Row.append(to_string(PathData.Order) + ",");
			Row.append(to_string(PathData.TrainIVTT + PathData.BusIVTT + PathData.TownBusIVTT + PathData.Transfer.OVTT) + ",");
			Row.append(to_string(PathData.TrainIVTT) + ",");
			Row.append(to_string(PathData.BusIVTT) + ",");
			Row.append(to_string(PathData.TownBusIVTT) + ",");
			Row.append(to_string(PathData.Transfer.OVTT) + ",");
			Row.append(to_string(PathData.TrafficVolumeForPath) + ",");
			Row.append(PathStr);

			FileStream.write(Row.c_str(), Row.length());
			FileStream.write(NewLine.c_str(), NewLine.length());
		}
		FileStream.write(NewLine.c_str(), NewLine.length());
	}

	FileStream.close();
}

void CSVWriter::WriteCSVFile(const map<uint64_t, GenerationBestResultData>& InBestResultData)
{
	string FileName = "./Output/";
	FileName.append("GenerationBestResult.csv");

	ofstream FileStream;
	FileStream.open(FileName);
	if (FileStream.is_open())
	{
		string NewLine = "\n";
		string IndexRow = "GenerationNumber,Fitness,P2,P3,ObjectFunctionValue,NumberOfBuses,NumberOfTownBusRoutes,TotalUserCost,OperatorCost,TotalRouteDistance,ChromosomeName\n";
		FileStream.write(IndexRow.c_str(), IndexRow.length());

		for (const auto& ResultPair : InBestResultData)
		{
			string Row = "";
			Row.append(to_string(ResultPair.first) + ",");
			Row.append(to_string(ResultPair.second.FitnessData.FitnessValue) + ",");
			//Row.append(to_string(ResultPair.second.FitnessData.P1Value) + ",");
			Row.append(to_string(ResultPair.second.FitnessData.P2Value) + ",");
			Row.append(to_string(ResultPair.second.FitnessData.P3Value) + ",");
			Row.append(to_string(ResultPair.second.FitnessData.ObjectFunctionValue) + ",");
			Row.append(to_string(ResultPair.second.FitnessData.NumberOfBuses) + ",");
			Row.append(to_string(ResultPair.second.FitnessData.NumberOfTownBusRoutes) + ",");
			Row.append(to_string(ResultPair.second.FitnessData.TotalCustomerCost) + ",");
			Row.append(to_string(ResultPair.second.FitnessData.TownBusOperatorCost) + ",");
			Row.append(to_string(ResultPair.second.FitnessData.TotalRouteDistance) + ",");
			Row.append("Chromosome" + to_string(ResultPair.second.ChromosomeNumber));

			FileStream.write(Row.c_str(), Row.length());
			FileStream.write(NewLine.c_str(), NewLine.length());
		}
	}

	FileStream.close();
}

void CSVWriter::WriteCSVFile(const vector<NodeData>& InFinalResultData)
{
	string FileName = "./Output/";
	FileName.append("Result.csv");

	ofstream FileStream;
	FileStream.open(FileName);
	if (FileStream.is_open())
	{
		string NewLine = "\n";
		string IndexRow = "Path\n";
		FileStream.write(IndexRow.c_str(), IndexRow.length());

		string Row = "";
		for (const auto& Node : InFinalResultData)
			Row.append(to_string(Node.Num) + " ");

		FileStream.write(Row.c_str(), Row.length());
		FileStream.write(NewLine.c_str(), NewLine.length());
	}

	FileStream.close();
}