
#include "core.hpp"


Core::Core(unsigned int freq, cpuArchitecture_t type)
{
	this->frequency=freq;
	this->coreType=type;
	this->idle=true;
	this->assignedWork=-1;
}

Core::Core()
{
    this->idle=true;
    this->assignedWork=-1;
}

Core::~Core()
{
}

void Core::setFrequency(unsigned int freq)
{
	this->frequency=freq;
}

unsigned int Core::getFrecuency(void)
{
	return this->frequency;
}

void Core::setCoreType(cpuArchitecture_t type)
{
	this->coreType=type;

}

cpuArchitecture_t Core::getCoreType(void)
{
	return this->coreType;
}

void Core::setCurrentLoad(unsigned int load)
{
	this->currentLoad=load;
}

unsigned int Core::getCurrentLoad(void)
{
	return this->currentLoad;
}

void Core::setAssignedWork(int workId)
{
    this->assignedWork=workId;
}
int Core::getAssignedWork(void)
{
    return this->assignedWork;
}

void Core::setIdle(bool isIdle)
{
    this->idle=isIdle;
}

bool Core::isIdle(void)
{
    return this->idle;
}
