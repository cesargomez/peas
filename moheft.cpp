#include "moheft.hpp"

MOHEFT::MOHEFT()
{
    solutions.resize(K);
    for (int i = 0; i < K; ++i)
    {
        solutions[i].resize(MAXNUMSERVERS);
        for (int j = 0; j < MAXNUMSERVERS; ++j)
            solutions[i][j].resize(MAXNUMCORES);
    }

    //Initialize Vectors
    for (int i = 0; i < K; ++i)
    {
        realResponseTime[i]=0;
        realPowerUsage[i]=0;
    }
}

MOHEFT::~MOHEFT()
{

}

void MOHEFT::printSolutions(void)
{
   // int in;
    for (int i=0;i<solutions.size();i++)
    {
        std::cout<<"Solución: "<<i<<std::endl<<"-------------------------------"<<std::endl;
        for (int j=0;j<solutions[i].size();j++)
        {
            for (int k=0;k<solutions[i][j].size();k++)
            {
                std::cout<<" "<<solutions[i][j][k]<<" ";
            }
            std::cout<<std::endl;
        }
    }
}

void MOHEFT::initializeSolutions(Queue *serverPool)
{
    int i,j,k;
    unsigned int numberServers=((Queue *)serverPool)->getNumberServers();

    solutions.resize(K);

    for (i=0;i<K;i++)
    {
        solutions[i].resize(numberServers);
        for (j=0;j<numberServers;j++)
        {
            Server *server=((Queue *)serverPool)->getServer(j);
            unsigned int numberCoresServer = (server->getNumberCPUs())*(server->getNumberCoresCPU());
            solutions[i][j].resize(numberCoresServer);
            for (k=0;k<numberCoresServer;k++)
            {
                solutions[i][j][k]=server->getCore(k)->getAssignedWork();
            }
        }
    }
}

void MOHEFT::generateSolutions(Work *work, Queue *serverPool)
{
    unsigned int numCoresAllocated[K]={0};
    unsigned int numberServers=((Queue *)serverPool)->getNumberServers();
    for (int i=0;i<K;i++)
    {
        for (int j=nextServer;j<numberServers;j++)
        {
            Server *server=((Queue *)serverPool)->getServer(j);
            if (work->getAllocatedProcessors()<=server->getIdleCores())
            {
                for(int z=0;z<server->getTotalCores() && numCoresAllocated[i]<work->getAllocatedProcessors();z++)
                {
                    if (-1 == solutions[i][j][z]) //If the core is Idle and there are also unallocated cores the work is assigned
                    {
                        solutions[i][j][z]=work->getJobNumber();
                        numCoresAllocated[i]++;
                    }
                }
                if (j==numberServers) nextServer=0;
                else nextServer=j+1;
                break;
            }
        }
    }


    unsigned int nextCoreInServer=nextServer;
    for (unsigned int i=0;i<K;i++)
    {

        while (work->getAllocatedProcessors()>numCoresAllocated[i])
        {
            if (nextCoreInServer>=numberServers) nextCoreInServer=0;
            for (unsigned int j=nextCoreInServer;j<numberServers && numCoresAllocated[i]<work->getAllocatedProcessors();j++)
            {
                Server *server=((Queue *)serverPool)->getServer(j);
                for (unsigned int z=0;z<server->getTotalCores() && numCoresAllocated[i]<work->getAllocatedProcessors();z++)
                {
                    if (-1 == solutions[i][j][z]) //If the core is Idle and there are also unallocated cores the work is assigned
                    {
                        solutions[i][j][z]=work->getJobNumber();
                        numCoresAllocated[i]++;
                        if (numCoresAllocated[i]==work->getAllocatedProcessors())
                        {
                            nextCoreInServer=j;
                            break;
                        }
                    }
                }
            }
            nextCoreInServer++;
        }
    }
}



void MOHEFT::calculatePowerAndResponsetime(Work *work, Queue *serverPool)
{
    float necessaryTime[K]; //RealRunTime
    for (int i=0;i<this->solutions.size();i++) //for every individual
    {
        for (int j=0;j<solutions[i].size();j++)
            for (int k=0;k<solutions[i][j].size();k++)
                if (work->getJobNumber() == solutions[i][j][k])
                {
                    necessaryTime[i]=((serverPool->getAverageSSJ_opsPerCoreAt100()/serverPool->getServer(j)->getCoreActiveSsj_ops100())*work->getRunTime());
                    float responseTime=work->getSubmitTime()+necessaryTime[i];
                    if (realResponseTime[i]<responseTime)
                    {
                        realResponseTime[i]=responseTime;
                    }
                }

        for (int j=0;j<solutions[i].size();j++)
            for (int k=0;k<solutions[i][j].size();k++)
                if (work->getJobNumber() == solutions[i][j][k])
                {
                    realPowerUsage[i]+=necessaryTime[i]*serverPool->getServer(j)->getCoreActivePower100();
                }

        realPowerUsage[i]=realPowerUsage[i]/3600000; //To Kwh
    }
}

