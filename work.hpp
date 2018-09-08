#ifndef WORK_HPP
#define WORK_HPP
#include <iostream>
/* The Standard Workload Format (swf) was defined in order to ease the use of workload logs and models. With it, programs that analyze workloads or simulate system scheduling need only be able to parse a single format, and can be applied to multiple workloads.
The standard workload format abides by the following principles:

The files are portable and easy to parse:
Each workload is stored in a single ASCII file.
Each job (or roll) is represented by a single line in the file.
Lines contain a predefined number of fields, which are mostly integers, separated by whitespace. Fields that are irrelevant for a specific log or model appear with a value of -1.
Comments are allowed, as identified by lines that start with a `;'. In particular, files are expected to start with a set of header comments that define the environment or model.
The same format is used for models and logs. This implies that in each context certain fields may be redundant; for example, logs do not contain data about feedback and depdencies among jobs, which might appear in models, whereas models do not contain data about wait times in queues.
The format is completely defined, with no scope for user extensability. Thus you are guaranteed to be able to parse any file that adheres to the standard, and multiple competing and incompatible extensions are avoided. If experience shows that important attributes have been left out, they will be included in the future by creating an updated version of the standard.
*/

class Work
{
    int jobNumber; // a counter field, starting from 1.
    int submitTime; // in seconds. The earliest time the log refers to is zero, and is the submittal time the of the first job. The lines in the log are sorted by ascending submittal times. It makes sense for jobs to also be numbered in this order.
    int waitTime; //  in seconds. The difference between the job's submit time and the time at which it actually began to run. Naturally, this is only relevant to real logs, not to models.
    int runTime; // in seconds. The wall clock time the job was running (end time minus start time).
    int allocatedProcessors; //an integer. In most cases this is also the number of processors the job uses; if the job does not use all of them, we typically don't know about it.
    int averageCPUTimeUsed; //both user and system, in seconds. This is the average over all processors of the CPU time used, and may therefore be smaller than the wall clock runtime. If a log contains the total CPU time used by all the processors, it is divided by the number of allocated processors to derive the average.
    int usedMemory; //  in kilobytes. This is again the average per processor.
    int requestedNumberProcessors;
    int requestedTime; // This can be either runtime (measured in wallclock seconds), or average CPU time per processor (also in seconds) -- the exact meaning is determined by a header comment. In many logs this field is used for the user runtime estimate (or upper bound) used in backfilling. If a log contains a request for total CPU time, it is divided by the number of requested processors.
    int requestedMemory; // (again kilobytes per processor).
    int status; // 1 if the job was completed, 0 if it failed, and 5 if cancelled. If information about chekcpointing or swapping is included, other values are also possible. See usage note below. This field is meaningless for models, so would be -1.
    int userId; //  a natural number, between one and the number of different users.
    int groupId; // a natural number, between one and the number of different groups. Some systems control resource usage by groups rather than by individual users.
    int appNumber; //a natural number, between one and the number of different applications appearing in the workload. in some logs, this might represent a script file used to run jobs rather than the executable directly; this should be noted in a header comment.
    int queueNumber; //  a natural number, between one and the number of different queues in the system. The nature of the system's queues should be explained in a header comment. This field is where batch and interactive jobs should be differentiated: we suggest the convention of denoting interactive jobs by 0.
    int partitionNumber; // a natural number, between one and the number of different partitions in the systems. The nature of the system's partitions should be explained in a header comment. For example, it is possible to use partition numbers to identify which machine in a cluster was used.
    int precedingJobNumber; // this is the number of a previous job in the workload, such that the current job can only start after the termination of this preceding job. Together with the next field, this allows the workload to include feedback as described below.
    int thinkTimeFromPrecedingJob; // this is the number of seconds that should elapse between the termination of the preceding job and the submittal of this one.
    /*********************************/
    float powerUsage; //This is based on the real power usage once the work has finished
    unsigned int realRunTime; //Real time expent by the work on the simulation
    unsigned int realFinishTime; //Real finish time of the work
    unsigned int realStartTime;
    unsigned int realResponseTime;
    /*********************************/
    public:
        int getJobNumber (void);
        int getSubmitTime (void);
        int getWaitTime (void);
        int getRunTime (void);
        int getAllocatedProcessors (void);
        int getAverageCPUTimeUsed (void);
        int getUsedMemory (void);
        int getRequestedNumberProcessors (void);
        int getRequestedTime (void);
        int getRequestedMemory (void);
        int getStatus (void);
        int getUserId (void);
        int getGroupId (void);
        int getAppNumber (void);
        int getQueueNumber (void);
        int getPartitionNumber (void);
        int getPrecedingJobNumber (void);
        int getThinkTimeFromPrecedingJob (void);

        float getPowerUsage(void);
        unsigned int getRealRunTime(void);
        unsigned int getRealFinishTime(void);
        unsigned int getRealStartTime(void);
        unsigned int getRealResponseTime(void);

        void  setJobNumber (int number);
        void  setSubmitTime (int number);
        void  setWaitTime (int number);
        void  setRunTime (int number);
        void  setAllocatedProcessors (int number);
        void  setAverageCPUTimeUsed (int number);
        void  setUsedMemory (int number);
        void  setRequestedNumberProcessors (int number);
        void  setRequestedTime (int number);
        void  setRequestedMemory (int number);
        void  setStatus (int number);
        void  setUserId (int number);
        void  setGroupId (int number);
        void  setAppNumber (int number);
        void  setQueueNumber (int number);
        void  setPartitionNumber (int number);
        void  setPrecedingJobNumber (int number);
        void  setThinkTimeFromPrecedingJob (int number);

        void setPowerUsage(float number);
        void setRealRunTime(unsigned int number);
        void setRealFinishTime(unsigned int number);
        void setRealStartTime(unsigned int number);
        void setRealResponseTime(unsigned int number);

        Work();
        ~Work();
        void printWork(void);

};

#endif
