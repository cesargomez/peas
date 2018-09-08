#include "server.hpp"

Server::Server(std::string name, unsigned int nCPUs, unsigned int nCoresCPU, unsigned int ramMB)
{
    unsigned int i, cores=nCPUs*nCoresCPU;
    Core *core;
    this->setServerName(name);
    this->setNumberCPUs(nCPUs);
    this->setNumberCoresCPU(nCoresCPU);
    this->setRam(ramMB);
    this->setIdleCores(nCPUs*nCoresCPU); //the number of idle cores at the beginning will be the total number of cores
    for (i=0;i<cores;i++)
    {
        core=new Core();
        this->addCore(core);
    }
}
Server::~Server()
{

}

std::string Server::getServerName(void)
{
    return this->serverName;
}

void Server::setServerName(std::string name)
{
    this->serverName=name;
}

unsigned int Server::getNumberCPUs(void)
{
    return this->numberCPUs;
}

void Server::setNumberCPUs(unsigned int number)
{
    this->numberCPUs=number;
}

unsigned int Server::getNumberCoresCPU(void)
{
    return this->numberCoresCPU;
}

void Server::setNumberCoresCPU(unsigned int number)
{
    this->numberCoresCPU=number;
}

unsigned int Server::getIdleCores(void)
{
    return this->idleCores;
}

void Server::setIdleCores(unsigned int number)
{
    this->idleCores=number;
}

unsigned int Server::getTotalCores(void)
{
    return (this->numberCoresCPU)*(this->numberCPUs);
}

unsigned int Server::getRam(void)
{
    return this->ram;
}

void Server::setRam(unsigned int ramMB)
{
    this->ram=ramMB;
}

unsigned int Server::getSsj_ops(unsigned int load)
{
    return this->ssj_op[load];
}

void Server::setSsj_ops(unsigned int ops, unsigned int load)
{
    this->ssj_op[load]=ops;
}

unsigned int Server::getActivePower(unsigned int load)
{
    return this->activePower[load];
}

void Server::setActivePower(unsigned int power, unsigned int load)
{
    this->activePower[load]=power;
}

Core *Server::getCore(unsigned int pos)
{
    return this->cores[pos];
}

void Server::addCore(Core *core)
{
    this->cores.push_back(core);
}

void Server::calculateCoreActivePower100(void)
{
        coreActivePower100=(getActivePower(10)/(getNumberCoresCPU()*getNumberCPUs()));
}

void Server::calculateCoreActiveSsj_ops100(void)
{
        coreActiveSsj_ops100=(getSsj_ops(10)/(getNumberCoresCPU()*getNumberCPUs()));
}

unsigned int Server::getCoreActivePower100(void)
{
        return coreActivePower100;
}

unsigned int Server::getCoreActiveSsj_ops100(void)
{
        return coreActiveSsj_ops100;
}

void Server::printServer(void)
{
    std::cout<<"--------------------------------------------------------------"<<std::endl;
    std::cout<<this->getServerName()<<std::endl;
    std::cout<<"--------------------------------------------------------------"<<std::endl;
    std::cout<<"Number of CPUs: "<<this->getNumberCPUs()<<std::endl;
    std::cout<<"Number of cores per CPU: "<<this->getNumberCoresCPU()<<std::endl;
    std::cout<<"RAM: "<<this->getRam()<<std::endl;
    unsigned int i;
    for (i=0;i<11;i++)
    {
            if (0==i)
            {
                std::cout<<"SSJ_OPS "<<"["<<i*10<<"%]:"<<this->getSsj_ops(i)<<std::endl;
            std::cout<<"Active Power "<<"["<<i*10<<"%]:"<<this->getActivePower(i)<<std::endl;
            }
            else{
                std::cout<<"SSJ_OPS "<<"["<<i*10<<"%]:"<<this->getSsj_ops(i)<<std::endl;
                std::cout<<"Active Power "<<"["<<i*10<<"%]:"<<this->getActivePower(i)<<std::endl;
            }
    }


    std::cout<<"                                                              "<<std::endl;
    std::cout<<"                                                              "<<std::endl;
}


void Server::allocateWork_Server_FirstFit(Work *work, unsigned int numberCores)
{
    unsigned int allocated=0;
    for (std::vector<Core *>::iterator it = cores.begin(); it != cores.end(); ++it)
    {
        if ((*it)->isIdle() && allocated!=numberCores) //If the core is Idle and there are unallocated cores then the work is assigned
        {
            (*it)->setIdle(false);
            (*it)->setAssignedWork(work->getJobNumber());
            allocated++;
        }
    }
}

void Server::printAssignedWorks(void)
{
    for (std::vector<Core *>::iterator it = cores.begin(); it != cores.end(); ++it)
    {
        std::cout<<(*it)->getAssignedWork()<<" ";
    }
    std::cout<<std::endl;
}
