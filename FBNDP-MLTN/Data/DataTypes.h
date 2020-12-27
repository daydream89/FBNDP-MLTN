#pragma once

#include <string>

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
	uint32_t X;
	uint32_t Y;
};

// ��Ʈ��ũ ��忡 ���� data
struct NodeData
{
	uint32_t Num;			// node ��ȣ
	Coordinate Coord;		// 2���� ��ǥ
	uint32_t U;	// not use
	float TransferTime;		// ȯ�� �ð�
	NodeType Type;			// node ����
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