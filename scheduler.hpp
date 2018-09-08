#include "queue.hpp"
#include "workpool.hpp"
#include <iostream>
#include <climits>
#include "nsga-ii.hpp"
#include "ABC.hpp"
#include "moheft.hpp"
#include "types.hpp"

class Scheduler
{
    unsigned int schedulingPolicy;
    unsigned int selectionPolicy;
    double elapsedSeconds=0;
    unsigned int position = 0;


    Queue *serverPool;
    WorkPool *workload;
    WorkPool *finishedJobs;
    WorkPool *acceptedJobs;
    WorkPool *unacceptedJobs;
    WorkPool *queuedJobs;

    bool Starved[MAXJOBNUMBER]={ };

    public:
        Scheduler(unsigned int schePolicy, unsigned int selPolicy);
        ~Scheduler();
        void readWorkload (std::string filename);
        void readServerPool (std::string filename);
        void printWorkLoad(void);
        void printServerPool(void);
        void runSimulation(void);
        void PolicyScheduler(unsigned int simulationTime);
        void SP_FCFS(unsigned int simulationTime);
        void SP_FCFS_EASY_Backfilling(unsigned int simulationTime);
        void SP_FCFS_FATTENED_Backfilling(unsigned int simulationTime);
        void SP_FCFS_CD_EASY_Backfilling(unsigned int simulationTime);
        void SP_FCFS_Conservative_Backfilling(unsigned int simulationTime);
        void ResourceSelector(unsigned int simulationTime);
        void RSP_FirstFit(unsigned int simulationTime);
        void RSP_MOHEFT(unsigned int simulationTime);
        void RSP_NSGA_II(unsigned int simulationTime);
        void RSP_ABC(unsigned int simulationTime);


        void FreeResources(unsigned int finishTime);
        float CalculateTotalWorkPowerConsumption(void);
        unsigned int calculateJobStartTime(Work *work, unsigned int simulationTime);
        unsigned int calculateNumberFreeCores(unsigned int simulationTime);

        void EvaluationMetrics(void);
};
