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
	uint32_t X;
	uint32_t Y;
};

// 네트워크 노드에 대한 data
struct NodeData
{
	uint32_t Num;			// node 번호
	Coordinate Coord;		// 2차원 좌표
	uint32_t U;	// not use
	float TransferTime;		// 환승 시간
	NodeType Type;			// node 종류
};

// 네트워크 링크에 대한 data
struct LinkData
{
	uint32_t FromNodeNum;	// 시작 node
	uint32_t ToNodeNum;		// 종료 node
	float Length;			// 실제 길이 (직선 거리 아님)
	LinkType Type;			// link 종류
	float Speed;			// link 속도 (버스가 해당 링크를 통과하는 속도)
};

// 대중교통 노선의 노드 data
struct RouteData
{
	string Name;		// Route, 노선 이름
	uint32_t Order;		// 노선의 node 순서
	float CumDistance;	// 누적 거리
	uint32_t Node;		// node 번호
};

// 대중교통 노선의 운영 정보
struct OperatingData
{
	string Name;		// Route, 노선 이름
	uint32_t Dispatch;	// 배차 간격
	float Speed;		// 버스 또는 전철이 운영되는 평균속도
};

// 시점 -> 종점 통행량
struct TrafficVolumeData
{
	uint32_t FromNodeNum;	// 시작 node
	uint32_t ToNodeNum;		// 종료 node
	uint32_t TrafficVolume;	// 통행량
};

// 노드 간 직선거리
struct DistanceData
{
	uint32_t FromNodeNum;	// 시작 node
	uint32_t ToNodeNum;		// 종료 node
	float Distance;			// 직선 거리
};