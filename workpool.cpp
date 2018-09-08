#include <iostream>
using std::cout;
using std::endl;

#include <fstream>
using std::ifstream;

#include <cstring>

#include "workpool.hpp"
#include "csv_parser.hpp"

WorkPool::WorkPool()
{
}

WorkPool::~WorkPool()
{
}

void WorkPool::readWorkList(const char *filename)
{

  const int MAX_CHARS_PER_LINE = 512;
  const int MAX_TOKENS_PER_LINE = 20;
  const char* const DELIMITER = " ";

  // create a file-reading object
  ifstream fin;
  fin.open(filename); // open a file
  if (!fin.good())
  {
    cout<<"File "<<filename<<" was not found."<<endl;
  }

  int nline=0;

  // read each line of the file
  while (!fin.eof())
  {
    // read an entire line into memory
    char buf[MAX_CHARS_PER_LINE];
    fin.getline(buf, MAX_CHARS_PER_LINE);

    // parse the line into blank-delimited tokens
    int n = 0; // a for-loop index
    if (19 < nline) //skip 20 description lines
    {
        // array to store memory addresses of the tokens in buf
        const char* token[MAX_TOKENS_PER_LINE] = {}; // initialize to 0

        // parse the line
        token[0] = strtok(buf, DELIMITER); // first token
        if (token[0]) // zero if line is blank
        {
          for (n = 1; n < MAX_TOKENS_PER_LINE; n++)
          {
            token[n] = strtok(0, DELIMITER); // subsequent tokens
            if (!token[n]) break; // no more tokens
          }
            // process the tokens
            Work *w=new (Work);
            w->setJobNumber (atoi(token[0]));
            w->setSubmitTime (atoi(token[1]));
            w->setWaitTime (atoi(token[2]));
            w->setRunTime (atoi(token[3]));
            w->setAllocatedProcessors (atoi(token[4]));
            w->setAverageCPUTimeUsed (atoi(token[5]));
            w->setUsedMemory (atoi(token[6]));
            w->setRequestedNumberProcessors (atoi(token[7]));
            w->setRequestedTime (atoi(token[8]));
            w->setRequestedMemory (atoi(token[9]));
            w->setStatus (atoi(token[10]));
            w->setUserId (atoi(token[11]));
            w->setGroupId (atoi(token[12]));
            w->setAppNumber (atoi(token[13]));
            w->setQueueNumber (atoi(token[14]));
            w->setPartitionNumber (atoi(token[15]));
            w->setPrecedingJobNumber (atoi(token[16]));
            w->setThinkTimeFromPrecedingJob (atoi(token[17]));
            this->addWork(w);
        }
    }
    nline++;
  }
}

void WorkPool::addWork(Work *work)
{
    this->works.push_back(work);
}

Work *WorkPool::getWork(unsigned int pos)
{
    return this->works[pos];
}

unsigned int WorkPool::findWork(Work *w)
{
    unsigned int i=0;
    for (std::vector<Work *>::iterator it = works.begin(); it != works.end(); ++it)
    {
        if (w->getJobNumber()==(*it)->getJobNumber()) return i;
        i++;
    }
}

void WorkPool::deleteWork(unsigned int pos)
{
    this->works.erase (works.begin()+pos);
}

void WorkPool::printWorkList(void)
{
    for (std::vector<Work *>::iterator it = works.begin(); it != works.end(); ++it) (*it)->printWork();
}

unsigned int WorkPool::getLastSubmitTime(void)
{
    unsigned int lastSubmitTime = 0;
    for (std::vector<Work *>::iterator it = works.begin(); it != works.end(); ++it)
    {
        if ((*it)->getSubmitTime()> lastSubmitTime)
            lastSubmitTime=(*it)->getSubmitTime();
    }
    return lastSubmitTime;
}

unsigned int WorkPool::getNumberWorks(void)
{
    return works.size();
}

void WorkPool::updateTotalWaitTime(unsigned int waitTime)
{
    totalWaitTime+=waitTime;
}

float WorkPool::calculateAWT(void)
{
    if (0 < this->getNumberWorks())
        return (totalWaitTime/this->getNumberWorks());
    else return 0;
}


float WorkPool::calculateStandardDeviationWT(void)
{
    float awt=calculateAWT(), sum=0, numJobs=0;
    for (std::vector<Work *>::iterator it = works.begin(); it != works.end(); ++it)
    {
            sum+=(*it)->getRealStartTime()-(*it)->getSubmitTime()-awt;
            numJobs++;
    }
    float variance=sum/numJobs;
    return sqrt(variance);
}

float WorkPool::calculateART(void)
{
    return this->calculateAWT()+this->calculateAET();
}

float WorkPool::calculateStandardDeviationRT(void)
{
    float art=calculateART(), sum=0, numJobs=0;
    for (std::vector<Work *>::iterator it = works.begin(); it != works.end(); ++it)
    {
            sum+=(*it)->getRealFinishTime()-(*it)->getSubmitTime()-art;
            numJobs++;
    }
    float variance=sum/numJobs;
    return sqrt(variance);
}

float WorkPool::calculateAET(void)
{
    unsigned int totalExecutionTime=0;
    for (std::vector<Work *>::iterator it = works.begin(); it != works.end(); ++it)
    {
            totalExecutionTime+=(*it)->getRealRunTime();
    }
    return totalExecutionTime/this->getNumberWorks();

}

float WorkPool::calculateBoundedSlowDown(void)
{
    unsigned int total=0;
    for (std::vector<Work *>::iterator it = works.begin(); it != works.end(); ++it)
    {
            if ((*it)->getRealRunTime()>600) //Jobs with a length of more than 10 minutes
                total+=(*it)->getRealResponseTime()/(*it)->getRealRunTime();
            else
                total+=(*it)->getRealResponseTime()/600;
    }
    return total/this->getNumberWorks();
}

float WorkPool::calculatePerProcessorBoundedSlowDown(void)
{
    unsigned int total=0;
    for (std::vector<Work *>::iterator it = works.begin(); it != works.end(); ++it)
    {
            if ((*it)->getRealRunTime()>600) //Jobs with a length of more than 10 minutes
                total+=(*it)->getRealResponseTime()/((*it)->getRealRunTime()*(*it)->getAllocatedProcessors());
            else
                total+=(*it)->getRealResponseTime()/(600*(*it)->getAllocatedProcessors());
    }
    return total/this->getNumberWorks();
}


unsigned int WorkPool::getNextEvent(unsigned int simulationTime)
{
    unsigned int nextEvent=UINT_MAX;
    unsigned int encontrado=false;
        for (std::vector<Work *>::iterator it = works.begin(); it != works.end(); ++it)
        {
            if ((*it)->getSubmitTime()>= simulationTime && (*it)->getSubmitTime()< nextEvent)
            {
                encontrado=true;
                nextEvent=(*it)->getSubmitTime();
            }

            if ((*it)->getRealFinishTime()>= simulationTime && (*it)->getRealFinishTime()< nextEvent)
            {
                encontrado=true;
                nextEvent=(*it)->getRealFinishTime();
            }
        }

    if (encontrado)
    {
        std::cout<<"[WORKLOAD] ******* Next Event: "<<nextEvent<<std::endl;
        return nextEvent;
    }
    else {
        std::cout<<"[WORKLOAD] ******* Not found ******* Next Event: "<<simulationTime+1<<std::endl;
        return (simulationTime+1);
    }
}
