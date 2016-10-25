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
  int start_time ;
  int wait_time ;
  int run_time;
  int priority;
	int rrindex ;

} job_t;
struct scheduler {
  priqueue_t *queues;
  scheme_t Type;
  int core_num;
  int job_nums;
  int total_wait;
  int total_respones;
  int total_turn_around;
  job_t **running_jobs;  // pointers to job inside q that are running

};
struct scheduler s;
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
int pri(const void * a, const void * b)
{
  job_t job1 = *((job_t *)a);
  job_t job2 = *((job_t *)b);
	if (job1.job_id == job2.job_id)
		return 0;
	return ( job1.priority -  job2.priority );
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
      return 0;
	return ( job1.rrindex - job2.rrindex );
	 //modify for run time
}
 priqueue_t make_prique(scheme_t type)
 {
	 priqueue_t newqueue;
	 switch (type) {
		case SJF:
		priqueue_init(&newqueue, sjf);
    break;
		case PSJF:

		priqueue_init(&newqueue, sjf);
    break;
		case PRI:
		priqueue_init(&newqueue, pri);
    break;
		case PPRI:

		priqueue_init(&newqueue, pri);
    break;
		case RR:
		priqueue_init(&newqueue, rr);
    break;
		default: //FCFS
		priqueue_init(&newqueue, fcfs);
    break;

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
void init_scheduler(struct scheduler *s, int cores, scheme_t scheme )
{
  s->queues =  malloc(cores * sizeof(priqueue_t));
  s->running_jobs = malloc(cores *sizeof(job_t));
	s->core_num = cores;
  for (int i = 0; i < cores; i++) {
    s->queues[i]=make_prique(scheme);
  }
  s->job_nums=0;
  s->total_wait=0;
  s->total_respones=0;
  s->total_turn_around=0;
  s->Type= scheme;

  return;

}
void scheduler_start_up(int cores, scheme_t scheme)
{
  //priqueue_t queues[cores];

  init_scheduler(&s, cores, scheme);

	//set equal to glabal queue

}
	// helper funiction to make a job
job_t* init_job(int job_number, int time, int running_time, int priority,int wait_time, int rrindex)
{
	job_t *newjob= malloc(sizeof(job_t));
	newjob->job_id=job_number;
	newjob->sent_time=time;
	newjob->run_time=running_time;
	newjob->priority= priority;
	newjob->wait_time= wait_time;
	newjob -> start_time=0;
	newjob -> rrindex =rrindex;
	return newjob;


}

int get_current_wait_timeto(priqueue_t* q, int end_index, job_t* running_job, int time)
{

  int time_left=running_job->run_time - (time - running_job->start_time);
  job_t temp;
  int i=0;
  while(i<end_index)
  {
    if(priqueue_at(q, i)!= NULL)
    {
      temp=*(job_t *)priqueue_at(q, i);
      time_left= time_left + temp.run_time;
    }
    i++;
  }
  return time_left;


}
int findcore_id(int time){
 int core_id = 1;
 int smallest_core= 0;
 int runnext =-1;
 int current_wait_time=0;
 if(priqueue_size(&s.queues[0]) == 0)
  return 0;
 int smallest_wait_time = get_current_wait_timeto(&s.queues[0], priqueue_size(&s.queues[0]),s.running_jobs[0], time);
 while(core_id<s.core_num)
 {
	 if(priqueue_size(&s.queues[core_id])== 0)
	 {
		 smallest_core = core_id;
		 break;
	 }
   current_wait_time = get_current_wait_timeto(&s.queues[core_id], priqueue_size(&s.queues[core_id]),s.running_jobs[core_id], time);
	 if(current_wait_time < smallest_wait_time)
   {
		 smallest_core= core_id;
     smallest_wait_time=current_wait_time;
   }
	 core_id++;
 }
 return smallest_core;
}


int findcore_id_prioty(int time, int priority){
 int core_id = 0;
 int smallest_core= -1;
 job_t temp_job;
 int smallest_index=priqueue_size(&s.queues[0]);
 for(int j =0; j<priqueue_size(&s.queues[0]); j++)
 {
  temp_job= *(job_t *)priqueue_at(&s.queues[0],j);
   if(priority< temp_job.priority)
   {
     smallest_core= 0;
     smallest_index = j;
     break;
   }
 }
 int runnext =-1;
 for (int i = 1; i < s.core_num; i++)
{
   for(int j =0; j<priqueue_size(&s.queues[i]); j++)
   {
     temp_job= *(job_t *)priqueue_at(&s.queues[0],j);
     if(priority< temp_job.priority)
     {
       if(j< smallest_index)
       {
         smallest_index= j;
         smallest_core = i;
       }
       break;
     }

   }
   if(smallest_index == 0)
    break;
 }
if(smallest_core < 0)
  smallest_core= findcore_id(time);
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
  @param runninP_IP, UDP_PORT))
  */
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
	job_t *newjob= init_job(job_number,time,running_time,priority, 0, 0);
  s.job_nums++;
  int core_id;
	if (!(s.Type == PPRI)){
     core_id=findcore_id(time);
  }else
  {
     core_id=findcore_id_prioty(time, priority);
  }// based on size
	int test_size = priqueue_size(&s.queues[core_id]); // in theory should running job
  // TODO:  make this work for ppri a running variable
  if(test_size == 0)
  {
    newjob-> start_time = time;
    priqueue_offer( &s.queues[core_id], newjob);
    //TODO this only works for FCFS
    s.running_jobs[core_id]= newjob;
    return core_id;
  }
  job_t temp_job =*(job_t *)priqueue_peek(&s.queues[core_id]);
  int test_time= temp_job.run_time - (time -temp_job.start_time);
  if((s.Type == PPRI && newjob->priority <temp_job.priority) ||(s.Type == PSJF && newjob->run_time <test_time) ){
   job_t *temp = priqueue_poll(&s.queues[core_id]);
    temp->run_time = test_time;
    newjob-> start_time = time;
    priqueue_offer( &s.queues[core_id], newjob);
    scheduler_new_job(temp->job_id, time,temp->run_time, temp->priority);
    free(temp);
    s.running_jobs[core_id]= newjob;
   return core_id;
 }
  priqueue_offer( &s.queues[core_id], newjob);
  // offer to correct queue


  return -1;
}


