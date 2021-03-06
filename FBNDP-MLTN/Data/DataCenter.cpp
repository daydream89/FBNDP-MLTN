#include "DataCenter.h"

DataCenter* DataCenter::Instance;

void DataCenter::SetNodeData(const FileDataList& InData)
{
	for (const auto& RowData : InData)
	{
		if (RowData.size() < 7)
		{
			continue;
		}

		NodeData Data;
		Data.Num = static_cast<uint64_t>(stoul(RowData.at(1)));
		Data.Coord = Coordinate(static_cast<uint64_t>(stoul(RowData.at(2))), static_cast<uint64_t>(stoul(RowData.at(3))));
		Data.TransferTime = stof(RowData.at(5));

		// todo. convert enum class to string & compare string
		Data.Type = NodeType::NodeTypeMax;
		string TypeString = RowData.at(6);
		if (TypeString.compare("busstop") == 0)
			Data.Type = NodeType::BusStop;
		else if (TypeString.compare("station") == 0)
			Data.Type = NodeType::Station;

		NodeDataList.emplace_back(Data);
	}
}

void DataCenter::SetLinkData(const FileDataList& InData)
{
	for (const auto& RowData : InData)
	{
		if (RowData.size() < 11)
		{
			continue;
		}

		LinkData Data;
		Data.FromNodeNum = static_cast<uint64_t>(stoul(RowData.at(1)));
		Data.ToNodeNum = static_cast<uint64_t>(stoul(RowData.at(2)));
		Data.Length = stof(RowData.at(3));
		Data.Type = static_cast<LinkType>(stoi(RowData.at(5)));
		Data.Speed = stof(RowData.at(8));
		LinkDataList.emplace_back(Data);
	}
}

void DataCenter::SetRouteData(const FileDataList& InData)
{
	for (const auto& RowData : InData)
	{
		if (RowData.size() < 4)
		{
			continue;
		}

		string Name = RowData.at(0);
		uint64_t Order = static_cast<uint64_t>(stoul(RowData.at(1)));

		RouteData Data;
		Data.CumDistance = stof(RowData.at(2));
		Data.Node = static_cast<uint64_t>(stoul(RowData.at(3)));

		if (RouteDataMap.find(Name) == RouteDataMap.end())
		{
			map<uint64_t, RouteData> RouteMap;
			RouteMap.insert(make_pair(Order, Data));
			RouteDataMap.insert(make_pair(Name, RouteMap));
		}
		else
		{
			auto RouteDataPair = RouteDataMap.find(Name);
			RouteDataPair->second.insert(make_pair(Order, Data));
		}
	}
}

void DataCenter::SetOperatingData(const FileDataList& InData)
{
	for (const auto& RowData : InData)
	{
		if (RowData.size() < 3)
		{
			continue;
		}

		string Name = RowData.at(0);
		if (OperatingDataMap.find(Name) == OperatingDataMap.end())
		{
			OperatingData Data;
			Data.Dispatch = static_cast<uint64_t>(stoul(RowData.at(1)));
			Data.Speed = stof(RowData.at(2));
			OperatingDataMap.insert(make_pair(Name, Data));
		}
	}
}

void DataCenter::SetTrafficVolumeData(const FileDataList& InData)
{
	for (const auto& RowData : InData)
	{
		if (RowData.size() < 3)
		{
			continue;
		}

		TrafficVolumeData Data;
		Data.FromNodeNum = static_cast<uint64_t>(stoul(RowData.at(0)));
		Data.ToNodeNum = static_cast<uint64_t>(stoul(RowData.at(1)));
		Data.TrafficVolume = static_cast<uint64_t>(stoul(RowData.at(2)));
		TrafficVolumeDataList.emplace_back(Data);
	}
}

void DataCenter::SetDistanceData(const FileDataList& InData)
{
	for (const auto& RowData : InData)
	{
		if (RowData.size() < 3)
		{
			continue;
		}

		DistanceData Data;
		Data.FromNodeNum = static_cast<uint64_t>(stoul(RowData.at(0)));
		Data.ToNodeNum = static_cast<uint64_t>(stoul(RowData.at(1)));
		Data.Distance = stof(RowData.at(2));
		DistanceDataList.emplace_back(Data);
	}
}

void DataCenter::SetUserInputData(const FileDataList& InData)
{
	for (const auto& RowData : InData)
	{
		if (RowData.size() < 2)
			continue;

		string Name = RowData.at(0);
		string Value = RowData.at(1);

		if (Name == "PassageTimeDiff")
		{
			UserInput.PassageTimeDiff = stof(Value);
		}
		else if (Name == "IVTTCoefficient")
		{
			UserInput.MNLCoefficient.IVTTCoef = stof(Value);
		}
		else if (Name == "OVTTCoefficient")
		{
			UserInput.MNLCoefficient.OVTTCoef = stof(Value);
		}
		else if (Name == "CTPICoefficient")
		{
			UserInput.MNLCoefficient.CTPICoef = stof(Value);
		}
		else if (Name == "RELICoefficient")
		{
			UserInput.MNLCoefficient.RELICoef = stof(Value);
		}
		else if (Name == "CIRCCoefficient")
		{
			UserInput.MNLCoefficient.CIRCCoef = stof(Value);
		}
		else if (Name == "TownBusTimeCost")
		{
			UserInput.TownBusTimeCost = stof(Value);
		}
		else if (Name == "BusTimeCost")
		{
			UserInput.BusTimeCost = stof(Value);
		}
		else if (Name == "TrainTimeCost")
		{
			UserInput.TrainTimeCost = stof(Value);
		}
		else if (Name == "WaitTimeCost")
		{
			UserInput.WaitTimeCost = stof(Value);
		}
		else if (Name == "TransferTimeCost")
		{
			UserInput.TransferTimeCost = stof(Value);
		}
		else if (Name == "TownBusSpeed")
		{
			UserInput.TownBusSpeed = stof(Value);
		}
		else if (Name == "TownBusOperationCost")
		{
			UserInput.TownBusOperationCost = stof(Value);
		}
		else if (Name == "TownBusDispatchesPerHour")
		{
			UserInput.TownBusDispatchesPerHour = static_cast<uint64_t>(stoul(Value));
		}
		else if (Name == "PanaltyFactor")
		{
			UserInput.PanaltyFactor = stof(Value);
		}
		else if (Name == "PanaltyFactor2")
		{
			UserInput.PanaltyFactor2 = stof(Value);
		}
		else if (Name == "NumberOfBusesGiven")
		{
			UserInput.NumberOfBusesGiven = static_cast<uint64_t>(stoul(Value));
		}
		else if (Name == "OperatingHoursPerDay")
		{
			UserInput.OperatingHoursPerDay = stof(Value);
		}
		else if (Name == "TownBusNodesNum")
		{
			for (uint64_t i = 0; i < stoull(Value); ++i)
			{
				UserInput.TownBusNodesNum.emplace_back(stoull(RowData.at(i + 2)));
			}
		}
		else if (Name == "NoCrossoverNum")
		{
			UserInput.NoCrossoverNum = static_cast<uint64_t>(stoul(Value));
		}
		else if (Name == "PopulationNum")
		{
			UserInput.PopulationNum = static_cast<uint64_t>(stoul(Value));
		}
		else if (Name == "MaxGeneration")
		{
			UserInput.MaxGeneration = static_cast<uint64_t>(stoul(Value));
		}
	}
}

void DataCenter::AddShortestPathDataList(const vector<ShortestPathData>& InPathDataList)
{
	for (const auto& PathData : InPathDataList)
		ShortestPathDataList.push_back(PathData);
}