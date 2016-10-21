/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"


/**
  Stores information making up a job to be scheduled including any statistics.

  You may need to define some global variables or a struct to store your job queue elements.
*/
typedef struct _job_t
{
  int job_id;
  int sent_time;
  int end_time ;
  int wait_time ;
  int run_time;
  int priority;
	int rrindex ;

} job_t;
priqueue_t* queues;
int fcfs(const void * a, const void * b)
{

  job_t job1 = *((job_t *)a);
  job_t job2 = *((job_t *)b);
	if (job1.job_id == job2.job_id)
		return 0;
	return ( job1.sent_time -  job2.sent_time );

}
int sjf(const void * a, const void * b)
{
  job_t job1 = *((job_t *)a);
  job_t job2 = *((job_t *)b);
	if (job1.job_id == job2.job_id)
		return 0;
	return ( job1.run_time -  job2.run_time );
}

int psjf(const void * a, const void * b)
{
  job_t job1 = *((job_t *)a);
  job_t job2 = *((job_t *)b);
	if (job1.job_id == job2.job_id)
		return 0;
  if(job1.priority != job2.priority)
	 return ( job1.priority -  job2.priority );
  return ( job1.run_time -  job2.run_time );
}
int pri(const void * a, const void * b)
{
  job_t job1 = *((job_t *)a);
  job_t job2 = *((job_t *)b);
	if (job1.job_id == job2.job_id)
		return 0;
	return ( job1.priority -  job2.priority );
}
int ppri(const void * a, const void * b)
{
	job_t job1 = *((job_t *)a);
  job_t job2 = *((job_t *)b);
	if (job1.job_id == job2.job_id)
		return 0;
	return ( job1.priority -  job2.priority );//modify for run time
}
int rr(const void * a, const void * b)
{
	job_t job1 = *((job_t *)a);
  job_t job2 = *((job_t *)b);
	if (job1.job_id == job2.job_id)
		return 0;
	if (job1.rrindex == job2.rrindex)
		return ( job1.rrindex - job2.rrindex );
	return ( *(int*)a - *(int*)b ); //modify for run time
}
 priqueue_t make_prique(scheme_t type)
 {
	 priqueue_t newqueue;
	 switch (type) {
		case SJF:
		priqueue_init(&newqueue, sjf);
		case PSJF:
		priqueue_init(&newqueue, psjf);
		case PRI:
		priqueue_init(&newqueue, pri);
		case PPRI:
		priqueue_init(&newqueue, pri);
		case RR:
		priqueue_init(&newqueue, rr);
		default: //FCFS
		priqueue_init(&newqueue, fcfs);

	 }
	 return newqueue;

 }

/**
  Initalizes the scheduler.

  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/
void scheduler_start_up(int cores, scheme_t scheme)
{
  priqueue_t queues[cores];
	core_num = cores -1 ;
  for (int i = 0; i < cores; i++) {
    queues[i]=make_prique(scheme);
  }
	//set equal to glabal queue

}
	// helper funiction to make a job
job_t* init_job(int job_number, int time, int running_time, int priority)
{
	job_t newjob*= malloc(sizeof(job_t));
	newjob->job_id=job_number;
	newjob->sent_time=time;
	newjob->run_time=running_time;
	newjob->priority= priority;
	newjob->wait_time= 0;
	newjob -> end_time=-1;
	newjob -> rrindex =0;
	return newjob


}
int findcore_id(){
 int core_id, smallest_core= 0;
 int runnext =-1;
 while(core_id<core_num) //TODO: firgure out what to do with core_num and queues
 {
	 if(priqueue_size(&queues[core_id])== 0)
	 {
		 smallest_core = core_id;
		 break;
	 }
	 if(priqueue_size(&queues[core_id]) < priqueue_size[smallest_core])
		 smallest_core= core_id;
	 core_id++;
 }
 return smallest_core;
}
/**
  Called when a new job arrives.

  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumptions:
    - You may assume that every job wil have a unique arrival time.

  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made. TODO firgure out this

 */



 int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
	job_t newjob*= init_job(job_number,time,running_time,priority);
	core_id=findcore_id(); // based on size
	priqueue_offer( queues[core_id], newjob);

	// offer to correct queue
	return core_id;
}


/**
  Called when a job has completed execution.

  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.

  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
	job_t *test= priqueue_peek(queues[core_id]);
	int size = priqueue_size[queues[core_id]];
	int i= 0;
	while (i<size){
			test = priqueue_at(queues[core_id], i);
			if(test->job_id == job_number)
			{
				priqueue_remove_at(queues[core_id], i)
				free(test);
				break;
			}
		i++;
	}
	test= priqueue_peek(queues[core_id]);

	//peek at top get job_id
	// return job_id of front
	returntest->job_id ;
}


/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.

  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.

  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
	//job = pop
	//job round robin index +1
	//might want t
	//offer job back to the queue
	return -1;

}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete
		(all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
	return 0.0;
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
	return 0.0;
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	return 0.0;
}


/**
  Free any memory associated with your scheduler.

  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{

}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:

    2(-1) 4(0) 1(-1)

  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{

}
