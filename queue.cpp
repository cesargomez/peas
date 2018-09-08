#include "queue.hpp"
#include "csv_parser.hpp"

Queue::Queue()
{


}

Queue::~Queue()
{

}

void Queue::addServer(Server *server)
{
    this->servers.push_back(server);
}

Server *Queue::getServer(unsigned int pos)
{
    return this->servers[pos];
}

unsigned int Queue::getNumberServers()
{
    return this->servers.size();
}
void Queue::readServerList(std::string filename)
{
        /* Declare the variables to be used */
        const char field_terminator = ',';
        const char line_terminator  = '\n';
        const char enclosure_char   = '"';

        csv_parser file_parser;

        /* Define how many records we're gonna skip. This could be used to skip the column definitions. */
        file_parser.set_skip_lines(1);

        /* Specify the file to parse */
        file_parser.init(filename.c_str());

        /* Here we tell the parser how to parse the file */
        file_parser.set_enclosed_char(enclosure_char, ENCLOSURE_OPTIONAL);

        file_parser.set_field_term_char(field_terminator);

        file_parser.set_line_term_char(line_terminator);

        unsigned int row_count = 1U;

        /* Check to see if there are more records, then grab each row one at a time */
        while(file_parser.has_more_rows())
        {

                /* Get the record */
                csv_row row = file_parser.get_row();
                Server *server=new Server(row[3].c_str(), atoi(row[40].c_str()), atoi(row[41].c_str()), atoi(row[51].c_str()));

                unsigned int i;
                for (i=0;i<11;i++)
                {
                    if (0==i) //If load==0 no SSJ_OPS and IDLE consumption
                    {
                        server->setSsj_ops(0, i);
                        server->setActivePower(atoi(row[28].c_str()), i);
                    }
                    else
                    {
                    server->setSsj_ops(atoi(row[18-i].c_str()), i);
                    server->setActivePower(atoi(row[28-i].c_str()), i);
                    }
                }

                server->calculateCoreActivePower100(); //Active Power per core
                server->calculateCoreActiveSsj_ops100(); //Ssj_ops per core
                this->servers.push_back(server);
        }

}


void Queue::printServerList(void)
{
    for (std::vector<Server *>::iterator it = servers.begin(); it != servers.end(); ++it) (*it)->printServer();
}

float Queue::getAverageSSJ_opsPerCoreAt100(void) //Get the average SSJ_OPs per core at 100% target load
{
    return AverageSSJ_opsPerCoreAt100;
}


void Queue::calculateAverageSSJ_opsPerCoreAt100(void) //Get the average SSJ_OPs per core at 100% target load
{
    float totalNumberCores=0;
    float totalSSJ_OpsAt100=0;

    for (std::vector<Server *>::iterator it = servers.begin(); it != servers.end(); ++it)
    {
        totalNumberCores+=((*it)->getNumberCPUs())*((*it)->getNumberCoresCPU());
        totalSSJ_OpsAt100+=(*it)->getSsj_ops(10);
    }
    AverageSSJ_opsPerCoreAt100=totalSSJ_OpsAt100/totalNumberCores;
}

unsigned int Queue::getIdleCores(void)
{

    unsigned int idleCores = 0;
    Core *core;
    for (std::vector<Server *>::iterator it = servers.begin(); it != servers.end(); ++it)
    {
        for (unsigned int i=0; i<(*it)->getTotalCores();i++)
        {
            core=(*it)->getCore(i);
            if (-1 == core->getAssignedWork())
            {
                idleCores++;
            }
        }
    }
    return idleCores;
}

unsigned int Queue::getTotalCores(void)
{
    unsigned int totalCores = 0;
    for (std::vector<Server *>::iterator it = servers.begin(); it != servers.end(); ++it)
    {
        totalCores+= (*it)->getTotalCores();
    }
    return totalCores;
}

