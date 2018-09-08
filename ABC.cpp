#include "ABC.hpp"


ABC::ABC()
{
    foodSources.resize(FOODSOURCES);
    for (int i = 0; i < FOODSOURCES; ++i)
    {
        foodSources[i].resize(MAXNUMSERVERS);
        for (int j = 0; j < MAXNUMSERVERS; ++j)
        {
            foodSources[i][j].resize(MAXNUMCORES);
        }
    }

    trial.resize(FOODSOURCES);

    //Initialize Vectors
    for (int i = 0; i < SWARMSIZE; ++i)
    {
        realResponseTime[i]=0;
        realPowerUsage[i]=0;
        dominationCounter[i]=0;
    }
}

ABC::~ABC()
{

}

void ABC::initializeVectors(Queue *serverPool)
{
    int i,j,k;
    unsigned int numberServers=((Queue *)serverPool)->getNumberServers();

    foodSources.resize(FOODSOURCES);
    trial.resize(FOODSOURCES);


    for (i=0; i<FOODSOURCES; i++)
    {
        foodSources[i].resize(numberServers);
        for (j=0; j<numberServers; j++)
        {
            Server *server=((Queue *)serverPool)->getServer(j);
            unsigned int numberCoresServer = (server->getNumberCPUs())*(server->getNumberCoresCPU());
            foodSources[i][j].resize(numberCoresServer);
            for (k=0; k<numberCoresServer; k++)
            {
                foodSources[i][j][k]=server->getCore(k)->getAssignedWork();
            }
        }

    }
}

void ABC::generateRandomFoodSources(Work *work)
{

    std::minstd_rand generator(time(0));
    unsigned int job=work->getJobNumber();

    for (int i=0; i<FOODSOURCES; i++)
    {
        int j=0;
        std::uniform_int_distribution<int> distribution1(0,foodSources[i].size()-1);
        while (j<work->getAllocatedProcessors())
        {
            int selectedServer=distribution1(generator);  // generates random number in the range 0,parents[i].size()
            std::uniform_int_distribution<int> distribution2(0,foodSources[i][selectedServer].size()-1);
            int selectedCore=distribution2(generator);

            if (-1 == foodSources[i][selectedServer][selectedCore]) //If the core is Idle, the work is assigned
            {
                foodSources[i][selectedServer][selectedCore]=job;
                j++;
            }
        }
    }
}

void ABC::calculatePowerAndResponsetime(Work *work, Queue *serverPool)
{
    unsigned int job=work->getJobNumber();
    for (int i = 0; i < SWARMSIZE; ++i)
    {
        realResponseTime[i]=0;
        realPowerUsage[i]=0;
    }
    for (int i=0; i<this->foodSources.size(); i++) //for every individual
    {
        for (int j=0; j<foodSources[i].size(); j++)
            for (int k=0; k<foodSources[i][j].size(); k++)
                if (job == foodSources[i][j][k])
                {
                    necessaryTime[i]=((serverPool->getAverageSSJ_opsPerCoreAt100()/serverPool->getServer(j)->getCoreActiveSsj_ops100())*work->getRunTime());
                    float responseTime=work->getSubmitTime()+necessaryTime[i];
                    if (realResponseTime[i]<responseTime)
                    {
                        realResponseTime[i]=responseTime;
                    }
                }

        for (int j=0; j<foodSources[i].size(); j++)
            for (int k=0; k<foodSources[i][j].size(); k++)
                if (job == foodSources[i][j][k])
                {
                    realPowerUsage[i]+=necessaryTime[i]*serverPool->getServer(j)->getCoreActivePower100();
                }

        realPowerUsage[i]=realPowerUsage[i]/3600000; //To Kwh
    }
}

void ABC::printPowerAndResponsetime(void)
{

    for (int i=0; i<this->foodSources.size(); i++) //for every foodSource
    {
        std::cout<<"Power Usage Individual ["<<i<<"]="<<realPowerUsage[i]<<std::endl;
        std::cout<<"Real Response Time Individual ["<<i<<"]="<<realResponseTime[i]<<std::endl;
    }
}

void ABC::printCrowdingDistance(void)
{
    for (unsigned int i=0; i<crowdingDistance.size(); i++)
        for (unsigned int j=0; j<crowdingDistance[i].size(); j++)
            std::cout<<"Front: "<<i+1<<"Crowding distance ["<<j<<"]="<<crowdingDistance[i][j]<<std::endl;

}

