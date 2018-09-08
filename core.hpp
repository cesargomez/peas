#ifndef CORE_HPP
#define CORE_HPP
#include "types.hpp"
#include <string>

class Core {

		unsigned int frequency; // Frecuency of the core in MHz
		cpuArchitecture_t coreType; //Architecture of the CPU
		unsigned int currentLoad; //percentage of use of the core (from IDLE=0% to 100%)

		bool idle;
		int assignedWork;

	public:
		Core(unsigned int freq, cpuArchitecture_t type);
		Core();
		~Core();

		void setFrequency(unsigned int freq);
		unsigned int getFrecuency(void);

		void setCoreType(cpuArchitecture_t freq);
		cpuArchitecture_t getCoreType(void);

		void setCurrentLoad(unsigned int load);
		unsigned int getCurrentLoad(void);

        void setAssignedWork(int workId);
        int getAssignedWork(void);

        void setIdle(bool isIdle);
        bool isIdle(void);
};
#endif
