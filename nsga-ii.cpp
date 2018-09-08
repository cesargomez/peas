#include "nsga-ii.hpp"

NSGAII::NSGAII(unsigned int numIndividuals, unsigned int numGenerations)
{
    parents.resize(INDIVIDUALS);
    for (int i = 0; i < INDIVIDUALS; ++i)
    {
        parents[i].resize(MAXNUMSERVERS);
        for (int j = 0; j < MAXNUMSERVERS; ++j)
            parents[i][j].resize(MAXNUMCORES);
    }

    //Initialize Domination Counter
    for (int i = 0; i < INDIVIDUALS*2; ++i)
    {
        realResponseTime[i]=0;
        realPowerUsage[i]=0;
        dominationCounter[i]=0;
    }
}

NSGAII::NSGAII()
{
    parents.resize(INDIVIDUALS);
    for (int i = 0; i < INDIVIDUALS; ++i)
    {
        parents[i].resize(MAXNUMSERVERS);
        for (int j = 0; j < MAXNUMSERVERS; ++j)
            parents[i][j].resize(MAXNUMCORES);
    }

    //Initialize Domination Counter
    for (int i = 0; i < INDIVIDUALS*2; ++i)
    {
        realResponseTime[i]=0;
        realPowerUsage[i]=0;
        dominationCounter[i]=0;
    }
}

NSGAII::~NSGAII()
{

}

unsigned int NSGAII::getIndividuals()
{
    return INDIVIDUALS;
}

unsigned int NSGAII::getGenerations()
{
    return GENERATIONS;
}

void NSGAII::initializeVectors(Queue *serverPool)
{
    int i,j,k;
    unsigned int numberServers=((Queue *)serverPool)->getNumberServers();

    parents.resize(INDIVIDUALS);


    for (i=0;i<INDIVIDUALS;i++)
    {
        parents[i].resize(numberServers);
        for (j=0;j<numberServers;j++)
        {
            Server *server=((Queue *)serverPool)->getServer(j);
            unsigned int numberCoresServer = (server->getNumberCPUs())*(server->getNumberCoresCPU());
            parents[i][j].resize(numberCoresServer);
            for (k=0;k<numberCoresServer;k++)
            {
                parents[i][j][k]=server->getCore(k)->getAssignedWork();
            }
        }
    }

    this->offspring=this->parents; //Copy empty 3D matrices

}

void NSGAII::printParents(void)
{
    for (int i=0;i<parents.size();i++)
    {
        std::cout<<"Individual: "<<i<<std::endl<<"-------------------------------"<<std::endl;
        for (int j=0;j<parents[i].size();j++)
        {
            for (int k=0;k<parents[i][j].size();k++)
            {
                std::cout<<" "<<parents[i][j][k]<<" ";
            }
            std::cout<<std::endl;
        }
    }
}

void NSGAII::printCollect(void)
{
    for (int i=0;i<collect.size();i++)
    {
        std::cout<<"Individual: "<<i<<std::endl<<"-------------------------------"<<std::endl;
        for (int j=0;j<collect[i].size();j++)
        {
            for (int k=0;k<collect[i][j].size();k++)
            {
                std::cout<<" "<<collect[i][j][k]<<" ";
            }
            std::cout<<std::endl;
        }
    }
}


void NSGAII::printParetoFronts(void)
{
    for (int i=0;i<paretoFront.size();i++)
    {
        std::cout<<"Front: "<<i<<std::endl<<"-------------------------------"<<std::endl;
        std::cout<<"Individual: ";
        for (int j=0;j<paretoFront[i].size();j++)
        {
            std::cout<<paretoFront[i][j]<<" ";
        }
        std::cout<<std::endl;
    }
}

void NSGAII::printDominated(void)
{
    for (int i=0;i<collect.size();i++)
    {
        std::cout<<"Individual: "<<i<<std::endl<<"-------------------------------"<<std::endl;


        for (int j=0;j<dominated[i].size();j++)
        {
            std::cout<<" "<<dominated[i][j]<<std::endl;
        }
    }
}

void NSGAII::generateRandomPopulation(Work *work)
{
    std::minstd_rand generator(std::random_device{}());

    for (int i=0;i<INDIVIDUALS;i++)
    {
        std::uniform_int_distribution<int> distribution1(0,parents[i].size()-1);
        int j=0;
        while (j<work->getAllocatedProcessors())
        {
            int selectedServer=distribution1(generator);  // generates random number in the range 0,parents[i].size()
            std::uniform_int_distribution<int> distribution2(0,parents[i][selectedServer].size()-1);
            int selectedCore=distribution2(generator);

            if (-1 == parents[i][selectedServer][selectedCore]) //If the core is Idle, the work is assigned
            {
                parents[i][selectedServer][selectedCore]=work->getJobNumber();
                j++;
            }
        }
    }
    offspring=parents;
}

