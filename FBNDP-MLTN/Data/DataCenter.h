#pragma once

#include <vector>

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
	vector<NodeData> GetNodeData() { return NodeDataList; }

	void SetLinkData(const FileDataList& InData);
	vector<LinkData> GetLinkData() { return LinkDataList; }

	void SetRouteData(const FileDataList& InData);
	vector<RouteData> GetRouteData() { return RouteDataList; }

	void SetOperatingData(const FileDataList& InData);
	vector<OperatingData> GetOperatingData() { return OperatingDataList; }

	void SetTrafficVolumeData(const FileDataList& InData);
	vector<TrafficVolumeData> GetTrafficVolumeData() { return TrafficVolumeDataList; }

	void SetDistanceData(const FileDataList& InData);
	vector<DistanceData> GetDistanceData() { return DistanceDataList; }

private:
	DataCenter() {}
	~DataCenter() {}

	static DataCenter* Instance;

	vector<NodeData> NodeDataList;
	vector<LinkData> LinkDataList;
	vector<RouteData> RouteDataList;
	vector<OperatingData> OperatingDataList;
	vector<TrafficVolumeData> TrafficVolumeDataList;
	vector<DistanceData> DistanceDataList;
};