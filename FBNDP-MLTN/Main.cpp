#include "Data/DataCenter.h"
#include "Data/DataTypes.h"
#include "File/CSVReader.h"
#include "File/CSVWriter.h"
#include "Fitness/FitnessCalculator.h"
#include "Population/Population.h"

using namespace std;

int main(int argc, char* argv[])
{
	CSVWriter Writer;

	if (auto* DataCenter = DataCenter::GetInstance())
	{
		// file read
		CSVReader Reader;
		DataCenter->SetLinkData(Reader.GetFileData("./Input/1_Link.csv"));
		DataCenter->SetNodeData(Reader.GetFileData("./Input/2_Node.csv"));
		DataCenter->SetRouteData(Reader.GetFileData("./Input/3_transit_route_node_info.csv"));
		DataCenter->SetOperatingData(Reader.GetFileData("./Input/4_transit_route_oper_info.csv"));
		DataCenter->SetTrafficVolumeData(Reader.GetFileData("./Input/5_OD_demand.csv"));
		DataCenter->SetDistanceData(Reader.GetFileData("./Input/6_Direct_distance.csv"));
		DataCenter->SetUserInputData(Reader.GetFileData("./Input/0_UserInput.csv"));

		// Generate initial population
		uint64_t PopulationNum = DataCenter->GetUserInputData().PopulationNum;
		Population InitialPopulation(PopulationNum);

		uint64_t MaxGeneration = DataCenter->GetUserInputData().MaxGeneration;
		for (uint64_t GenerationNum = 0 ; GenerationNum < MaxGeneration; ++GenerationNum)
		{
			printf("\n\n%llust Generation Data\n", GenerationNum + 1);
			Writer.WriteCSVFile(GenerationNum+1, DataCenter->GetAllChromosomeRoutesDataRef());
			// Calculate fitness
			for (int i = 0; i < PopulationNum; ++i)
			{
				FitnessCalculator Fitness(i, 2);
				double ChromosomeFitness = Fitness.Calculate();
				InitialPopulation.GetChromosomeRef(i).SetFitnessValue(ChromosomeFitness);
				printf("Finished Calculate %dth Chromosome.\n", i);

				Writer.WriteCSVFile(GenerationNum+1, i+1, DataCenter->GetShortestPathDataList());
			}
			InitialPopulation.GetNextGeneration();
		}
	}

	printf("\nCalculation Finished! Press any key.\n");
	getchar();

	return 0;
}