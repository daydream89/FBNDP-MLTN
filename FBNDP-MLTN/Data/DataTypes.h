#pragma once

#include <string>
#include <vector>

using namespace std;

typedef vector<vector<string>> FileDataList;

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
	uint32_t X;
	uint32_t Y;

	Coordinate()
		: X(0)
		, Y(0)
	{ }

	Coordinate(uint32_t XCoord, uint32_t YCoord)
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
	uint32_t Num = -1;			// node ��ȣ
	Coordinate Coord;			// 2���� ��ǥ
	float TransferTime = 0.f;	// ȯ�� �ð�
	NodeType Type;				// node ����
};

// ��Ʈ��ũ ��ũ�� ���� data
struct LinkData
{
	uint32_t FromNodeNum = 0;					// ���� node
	uint32_t ToNodeNum = 0;						// ���� node
	float Length = 0.f;							// ���� ���� (���� �Ÿ� �ƴ�)
	LinkType Type = LinkType::SiGoonDo;			// link ����
	float Speed = 0.f;							// link �ӵ� (������ �ش� ��ũ�� ����ϴ� �ӵ�)
};

// ���߱��� �뼱�� ��� data
struct RouteData
{
	string Name = "";			// Route, �뼱 �̸�
	uint32_t Order = 0;			// �뼱�� node ����
	float CumDistance = 0.f;	// ���� �Ÿ�
	uint32_t Node = 0;			// node ��ȣ
};

// ���߱��� �뼱�� � ����
struct OperatingData
{
	string Name = "";		// Route, �뼱 �̸�
	uint32_t Dispatch = 0;	// ���� ����
	float Speed = 0.f;		// ���� �Ǵ� ��ö�� ��Ǵ� ��ռӵ�
};

// ���� -> ���� ���෮
struct TrafficVolumeData
{
	uint32_t FromNodeNum = 0;	// ���� node
	uint32_t ToNodeNum = 0;		// ���� node
	uint32_t TrafficVolume = 0;	// ���෮
};

// ��� �� �����Ÿ�
struct DistanceData
{
	uint32_t FromNodeNum = 0;	// ���� node
	uint32_t ToNodeNum = 0;		// ���� node
	float Distance = 0.f;		// ���� �Ÿ�
};

// ���α׷� ������ �ʿ��� �߰� ������ ����
struct UserInputData
{
	float PassageTimeDiff = 5.f;

	MNLCoefficientData MNLCoefficient;
};

enum class EPathFinderCostType
{
	Length = 0,
	Duration = 1,
};

struct PathFinderData
{
	vector<NodeData> Graph;
	uint32_t StartNodeNum = 0;
	uint32_t EndNodeNum = 0;
	EPathFinderCostType CostType = EPathFinderCostType::Length;
	uint32_t NumberOfPath = 1;

	PathFinderData(const vector<NodeData>& InGraph, uint32_t InStart, uint32_t InEnd, EPathFinderCostType InCostType, uint32_t InPathNum)
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
	float Cost = 0.f;
};