#ifndef MOHEFT_HPP_INCLUDED
#define MOHEFT_HPP_INCLUDED

#include "queue.hpp"
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>    // std::sort
#include <functional>   // std::bind
#include <iostream>
#include <fstream>
#include "types.hpp"

using std::vector;

class MOHEFT {

    vector<vector<vector<int> > > solutions; //Three-dimensional Matrix of solutions (SolutionNumber*Server*Core)
    unsigned int realResponseTime[K]; //Real Responsetime of the solutions
    float realPowerUsage[K]; //Power consumption of the solutions

    float crowdingDistance[K]; //Crowding Distance for every solution
    float euclideanDistance[K]; //Normalized Euclidean Distance
    unsigned int nextServer=0;

    public:

        MOHEFT();
        ~MOHEFT();


        void initializeSolutions(Queue *serverPool);
        void generateSolutions(Work *work,Queue *serverPool);
        void printSolutions(void);

        std::vector<std::vector<int> > runMOHEFT(Queue *serverPool, Work *work);
        void calculatePowerAndResponsetime(Work *work, Queue *serverPool);
        void insertSolutionInOrderedVector(unsigned int individual);
        void crowdingDistanceAssignment(Work *work, Queue *serverPool);
        void normalizeResponsetimeAndPower(Work *work, Queue *serverPool);
        void calculateEuclideanDistanceFrom0(Work *work, Queue *serverPool);
        std::vector<std::vector<int> > returnBestSolution(Work *work, Queue *serverPool);

};



#endif // MOHEFT_HPP_INCLUDED