void ABC::insertIndividualInParetoFront(unsigned int individual, unsigned int frontNumber) //insert individual in Pareto front
{
    paretoFront[frontNumber].push_back(individual);

}


void ABC::crowdingDistanceAssignment(Work *work, Queue *serverPool)
{
    crowdingDistance.clear();
    crowdingDistance.resize(paretoFront.size());

    vector < std::pair <float, unsigned int > > responsetimeVector; //Real Responsetime of the solutions
    vector < std::pair <float, unsigned int > > powerVector; //Power consumption of the solutions



    for (unsigned int k=0; k<paretoFront.size(); k++)
    {

        responsetimeVector.clear();
        powerVector.clear();


        unsigned int numberSolutions=paretoFront[k].size();

        crowdingDistance[k].resize(SWARMSIZE);

        for (unsigned int i=0;i<SWARMSIZE;i++)
        {
            crowdingDistance[k][i]=0;
        }

        for (unsigned int i=0; i<numberSolutions; i++)
        {
            responsetimeVector.push_back(std::make_pair(realResponseTime[paretoFront[k][i]],paretoFront[k][i]));
            powerVector.push_back(std::make_pair(realPowerUsage[paretoFront[k][i]], paretoFront[k][i]));
        }

        //Sort both vectors in increasing order
        struct sort_pred
        {
            bool operator()(const std::pair<float,float> &left, const std::pair<float,float> &right)
            {
                return left.first < right.first;
            }
        };
        std::sort(responsetimeVector.begin(), responsetimeVector.end(), sort_pred());
        std::sort(powerVector.begin(), powerVector.end(), sort_pred());


        for (unsigned int i=1;i<numberSolutions-1;i++)
        {
            crowdingDistance[k][responsetimeVector[i].second]=(responsetimeVector[i+1].first-responsetimeVector[i-1].first);
        }
        for (unsigned int i=1;i<numberSolutions-1;i++)
        {
            crowdingDistance[k][powerVector[i].second]=crowdingDistance[k][powerVector[i].second]+(powerVector[i+1].first-powerVector[i-1].first);
        }

        //Calculate crowding distance of every solution
        crowdingDistance[k][responsetimeVector[0].second] = std::numeric_limits<float>::infinity();
        crowdingDistance[k][responsetimeVector[responsetimeVector.size()-1].second] = std::numeric_limits<float>::infinity();
        crowdingDistance[k][powerVector[0].second] = std::numeric_limits<float>::infinity();
        crowdingDistance[k][powerVector[powerVector.size()-1].second] = std::numeric_limits<float>::infinity();

    }
}


