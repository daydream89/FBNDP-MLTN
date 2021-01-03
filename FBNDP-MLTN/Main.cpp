#include "CSVReader/CSVReader.h"
#include "Data/DataCenter.h"
#include "Data/DataTypes.h"
#include "Population/Population.h"

using namespace std;

int main(int argc, char* argv[])
{
	CSVReader Reader;

	// file read
	if (auto* DataCenter = DataCenter::GetInstance())
	{
		DataCenter->SetLinkData(Reader.GetFileData("./Input/1_Link.csv"));
		DataCenter->SetNodeData(Reader.GetFileData("./Input/2_Node.csv"));
		DataCenter->SetRouteData(Reader.GetFileData("./Input/3_transit_route_node_info.csv"));
		DataCenter->SetOperatingData(Reader.GetFileData("./Input/4_transit_route_oper_info.csv"));
		DataCenter->SetTrafficVolumeData(Reader.GetFileData("./Input/5_OD_demand.csv"));
		DataCenter->SetDistanceData(Reader.GetFileData("./Input/6_Direct_distance.csv"));
	}

	// Generate initial population
	
	/*
	if (auto* Population = Population::GetInstance())
	{
		printf("Initial Population Generated\n");
	}
	else
	{
		printf("Initial Population Generation fail\n");
	}
	*/

	// Calculate fitness


	return 0;
}