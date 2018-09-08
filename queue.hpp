#ifndef QUEUE_HPP
#define QUEUE_HPP

#include "server.hpp"
#include "work.hpp"

class Queue
{
    std::vector<Server *> servers;
    float AverageSSJ_opsPerCoreAt100;
    public:
        Queue();
        ~Queue();
        void addServer(Server *server);
        Server *getServer(unsigned int pos);
        unsigned int getNumberServers();
        void readServerList(std::string fileName);
        void printServerList(void);
        void calculateAverageSSJ_opsPerCoreAt100(void); //Get the average SSJ_OPs per core at 100% target load
        float getAverageSSJ_opsPerCoreAt100(void); //Get the average SSJ_OPs per core at 100% target load
        unsigned int getIdleCores(void);
        unsigned int getTotalCores(void);
        void allocateWork_FirstFit(Work *work, unsigned int simulationTime);
        void allocateWork_MOHEFT(Work *work, unsigned int simulationTime, std::vector<std::vector<int> > &solution);
        void allocateWork_NSGA_II(Work *work, unsigned int simulationTime, std::vector<std::vector<int> > &solution);
        void calculateWorkPowerUsage(Work *work);
        void FreeServers(unsigned int JobNumber);
        void printAssignedWorks(void);
};

#endif