void NSGAII::combineParentsOffspring(void)
{
    collect.clear();
    collect.resize(INDIVIDUALS*2);
    collect=parents;
    collect.insert(collect.end(),offspring.begin(),offspring.end());
}

void NSGAII::calculatePowerAndResponsetime(Work *work, Queue *serverPool)
{
    unsigned int job=work->getJobNumber();
    unsigned int jobRuntime=work->getRunTime();
    unsigned int jobSubmitTime=work->getSubmitTime();

    float averageSSJ=serverPool->getAverageSSJ_opsPerCoreAt100();
    for (int i=0;i<this->collect.size();i++) //for every individual
    {
        for (int j=0;j<collect[i].size();j++)
        {
            unsigned int coreActiveSsj=serverPool->getServer(j)->getCoreActiveSsj_ops100();
            for (int k=0;k<collect[i][j].size();k++)
                if (job == collect[i][j][k])
                {
                    necessaryTime[i]=((averageSSJ/coreActiveSsj)*jobRuntime);
                    float responseTime=jobSubmitTime+necessaryTime[i];
                    if (realResponseTime[i]<responseTime)
                    {
                        realResponseTime[i]=responseTime;
                    }
                }
        }

        for (int j=0;j<collect[i].size();j++)
        {
            unsigned int coreActivePower=serverPool->getServer(j)->getCoreActivePower100();
            for (int k=0;k<collect[i][j].size();k++)
                if (job == collect[i][j][k])
                {
                    realPowerUsage[i]+=necessaryTime[i]*coreActivePower;
                }
        }
        realPowerUsage[i]=realPowerUsage[i]/3600000; //To Kwh
    }
}

void NSGAII::calculatePowerAndResponsetimeOfSolutions(Work *work, Queue *serverPool)
{
    unsigned int job=work->getJobNumber();
    for (int i=0;i<this->parents.size();i++) //for every individual
    {
        for (int j=0;j<parents[i].size();j++)
            for (int k=0;k<parents[i][j].size();k++)
                if (job == parents[i][j][k])
                {
                    necessaryTime[i]=((serverPool->getAverageSSJ_opsPerCoreAt100()/serverPool->getServer(j)->getCoreActiveSsj_ops100())*work->getRunTime());
                    float responseTime=work->getSubmitTime()+necessaryTime[i];
                    if (realResponseTime[i]<responseTime)
                    {
                        realResponseTime[i]=responseTime;
                    }
                }

        for (int j=0;j<parents[i].size();j++)
            for (int k=0;k<parents[i][j].size();k++)
                if (job == parents[i][j][k])
                {
                    realPowerUsage[i]+=necessaryTime[i]*serverPool->getServer(j)->getCoreActivePower100();
                }

        realPowerUsage[i]=realPowerUsage[i]/3600000; //To Kwh
    }
}

void NSGAII::printPowerAndResponsetime(void)
{

    for (int i=0;i<this->collect.size();i++) //for every individual
    {
        std::cout<<"Power Usage Individual ["<<i<<"]="<<realPowerUsage[i]<<std::endl;
        std::cout<<"Real Response Time Individual ["<<i<<"]="<<realResponseTime[i]<<std::endl;
    }
}

void NSGAII::insertIndividualInOrderedFront(unsigned int individual) //insert individual in Ordered front
{
    orderedFrontCD.push_back(individual);
}

void NSGAII::insertIndividualInParetoFront(unsigned int individual, unsigned int frontNumber) //insert individual in Pareto front
{
    paretoFront[frontNumber].push_back(individual);

}