void ABC::sendEmployedBees(Work *work,Queue *serverPool, unsigned int generation)
{
    vector<vector<int> > crossoverBee;
    vector<vector<int> > employedBee;
    for (int i = 0; i < SWARMSIZE; ++i)
    {
        realResponseTime[i]=0;
        realPowerUsage[i]=0;
        dominationCounter[i]=0;
    }
    this->calculatePowerAndResponsetime(work, serverPool);

    for (unsigned int i=0; i<EMPLOYED; i++)
    {
        crossoverBee.clear();
        employedBee.clear();
        employedBee=foodSources[i];

        std::minstd_rand generator(std::random_device {}());


        if (0==generation)
        {
            //Copy a random bee in
            std::uniform_int_distribution<unsigned int> distribution(0,FOODSOURCES-1);
            unsigned int random=i;
            while (random==i)
            {
                random = distribution(generator);
            }
            crossoverBee=foodSources[random];
        }
        else {
            crossoverBee=randomBestFoodSource1;
        }


        std::uniform_int_distribution<int> distribution11(0,crossoverBee.size()-1);
        //Crossover of employedBee and a random
        // We do n/3 steps
        unsigned int numberSteps=serverPool->getTotalCores()/3;
        for (unsigned int j=0; j<numberSteps; j++)
        {
            int selectedServer1=distribution11(generator);
            std::uniform_int_distribution<int> distribution12(0,crossoverBee[selectedServer1].size()-1);
            int selectedCore1=distribution12(generator);

            unsigned int selected=crossoverBee[selectedServer1][selectedCore1];
            unsigned int selectedServer2, selectedCore2;

            bool different =true;
            std::uniform_int_distribution<int> distribution21(0,foodSources[i].size()-1);
            while (different)
            {
                selectedServer2=distribution21(generator);
                std::uniform_int_distribution<int> distribution22(0,foodSources[i][selectedServer2].size()-1);
                selectedCore2=distribution22(generator);
                if (selected==foodSources[i][selectedServer2][selectedCore2]) different=false;
            }
            crossoverBee[selectedServer1][selectedCore1]=crossoverBee[selectedServer2][selectedCore2];
            crossoverBee[selectedServer2][selectedCore2]=selected;
        }

        if (dominates(work,serverPool,crossoverBee,employedBee,i))
        {
            //std::cout<<"Crossover dominates "<<i<<std::endl;
            for (int j=0; j<crossoverBee.size(); j++)
            {
                for (int k=0; k<crossoverBee[j].size(); k++)
                {
                    foodSources[i][j][k]=crossoverBee[j][k];
                }
            }
            trial[i]=0;
        }
        else
        {
            trial[i]++;
        }

    }
}
bool ABC::dontDominates(Work *work,Queue *serverPool,unsigned int positionOnlookerBee, vector<vector<int>> mutatedBee)
{
    float timeMutated;
    float realResponseTimeMutated=0, realPowerUsageMutated=0;
    unsigned int job=work->getJobNumber();

    //Calculate power and response times
    for (int j=0; j<mutatedBee.size(); j++)
        for (int k=0; k<mutatedBee[j].size(); k++)
            if (job == mutatedBee[j][k])
            {
                timeMutated=((serverPool->getAverageSSJ_opsPerCoreAt100()/serverPool->getServer(j)->getCoreActiveSsj_ops100())*work->getRunTime());
                float responseTime=work->getSubmitTime()+timeMutated;
                if (realResponseTimeMutated<responseTime)
                {
                    realResponseTimeMutated=responseTime;
                }
            }
    for (int j=0; j<mutatedBee.size(); j++)
        for (int k=0; k<mutatedBee[j].size(); k++)
            if (job == mutatedBee[j][k])
            {
                realPowerUsageMutated+=timeMutated*serverPool->getServer(j)->getCoreActivePower100();
            }

    realPowerUsageMutated=realPowerUsageMutated/3600000; //To Kwh

    if (realResponseTime[positionOnlookerBee]>=realResponseTimeMutated || realPowerUsage[positionOnlookerBee]>=realPowerUsageMutated) return true;
    return false;
}

bool ABC::dominates(Work *work,Queue *serverPool,vector<vector<int>> bee1, vector<vector<int>> bee2, unsigned int positionBee2)
{

    unsigned int timeBee1;
    float realResponseTimeBee1=0, realPowerUsageBee1=0;
    unsigned int job=work->getJobNumber();

    //Calculate power and response times
    for (int j=0; j<bee1.size(); j++)
        for (int k=0; k<bee1[j].size(); k++)
            if (job == bee1[j][k])
            {
                timeBee1=((serverPool->getAverageSSJ_opsPerCoreAt100()/serverPool->getServer(j)->getCoreActiveSsj_ops100())*work->getRunTime());
                float responseTime=work->getSubmitTime()+timeBee1;
                if (realResponseTimeBee1<responseTime)
                {
                    realResponseTimeBee1=responseTime;
                }
            }

    for (int j=0; j<bee1.size(); j++)
        for (int k=0; k<bee1[j].size(); k++)
            if (job == bee1[j][k])
            {
                realPowerUsageBee1+=timeBee1*serverPool->getServer(j)->getCoreActivePower100();
            }

    realPowerUsageBee1=realPowerUsageBee1/3600000; //To Kwh

    if (realResponseTimeBee1<=realResponseTime[positionBee2] && realPowerUsageBee1<realPowerUsage[positionBee2] || realResponseTimeBee1<realResponseTime[positionBee2] && realPowerUsageBee1<=realPowerUsage[positionBee2]) return true;
    return false;

}

unsigned int ABC::selectEmployedBee(void)
{
    std::minstd_rand generator(std::random_device {}());

    //Copy a random bee in
    unsigned int random=0;
    std::uniform_int_distribution<unsigned int> distribution(0,100);
    random = distribution(generator);
    float offset=0;
    for (unsigned int i=0; i<FOODSOURCES; i++)
    {
        offset=offset+probabilityVector[i]*100;
        if (offset>=random)
        {
            return i;
        }
    }
    return FOODSOURCES-1;
}