void Queue::allocateWork_FirstFit(Work *work, unsigned int simulationTime)
{
    bool allocated=false;
    unsigned int remainingCores= work->getAllocatedProcessors();
    unsigned int realRunTime = 0;

    while (!allocated)
    {
        for (std::vector<Server *>::iterator it = servers.begin(); it != servers.end(); ++it)
        {
            if (remainingCores<=(*it)->getIdleCores())
            {
                (*it)->allocateWork_Server_FirstFit(work,remainingCores);
                (*it)->setIdleCores((*it)->getIdleCores()-remainingCores);
                realRunTime=(this->getAverageSSJ_opsPerCoreAt100()/(*it)->getCoreActiveSsj_ops100())*work->getRunTime(); //Real Run time at 100%
                if (realRunTime > work->getRealRunTime())
                {
                    work->setRealRunTime(realRunTime);
                }
                work->setRealFinishTime(work->getRealRunTime()+simulationTime);
                work->setRealStartTime(simulationTime);
                work->setRealResponseTime(work->getRealFinishTime()-work->getSubmitTime());
                allocated=true;
                break;
            }
            else if (remainingCores>(*it)->getIdleCores()) //si se necesitan más procesadores de los que hay libres en el servidor
            {
                if (0<(*it)->getIdleCores()) //Si hay algún procesador libre
                {
                    remainingCores-=(*it)->getIdleCores();
                    //Calcular tiempo de finalización y consumo
                    (*it)->allocateWork_Server_FirstFit(work,(*it)->getIdleCores());
                    (*it)->setIdleCores(0);

                    realRunTime=(this->getAverageSSJ_opsPerCoreAt100()/(*it)->getCoreActiveSsj_ops100())*work->getRunTime(); //Real Run time at 100%
                    if (realRunTime > work->getRealRunTime())
                    {
                        work->setRealRunTime(realRunTime);//UpdateRealRunTime
                    }
                }
                else continue; //si no hay ningún procesador libre
            }
        }
    }
}
void Queue::allocateWork_MOHEFT(Work *work, unsigned int simulationTime, std::vector<std::vector<int> > &solution)
{
    bool allocated=false;
    unsigned int remainingCores= work->getAllocatedProcessors();
    unsigned int realRunTime = 0;

    work->setRealStartTime(simulationTime);
    for (unsigned i=0;i<solution.size();i++)
    {
        for (unsigned j=0;j<solution[i].size();j++)
        {
            if (work->getJobNumber() == solution[i][j])
                {
                    (*servers.at(i)).getCore(j)->setAssignedWork(solution[i][j]);
                    (*servers.at(i)).getCore(j)->setIdle(false);
                    (*servers.at(i)).setIdleCores((*servers.at(i)).getIdleCores()-1);

                    realRunTime=(this->getAverageSSJ_opsPerCoreAt100()/(*servers.at(i)).getCoreActiveSsj_ops100())*work->getRunTime(); //Real Run time at 100%
                    if (realRunTime > work->getRealRunTime())
                    {
                        work->setRealRunTime(realRunTime);
                    }
                }
        }
        //std::cout<<std::endl;
    }
    work->setRealFinishTime(work->getRealRunTime()+work->getRealStartTime());
    work->setRealResponseTime(work->getRealFinishTime()-work->getSubmitTime());
    std::cout<<"Job: "<<work->getJobNumber()<<" Finish time: "<<work->getRealFinishTime()<<std::endl;
}

void Queue::allocateWork_NSGA_II(Work *work, unsigned int simulationTime, std::vector<std::vector<int> > &solution)
{
    bool allocated=false;
    unsigned int remainingCores= work->getAllocatedProcessors();
    unsigned int realRunTime = 0;

    work->setRealStartTime(simulationTime);

    for (unsigned i=0;i<solution.size();i++)
    {
        for (unsigned j=0;j<solution[i].size();j++)
        {
            if (work->getJobNumber() == solution[i][j])
                {
                    (*servers.at(i)).getCore(j)->setAssignedWork(solution[i][j]);
                    (*servers.at(i)).getCore(j)->setIdle(false);
                    (*servers.at(i)).setIdleCores((*servers.at(i)).getIdleCores()-1);

                    realRunTime=(this->getAverageSSJ_opsPerCoreAt100()/(*servers.at(i)).getCoreActiveSsj_ops100())*work->getRunTime(); //Real Run time at 100%
                    if (realRunTime > work->getRealRunTime())
                    {
                        work->setRealRunTime(realRunTime);
                    }
                }
        }
        //std::cout<<std::endl;
    }
    work->setRealFinishTime(work->getRealRunTime()+work->getRealStartTime());
    work->setRealResponseTime(work->getRealFinishTime()-work->getSubmitTime());
    std::cout<<"Job: "<<work->getJobNumber()<<" finish time: "<<work->getRealFinishTime()<<std::endl;
}

void Queue::calculateWorkPowerUsage(Work *work)
{
    float powerUsage=0; //Power usage in Kwh
    unsigned int i=0;
    Core *core;
    for (std::vector<Server *>::iterator it = servers.begin(); it != servers.end(); ++it)
    {
            for (i=0; i<(*it)->getTotalCores();i++)
            {
               core=(*it)->getCore(i);
               if (work->getJobNumber()==core->getAssignedWork())
               {
                   powerUsage+=work->getRealRunTime()*(*it)->getCoreActivePower100(); //Active power at 100%
               }
            }
    }
    work->setPowerUsage(powerUsage/3600000);
}

void Queue::FreeServers(unsigned int jobNumber)
{
    unsigned int i=0;//, numberCoresFreed=0;
    Core *core;
    for (std::vector<Server *>::iterator it = servers.begin(); it != servers.end(); ++it)
    {
            unsigned int CoresFreed=0;
            for (i=0; i<(*it)->getTotalCores();i++)
            {
                core=(*it)->getCore(i);
                if (jobNumber == core->getAssignedWork())
                {
                    CoresFreed++;
                    core->setAssignedWork(-1);
                    core->setIdle(true);
                }
            }
            (*it)->setIdleCores((*it)->getIdleCores()+CoresFreed);
    }
}

void Queue::printAssignedWorks(void)
{

    std::cout<<"Cluster status: "<<std::endl;
    std::cout<<"------------------------------------------------------------------------"<<std::endl;
    for (std::vector<Server *>::iterator it = servers.begin(); it != servers.end(); ++it)
    {
        (*it)->printAssignedWorks();
    }
    std::cout<<"------------------------------------------------------------------------"<<std::endl<<std::endl;
}
