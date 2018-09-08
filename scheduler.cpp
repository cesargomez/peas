#include "scheduler.hpp"
#include <time.h>

Scheduler::Scheduler(unsigned int schePolicy, unsigned int selPolicy)
{
    this->schedulingPolicy=schePolicy;
    this->selectionPolicy=selPolicy;
    workload=new(WorkPool);
    serverPool=new(Queue);
    finishedJobs=new(WorkPool);
    acceptedJobs=new(WorkPool);
    unacceptedJobs=new(WorkPool);
    queuedJobs=new(WorkPool);
}

Scheduler::~Scheduler()
{
    delete (workload);
    delete (serverPool);
    delete (finishedJobs);
    delete (acceptedJobs);
    delete (unacceptedJobs);
    delete (queuedJobs);
}

void Scheduler::readWorkload(std::string filename)
{
    workload->readWorkList(filename.c_str());
}

void Scheduler::readServerPool(std::string filename)
{
    serverPool->readServerList(filename);
    serverPool->calculateAverageSSJ_opsPerCoreAt100();
}

void Scheduler::printWorkLoad(void)
{
    workload->printWorkList();
}

void Scheduler::printServerPool(void)
{
    serverPool->printServerList();
}

void Scheduler::runSimulation(void)
{
    unsigned int simulationTime = -1; //Simulation time starts at 0
    unsigned int lastSubmitTime = 0;

    lastSubmitTime = workload->getLastSubmitTime();
    while (workload->getNumberWorks() > (finishedJobs->getNumberWorks()+unacceptedJobs->getNumberWorks()))
    {

        simulationTime=workload->getNextEvent(simulationTime+1);
        this->PolicyScheduler(simulationTime);
        this->ResourceSelector(simulationTime);
        this->FreeResources(simulationTime);
    }
    finishedJobs->printWorkList();

    std::cout<<"-----------------------PERFORMANCE METRICS------------------------"<<std::endl;
    this->EvaluationMetrics();
    //Calculate total work power consumption
    std::cout<<"-----------------------TOTAL POWER CONSUMPTION------------------------"<<std::endl;
    std::cout<<"Power consumption: "<<this->CalculateTotalWorkPowerConsumption()<<" KWh."<<std::endl;
    std::cout<<"-----------------------FINISHED------------------------"<<std::endl;

}

void Scheduler::PolicyScheduler(unsigned int simulationTime)
{
    unsigned int policy=schedulingPolicy;
    clock_t tic = clock();

    switch (policy)
    {
        case 0: SP_FCFS(simulationTime);break;
        case 1: SP_FCFS_Conservative_Backfilling(simulationTime);break;
        case 2: SP_FCFS_EASY_Backfilling(simulationTime);break;
        case 3: SP_FCFS_FATTENED_Backfilling(simulationTime);break;
        /*case 4: SP_FCFS_CD_EASY_Backfilling(simulationTime);break;*/
        default: SP_FCFS(simulationTime); break;
    }
    clock_t toc = clock();
    elapsedSeconds += (double)(toc - tic) / CLOCKS_PER_SEC;

}

void Scheduler::SP_FCFS(unsigned int simulationTime)
{
    Work *work;

    unsigned int numberJobs = workload->getNumberWorks();
    while ((position<numberJobs))
    {
        work=workload->getWork(position);

        if (work->getSubmitTime() == simulationTime)
        {

            if (work->getAllocatedProcessors()>serverPool->getTotalCores())
            {
                unacceptedJobs->addWork(work);
            }

            else
            {
                queuedJobs->addWork(work);
                std::cout<<"[Simulation time: "<<simulationTime<<"] [SCHEDULER] Job "<<work->getJobNumber()<<" is queued (FCFS)."<<std::endl;
            }
        }
        else if (work->getSubmitTime()> simulationTime) break; //Si el trabajo llega después se corta el bucle
        position++;
    }
    unsigned int idleCores = serverPool->getIdleCores();
    if (0<queuedJobs->getNumberWorks())
    {
        work=queuedJobs->getWork(0);
        if (work->getAllocatedProcessors()<=idleCores)
        {
            acceptedJobs->addWork(work);
            std::cout<<"[Simulation time: "<<simulationTime<<"] [SCHEDULER] Job "<<work->getJobNumber()<<" is accepted (enough processors on the pool)."<<std::endl;
            idleCores=idleCores-work->getAllocatedProcessors();
            queuedJobs->deleteWork(0);
        }
    }
}

