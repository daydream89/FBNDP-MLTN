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
	const vector<NodeData>& GetNodeData() { return NodeDataList; }

	void SetLinkData(const FileDataList& InData);
	const vector<LinkData>& GetLinkData() { return LinkDataList; }

	void SetRouteData(const FileDataList& InData);
	const vector<RouteData>& GetRouteData() { return RouteDataList; }

	void SetOperatingData(const FileDataList& InData);
	const vector<OperatingData>& GetOperatingData() { return OperatingDataList; }

	void SetTrafficVolumeData(const FileDataList& InData);
	const vector<TrafficVolumeData>& GetTrafficVolumeData() { return TrafficVolumeDataList; }

	void SetDistanceData(const FileDataList& InData);
	const vector<DistanceData>& GetDistanceData() { return DistanceDataList; }

	void SetUserInputData(const FileDataList& InData);
	const UserInputData& GetUserInputData() { return UserInput; }

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

	UserInputData UserInput;
};