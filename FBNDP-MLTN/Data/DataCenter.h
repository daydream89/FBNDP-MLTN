#pragma once

#include <vector>

#include "DataTypes.h"

using namespace std;

class DataCenter
{
public:
	DataCenter* GetInstance()
	{
		if (!Instance)
		{
			Instance = new DataCenter();
		}
		
		return Instance;
	}

	void SetNodeData(const vector<string>& Data);
	vector<NodeData> GetNodeData() { return NodeDataList; }

	void SetLinkData(const vector<string>& Data);
	vector<LinkData> GetLinkData() { return LinkDataList; }

	void SetRouteData(const vector<string>& Data);
	vector<RouteData> GetRouteData() { return RouteDataList; }

	void SetOperatingData(const vector<string>& Data);
	vector<OperatingData> GetOperatingData() { return OperatingDataList; }

	void SetTrafficVolumeData(const vector<string>& Data);
	vector<TrafficVolumeData> GetTrafficVolumeData() { return TrafficVolumeDataList; }

	void SetDistanceData(const vector<string>& Data);
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