#pragma once

#include "../Data/DataTypes.h"

using namespace std;

class PopulationMember
{
public:
	PopulationMember(vector<NodeData> BusNode, vector<NodeData> RailNode);
	const vector<NodeData>& GetSelectedLink() { return SelectedLink; };

private:

	void SelectRailNode();
	void SelectBusNode();

	vector<NodeData> LeftBusNode;
	vector<NodeData> SelectedBusNode;
	vector<NodeData> RailNode;
	vector<NodeData> SelectedLink;
};


class Population
{
public:
	Population(int MemberNum);
	const vector<PopulationMember>& GetPopulation() { return PopulationMemberList; };

private:
	int MemberNum;
	vector<PopulationMember> PopulationMemberList;
	vector<NodeData> BusNode;
	vector<NodeData> RailNode;
};