void ABC::sendOnlookerBees(Work *work,Queue *serverPool)
{
    onlookerSources.clear();
    onlookerSources.resize(foodSources.size());
    vector<vector<int> > onlookerBee;
    vector<vector<int> > mutatedBee;

    std::minstd_rand generator(std::random_device {}());


    for (unsigned int i=0; i<FOODSOURCES; i++)
    {
        mutatedBee.clear();
        unsigned int onlookerBeePosition=this->selectEmployedBee();
        onlookerBee=foodSources[onlookerBeePosition]; //Select employed bee based on probability vector
        copy(onlookerBee.begin(), onlookerBee.end(), back_inserter(mutatedBee));
        std::uniform_int_distribution<int> distribution1(0,mutatedBee.size()-1);
        int selectedServer1=distribution1(generator);
        std::uniform_int_distribution<int> distribution2(0,mutatedBee[selectedServer1].size()-1);
        int selectedCore1=distribution2(generator);
        std::uniform_int_distribution<int> distribution3(0,mutatedBee.size()-1);
        int selectedServer2=distribution3(generator);
        std::uniform_int_distribution<int> distribution4(0,mutatedBee[selectedServer2].size()-1);
        int selectedCore2=distribution4(generator);

        unsigned int secondCore=mutatedBee[selectedServer2][selectedCore2];
        mutatedBee[selectedServer2][selectedCore2]=mutatedBee[selectedServer1][selectedCore1];
        mutatedBee[selectedServer1][selectedCore1]=secondCore;

        if (dontDominates(work,serverPool,onlookerBeePosition,mutatedBee))
        {
            copy(mutatedBee.begin(), mutatedBee.end(), back_inserter(onlookerSources[i]));
            trial.push_back(0);
        }
        else
        {
            copy(onlookerBee.begin(), onlookerBee.end(), back_inserter(onlookerSources[i]));
            trial.push_back(trial[onlookerBeePosition]);
        }
    }
}

void ABC::sendScoutBees(Queue *serverPool, unsigned int generation)
{
    for (unsigned int i=0;i<foodSources.size();i++)
    {
        if (trial[i]>ATTEMPTS)
        {
            vector<vector<int> > crossoverBee;
            vector<vector<int> > scoutBee;

            scoutBee=foodSources[i];

            std::minstd_rand generator(std::random_device {}());

            if (0==generation)
            {
                //Copy a random bee in
                unsigned int random=i;
                std::uniform_int_distribution<unsigned int> distribution(0,foodSources.size()-1);
                while (random==i)
                {
                    random = distribution(generator);
                }
                crossoverBee=foodSources[random];
            }
            else {
                crossoverBee=randomBestFoodSource2;
            }

            //Crossover of employedBee and a random
            // We do n/3 steps
            std::uniform_int_distribution<int> distribution11(0,crossoverBee.size()-1);
            unsigned int numberSteps=serverPool->getTotalCores()/3;
            for (unsigned int j=0; j<numberSteps; j++)
            {
                int selectedServer1=distribution11(generator);
                std::uniform_int_distribution<int> distribution12(0,crossoverBee[selectedServer1].size()-1);
                int selectedCore1=distribution12(generator);

                unsigned int selected=crossoverBee[selectedServer1][selectedCore1];
                unsigned int selectedServer2, selectedCore2;

                std::uniform_int_distribution<int> distribution21(0,scoutBee.size()-1);
                bool different =true;
                while (different)
                {
                    selectedServer2=distribution21(generator);
                    std::uniform_int_distribution<int> distribution22(0,scoutBee[selectedServer2].size()-1);
                    selectedCore2=distribution22(generator);
                    if (selected==scoutBee[selectedServer2][selectedCore2]) different=false;
                }
                crossoverBee[selectedServer1][selectedCore1]=crossoverBee[selectedServer2][selectedCore2];
                crossoverBee[selectedServer2][selectedCore2]=selected;
            }

            for (unsigned int j=0;j<foodSources[i].size();j++)
            {
                for (unsigned int k=0;k<foodSources[i][j].size();k++)
                {
                    foodSources[i][j][k]=crossoverBee[j][k];
                }
            }
            trial[i]=0;
        }
    }
}

void ABC::printMOFitness(void)
{
    for (unsigned int i=0; i<FOODSOURCES; i++)
        std::cout<<"Food Source: "<<i<<" MOFitness: "<<MOFitness[i]<<std::endl;

}

void ABC::printProbabilityVector(void)
{
    float totalSum=0;
    for (unsigned int i=0; i<FOODSOURCES; i++)
    {
        std::cout<<"Food Source: "<<i<<" Probability: "<<probabilityVector[i]<<std::endl;
    }

}