unsigned int Scheduler::calculateNumberFreeCores(unsigned int simulationTime)
{
    unsigned int numberFreeCores=serverPool->getTotalCores();
    unsigned int position=0;
    Work *work;

    while (position<workload->getNumberWorks())
    {
        work=workload->getWork(position);
        if (work->getRealFinishTime()> simulationTime)
        {
            numberFreeCores=numberFreeCores-work->getAllocatedProcessors();
        }
        position++;
    }

    return numberFreeCores;
}

unsigned int Scheduler::calculateJobStartTime(Work *work, unsigned int simulationTime)
{
    unsigned int i=0;
    unsigned int numberFreeCores=0;

    for (i=simulationTime;i<UINT_MAX;i++)
    {
        numberFreeCores=this->calculateNumberFreeCores(i);
        if (work->getAllocatedProcessors()<=numberFreeCores)
        {
            return i;
        }
        i++;
    }
}

void Scheduler::SP_FCFS_Conservative_Backfilling(unsigned int simulationTime)
{
    unsigned int numberJobs = workload->getNumberWorks();
    Work *work;
    unsigned int i;

    while (position<numberJobs)
    {
        work=workload->getWork(position);

        if (work->getSubmitTime() == simulationTime)
        {
            if (work->getAllocatedProcessors()>serverPool->getTotalCores())
            {
                unacceptedJobs->addWork(work);
            }
            else
            {
                queuedJobs->addWork(work);
                std::cout<<"[Simulation time: "<<simulationTime<<"] [SCHEDULER] Job "<<work->getJobNumber()<<" has been queued."<<std::endl;
            }
        }
        else if (work->getSubmitTime()> simulationTime) break;
        position++;
    }


    unsigned int idleCores = serverPool->getIdleCores();

    unsigned int jobsStartTime[queuedJobs->getNumberWorks()];
    unsigned int jobsFinishTime[queuedJobs->getNumberWorks()];
    for (unsigned int i=0;i<queuedJobs->getNumberWorks();i++)
    {
        work=queuedJobs->getWork(i);
        jobsStartTime[i]=calculateJobStartTime(work, simulationTime);
        jobsFinishTime[i]=jobsStartTime[i]+work->getRunTime();
    }

    int queued=queuedJobs->getNumberWorks();

    for (i=queued;((i>0));i--)
    {
        bool oneDelayed=false;
        for (int j=i-1;((j>0) && !oneDelayed);j--)
        {
            if (jobsFinishTime[i-1]>=jobsStartTime[j-1])
            {
                oneDelayed=true;
            }
        }
        work=queuedJobs->getWork(i-1);
        if ((!oneDelayed) && (work->getAllocatedProcessors()<=idleCores))
        {
            acceptedJobs->addWork(work);
            idleCores=idleCores-work->getAllocatedProcessors();
            queuedJobs->deleteWork(i-1);
        }
    }

}


void Scheduler::SP_FCFS_EASY_Backfilling(unsigned int simulationTime)
{
    unsigned int numberJobs = workload->getNumberWorks();
    Work *work;
    unsigned int i;

    while (position<numberJobs)
    {
        work=workload->getWork(position);
        if (work->getSubmitTime() == simulationTime)
        {
            if (work->getAllocatedProcessors()>serverPool->getTotalCores())
            {
                unacceptedJobs->addWork(work);
            }
            else
            {
                queuedJobs->addWork(work);
                std::cout<<"[Simulation time: "<<simulationTime<<"] [SCHEDULER] Job "<<work->getJobNumber()<<" has been queued."<<std::endl;
            }
        }
        else if (work->getSubmitTime()> simulationTime) break;
        position++;
    }

    unsigned int idleCores = serverPool->getIdleCores();
    unsigned int jobsStartTime[queuedJobs->getNumberWorks()];
    unsigned int jobsFinishTime[queuedJobs->getNumberWorks()];
    unsigned int queued=queuedJobs->getNumberWorks();

    for (unsigned int i=0;i<queued;i++)
    {
        work=queuedJobs->getWork(i);
        jobsStartTime[i]=calculateJobStartTime(work, simulationTime);
        jobsFinishTime[i]=jobsStartTime[i]+work->getRunTime();
    }

    for (unsigned int i=1;i<queuedJobs->getNumberWorks();i++)
    {
        work=queuedJobs->getWork(i);
        if (jobsFinishTime[i]<jobsStartTime[0] && work->getAllocatedProcessors()<=idleCores)
        {
            acceptedJobs->addWork(work);
            idleCores=idleCores-work->getAllocatedProcessors();
            queuedJobs->deleteWork(i);
        }
    }

    if (queued)
    {
        work=queuedJobs->getWork(0);
        if (work->getAllocatedProcessors()<=idleCores)
        {
            acceptedJobs->addWork(work);
            idleCores=idleCores-work->getAllocatedProcessors();
            queuedJobs->deleteWork(0);
        }
    }
}

