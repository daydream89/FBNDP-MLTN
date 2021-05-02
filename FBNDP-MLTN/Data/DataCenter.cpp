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
		transform(TypeString.begin(), TypeString.end(), TypeString.begin(), tolower);
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
		transform(Name.begin(), Name.end(), Name.begin(), tolower);
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
		transform(Name.begin(), Name.end(), Name.begin(), tolower);
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
		transform(Name.begin(), Name.end(), Name.begin(), tolower);
		string Value = RowData.at(1);
		transform(Value.begin(), Value.end(), Value.begin(), tolower);

		if (Name == "passagetimediff")
		{
			UserInput.PassageTimeDiff = stof(Value);
		}
		else if (Name == "ivttcoefficient")
		{
			UserInput.MNLCoefficient.IVTTCoef = stof(Value);
		}
		else if (Name == "ovttcoefficient")
		{
			UserInput.MNLCoefficient.OVTTCoef = stof(Value);
		}
		else if (Name == "ctpicoefficient")
		{
			UserInput.MNLCoefficient.CTPICoef = stof(Value);
		}
		else if (Name == "relicoefficient")
		{
			UserInput.MNLCoefficient.RELICoef = stof(Value);
		}
		else if (Name == "circcoefficient")
		{
			UserInput.MNLCoefficient.CIRCCoef = stof(Value);
		}
		else if (Name == "townbustimecost")
		{
			UserInput.TownBusTimeCost = stof(Value);
		}
		else if (Name == "bustimecost")
		{
			UserInput.BusTimeCost = stof(Value);
		}
		else if (Name == "traintimecost")
		{
			UserInput.TrainTimeCost = stof(Value);
		}
		else if (Name == "waittimecost")
		{
			UserInput.WaitTimeCost = stof(Value);
		}
		else if (Name == "transfertimecost")
		{
			UserInput.TransferTimeCost = stof(Value);
		}
		else if (Name == "townbusspeed")
		{
			UserInput.TownBusSpeed = stof(Value);
		}
		else if (Name == "townbusoperationcost")
		{
			UserInput.TownBusOperationCost = stof(Value);
		}
		else if (Name == "townbusdispatchesperhour")
		{
			UserInput.TownBusDispatchesPerHour = static_cast<uint64_t>(stoul(Value));
		}
		else if (Name == "townbusseat")
		{
			UserInput.TownBusSeat = static_cast<uint64_t>(stoul(Value));
		}
		else if (Name == "routefixcost")
		{
			UserInput.RouteFixCost = static_cast<uint64_t>(stoul(Value));
		}
		else if (Name == "panaltyfactor")
		{
			UserInput.PanaltyFactor = stod(Value);
		}
		else if (Name == "panaltyfactor2")
		{
			UserInput.PanaltyFactor2 = stod(Value);
		}
		else if (Name == "panaltyfactor3")
		{
			UserInput.PanaltyFactor3 = stod(Value);
		}
		else if (Name == "panaltyfactor4")
		{
			UserInput.PanaltyFactor4 = stod(Value);
		}
		else if (Name == "loadfactor")
		{
			UserInput.LoadFactor = stod(Value);
		}
		else if (Name == "maxroutelength")
		{
			UserInput.MaxRouteLength = stod(Value);
		}
		else if (Name == "numberofbusesgiven")
		{
			UserInput.NumberOfBusesGiven = static_cast<uint64_t>(stoul(Value));
		}
		else if (Name == "operatinghoursperday")
		{
			UserInput.OperatingHoursPerDay = stof(Value);
		}
		else if (Name == "townbusnodesnum")
		{
			for (uint64_t i = 0; i < stoull(Value); ++i)
			{
				UserInput.TownBusNodesNum.emplace_back(stoull(RowData.at(i + 2)));
			}
		}
		else if (Name == "nocrossovernum")
		{
			UserInput.NoCrossoverNum = static_cast<uint64_t>(stoul(Value));
		}
		else if (Name == "populationnum")
		{
			UserInput.PopulationNum = static_cast<uint64_t>(stoul(Value));
		}
		else if (Name == "maxgeneration")
		{
			UserInput.MaxGeneration = static_cast<uint64_t>(stoul(Value));
		}
		else if (Name == "crossoverpercent")
		{
			UserInput.CrossoverPercent = static_cast<uint64_t>(stoul(Value));
		}
		else if (Name == "mutationpercent")
		{
			UserInput.MutationPercent = static_cast<uint64_t>(stoul(Value));
		}
		else if (Name == "maxfitnessunchangedgeneration")
		{
			UserInput.MaxFitnessUnchangedGeneration = static_cast<uint64_t>(stoul(Value));
		}
		else if (Name == "exchangechromosomenum")
		{
			UserInput.ExchangeChromosomeNum = static_cast<uint64_t>(stoul(Value));
		}
	}
}

void DataCenter::AddShortestPathDataList(const vector<ShortestPathData>& InPathDataList)
{
	for (const auto& PathData : InPathDataList)
		ShortestPathDataList.push_back(PathData);
}

void DataCenter::AddBestResultData(uint64_t GenerationNum, const GenerationBestResultData& ResultData)
{
	if (BestResultData.find(GenerationNum) == BestResultData.end())
		BestResultData.insert(make_pair(GenerationNum, ResultData));
}

void DataCenter::AddBestResultData(uint64_t GenerationNum, uint64_t ChromosomeNum, const FitnessResultData& InFitnessResult)
{
	if (BestResultData.find(GenerationNum) == BestResultData.end())
	{
		GenerationBestResultData Data;
		Data.FitnessData = InFitnessResult;
		Data.ChromosomeNumber = ChromosomeNum;
		BestResultData.insert(make_pair(GenerationNum, Data));
	}
}