void ABC::generateProbabilityVector(Work *work, Queue *serverPool)
{
    float fitnessSum=0;
    this->calculatePowerAndResponsetime(work,serverPool);
    this->fastNonDominatedSort(work, serverPool);
    this->crowdingDistanceAssignment(work, serverPool);

    for (unsigned int i=0; i<FOODSOURCES; i++)
    {
        for (unsigned int j=0; j<paretoFront.size(); j++)
            for (unsigned int k=0; k<paretoFront[j].size(); k++)
                if (paretoFront[j][k]==i)
                {
                    MOFitness[i]=1/(pow(2,j)+1/(1+crowdingDistance[j][i]));
                    fitnessSum=fitnessSum+MOFitness[i];
                }

    }

    for (unsigned int i; i<FOODSOURCES; i++)
        probabilityVector[i]=MOFitness[i]/fitnessSum;
}



void ABC::fastNonDominatedSort(Work *work, Queue *serverPool)
{

    //Clear paretoFront
    paretoFront.clear();

    //Clear dominationCounter[] and dominated[]
    for (unsigned int i=0; i<FOODSOURCES; i++)
    {
        dominated[i].clear();
        dominationCounter[i]=0;
    }

    for (unsigned int i=0; i<this->foodSources.size(); i++)
    {
        for (unsigned int j=0; j<this->foodSources.size(); j++)
            if (i != j)
            {
                if (((realResponseTime[i]<realResponseTime[j]) && (realPowerUsage[i]<=realPowerUsage[j])) || ((realResponseTime[i]<=realResponseTime[j]) && (realPowerUsage[i]<realPowerUsage[j])))
                {
                    dominated[i].push_back(j);
                }
                else if (((realResponseTime[j]<realResponseTime[i]) && (realPowerUsage[j]<=realPowerUsage[i])) || ((realResponseTime[j]<=realResponseTime[i]) && (realPowerUsage[j]<realPowerUsage[i])))
                {
                    dominationCounter[i]++;
                }
            }
        if (0 == dominationCounter[i]) //i is a member of the first front
        {
            if (paretoFront.empty())
            {
                vector<int> row;
                paretoFront.push_back(row);
            }
            insertIndividualInParetoFront(i, 0); //insert individual i in Pareto front 0
        }
    }

    unsigned int i=0;

    vector<unsigned int> H;

    while (i<paretoFront.size())
    {

        H.clear(); //Clear the vector

        for (unsigned int j=0; j<paretoFront[i].size(); j++)
        {
            unsigned int individual=paretoFront[i][j];
            for (unsigned int z=0; z<dominated[individual].size(); z++)
            {
                unsigned int q=dominated[individual][z];
                dominationCounter[q]--;
                if (0 == dominationCounter[q])
                {
                    H.push_back(q);
                }
            }
        }
        for (unsigned int j=0; j<H.size(); j++)
        {
            if (paretoFront.size()<=(i+1))
            {
                vector<int> row;
                paretoFront.push_back(row);
            }
            insertIndividualInParetoFront(H[j],i+1);
        }
        i++;
    }
}


void ABC::printParetoFronts(void)
{
    for (int i=0; i<paretoFront.size(); i++)
    {
        std::cout<<"Front: "<<i+1<<std::endl<<"-------------------------------"<<std::endl;
        std::cout<<"Individual: ";
        for (int j=0; j<paretoFront[i].size(); j++)
        {
            std::cout<<paretoFront[i][j]<<" ";
        }
        std::cout<<std::endl;
    }
}


void ABC::generateCompleteSwarm(void)
{
    copy(onlookerSources.begin(), onlookerSources.end(), back_inserter(foodSources));
}

void ABC::normalizeResponsetimeAndPower(Work *work, Queue *serverPool)
{


    float maxPowerConsumption=0, maxRealResponseTime=0, minPowerConsumption=std::numeric_limits<float>::infinity(), minRealResponseTime=std::numeric_limits<float>::infinity();

    //Search for maximum values
    for (int i=0; i<foodSources.size(); i++) //All the solutions at parents
    {
        if (realResponseTime[i]>maxRealResponseTime) maxRealResponseTime=realResponseTime[i];
        if (realPowerUsage[i]>maxPowerConsumption) maxPowerConsumption=realPowerUsage[i];
    }

    //Search for minimum values
    for (int i=0; i<foodSources.size(); i++) //All the solutions at parents
    {
        if (realResponseTime[i]<minRealResponseTime) minRealResponseTime=realResponseTime[i];
        if (realPowerUsage[i]<minPowerConsumption) minPowerConsumption=realPowerUsage[i];
    }

    //Calculate Normalized values
    for (int i=0; i<foodSources.size(); i++) //All the solutions at parents
    {
        if (minRealResponseTime==maxRealResponseTime)
        {
            realResponseTime[i]=1;
        }
        else
        {
            realResponseTime[i]=(realResponseTime[i]-minRealResponseTime)/(maxRealResponseTime-minRealResponseTime);
        }
        if (minPowerConsumption==maxPowerConsumption)
        {
            realPowerUsage[i]=1;
        }
        else
        {
            realPowerUsage[i]=(realPowerUsage[i]-minPowerConsumption)/(maxPowerConsumption-minPowerConsumption);
        }

    }

}


