#ifndef SERVER_HPP
#define SERVER_HPP
#include "core.hpp"
#include "work.hpp"
#include <vector>
#include <iostream>

class Server
{
		std::string serverName;
		unsigned int numberCPUs; //number of CPUs of the server
		unsigned int numberCoresCPU; //number of cores per CPU
		unsigned int idleCores; //number of idle cores in the server
		unsigned int ram;// Amount of RAM in MBs
		unsigned int ssj_op[11]; //ssj_ops per target load [IDLE, 10%, 20%, 30%, 40%, 50%, 60%, 70%, 80%, 90%, 100%]
		unsigned int activePower[11]; //Average Active Power in W per target load [IDLE, 10%, 20%, 30%, 40%, 50%, 60%, 70%, 80%, 90%, 100%]
		std::vector<Core *> cores; //vector of cores

		unsigned int coreActivePower100; //Active Power per core at 100% load
		unsigned int coreActiveSsj_ops100; //Ssj_ops per core at 100% load

	public:
		Server(std::string name, unsigned int nCPUs, unsigned int nCoresCPU, unsigned int ramMB);
		~Server();

        std::string getServerName(void);
        void setServerName(std::string name);

		unsigned int getNumberCPUs(void);
		void setNumberCPUs(unsigned int number);

		unsigned int getNumberCoresCPU(void);
		void setNumberCoresCPU(unsigned int number);

        unsigned int getIdleCores(void);
		void setIdleCores(unsigned int number);

		unsigned int getTotalCores(void);

		unsigned int getRam(void);
		void setRam(unsigned int ramMB);

        unsigned int getSsj_ops(unsigned int load);
		void setSsj_ops(unsigned int ops, unsigned int load);

		unsigned int getActivePower(unsigned int load);
		void setActivePower(unsigned int power, unsigned int load);

		Core *getCore(unsigned int pos);
		void addCore(Core *core);

 		void calculateCoreActivePower100(void); //Active Power per core
		void calculateCoreActiveSsj_ops100(void); //Ssj_ops per core

		unsigned int getCoreActivePower100(void); //Active Power per core
		unsigned int getCoreActiveSsj_ops100(void); //Ssj_ops per core

        void allocateWork_Server_FirstFit(Work *work, unsigned int numberCores);

		void printServer(void);

		void printAssignedWorks(void);

};

#endif
