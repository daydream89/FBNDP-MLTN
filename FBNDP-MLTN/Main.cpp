#include "Data/DataCenter.h"
#include "Data/DataTypes.h"
#include "File/CSVReader.h"
#include "File/CSVWriter.h"
#include "Fitness/FitnessCalculator.h"
#include "Population/Population.h"
#include "Util/Utils.h"
#include <stdio.h>

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
		double BestFitnessResult = 0;
		uint64_t KeepedFitnessGen = 0;
		for (uint64_t GenerationNum = 0 ; GenerationNum < MaxGeneration; ++GenerationNum)
		{
			printf("\n\n%llust Generation Data\n", GenerationNum + 1);
			Writer.WriteCSVFile(GenerationNum+1, DataCenter->GetAllChromosomeRoutesDataRef());
			// Calculate fitness
			for (int i = 0; i < PopulationNum; ++i)
			{
				FitnessCalculator Fitness(i, 2);
				FitnessResultData ChromosomeFitnessResult = Fitness.Calculate();
				InitialPopulation.GetChromosomeRef(i).SetFitnessResult(ChromosomeFitnessResult);
				printf("Finished Calculate %dth Chromosome.\n", i);

				Writer.WriteCSVFile(GenerationNum+1, i+1, DataCenter->GetShortestPathDataList());
			}

			uint64_t BestChromosomeNum = -1;
			if (Util::FindBestChromosome(InitialPopulation, BestChromosomeNum))
			{
				Chromosome BestChromosome = InitialPopulation.GetChromosome(BestChromosomeNum);
				DataCenter->AddBestResultData(GenerationNum + 1, BestChromosomeNum, BestChromosome.GetFitnessResult());

				if (BestFitnessResult < BestChromosome.GetFitnessValue())
				{
					BestFitnessResult = BestChromosome.GetFitnessValue();
					KeepedFitnessGen = 0;
				}
				else
				{
					KeepedFitnessGen++;
					if (KeepedFitnessGen >= DataCenter->GetUserInputData().MaxFitnessUnchangedGeneration) {
						uint64_t ExchangingChromosomeNum = DataCenter->GetUserInputData().ExchangeChromosomeNum;
						for (uint64_t RemoveCount = 0; RemoveCount < ExchangingChromosomeNum; ++RemoveCount)
						{
							uint64_t WorstChromosomeNum = -1;
							if (Util::FindWorstChromosome(InitialPopulation, WorstChromosomeNum))
							{
								InitialPopulation.RemoveChromosomeArrayAt(WorstChromosomeNum);
							}
						}
						InitialPopulation.AddInitialChromosome(ExchangingChromosomeNum);
						KeepedFitnessGen = 0;
					}
				}
			}

			InitialPopulation.GetNextGeneration();
		}

		Writer.WriteCSVFile(DataCenter->GetAllBestResultData());
	}

	printf("\nCalculation Finished! Press any key.\n");
	getchar();

	return 0;
}