#ifndef NSGA_II_HPP_INCLUDED
#define NSGA_II_HPP_INCLUDED

#include "queue.hpp"
#include <vector>
#include <random>
#include <ctime>
//#include <utility>      // std::pair, std::make_pair
#include <algorithm>    // std::sort
#include <functional>   // std::bind
#include <iostream>
#include <fstream>

using std::vector;

class NSGAII {

    vector<vector<vector<int> > > parents; //Three-dimensional Matrix of parent vector (Individual*Server*Core)
    vector<vector<vector<int> > > offspring; //Three-dimensional Matrix of children vector (Individual*Server*Core)
    vector<vector<vector<int> > > collect; //Three-dimensional Matrix of collect vector (Individual*Server*Core)
    vector<vector<int> > paretoFront; //FrontxIndividualPosition
    vector<int> orderedFrontCD; //Front ordered attending to crowding distance of the individuals
    float crowdingDistance[INDIVIDUALS*2]; //Crowding Distance for every individual
    unsigned int realResponseTime[INDIVIDUALS*2]; //Real Responsetime of the individuals/solutions
    float necessaryTime[INDIVIDUALS*2]; //RealRunTime
    float realPowerUsage[INDIVIDUALS*2]; //Power consumption of the individuals/solutions
    unsigned int dominationCounter[INDIVIDUALS*2]; //rank
    vector<unsigned int> dominated[INDIVIDUALS*2]; // dominated[i] will contain all the individuals that are dominated by "i"
    float euclideanDistance[INDIVIDUALS]; //Normalized Euclidean Distance

    public:
        NSGAII(unsigned int numIndividuals, unsigned int numGenerations);
        NSGAII();
        ~NSGAII();

        unsigned int getIndividuals(void);
        unsigned int getGenerations(void);
        void initializeVectors(Queue *serverPool);
        void generateRandomPopulation(Work *work);
        void printParents(void);
        void printCollect(void);
        void printParetoFronts(void);
        void printDominated(void);
        std::vector<std::vector<int> > runNSGA(Queue *serverPool, Work *work);
        void combineParentsOffspring(void);
        void calculatePowerAndResponsetime(Work *work, Queue *serverPool); //Collect
        void calculatePowerAndResponsetimeOfSolutions(Work *work, Queue *serverPool); //Parents for Euclidean Distance
        void printPowerAndResponsetime(void);
        void insertIndividualInOrderedFront(unsigned int individual);
        void insertIndividualInParetoFront(unsigned int individual, unsigned int frontNumber);
        void crowdingDistanceAssignment(unsigned int Front,Work *work, Queue *serverPool);
        void fastNonDominatedSort(Work *work, Queue *serverPool);
        void generateOffspringPopulation(Work *work, Queue *serverPool);
        unsigned int returnMaxValueIndex(unsigned int Front);
        void generatePlot(unsigned int generation);
        void normalizeResponsetimeAndPower(Work *work, Queue *serverPool);
        void calculateEuclideanDistanceFrom0(Work *work, Queue *serverPool);
        std::vector<std::vector<int> > returnBestParent(Work *work, Queue *serverPool);

};


#endif // NSGA_II_HPP_INCLUDED
