#pragma once

#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

static const string TownBusStr = "townbus";

enum class eProgramMode
{
	GenericAlgorithm,
	FitnessCalculator,
};

enum class NodeType
{
	BusStop = 0,
	Station,
	NodeTypeMax,
};

enum class LinkType
{
	NationalExpressway = 101,	// 고속국도
	Expressway = 102,			// 도시 고속화 도로
	GeneralNationalRoad = 103,	// 일반 국도
	MetropolitanRoad = 104,		// 특별.광역시 도로
	GookJiDo = 105,				// 국가지원지방도
	LocalLoad = 106,			// 지방도
	SiGoonDo = 107,				// 시군도
	HighwayRamp = 108,			// 고속도로 연결 램프
	Rail = 200,					// 철도
	CentroidConnector = 999,	// 센트로이드 커넥터
};

struct Coordinate
{
	uint64_t X;
	uint64_t Y;

	Coordinate()
		: X(0)
		, Y(0)
	{ }

	Coordinate(uint64_t XCoord, uint64_t YCoord)
		: X(XCoord)
		, Y(YCoord)
	{ }
};

struct MNLCoefficientData
{
	float IVTTCoef = -0.0176f;
	float OVTTCoef = -0.0296f;
	float CTPICoef = -3.8418f;
	float RELICoef = 3.1469f;
	float CIRCCoef = -0.3896f;
};

// 네트워크 노드에 대한 data
struct NodeData
{
	uint64_t Num = -1;			// node 번호
	Coordinate Coord;			// 2차원 좌표
	float TransferTime = 0.f;	// 환승 시간
	NodeType Type = NodeType::BusStop;	// node 종류
};

// 네트워크 링크에 대한 data
struct LinkData
{
	uint64_t FromNodeNum = 0;					// 시작 node
	uint64_t ToNodeNum = 0;						// 종료 node
	float Length = 0.f;							// 실제 길이 (직선 거리 아님)
	LinkType Type = LinkType::SiGoonDo;			// link 종류
	float Speed = 0.f;							// link 속도 (버스가 해당 링크를 통과하는 속도)
};

/* 대중교통 노선의 노드 data.
	Name과 Order는 DataCenter에 이중map으로 구성 */
struct RouteData
{
	float CumDistance = 0.f;	// 누적 거리
	uint64_t Node = 0;			// node 번호
};

/* 대중교통 노선의 운영 정보.
	Name은 DataCenter에 map으로 구성 */
struct OperatingData
{
	uint64_t Dispatch = 0;	// 배차 간격
	float Speed = 0.f;		// 버스 또는 전철이 운영되는 평균속도
};

// 시점 -> 종점 통행량
struct TrafficVolumeData
{
	uint64_t FromNodeNum = 0;	// 시작 node
	uint64_t ToNodeNum = 0;		// 종료 node
	uint64_t TrafficVolume = 0;	// 통행량
};

// 노드 간 직선거리
struct DistanceData
{
	uint64_t FromNodeNum = 0;	// 시작 node
	uint64_t ToNodeNum = 0;		// 종료 node
	float Distance = 0.f;		// 직선 거리
};

// 프로그램 구동에 필요한 추가 데이터 모음
struct 
	UserInputData
{
	eProgramMode ProgramMode = eProgramMode::GenericAlgorithm;
	float PassageTimeDiff = 5.f;			// 적합도 계산 시의 통행 시간 차이
	MNLCoefficientData MNLCoefficient;		// MNL Model에서 사용하는 계수 모음
	float TownBusTimeCost = 10000.f;		// 마을버스 탑승 시간 비용 (원/인*시간)
	float BusTimeCost = 10000.f;			// 버스 탑승 시간 비용 (원/인*시간)
	float TrainTimeCost = 10000.f;			// 전철 탑승 시간 비용 (원/인*시간)
	float WaitTimeCost = 10000.f;			// 대기 시간 비용 (원/인*시간)
	float TransferTimeCost = 10000.f;		// 환승 시간 비용 (원/인*시간)
	float TownBusSpeed = 30.f;				// 마을버스 운영 속도 (km/h)
	float TownBusOperationCost = 1600.f;	// 마을버스 운영 비용 (원/대*km)
	uint64_t TownBusDispatchesPerHour = 12;	// 마을버스 배차 횟수 (회/hour)
	uint64_t TownBusSeat = 30;				// 마을버스 좌석 수
	uint64_t RouteFixCost = 10000;			// 마을버스 노선의 고정 비용
	double PanaltyFactor = 0.0001;			// 적합도 함수에서 사용하는 값
	double PanaltyFactor2 = 0.001;			// 적합도 함수에서 사용하는 값
	double PanaltyFactor3 = 0.001;			// 적합도 함수에서 사용하는 값
	double PanaltyFactor4 = 0.001;			// 적합도 함수에서 사용하는 값
	double LoadFactor = 1.2;				// 적합도 함수에서 사용하는 값
	double OperatorCostOption1 = 1;			// 목적 함수에서 사용하는 값
	double OperatorCostOption2 = 0;			// 목적 함수에서 사용하는 값
	double MaxRouteLength = 3;				// 최대 노선 길이
	float OperatingHoursPerDay = 10.f;		// 하루 운영 시간
	vector<uint64_t> TownBusNodesNum;		// 마을 버스가 정차하는 노드들
	uint64_t NoCrossoverNum = 2;			// 교차변이하지 않을 유전자의 수
	uint64_t PopulationNum = 8;				// 유전 집단의 수
	uint64_t MaxGeneration = 10;			// 유전 세대 수
	uint64_t CrossoverPercent = 80;			// 교차 확률 
	uint64_t MutationPercent = 1;			// 변이 확률 
	uint64_t MaxFitnessUnchangedGeneration = 3; //최고 적합도가 유지될 수 있는 세대 수(그 이상이면 초기집단생성으로 만든 유전자로 하위 유전자를 교체)
	uint64_t ExchangeChromosomeNum = 4;		// 초기집단생성 유전자로 교체할 하위 유전자의 수
	uint64_t MaxMutationOccurNum = 3;		// Mutation 발생시 변이가 발생할수 있는 최대 횟수
	vector<uint64_t> TownBusRoutes;			// GenericAlgorithm을 통하지 않고 적합도 계산만 하는 경우의 Path
	uint64_t PopulationGenerationMethod = 0;// 초기집단 생성의 방법(0 이면 기존, 1이면 새로운 방식) 
};