void Scheduler::SP_FCFS_FATTENED_Backfilling(unsigned int simulationTime)
{
    unsigned int numberJobs = workload->getNumberWorks();
    Work *work;
    unsigned int i;

    while (position<numberJobs)
    {
        work=workload->getWork(position);
        if (work->getSubmitTime() == simulationTime)
        {
            if (work->getAllocatedProcessors()>serverPool->getTotalCores())
            {
                unacceptedJobs->addWork(work);
            }
            else
            {
                queuedJobs->addWork(work);
                std::cout<<"[Simulation time: "<<simulationTime<<"] [SCHEDULER] Job "<<work->getJobNumber()<<" has been queued."<<std::endl;
            }
        }
        else if (work->getSubmitTime()> simulationTime) break;
        position++;
    }

    unsigned int idleCores = serverPool->getIdleCores();
    unsigned int jobsStartTime[queuedJobs->getNumberWorks()];
    unsigned int jobsFinishTime[queuedJobs->getNumberWorks()];
    unsigned int queued=queuedJobs->getNumberWorks();

    for (unsigned int i=0;i<queued;i++)
    {
        work=queuedJobs->getWork(i);
        jobsStartTime[i]=calculateJobStartTime(work, simulationTime);
        jobsFinishTime[i]=jobsStartTime[i]+work->getRunTime();
    }

    for (unsigned int i=1;i<queuedJobs->getNumberWorks();i++)
    {
        work=queuedJobs->getWork(i);
        unsigned int awt=finishedJobs->calculateAWT();

        if (jobsFinishTime[i]<(jobsStartTime[0]+awt) && work->getAllocatedProcessors()<=idleCores && !Starved[queuedJobs->getWork(0)->getJobNumber()])
        {
            if (jobsFinishTime[i]>=jobsStartTime[0])
            {
                Starved[queuedJobs->getWork(0)->getJobNumber()]=true;
                std::cout<<"Job["<<queuedJobs->getWork(0)->getJobNumber()<<"]"<<"Starved"<<std::endl;
            }
            acceptedJobs->addWork(work);
            idleCores=idleCores-work->getAllocatedProcessors();
            queuedJobs->deleteWork(i);
        }
    }

    if (queued)
    {
        work=queuedJobs->getWork(0);
        if (work->getAllocatedProcessors()<=idleCores)
        {
            acceptedJobs->addWork(work);
            idleCores=idleCores-work->getAllocatedProcessors();
            queuedJobs->deleteWork(0);
        }
    }
}

void Scheduler::ResourceSelector(unsigned int simulationTime)
{
    unsigned int policy=selectionPolicy;
    switch (policy)
    {
        case 0: RSP_FirstFit(simulationTime); break;
        case 1: RSP_MOHEFT(simulationTime); break;
        case 2: RSP_NSGA_II(simulationTime); break;
        case 3: RSP_ABC(simulationTime); break;
        default: RSP_FirstFit(simulationTime);
    }
}

void Scheduler::RSP_FirstFit(unsigned int simulationTime)
{
    Work *work;
    unsigned int position=0;
    unsigned int numberJobs=acceptedJobs->getNumberWorks();
    while (position < numberJobs) //While there are accepted jobs
    {
        work=acceptedJobs->getWork(0);
        std::cout<<"[Simulation time: "<<simulationTime<<"] [SCHEDULER] Job "<<work->getJobNumber()<<" is executed."<<std::endl;
        serverPool->allocateWork_FirstFit(work, simulationTime);
        serverPool->printAssignedWorks();
        serverPool->calculateWorkPowerUsage(work);
        acceptedJobs->deleteWork(0);
        position++;
    }
}

void Scheduler::RSP_MOHEFT(unsigned int simulationTime)
{
    Work *work;
    std::vector<std::vector<int> > solution;
    unsigned int position=0;
    unsigned int numberJobs=acceptedJobs->getNumberWorks();
    while (position < numberJobs) //While there are accepted jobs
    {
        work=acceptedJobs->getWork(0);
        MOHEFT *moheft= new MOHEFT();
        solution=moheft->runMOHEFT(serverPool, work);
        serverPool->allocateWork_MOHEFT(work, simulationTime, solution);
        serverPool->printAssignedWorks();
        serverPool->calculateWorkPowerUsage(work);
        acceptedJobs->deleteWork(0);
        position++;
        delete(moheft);
    }
}

