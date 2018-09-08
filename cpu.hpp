#include "types.hpp"
#include <string>

class CPU {

		unsigned int frequency; // Frecuency of the processor in MHz
		cpuArchitecture_t cpuType; //Architecture of the CPU
		//unsigned int cores; //Number of cores of the CPU (from 0 to cores-1)
		unsigned int *currentLoad; //Vector with the the percentage of use of every core (from IDLE=0% to 100%)
		std::string cpuName; //Name of the CPU
		bool idle=true;
		int assignedWork=-1;

	public:
		CPU(unsigned int freq, cpuArchitecture_t type, unsigned int cores, std::string cpuName);
		~CPU();

		void setFrequency(unsigned int freq);
		unsigned int getFrecuency(void);

		void setCpuType(cpuArchitecture_t freq);
		cpuArchitecture_t getCpuType(void);

		void setCores(unsigned int numCores);
		unsigned int getCores(void);

		void setCurrentLoad(unsigned int load, unsigned int core);
		unsigned int getCurrentLoad(unsigned int core);

		void setCpuName(std::string name);
		std::string getCpuName(void);

        void setAsignedWork(int workId);
        int getAsignedWork(void);

        void setIdle(bool isIdle);
        bool getIdle(void);
};
