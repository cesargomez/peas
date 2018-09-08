#include "work.hpp"

int Work::getJobNumber (void)
{
    return this->jobNumber;
}
int Work::getSubmitTime (void)
{
    return this->submitTime;
}
int Work::getWaitTime (void)
{
    return this->waitTime;
}
int Work::getRunTime (void)
{
    return this->runTime;
}
int Work::getAllocatedProcessors (void)
{
    return this->allocatedProcessors;
}
int Work::getAverageCPUTimeUsed (void)
{
    return this->averageCPUTimeUsed;
}
int Work::getUsedMemory (void)
{
    return this->usedMemory;
}
int Work::getRequestedNumberProcessors (void)
{
    return this->requestedNumberProcessors;
}
int Work::getRequestedTime (void)
{
    return this->requestedTime;
}
int Work::getRequestedMemory (void)
{
    return this->requestedMemory;
}
int Work::getStatus (void)
{
    return this->status;
}
int Work::getUserId (void)
{
    return this->userId;
}
int Work::getGroupId (void)
{
    return this->groupId;
}
int Work::getAppNumber (void)
{
    return this->appNumber;
}
int Work::getQueueNumber (void)
{
    return this->queueNumber;
}
int Work::getPartitionNumber (void)
{
    return this->partitionNumber;
}
int Work::getPrecedingJobNumber (void)
{
    return this->precedingJobNumber;
}
int Work::getThinkTimeFromPrecedingJob (void)
{
    return this->thinkTimeFromPrecedingJob;
}
float Work::getPowerUsage(void)
{
    return this->powerUsage;
}

unsigned int Work::getRealRunTime(void)
{
    return this->realRunTime;
}

unsigned int Work::getRealFinishTime(void)
{
    return this->realFinishTime;
}

unsigned int Work::getRealStartTime(void)
{
    return this->realStartTime;
}

unsigned int Work::getRealResponseTime(void)
{
    return this->getRealStartTime()+this->getRealRunTime();
}

void Work::setPowerUsage(float number)
{
    this->powerUsage=number;
}

void Work::setRealRunTime(unsigned int number)
{
    this->realRunTime=number;
}

void Work::setRealFinishTime(unsigned int number)
{
    this->realFinishTime=number;
}

void Work::setRealStartTime(unsigned int number)
{
    this->realStartTime=number;
}
void Work::setRealResponseTime(unsigned int number)
{
    this->realResponseTime=number;
}

void Work::setJobNumber (int number)
{
    this->jobNumber=number;
}
void Work::setSubmitTime (int number)
{
    this->submitTime=number;
}
void Work::setWaitTime (int number)
{
    this->waitTime=number;
}
void Work::setRunTime (int number)
{
    this->runTime=number;
}
void Work::setAllocatedProcessors (int number)
{
    this->allocatedProcessors=number;
}
void Work::setAverageCPUTimeUsed (int number)
{
    this->averageCPUTimeUsed=number;
}
void Work::setUsedMemory (int number)
{
    this->usedMemory=number;
}
void Work::setRequestedNumberProcessors (int number)
{
    this->requestedNumberProcessors=number;
}
void Work::setRequestedTime (int number)
{
    this->requestedTime=number;
}
void Work::setRequestedMemory (int number)
{
    this->requestedMemory=number;
}
void Work::setStatus (int number)
{
    this->status=number;
}
void Work::setUserId (int number)
{
    this->userId=number;
}
void Work::setGroupId (int number)
{
    this->groupId=number;
}
void Work::setAppNumber (int number)
{
    this->appNumber=number;
}
void Work::setQueueNumber (int number)
{
    this->queueNumber=number;
}
void Work::setPartitionNumber (int number)
{
    this->partitionNumber=number;
}
void Work::setPrecedingJobNumber (int number)
{
    this->precedingJobNumber=number;
}
void Work::setThinkTimeFromPrecedingJob (int number)
{
    this->thinkTimeFromPrecedingJob=number;
}

Work::Work()
{
    this->realRunTime=0;
    this->realFinishTime=0;
    this->powerUsage=0;
}

Work::~Work()
{

}

void Work::printWork(void)
{
    std::cout<<"#########################################################"<<std::endl;
    std::cout<<"Job number: "<<this->getJobNumber()<<std::endl;
    std::cout<<"Submit time: "<<this->getSubmitTime ()<<std::endl;
    //std::cout<<"Wait time: "<<this->getWaitTime ()<<std::endl;
    std::cout<<"Run time: " <<this->getRunTime ()<<std::endl;
    std::cout<<"Allocated processors: " <<this->getAllocatedProcessors ()<<std::endl;
    //std::cout<<"Average CPU time used: " <<this->getAverageCPUTimeUsed ()<<std::endl;
    //std::cout<<"Used memory: " <<this->getUsedMemory ()<<std::endl;
    std::cout<<"Requested number processors: " <<this->getRequestedNumberProcessors ()<<std::endl;
    //std::cout<<"Requested time: " <<this->getRequestedTime ()<<std::endl;
    //std::cout<<"Requested memory: " <<this->getRequestedMemory ()<<std::endl;
    //std::cout<<"Status: " <<this->getStatus ()<<std::endl;
    //std::cout<<"User Id: " <<this->getUserId ()<<std::endl;
    //std::cout<<"Group Id: " <<this->getGroupId ()<<std::endl;
    //std::cout<<"App Number: " <<this->getAppNumber ()<<std::endl;
    //std::cout<<"Queue Number: " <<this->getQueueNumber ()<<std::endl;
    //std::cout<<"Partition Number: " <<this->getPartitionNumber ()<<std::endl;
    //std::cout<<"Preceding job number: " <<this->getPrecedingJobNumber ()<<std::endl;
    //std::cout<<"Think time from preceding job: " <<this->getThinkTimeFromPrecedingJob ()<<std::endl;
    std::cout<<"-----------------------------------------------------------"<<std::endl;
    std::cout<<"Power Usage: "<<this->getPowerUsage()<<" KWh."<<std::endl;
    std::cout<<"Real Run Time: "<<this->getRealRunTime()<<std::endl;
    std::cout<<"Real Start Time: "<<this->getRealStartTime()<<std::endl;
    std::cout<<"Real Finish Time: "<<this->getRealFinishTime()<<std::endl;
    //std::cout<<"#########################################################"<<std::endl;
}