/*
  Called when a job has completed execution.

  The core_id, job_number and time parameters are provided for convenience.
  You may be able to calculate the values with your own data structure.
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
	job_t *test= priqueue_peek(&s.queues[core_id]);
	int size = priqueue_size(&s.queues[core_id]);
	int i= 0;
	while (i<size){
			test = priqueue_at(&s.queues[core_id], i);
			if(test->job_id == job_number)
			{
          if((test->sent_time + test-> run_time + test-> wait_time) <=time)
          {
    				priqueue_remove_at(&s.queues[core_id], i);
            if(test->rrindex == 0)
              s.total_respones= s.total_respones + test->wait_time;
            s.total_wait= s.total_wait + test->wait_time;
            s.total_turn_around= s.total_turn_around + (time - test->sent_time);
            free(test);
    				break;
          }
          return -1;
			}
		i++;
	}

  test= priqueue_peek(&s.queues[core_id]);
  s.running_jobs[core_id]= test;
	//peek at top get job_id
	// return job_id of front
  if (test == NULL)
    return -1;


  test->wait_time= test->wait_time +time - test->sent_time;
  test->start_time=time;
	return test->job_id;
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
	return (float)s.total_wait/ (float)s.job_nums;
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
  return (float)s.total_turn_around/ (float)s.job_nums;
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
  return (float)s.total_respones/ (float)s.job_nums;

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
  In our provided output, we have implemented this function to list the jobs in the order
   they are to be scheduled. Furthermore, we have also listed the current state
   of the job
   (either running on a given core or idle).
   For example, if we have a non-preemptive algorithm and job(id=4) has began running,
  job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority,
  the output in our sample output will be:

    2(-1) 4(0) 1(-1)

  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{

}