typedef vector<vector<string>> FileDataList;
typedef map<string, map<uint64_t, RouteData>> RouteMap;

enum class EPathFinderCostType
{
	Length = 0,
	Duration = 1,
};

struct PathFinderData
{
	vector<NodeData> Graph;
	vector<LinkData> GraphLink;
	RouteMap RouteDataMap;
	uint64_t StartNodeNum = 0;
	uint64_t EndNodeNum = 0;
	EPathFinderCostType CostType = EPathFinderCostType::Length;
	uint64_t NumberOfPath = 1;

	PathFinderData(const vector<NodeData>& InGraph, const vector<LinkData>& InLinkList, const RouteMap& InRouteMap, 
					uint64_t InStart, uint64_t InEnd, EPathFinderCostType InCostType, uint64_t InPathNum)
		: Graph(InGraph)
		, GraphLink(InLinkList)
		, RouteDataMap(InRouteMap)
		, StartNodeNum(InStart)
		, EndNodeNum(InEnd)
		, CostType(InCostType)
		, NumberOfPath(InPathNum)
	{ }

	PathFinderData(const vector<NodeData>& InGraph, uint64_t InStart, uint64_t InEnd, EPathFinderCostType InCostType, uint64_t InPathNum)
		: Graph(InGraph)
		, StartNodeNum(InStart)
		, EndNodeNum(InEnd)
		, CostType(InCostType)
		, NumberOfPath(InPathNum)
	{ }
};

enum class ETransportationType
{
	Bus = 0,
	Train = 1,
	TownBus = 2,
};

struct TransferData
{
	ETransportationType Type = ETransportationType::Bus;
	float TransferTime = 0.f;
	uint64_t DispatchesPerHour = 0;

	TransferData(ETransportationType InType, float InTransferTime, uint64_t InDispatches)
		: Type(InType)
		, TransferTime(InTransferTime)
		, DispatchesPerHour(InDispatches)
	{ }
};

struct OVTTData
{
	float InitialDispatchesPerHour = 0.f;
	vector<TransferData> TransferList;
	float OVTT = 0.f;
	uint16_t CTPI = 0;

	void Clear()
	{
		InitialDispatchesPerHour = 0.f;
		TransferList.clear();
		OVTT = 0.f;
		CTPI = 0;
	}
};

struct RouteCostData
{
	uint32_t NumberOfUsers = 0;
	double CostPerRoute = 0;
};

struct TownBusRouteData
{
	string RouteName = "";
	vector<pair<NodeData, bool>> TownBusStopCheck;
	double RouteCostPerPerson = 0.f;
	uint64_t NumOfPassengers = 0;
};
struct ShortestPathData
{
	int Order = 0;
	vector<NodeData> Path;
	TownBusRouteData TownBusData;
	float Cost = 0.f;	// IVTT + OVTT
	float IVTT = 0.f;	// TrainIVTT + BusIVTT + TownBusIVTT
	float TrainIVTT = 0.f;
	float BusIVTT = 0.f;
	float TownBusIVTT = 0.f;
	OVTTData Transfer;
	uint32_t TrafficVolumeForPath = 0;
	vector<string> PathRouteList;
};

enum class EWriteFileType
{
	ChromosomeData = 0,
	ShortestPath = 1,
	Result = 2,
};

struct FitnessResultData
{
	double TotalCustomerCost = 0;
	double TotalRouteDistance = 0;
	double TownBusOperatorCost = 0;
	double NumberOfBuses = 0;
	uint64_t NumberOfTownBusRoutes = 0;
	double ObjectFunctionValue = 0;
	double FitnessValue = 0;
	double P1Value = 0;
	double P2Value = 0;
	double P3Value = 0;
};

struct GenerationBestResultData
{
	FitnessResultData FitnessData;
	uint64_t ChromosomeNumber = 0;
};