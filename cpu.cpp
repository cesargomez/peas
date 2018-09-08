#include "cpu.hpp"


CPU::CPU(unsigned int freq, cpuArchitecture_t type, unsigned int numCores, std::string name)
{
	this->frequency=freq;
	this->cpuType=type;
	this->cores=numCores;
	this->currentLoad= new unsigned int[numCores];
	this->cpuName=name;

}

CPU::~CPU()
{
	delete[] this->currentLoad;
	this->currentLoad=0;
}

void CPU::setFrequency(unsigned int freq)
{
	this->frequency=freq;
}

unsigned int CPU::getFrecuency(void)
{
	return this->frequency;
}

void CPU::setCpuType(cpuArchitecture_t type)
{
	this->cpuType=type;

}

cpuArchitecture_t CPU::getCpuType(void)
{
	return this->cpuType;
}

void CPU::setCores(unsigned int numCores)
{
	this->cores=numCores;
}

unsigned int CPU::getCores(void)
{
	return this->cores;
}

void CPU::setCurrentLoad(unsigned int load, unsigned int core)
{
	this->currentLoad[core]=load;
}

unsigned int CPU::getCurrentLoad(unsigned int core)
{
	return this->currentLoad[core];
}

void CPU::setCpuName(std::string name)
{
	this->cpuName=name;
}

std::string CPU::getCpuName(void)
{
	return this->cpuName;
}

void CPU::setAsignedWork(int workId)
{
    this->assignedWork=workId;
}
int CPU::getAsignedWork(void)
{
    return this->assignedWork;
}

void CPU::setIdle(bool isIdle)
{
    this->idle=isIdle;
}

bool CPU::getIdle(void)
{
    return this->idle;
}
