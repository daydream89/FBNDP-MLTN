#pragma once

#include "../Data/DataTypes.h"

using namespace std;

struct NodeSelectPercent
{
	float Percentage;
	uint32_t BusNodeNum;
};



class PopulationMember 
{
public:
	PopulationMember(const vector<NodeData>& BusNode, 
		const vector<NodeData>& RailNode);
	const vector<LinkData>& GetSelectedLink() { return SelectedLink; };

private:
	void CopyBusNode(const vector<NodeData>& BusNode) 
	{	
		LeftBusNode.assign(BusNode.begin(), BusNode.end()); 
	}
	void CopyRailNode(const vector<NodeData>& RailNode)
	{
		MemberRailNode.assign(RailNode.begin(), RailNode.end());
	}

	NodeData SelectRailNode();

	vector<NodeSelectPercent> GenRouletteWheelPercent();
	NodeData SelectBusNode(vector<NodeSelectPercent> NodePercent);

	void AddBusLink();
	void CalculateNetworkCost();
	void DeleteBusNode();

	vector<NodeData> LeftBusNode;
	vector<NodeData> SelectedBusNode;
	vector<NodeData> MemberRailNode;
	vector<LinkData> SelectedLink;
};

#define MAX_POPULATION_MEMBER_NUM 8

class Population	/* Change to Singleton */
{
public:
	static Population* GetInstance()
	{
		if (!Instance)
		{
			Instance = new Population(MemberNum);
		}
		
		return Instance;
	}

private:
	Population(int MemberNum);
	void SetNodes(void);

	static Population* Instance;
	static const int MemberNum = MAX_POPULATION_MEMBER_NUM;
	vector<PopulationMember> PopulationMemberList; 
	vector<NodeData> BusNode;
	vector<NodeData> RailNode;
};
