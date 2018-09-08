#include <iostream>
#include <vector>
#include <climits>
#include <cmath>
#include "work.hpp"

class WorkPool
{
    std::vector<Work *> works;
    unsigned int totalWaitTime=0;

    public:
        WorkPool();
        void readWorkList(const char *filename);
        void addWork(Work *work);
        Work *getWork(unsigned int pos);
        void deleteWork(unsigned int pos);
        void printWorkList(void);
        unsigned int getLastSubmitTime(void);
        unsigned int getNumberWorks(void);
        unsigned int findWork(Work *w);
        ~WorkPool();

        //User-centric metrics
        void updateTotalWaitTime(unsigned int waitTime);
        float calculateAWT(void);
        float calculateStandardDeviationWT(void);
        float calculateART(void);
        float calculateStandardDeviationRT(void);
        float calculateAET(void);
        float calculateSlowDown(void);
        float calculateBoundedSlowDown(void);
        float calculatePerProcessorBoundedSlowDown(void);
        unsigned int getNextEvent(unsigned int simulationTime);
};
