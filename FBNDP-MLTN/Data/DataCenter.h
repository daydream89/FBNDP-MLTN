#pragma once

#include "DataTypes.h"

using namespace std;

class DataCenter
{
public:
	static DataCenter* GetInstance()
	{
		if (!Instance)
		{
			Instance = new DataCenter();
		}
		
		return Instance;
	}

	void SetNodeData(const FileDataList& InData);
	const vector<NodeData>& GetNodeData() { return NodeDataList; }

	void SetLinkData(const FileDataList& InData);
	const vector<LinkData>& GetLinkData() { return LinkDataList; }

	void SetRouteData(const FileDataList& InData);
	const RouteMap& GetRouteData() { return RouteDataMap; }

	void SetOperatingData(const FileDataList& InData);
	const map<string, OperatingData>& GetOperatingData() { return OperatingDataMap; }

	void SetTrafficVolumeData(const FileDataList& InData);
	const vector<TrafficVolumeData>& GetTrafficVolumeData() { return TrafficVolumeDataList; }

	void SetDistanceData(const FileDataList& InData);
	const vector<DistanceData>& GetDistanceData() { return DistanceDataList; }

	void SetUserInputData(const FileDataList& InData);
	const UserInputData& GetUserInputData() { return UserInput; }

	void AddTownBusRouteData(vector<ShortestPathData> TownBusRoute) { ChromosomeTownBusRouteData.emplace_back(TownBusRoute); };
	void ClearTownBusRouteData() { ChromosomeTownBusRouteData.clear(); };
	const vector<ShortestPathData>& GetChromosomeRoutesData(uint64_t ChromosomeNum) { return ChromosomeTownBusRouteData.at(ChromosomeNum); };
	vector<ShortestPathData>& GetChromosomeRoutesDataRef(uint64_t ChromosomeNum) { return ChromosomeTownBusRouteData.at(ChromosomeNum); };

	void AddShortestPathDataList(const vector<ShortestPathData>& InPathDataList);
	const vector<ShortestPathData>& GetShortestPathDataList() { return ShortestPathDataList; }
	void ClearShortestPathDataList() { ShortestPathDataList.clear(); }

private:
	DataCenter() {}
	~DataCenter() {}

	static DataCenter* Instance;

	// Data read from outside
	vector<NodeData> NodeDataList;
	vector<LinkData> LinkDataList;
	RouteMap RouteDataMap;
	map<string, OperatingData> OperatingDataMap;
	vector<TrafficVolumeData> TrafficVolumeDataList;
	vector<DistanceData> DistanceDataList;

	UserInputData UserInput;

	// Data calculated from inside
	vector<ShortestPathData> ShortestPathDataList;
	vector<vector<ShortestPathData>> ChromosomeTownBusRouteData;
};