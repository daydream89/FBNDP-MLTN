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
	uint32_t FromNodeNum;	// ���� node
	uint32_t ToNodeNum;		// ���� node
	float Length;			// ���� ���� (���� �Ÿ� �ƴ�)
	LinkType Type;			// link ����
	float Speed;			// link �ӵ� (������ �ش� ��ũ�� ����ϴ� �ӵ�)
};

// ���߱��� �뼱�� ��� data
struct RouteData
{
	string Name;		// Route, �뼱 �̸�
	uint32_t Order;		// �뼱�� node ����
	float CumDistance;	// ���� �Ÿ�
	uint32_t Node;		// node ��ȣ
};

// ���߱��� �뼱�� � ����
struct OperatingData
{
	string Name;		// Route, �뼱 �̸�
	uint32_t Dispatch;	// ���� ����
	float Speed;		// ���� �Ǵ� ��ö�� ��Ǵ� ��ռӵ�
};

// ���� -> ���� ���෮
struct TrafficVolumeData
{
	uint32_t FromNodeNum;	// ���� node
	uint32_t ToNodeNum;		// ���� node
	uint32_t TrafficVolume;	// ���෮
};

// ��� �� �����Ÿ�
struct DistanceData
{
	uint32_t FromNodeNum;	// ���� node
	uint32_t ToNodeNum;		// ���� node
	float Distance;			// ���� �Ÿ�
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
	float Cost;
};