#include "scheduler.hpp"
#include <iostream>
#include "ConfigFile.hpp"
#include <random>

int main (int argc, char* argv[])
{

    ConfigFile cf(argv[1]);

    double schedulingPolicy = cf.Value("Simulator","schedulingPolicy"); //FCFS=0, FCFS_Conservative_Backfilling=1, FCFS_EASY_Backfilling=2, Fattened_Backfilling=3
    double selectionPolicy = cf.Value("Simulator","selectionPolicy"); //FirstFit=0, MOHEFT=1, NSGAII=2, ABC=3

    std::string configFile = cf.Value("ClusterConfigurationFile","configFile");
    std::string workload = cf.Value("WorkloadFile","workload");

    Scheduler scheduler(schedulingPolicy,selectionPolicy);
    scheduler.readServerPool(configFile);
    scheduler.readWorkload(workload);
    scheduler.runSimulation();
	return 0;
}