void Scheduler::RSP_NSGA_II(unsigned int simulationTime)
{
    Work *work;
    std::vector<std::vector<int> > solution;
    unsigned int position=0;
    unsigned int numberJobs=acceptedJobs->getNumberWorks();
    while (position < numberJobs) //While there are accepted jobs
    {
        work=acceptedJobs->getWork(0);
        NSGAII *nsgaii= new NSGAII();
        solution=nsgaii->runNSGA(serverPool, work);
        serverPool->allocateWork_NSGA_II(work, simulationTime, solution);
        serverPool->printAssignedWorks();
        serverPool->calculateWorkPowerUsage(work);
        acceptedJobs->deleteWork(0);
        position++;
        delete(nsgaii);
    }
}

void Scheduler::RSP_ABC(unsigned int simulationTime)
{
    Work *work;
    std::vector<std::vector<int> > solution;
    unsigned int position=0;
    unsigned int numberJobs=acceptedJobs->getNumberWorks();
    while (position < numberJobs) //While there are accepted jobs
    {
        work=acceptedJobs->getWork(0);
        ABC *abc= new ABC();
        solution=abc->runABC(serverPool, work);
        serverPool->allocateWork_NSGA_II(work, simulationTime, solution);
        serverPool->printAssignedWorks();
        serverPool->calculateWorkPowerUsage(work);
        acceptedJobs->deleteWork(0);
        position++;
        delete(abc);
    }
}

/* Look for finished jobs to free their resources. Finished jobs are stored in "finishedJobs" */
void Scheduler::FreeResources(unsigned int finishTime)
{
    unsigned int position=0;
    unsigned int numberJobs=workload->getNumberWorks();
    unsigned int jobNumber=0;
    Work *work;

    while (position<numberJobs)
    {
            work=workload->getWork(position);
            if (finishTime == work->getRealFinishTime() && work->getRealFinishTime()!=0)
            {

              serverPool->FreeServers(work->getJobNumber());
              std::cout<<"Finished Job: "<<work->getJobNumber()<<std::endl;
              std::cout<<"Free cores "<<serverPool->getIdleCores()<<std::endl;
              serverPool->printAssignedWorks();
              finishedJobs->addWork(work);
              finishedJobs->updateTotalWaitTime(work->getRealStartTime()-work->getSubmitTime());
            }
            position++;
    }
}

float Scheduler::CalculateTotalWorkPowerConsumption(void)
{
    float powerConsumption=0;
    unsigned int i=0;

    for (i=0;i<this->finishedJobs->getNumberWorks();i++)
    {
        powerConsumption+=this->finishedJobs->getWork(i)->getPowerUsage();
    }
    return powerConsumption;
}


void Scheduler::EvaluationMetrics(void)
{
    std::cout.precision(10);
    //Calculate Average Waiting Time
    float awt=finishedJobs->calculateAWT();
    std::cout<<"Average Waiting Time (AWT): "<<awt<<std::endl;
    //Calculate Average Response Time
    float art=finishedJobs->calculateART();
    std::cout<<"Average Response Time (ART): "<<art<<std::endl;
    //Calculate Average Execution Time (AET)
    float aet=finishedJobs->calculateAET();
    std::cout<<"Average Execution Time (AET): "<<aet<<std::endl;

    float wtStandardDeviation=finishedJobs->calculateStandardDeviationWT();
    std::cout<<"Wait Time Standard Deviation: "<<wtStandardDeviation<<std::endl;

    float rtStandardDeviation=finishedJobs->calculateStandardDeviationRT();
    std::cout<<"Response Time Standard Deviation: "<<rtStandardDeviation<<std::endl;

    float bsld=finishedJobs->calculateBoundedSlowDown();
    std::cout<<"Bounded Slowdown (BSLD): "<<bsld<<std::endl;
    
    float ppbsld=finishedJobs->calculatePerProcessorBoundedSlowDown();
    std::cout<<"Per Processor Bounded Slowdown (PPBSLD): "<<ppbsld<<std::endl;

    std::cout<<"Scheduling elapsed time:"<<elapsedSeconds<<" seconds."<<std::endl;

}
