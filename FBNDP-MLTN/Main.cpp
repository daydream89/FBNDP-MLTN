#include "CSVReader/CSVReader.h"
#include "Data/DataCenter.h"
#include "Data/DataTypes.h"
#include "Fitness/FitnessCalculator.h"
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

	int PopulationNum = 1;
	// Generate initial population
	Population InitialPopulation(PopulationNum);

	// Calculate fitness
	for (int i = 0; i < PopulationNum; ++i)
	{
		auto ChromosomeData = InitialPopulation.GetChromosome(i);
		FitnessCalculator Fitness(ChromosomeData.GetChromosome(), 2);
	}

	return 0;
}