void MOHEFT::normalizeResponsetimeAndPower(Work *work, Queue *serverPool)
{

    float maxPowerConsumption=0, maxRealResponseTime=0, minPowerConsumption=std::numeric_limits<float>::infinity(), minRealResponseTime=std::numeric_limits<float>::infinity();

    //Search for maximum values
    for (int i=0;i<K;i++) //All the solutions at parents
    {
        if (realResponseTime[i]>maxRealResponseTime) maxRealResponseTime=realResponseTime[i];
        if (realPowerUsage[i]>maxPowerConsumption) maxPowerConsumption=realPowerUsage[i];
    }

    //Search for minimum values
    for (int i=0;i<K;i++) //All the solutions at parents
    {
        if (realResponseTime[i]<minRealResponseTime) minRealResponseTime=realResponseTime[i];
        if (realPowerUsage[i]<minPowerConsumption) minPowerConsumption=realPowerUsage[i];
    }

    //Calculate Normalized values
    for (int i=0;i<K;i++) //All the solutions at parents
    {
        realResponseTime[i]=(realResponseTime[i]-minRealResponseTime)/(maxRealResponseTime-minRealResponseTime);
        realPowerUsage[i]=(realPowerUsage[i]-minPowerConsumption)/(maxPowerConsumption-minPowerConsumption);

    }

}


void MOHEFT::crowdingDistanceAssignment(Work *work, Queue *serverPool)
{

    unsigned int numberSolutions=solutions.size();
    vector < std::pair <float, unsigned int> > responsetimeVector; //Real Responsetime of the solutions
    vector < std::pair <float, unsigned int > > powerVector; //Power consumption of the solutions

    //First of all we should normalized RunTime and Power
    normalizeResponsetimeAndPower(work, serverPool); //Normalized realRunTime and realPowerUsage vectors

    //Get runtime and power values of the solutions
    for (unsigned int i=0;i<numberSolutions;i++)
    {
        responsetimeVector.push_back(std::make_pair(realResponseTime[i],i));
        powerVector.push_back(std::make_pair(realPowerUsage[i], i));
        crowdingDistance[i]=0;
    }


    //Sort both vectors in increasing order
    struct sort_pred {
        bool operator()(const std::pair<float,unsigned int> &left, const std::pair<float,unsigned int> &right) {
            return left.first < right.first;
        }
    };

    std::sort(responsetimeVector.begin(), responsetimeVector.end(), sort_pred());
    std::sort(powerVector.begin(), powerVector.end(), sort_pred());

     std::sort(responsetimeVector.begin(), responsetimeVector.end(), sort_pred());
     std::sort(powerVector.begin(), powerVector.end(), sort_pred());

    //Calculate crowding distance of every solution

    crowdingDistance[responsetimeVector[0].second] = std::numeric_limits<float>::infinity();
    crowdingDistance[responsetimeVector[responsetimeVector.size()-1].second] = std::numeric_limits<float>::infinity();
    crowdingDistance[powerVector[0].second] = std::numeric_limits<float>::infinity();
    crowdingDistance[powerVector[powerVector.size()-1].second] = std::numeric_limits<float>::infinity();

    for (unsigned int i=0;i<numberSolutions;i++)
    {
        if (std::numeric_limits<float>::infinity() != crowdingDistance[responsetimeVector[i].second])
        {
            crowdingDistance[responsetimeVector[i].second]=(responsetimeVector[i+1].first-responsetimeVector[i-1].first);
        }
        if (std::numeric_limits<float>::infinity() != crowdingDistance[powerVector[i].second])
        {
            crowdingDistance[powerVector[i].second]=crowdingDistance[powerVector[i].second]+(powerVector[i+1].first-powerVector[i-1].first);
        }
    }

}
void MOHEFT::calculateEuclideanDistanceFrom0(Work *work, Queue *serverPool)
{
    normalizeResponsetimeAndPower(work,serverPool);
    //Calculate euclidean distance p=(0,0), q=(q1, q2)
    for (int i=0;i<K;i++) //All the solutions at parents
    {
        euclideanDistance[i]=sqrt(realResponseTime[i]*realResponseTime[i] + realPowerUsage[i]*realPowerUsage[i]);
    }
}


std::vector<std::vector<int> > MOHEFT::returnBestSolution(Work *work, Queue *serverPool)
{
    calculateEuclideanDistanceFrom0(work,serverPool);
    unsigned int bestSolution=0;
    float minimumEuclideanDistance = std::numeric_limits<float>::infinity();

    //Find the best solution
    for (int i=0;i<K;i++) //All the solutions
    {
        if (euclideanDistance[i]<minimumEuclideanDistance)
        {
            minimumEuclideanDistance=euclideanDistance[i];
            bestSolution=i;
        }
    }
    std::cout<<"[MOHEFT] Best Solution: "<<bestSolution<<std::endl;
    return solutions[bestSolution];
}

std::vector<std::vector<int> > MOHEFT::runMOHEFT(Queue *serverPool, Work *work)
{
    this->initializeSolutions(serverPool);
    this->generateSolutions(work, serverPool);
    this->calculatePowerAndResponsetime(work, serverPool);
    this->crowdingDistanceAssignment(work, serverPool);
    //Returns the best non-dominated solution (ordered)
    return returnBestSolution(work, serverPool);

}
