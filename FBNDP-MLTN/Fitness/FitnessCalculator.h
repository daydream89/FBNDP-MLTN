#pragma once

#include "../Data/DataCenter.h"

#include <vector>
#include <map>

using namespace std;

struct FitnessRouteData
{
	RouteData Route;
	TownBusRouteData* DataCenterRoutePtr = nullptr;
};
typedef map<string, map<uint64_t, FitnessRouteData>> FitnessRouteMap;

class FitnessCalculator
{
public:
	FitnessCalculator(int InChromosomeIndex, uint64_t PathNum);

	FitnessResultData Calculate();	// returns result of Fitness Function

private:
	double PassageAssignment();
	void SetPassageAssignmentForMNLModel(vector<ShortestPathData>& InOutPathList, uint64_t TrafficVolume);

	float CalcCurveNTransportationIVTT(ShortestPathData& PathData);

	void CalcCustomerCost(vector<ShortestPathData>& InPathList, double& OutCostSum);
	double CalcNetworkCost(double SumofCustomerCost);
	double CalcFitness(double NetworkCost);

	bool FindNodeNumberFromGraphData(uint64_t FromNodeNum, uint64_t ToNodeNum);

	void AddGraphDataToRouteDataMap(vector<ShortestPathData>& InOutPathDataList);
	void AddRouteDataMapToGraphData(const RouteMap& RouteDataMap, const vector<NodeData>& FullGraphData);
	void AddRouteDataMapToLinkData(const RouteMap& RouteDataMap, const vector<LinkData>& FullLinkData);
	void AddTrafficVolumeByTownBusRoute(const vector<string>& RouteNameList, uint64_t TrafficVolume);

	void SetGraphData(const vector<ShortestPathData>& InPathData);
	void SetLinkDataList(const vector<ShortestPathData>& InPathData, const vector<LinkData>& InFullLinkDataList);
	void AddLinkDataFromDefaultRouteData(const RouteMap& InRouteDataMap, const map<string, OperatingData>& InOperatingData);
	float FindLinkLength(const vector<LinkData>& InFullLinkDataList, uint64_t FromNodeNum, uint64_t ToNodeNum);

private:
	int ChromosomeIndex = 0;
	
	vector<NodeData> GraphData;
	vector<LinkData> LinkDataList;
	RouteMap RouteDataMap;
	float TotalLengthOfTownBusLine = 0.f;
	uint64_t NumberOfPath = 2;

	map<string, uint64_t> NumOfUsersPerTownBusRoute;
	
	float PassageTimeDiff = 5.f;
	MNLCoefficientData MNLCoefData;

	FitnessResultData ResultData;
};