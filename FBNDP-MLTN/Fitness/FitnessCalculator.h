#pragma once

#include "../Data/DataCenter.h"

#include <vector>

using namespace std;

class FitnessCalculator
{
public:
	FitnessCalculator(const vector<pair<NodeData, bool>>& InGraphData, uint64_t PathNum);

	double Calculate();	// returns result of Fitness Function

private:
	double PassageAssignment();
	void SetPassageAssignmentForMNLModel(vector<ShortestPathData>& InOutPathList, uint64_t TrafficVolume);

	float CalcCurveNTransportationIVTT(ShortestPathData& PathData);

	void CalcCustomerCost(const vector<ShortestPathData>& InPathList, double& OutCostSum);
	double CalcNetworkCost(double SumofCustomerCost);
	double CalcFitness(double NetworkCost);

	bool FindNodeNumberFromGraphData(uint64_t FromNodeNum, uint64_t ToNodeNum);

	void AddGraphDataToRouteDataMap(const vector<LinkData>& InFullLinkDataList);
	void AddRouteDataMapToGraphData(const RouteMap& RouteDataMap, const vector<NodeData>& FullGraphData);

	void SetGraphData(const vector<pair<NodeData, bool>>& InGraphData);
	void SetLinkDataList(const vector<pair<NodeData, bool>>& InFullGraphData, const vector<LinkData>& InFullLinkDataList);
	float FindLinkLength(const vector<LinkData>& InFullLinkDataList, uint64_t FromNodeNum, uint64_t ToNodeNum);

private:
	vector<NodeData> GraphData;
	vector<LinkData> LinkDataList;
	RouteMap RouteDataMap;
	float TotalLengthOfTownBusLine = 0.f;
	uint64_t NumberOfPath = 2;
	
	float PassageTimeDiff = 5.f;
	MNLCoefficientData MNLCoefData;
};