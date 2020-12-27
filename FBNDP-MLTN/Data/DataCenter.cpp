#include "DataCenter.h"

DataCenter* DataCenter::Instance;

void DataCenter::SetNodeData(const FileDataList& InData)
{
	for (auto RowData : InData)
	{
		if (RowData.size() < 7)
		{
			continue;
		}

		NodeData Data;
		Data.Num = static_cast<uint32_t>(stoul(RowData.at(1)));
		Data.Coord = Coordinate(static_cast<uint32_t>(stoul(RowData.at(2))), static_cast<uint32_t>(stoul(RowData.at(3))));
		Data.TransferTime = stof(RowData.at(5));

		// todo. convert enum class to string & compare string
		Data.Type = NodeType::NodeTypeMax;
		string TypeString = RowData.at(6);
		if (TypeString.compare("busstop"))
			Data.Type = NodeType::BusStop;
		else if (TypeString.compare("station"))
			Data.Type = NodeType::Station;

		NodeDataList.emplace_back(Data);
	}
}

void DataCenter::SetLinkData(const FileDataList& InData)
{
	for (auto RowData : InData)
	{
		if (RowData.size() < 11)
		{
			continue;
		}

		LinkData Data;
		Data.FromNodeNum = static_cast<uint32_t>(stoul(RowData.at(1)));
		Data.ToNodeNum = static_cast<uint32_t>(stoul(RowData.at(2)));
		Data.Length = stof(RowData.at(3));
		Data.Type = static_cast<LinkType>(stoi(RowData.at(5)));
		Data.Speed = stof(RowData.at(8));
		LinkDataList.emplace_back(Data);
	}
}

void DataCenter::SetRouteData(const FileDataList& InData)
{
	for (auto RowData : InData)
	{
		if (RowData.size() < 4)
		{
			continue;
		}

		RouteData Data;
		Data.Name = RowData.at(0);
		Data.Order = static_cast<uint32_t>(stoul(RowData.at(1)));
		Data.CumDistance = stof(RowData.at(2));
		Data.Node = static_cast<uint32_t>(stoul(RowData.at(3)));
		RouteDataList.emplace_back(Data);
	}
}

void DataCenter::SetOperatingData(const FileDataList& InData)
{
	for (auto RowData : InData)
	{
		if (RowData.size() < 3)
		{
			continue;
		}

		OperatingData Data;
		Data.Name = RowData.at(0);
		Data.Dispatch = static_cast<uint32_t>(stoul(RowData.at(1)));
		Data.Speed = stof(RowData.at(2));
		OperatingDataList.emplace_back(Data);
	}
}

void DataCenter::SetTrafficVolumeData(const FileDataList& InData)
{
	for (auto RowData : InData)
	{
		if (RowData.size() < 3)
		{
			continue;
		}

		TrafficVolumeData Data;
		Data.FromNodeNum = static_cast<uint32_t>(stoul(RowData.at(0)));
		Data.ToNodeNum = static_cast<uint32_t>(stoul(RowData.at(1)));
		Data.TrafficVolume = static_cast<uint32_t>(stoul(RowData.at(2)));
		TrafficVolumeDataList.emplace_back(Data);
	}
}

void DataCenter::SetDistanceData(const FileDataList& InData)
{
	for (auto RowData : InData)
	{
		if (RowData.size() < 3)
		{
			continue;
		}

		DistanceData Data;
		Data.FromNodeNum = static_cast<uint32_t>(stoul(RowData.at(0)));
		Data.ToNodeNum = static_cast<uint32_t>(stoul(RowData.at(1)));
		Data.Distance = stof(RowData.at(2));
		DistanceDataList.emplace_back(Data);
	}
}