#ifndef TYPES_HPP
#define TYPES_HPP

//NSGA-II
#define INDIVIDUALS 50
#define GENERATIONS 100
#define CROSSOVERFACTOR 0.9
#define MUTATIONFACTOR 0.1

//MOHEFT
#define K 2

//Cluster configuration
#define MAXNUMCORES 128
#define MAXNUMSERVERS 100

#define NSGAIIPLOT 1 //0=off, 1=on
#define PLOTFILE "plot.txt"

#define DELAYFACTOR 1.2
#define MAXJOBNUMBER 1000000


//ABC
#define SWARMSIZE 100 //population size
#define FOODSOURCES 50
#define SCOUTS 50 //Number of scourts bees
#define EMPLOYED 50
#define ATTEMPTS 15 //Number of attempts to consider a food source abandoned


enum cpuArchitecture_t {
	x86_64=0,
    x86_32,
	ia64,
	arm,
	mips
};

#endif
