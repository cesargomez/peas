# PEAS Simulator
PEAS - The Performance and Energy Aware Simulator

## Intro
PEAS Simulator is an event-driven job scheduler simulator that is meant to help in the evaluation and optimal utilization of hardware and energy resources of HPC environments.

## How does the simulator work?
1. Prior to the start of the simulation, a file containing the jobs workload in SWF format [0] and a CSV file with the cluster configuration, with data taken from the SPECpower_ssj2008 benchmark [1], are processed.
2. After the workload and the hardware resources are loaded into the simulator, the scheduling and resource selection policies are read from a configuration file [2].
3. Once all the configuration files are properly processed, the scheduler starts to decide which jobs are processed (Policy Scheduler) and, with the help of the Resource Selector, assigns free resources to the highest priority request. If the job requests more resources than the total number of resources provided by the whole cluster, the request is not accepted by the scheduler (Unaccepted Jobs). If at a particular moment there are not enough free resources, the job is queued (Queued Jobs) and waits until enough resources are available.
4. Finally, when all the jobs are finished, STDOUT will contain all the evaluation metrics of every job and the whole schedule log.

[0] The [Standard Workload Format](http://www.cs.huji.ac.il/labs/parallel/workload/swf.html) - The standard workload format (swf) was defined in order to ease the use of workload logs and models. With it, programs that analyze workloads or simulate system scheduling need only be able to parse a single format, and can be applied to multiple workloads. Examples of workload logs can be downloaded from [here](http://www.cs.huji.ac.il/labs/parallel/workload/logs.html).

[1] [SPECpower_ssj2008 Results](https://www.spec.org/power_ssj2008/results/) - The SPEC Power® benchmark suite measures the power and performance characteristics of server-class computer equipment. It is used to compare power and performance among different servers and serves as a toolset for use in improving server efficiency. This benchmark is targeted for use by hardware vendors, IT industry, computer manufacturers, and governments.

[2] PEAS Simulator configuration file - There is an example configuration file [here](https://github.com/cesargomez/peas/blob/master/config.cfg). There are some example clusters [here](https://github.com/cesargomez/peas/tree/master/Clusters) and some workloads [here](https://github.com/cesargomez/peas/tree/master/Workloads)

## How to run the simulator?
1. Clone the repo: ```git clone https://github.com/cesargomez/peas.git```
2. Compile the code:```cd peas && make```
3. Run it: ```./bin/Release/PEAS config.cfg```

If you want to change the configuration check the [config file](https://github.com/cesargomez/peas/blob/master/config.cfg).

## How to cite this work
* PEAS Simulator: [Performance and energy aware scheduling simulator for HPC: evaluating different resource selection methods](https://onlinelibrary.wiley.com/doi/10.1002/cpe.3607)
* Fattened backfilling algorithm: [Fattened backfilling: An improved strategy for job scheduling in parallel systems](https://www.sciencedirect.com/science/article/pii/S0743731516300788)
* Artificial bee colony algorithm: [Optimization of resources in parallel systems using a multiobjective artificial bee colony algorithm](https://link.springer.com/article/10.1007/s11227-018-2407-5)

## License
Please, refer to the [LICENSE file](https://github.com/cesargomez/peas/blob/master/LICENSE)