void NSGAII::fastNonDominatedSort(Work *work, Queue *serverPool)
{

    //Clear paretoFront
    paretoFront.clear();

    //Clear dominationCounter[] and dominated[]
    for (unsigned int i=0;i<INDIVIDUALS*2;i++)
    {
        dominated[i].clear();
        dominationCounter[i]=0;
    }


    this->calculatePowerAndResponsetime(work, serverPool); //It will calculate the power usage and real runtime of every individual

        for (unsigned int i=0;i<this->collect.size();i++)
        {
            for (unsigned int j=0;j<this->collect.size();j++)
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

            for (unsigned int j=0;j<paretoFront[i].size();j++)
            {
                unsigned int individual=paretoFront[i][j];
                for (unsigned int z=0;z<dominated[individual].size();z++)
                //for (unsigned int z=0;z<dominated[j].size();z++)
                {
                    unsigned int q=dominated[individual][z];
                    dominationCounter[q]--;
                    if (0 == dominationCounter[q])
                    {
                        H.push_back(q);
                    }
                }
            }
            for (unsigned int j=0;j<H.size();j++)
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

void NSGAII::crowdingDistanceAssignment(unsigned int Front, Work *work, Queue *serverPool)
{
    orderedFrontCD.clear();
    unsigned int numberSolutions=paretoFront[Front].size();
    vector < std::pair <float, unsigned int> > responsetimeVector; //Real Responsetime of the solutions
    vector < std::pair <float, unsigned int > > powerVector; //Power consumption of the solutions

    //First of all we should normalized RunTime and Power
    normalizeResponsetimeAndPower(work, serverPool); //Normalized realRunTime and realPowerUsage vectors

    //Get runtime and power values of the solutions
    for (unsigned int i=0;i<numberSolutions;i++)
    {
        responsetimeVector.push_back(std::make_pair(realResponseTime[paretoFront[Front][i]],paretoFront[Front][i]));
        powerVector.push_back(std::make_pair(realPowerUsage[paretoFront[Front][i]], paretoFront[Front][i]));
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


    for (unsigned int i=0;i<numberSolutions;i++)
    {
        unsigned int maxValueIndex=returnMaxValueIndex(Front);
        insertIndividualInOrderedFront(paretoFront[Front][maxValueIndex]);
        crowdingDistance[paretoFront[Front][maxValueIndex]]=-1;
    }
}

unsigned int NSGAII::returnMaxValueIndex(unsigned int Front)
{
    unsigned int maxValueIndex=0;
    float maxValue=0;
    for (unsigned int i=0;i<paretoFront[Front].size();i++)
    {
        if (crowdingDistance[paretoFront[Front][i]] > maxValue)
        {
            maxValue=crowdingDistance[paretoFront[Front][i]];
            maxValueIndex = i;
        }
    }
    return maxValueIndex;
}

void NSGAII::generateOffspringPopulation(Work *work, Queue *serverPool)
{
    offspring.clear();
    //First we do tournament selection (copy the best parents to the offspring)
    copy(parents.begin(),parents.end(),back_inserter(offspring));

    unsigned int workSize = work->getAllocatedProcessors();

    //Then we do crossover
    std::minstd_rand generator(std::random_device{}());


                                        /*************************************** UPMX CROSSOVER (Uniform partially-mapped crossover) *********************************************/
    std::uniform_int_distribution<int> distribution1(0,INDIVIDUALS-1);
    std::uniform_int_distribution<int> distribution2(0,INDIVIDUALS-1);
    for (int i=0;i<offspring.size()*CROSSOVERFACTOR;i++)
    {
        int chromosome1 = distribution1(generator);

        int chromosome2 = chromosome1;

        while (chromosome2==chromosome1) //Chromosomes have to be different
        {
                chromosome2 = distribution2(generator);
        }


        vector<vector<int> > newIndividual;

        newIndividual= offspring[chromosome1];


        // We do n/3 steps
        unsigned int numberSteps=serverPool->getTotalCores()/3;
        std::uniform_int_distribution<int> distribution11(0,offspring[chromosome1].size()-1);
        for (unsigned int j=0;j<numberSteps;j++)
        {
            int selectedServer1=distribution11(generator);
            std::uniform_int_distribution<int> distribution12(0,offspring[chromosome1][selectedServer1].size()-1);
            int selectedCore1=distribution12(generator);

            unsigned int selected=offspring[chromosome1][selectedServer1][selectedCore1];
            unsigned int selectedServer2, selectedCore2;

            bool different =true;
            std::uniform_int_distribution<int> distribution21(0,offspring[chromosome2].size()-1);
            while (different)
            {
                selectedServer2=distribution21(generator);
                std::uniform_int_distribution<int> distribution22(0,offspring[chromosome2][selectedServer2].size()-1);
                selectedCore2=distribution22(generator);
                if (selected==offspring[chromosome2][selectedServer2][selectedCore2]) different=false;
            }
            offspring[chromosome1][selectedServer1][selectedCore1]=offspring[chromosome1][selectedServer2][selectedCore2];
            offspring[chromosome1][selectedServer2][selectedCore2]=selected;
        }
    }

                                                /*************************************** PERMUTATION MUTATION *********************************************/
    //Finally we mutate the individuals
    unsigned int i=0;
    std::uniform_int_distribution<int> distribution3(0,offspring.size()-1);
    while (i<offspring.size()*MUTATIONFACTOR)
    {
        //Select a random process and replace current core with a new random core
        int chromosome=distribution3(generator);
        std::uniform_int_distribution<int> distribution4(0,offspring[chromosome].size()-1);
        int selectedServer1=distribution4(generator);
        std::uniform_int_distribution<int> distribution5(0,offspring[chromosome][selectedServer1].size()-1);
        int selectedCore1=distribution5(generator);

        //Chromosome should be the same, we only have to use a different core and/or server
        std::uniform_int_distribution<int> distribution7(0,offspring[chromosome].size()-1);
        int selectedServer2=distribution7(generator);
        std::uniform_int_distribution<int> distribution8(0,offspring[chromosome][selectedServer2].size()-1);
        int selectedCore2=distribution8(generator);
        int secondCore=offspring[chromosome][selectedServer2][selectedCore2];
        offspring[chromosome][selectedServer2][selectedCore2]=offspring[chromosome][selectedServer1][selectedCore1];
        offspring[chromosome][selectedServer1][selectedCore1]=secondCore;
        i++;
    }
}

void NSGAII::generatePlot(unsigned int generation)
{

    std::ofstream plotFile;
    plotFile.open (PLOTFILE);
    for (int i=0;i<INDIVIDUALS;i++)
    {
        plotFile <<realResponseTime[i]<<" "<<realPowerUsage[i]<<"\n";
    }
    plotFile.close();
}

void NSGAII::normalizeResponsetimeAndPower(Work *work, Queue *serverPool)
{
    calculatePowerAndResponsetimeOfSolutions(work, serverPool);

    float maxPowerConsumption=0, maxRealResponseTime=0, minPowerConsumption=std::numeric_limits<float>::infinity(), minRealResponseTime=std::numeric_limits<float>::infinity();

    //Search for maximum values
    for (int i=0;i<INDIVIDUALS;i++) //All the solutions at parents
    {
        if (realResponseTime[i]>maxRealResponseTime) maxRealResponseTime=realResponseTime[i];
        if (realPowerUsage[i]>maxPowerConsumption) maxPowerConsumption=realPowerUsage[i];
    }

    //Search for minimum values
    for (int i=0;i<INDIVIDUALS;i++) //All the solutions at parents
    {
        if (realResponseTime[i]<minRealResponseTime) minRealResponseTime=realResponseTime[i];
        if (realPowerUsage[i]<minPowerConsumption) minPowerConsumption=realPowerUsage[i];
    }

    //Calculate Normalized values
    for (int i=0;i<INDIVIDUALS;i++) //All the solutions at parents
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

void NSGAII::calculateEuclideanDistanceFrom0(Work *work, Queue *serverPool)
{
    normalizeResponsetimeAndPower(work,serverPool);
    //Calculate euclidean distance p=(0,0), q=(q1, q2)
    for (int i=0;i<INDIVIDUALS;i++) //All the solutions at parents
    {
        euclideanDistance[i]=sqrt(realResponseTime[i]*realResponseTime[i] + realPowerUsage[i]*realPowerUsage[i]);
    }
}

std::vector<std::vector<int> > NSGAII::returnBestParent(Work *work, Queue *serverPool)
{
    calculateEuclideanDistanceFrom0(work,serverPool);
    unsigned int bestSolution=0;
    float minimumEuclideanDistance = std::numeric_limits<float>::infinity();

    //Find the best solution
    for (int i=0;i<INDIVIDUALS;i++) //All the solutions at parents
    {
        if (euclideanDistance[i]<minimumEuclideanDistance)
        {
            minimumEuclideanDistance=euclideanDistance[i];
            bestSolution=i;
        }
    }
    return parents[bestSolution];

}

std::vector<std::vector<int> > NSGAII::runNSGA(Queue *serverPool, Work *work)
{

    this->initializeVectors(serverPool);
    this->generateRandomPopulation(work);

    int currentGeneration=0;

    while (currentGeneration<GENERATIONS)
    {
        this->combineParentsOffspring();
        this->fastNonDominatedSort(work, serverPool);
        this->parents.clear();



        unsigned int i=0;

        while (((this->parents.size()+paretoFront[i].size())<INDIVIDUALS) && (0 < paretoFront[i].size()))
        {
            for (unsigned int j=0;j<paretoFront[i].size();j++)
            {
                if (parents.size()<INDIVIDUALS) parents.push_back(collect[paretoFront[i][j]]);
            }
            i++;
        }

        if (parents.size()<INDIVIDUALS) crowdingDistanceAssignment(i, work, serverPool); //i is the Front number with more individuals than the number of individuals needed

        //Fill the parents with the best individuals after being sorted according to the crowding distance
        unsigned int j=0;
        while (parents.size()<INDIVIDUALS)
        {
            parents.push_back(collect[orderedFrontCD[j]]);
            j++;
        }

        currentGeneration++;

        if (currentGeneration<GENERATIONS) this->generateOffspringPopulation(work,serverPool);

    }

    //Returns the best non-dominated solution (ordered)
    return returnBestParent(work, serverPool);
}