void ABC::calculateEuclideanDistanceFrom0(Work *work, Queue *serverPool)
{
    normalizeResponsetimeAndPower(work,serverPool);
    //Calculate euclidean distance p=(0,0), q=(q1, q2)
    for (int i=0; i<FOODSOURCES; i++) //All the solutions at parents
    {
        euclideanDistance[i]=sqrt(realResponseTime[i]*realResponseTime[i] + realPowerUsage[i]*realPowerUsage[i]);
    }
}

std::vector<std::vector<int> > ABC::returnBestFoodSource(Work *work, Queue *serverPool)
{
    calculateEuclideanDistanceFrom0(work,serverPool);
    unsigned int bestSolution=0;
    float minimumEuclideanDistance = std::numeric_limits<float>::infinity();

    //Find the best solution
    for (int i=0; i<FOODSOURCES; i++) //All the solutions at parents
    {
        if (euclideanDistance[i]<minimumEuclideanDistance)
        {
            minimumEuclideanDistance=euclideanDistance[i];
            bestSolution=i;
        }
    }
    return foodSources[bestSolution];

}

void ABC::printFoodSources(void)
{
    for (int i=0; i<foodSources.size(); i++)
    {
        std::cout<<"Food Source: "<<i<<std::endl<<"-------------------------------"<<std::endl;
        for (int j=0; j<foodSources[i].size(); j++)
        {
            for (int k=0; k<foodSources[i][j].size(); k++)
            {
                std::cout<<" "<<foodSources[i][j][k]<<" ";
            }
            std::cout<<std::endl;
        }
    }
}

std::vector<std::vector<int> > ABC::runABC(Queue *serverPool, Work *work)
{

    this->initializeVectors(serverPool);
    this->generateRandomFoodSources(work);

    unsigned int currentGeneration=0;

    while (currentGeneration<GENERATIONS)
    {
        this->sendEmployedBees(work,serverPool, currentGeneration);
        this->generateProbabilityVector(work, serverPool);
        this->sendOnlookerBees(work, serverPool);
        this->generateCompleteSwarm();
        this->sendScoutBees(serverPool,currentGeneration);
        this->generateProbabilityVector(work, serverPool);

        for (unsigned int j=0;j<paretoFront.size();j++)
        {
            if (bestFoodSources.size()+paretoFront[j].size()<=FOODSOURCES)
            {
                for (unsigned int k=0;k<paretoFront[j].size();k++)
                {
                    bestFoodSources.push_back(foodSources[paretoFront[j][k]]);
                }
                if (bestFoodSources.size()==FOODSOURCES) break;
            }
            else {
                    for (unsigned int k=0;k<paretoFront[j].size();k++)
                    {
                        unsigned int maxIndex = distance(crowdingDistance[j].begin(), max_element(crowdingDistance[j].begin(), crowdingDistance[j].end()));
                        bestFoodSources.push_back(foodSources[maxIndex]);
                        crowdingDistance[j][maxIndex]=-1;
                        if (bestFoodSources.size()==FOODSOURCES) break;
                    }
                 }
            if (bestFoodSources.size()==FOODSOURCES) break;
        }

        //Select two of the best food sources for the crossovers
        std::minstd_rand generator(std::random_device {}());
        std::uniform_int_distribution<unsigned int> distribution(0,paretoFront[0].size()-1);
        randomBestFoodSource1= foodSources[paretoFront[0][distribution(generator)]];
        randomBestFoodSource2= foodSources[paretoFront[0][distribution(generator)]];

        //Clear previous generation and select best foodsources for next generation
        foodSources.clear();
        foodSources=bestFoodSources;
        bestFoodSources.clear();
        currentGeneration++;
    }
    return returnBestFoodSource(work, serverPool);
}
