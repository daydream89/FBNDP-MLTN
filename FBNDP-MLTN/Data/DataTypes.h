#pragma once

#include <string>
#include <vector>
#include <map>

using namespace std;

enum class NodeType
{
	BusStop = 0,
	Station,
	NodeTypeMax,
};

enum class LinkType
{
	NationalExpressway = 101,	// ��ӱ���
	Expressway = 102,			// ���� ���ȭ ����
	GeneralNationalRoad = 103,	// �Ϲ� ����
	MetropolitanRoad = 104,		// Ư��.������ ����
	GookJiDo = 105,				// �����������浵
	LocalLoad = 106,			// ���浵
	SiGoonDo = 107,				// �ñ���
	HighwayRamp = 108,			// ��ӵ��� ���� ����
	Rail = 200,					// ö��
	CentroidConnector = 999,	// ��Ʈ���̵� Ŀ����
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

// ��Ʈ��ũ ��忡 ���� data
struct NodeData
{
	uint64_t Num = -1;			// node ��ȣ
	Coordinate Coord;			// 2���� ��ǥ
	float TransferTime = 0.f;	// ȯ�� �ð�
	NodeType Type;				// node ����
};

// ��Ʈ��ũ ��ũ�� ���� data
struct LinkData
{
	uint64_t FromNodeNum = 0;					// ���� node
	uint64_t ToNodeNum = 0;						// ���� node
	float Length = 0.f;							// ���� ���� (���� �Ÿ� �ƴ�)
	LinkType Type = LinkType::SiGoonDo;			// link ����
	float Speed = 0.f;							// link �ӵ� (������ �ش� ��ũ�� ����ϴ� �ӵ�)
};

/* ���߱��� �뼱�� ��� data.
	Name�� Order�� DataCenter�� ����map���� ���� */
struct RouteData
{
	float CumDistance = 0.f;	// ���� �Ÿ�
	uint64_t Node = 0;			// node ��ȣ
};

/* ���߱��� �뼱�� � ����.
	Name�� DataCenter�� map���� ���� */
struct OperatingData
{
	uint64_t Dispatch = 0;	// ���� ����
	float Speed = 0.f;		// ���� �Ǵ� ��ö�� ��Ǵ� ��ռӵ�
};

// ���� -> ���� ���෮
struct TrafficVolumeData
{
	uint64_t FromNodeNum = 0;	// ���� node
	uint64_t ToNodeNum = 0;		// ���� node
	uint64_t TrafficVolume = 0;	// ���෮
};

// ��� �� �����Ÿ�
struct DistanceData
{
	uint64_t FromNodeNum = 0;	// ���� node
	uint64_t ToNodeNum = 0;		// ���� node
	float Distance = 0.f;		// ���� �Ÿ�
};

// ���α׷� ������ �ʿ��� �߰� ������ ����
struct UserInputData
{
	float PassageTimeDiff = 5.f;			// ���յ� ��� ���� ���� �ð� ����
	MNLCoefficientData MNLCoefficient;		// MNL Model���� ����ϴ� ��� ����
	uint64_t BusDispatchesPerHour = 5;		// ���� ���� Ƚ�� (ȸ/hour)
	uint64_t TrainDispatchesPerHour = 4;	// ��ö ���� Ƚ�� (ȸ/hour)
	float TownBusTimeCost = 10000.f;		// �������� ž�� �ð� ��� (��/��*�ð�)
	float BusTimeCost = 10000.f;			// ���� ž�� �ð� ��� (��/��*�ð�)
	float TrainTimeCost = 10000.f;			// ��ö ž�� �ð� ��� (��/��*�ð�)
	float WaitTimeCost = 10000.f;			// ��� �ð� ��� (��/��*�ð�)
	float TownBusSpeed = 30.f;				// �������� � �ӵ� (km/h)
	float TownBusOperationCost = 1600.f;	// �������� � ��� (��/��*km)
	uint64_t TownBusDispatchesPerHour = 12;	// �������� ���� Ƚ�� (ȸ/hour)
	float PanaltyFactor = 0.0001f;			// ���յ� �Լ����� ����ϴ� ��
};

enum class EPathFinderCostType
{
	Length = 0,
	Duration = 1,
};

struct PathFinderData
{
	vector<NodeData> Graph;
	uint64_t StartNodeNum = 0;
	uint64_t EndNodeNum = 0;
	EPathFinderCostType CostType = EPathFinderCostType::Length;
	uint64_t NumberOfPath = 1;

	PathFinderData(const vector<NodeData>& InGraph, uint64_t InStart, uint64_t InEnd, EPathFinderCostType InCostType, uint64_t InPathNum)
		: Graph(InGraph)
		, StartNodeNum(InStart)
		, EndNodeNum(InEnd)
		, CostType(InCostType)
		, NumberOfPath(InPathNum)
	{ }
};

struct ShortestPathData
{
	vector<NodeData> Path;
	float Cost = 0.f;	// IVTT + OVTT
	float IVTT = 0.f;
	float OVTT = 0.f;
	uint16_t CTPI = 0;
	uint32_t TrafficVolumeForPath = 0;
};

typedef vector<vector<string>> FileDataList;
typedef map<string, map<uint64_t, RouteData>> RouteMap;