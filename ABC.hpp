#ifndef ABC_HPP_INCLUDED
#define ABC_HPP_INCLUDED

#include "queue.hpp"
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>    // std::sort
#include <functional>   // std::bind
#include <iostream>
#include <fstream>

using std::vector;

class ABC {

    vector<vector<vector<int> > > foodSources; //Three-dimensional Matrix of individuals (Individual*Server*Core)
    vector<vector<vector<int> > > onlookerSources; //Three-dimensional Matrix of individuals (Individual*Server*Core)
    vector<vector<vector<int> > > bestFoodSources; //Three-dimensional Matrix of individuals (Individual*Server*Core)
    vector<vector<int> > randomBestFoodSource1,randomBestFoodSource2;


    vector<unsigned int> trial; //Number of tries in every foodSource
    vector<vector<int> > paretoFront; //FrontxIndividualPosition
    vector<unsigned int> dominated[SWARMSIZE]; // dominated[i] will contain all the individuals that are dominated by "i"
    unsigned int dominationCounter[SWARMSIZE]; //rank
    float MOFitness[FOODSOURCES];
    float probabilityVector[FOODSOURCES];

    unsigned int realResponseTime[SWARMSIZE]; //Real Responsetime of the individuals/solutions
    float necessaryTime[SWARMSIZE]; //RealRunTime
    float realPowerUsage[SWARMSIZE]; //Power consumption of the individuals/solutions
    float euclideanDistance[SWARMSIZE]; //Normalized Euclidean Distance
    vector<vector<float>> crowdingDistance; //Crowding Distance for every individual of each Front

    public:
        ABC();
        ~ABC();

        void printFoodSources(void);
        void printParetoFronts(void);
        void printCrowdingDistance(void);
        void printOrderedFront(void);
        void printMOFitness(void);
        void printProbabilityVector(void);

        void initializeVectors(Queue *serverPool);
        void generateRandomFoodSources(Work *work);
        void fastNonDominatedSort(Work *work, Queue *serverPool);
        void calculatePowerAndResponsetime(Work *work, Queue *serverPool);
        void printPowerAndResponsetime(void);
        void insertIndividualInParetoFront(unsigned int individual, unsigned int frontNumber);

        void sendEmployedBees(Work *work,Queue *serverPool, unsigned int generation);
        void sendOnlookerBees(Work *work,Queue *serverPool);
        unsigned int selectEmployedBee(void);
        void sendScoutBees(Queue *serverPool, unsigned int generation);
        bool dominates(Work *work,Queue *serverPool,vector<vector<int>> bee1, vector<vector<int>> bee2, unsigned int positionBee2);
        bool dontDominates(Work *work,Queue *serverPool,unsigned int positionOnlookerBee, vector<vector<int>> mutatedBee);

        void generateProbabilityVector(Work *work,Queue *serverPool);
        void generateCompleteSwarm(void);

        std::vector<std::vector<int> > runABC(Queue *serverPool, Work *work);
        void crowdingDistanceAssignment(Work *work, Queue *serverPool);
        std::vector<std::vector<int>>  returnBestFoodSource(Work *work, Queue *serverPool);
        void calculateEuclideanDistanceFrom0(Work *work, Queue *serverPool);
        void normalizeResponsetimeAndPower(Work *work, Queue *serverPool);
        void calculatePowerAndResponsetimeOfSolutions(Work *work, Queue *serverPool); //Parents for Euclidean Distance
        unsigned int returnMaxValueIndex(unsigned int Front);
        void insertIndividualInOrderedFront(unsigned int individual);
};

#endif // ABC_HPP_INCLUDED
