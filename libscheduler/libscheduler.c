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
  int origin_time;
  int priority;

  int has_run;

} job_t;
struct scheduler {
  priqueue_t queue;
  scheme_t Type;
  int core_num;
  int job_nums;
  int total_wait;
  int total_responses;
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
    if(job1.priority != job2.priority)
  	 return ( job1.priority -  job2.priority );

    return ( job1.sent_time -  job2.sent_time );
}
int psjf(const void * a, const void * b)
{
  job_t job1 = *((job_t *)a);
  job_t job2 = *((job_t *)b);
	if (job1.job_id == job2.job_id)
		return 0;
  if(job1.run_time != job2.run_time)
    return ( job1.run_time -  job2.run_time );

  return ( job1.origin_time -  job2.origin_time );
}

int ppri(const void * a, const void * b)
{
	job_t job1 = *((job_t *)a);
  job_t job2 = *((job_t *)b);
	if (job1.job_id == job2.job_id)
		return 0;
  if(job1.priority != job2.priority)
    return ( job1.priority -  job2.priority );
  // if(job1.sent_time !=  job2.sent_time)
  // {
  //   return ( job1.sent_time -  job2.sent_time );
  // }
  return ( job1.origin_time -  job2.origin_time );
}
int rr(const void * a, const void * b)
{
	if(b == NULL)
    return 0;
  return 1;

}
 priqueue_t make_prique(scheme_t type)
 {
	 priqueue_t newqueue;
	 switch (type) {
		case SJF:
		priqueue_init(&newqueue, sjf);
    break;
		case PSJF:

		priqueue_init(&newqueue, psjf);
    break;
		case PRI:
		priqueue_init(&newqueue, pri);
    break;
		case PPRI:

		priqueue_init(&newqueue, ppri);
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

  s->running_jobs = malloc(cores *sizeof(job_t));
	s->core_num = cores;
  s->queue=make_prique(scheme);
  for(int i =0; i<cores; i++)
    {
      s->running_jobs[i]= NULL;
    }

  s->job_nums=0;
  s->total_wait=0;
  s->total_responses=0;
  s->total_turn_around=0;
  s->Type= scheme;

  return;

}
void scheduler_start_up(int cores, scheme_t scheme)
{

  init_scheduler(&s, cores, scheme);

	//set equal to glabal queue

}
	// helper funiction to make a job
job_t* init_job(int job_number, int time, int running_time, int priority,int wait_time,  int has_run)
{
  job_t *newjob= malloc(sizeof(job_t));
	newjob->job_id=job_number;
	newjob->sent_time=time;
  newjob->origin_time=time;
	newjob->run_time=running_time;
	newjob->priority= priority;
	newjob->wait_time= wait_time;
	newjob -> start_time=0;
  newjob -> has_run = has_run;
	return newjob;


}


int findlowest_core( int time)
{
  job_t* temp= s.running_jobs[0];
  if (temp==NULL)
    return 0;
   int longest_rt= temp->run_time -(time - temp->start_time);
   int rt_core= 0;
   int rt_temp= longest_rt;
   int highest_pri = temp->priority;
   int pri_core = 0;
   int pri_start_time = temp->origin_time;
   int job_start_time = temp->origin_time;

  for (int i = 1; i < s.core_num; i++) {
    temp= s.running_jobs[i];
    if(s.running_jobs[i] == NULL)
      return i;
    rt_temp =temp->run_time -(time - temp->start_time);
    if(rt_temp>=longest_rt)
    {
      rt_core = i;
      longest_rt = rt_temp;
      job_start_time = temp->origin_time;

    }
    if(rt_temp == longest_rt && temp-> origin_time > job_start_time){
      rt_core = i;
      longest_rt = rt_temp;
      job_start_time = temp->origin_time;
    }
    if(temp->priority > highest_pri)
    {
      highest_pri =temp->priority;
      pri_core=i;
      pri_start_time= temp->origin_time;
    }
    if(temp->priority == highest_pri && temp-> origin_time > pri_start_time){
      highest_pri =temp->priority;
      pri_core=i;
      pri_start_time= temp->origin_time;
    }

  }
  if(s.Type == PPRI)
    return pri_core;
  return rt_core;
}
int find_empty_core(){

  for(int m=0; m<s.core_num; m++)
  {
    if(s.running_jobs[m]==NULL)
      return m;
  }
  return -1;
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
int find_index(int job_number)
{
  job_t *test;
  int size = priqueue_size(&s.queue);
  int i= 0;
	while (i<size)
  {
    test = priqueue_at(&s.queue, i);
    if(test->job_id == job_number)
    {
      return i;
    }
    i++;
  }
  return -1;
}
int scheduler_job(int job_number, int time, int running_time, int priority, int wait_time,  int has_run)
{

	job_t *newjob= init_job(job_number,time,running_time,priority, wait_time,  has_run);

  int core_id=find_empty_core();


  if(core_id != -1)
  {
    newjob-> start_time = time;
    s.running_jobs[core_id]= newjob;
    priqueue_offer( &s.queue, newjob);
    return core_id;
  }



  priqueue_offer( &s.queue, newjob);

  if((s.Type == PPRI||s.Type == PSJF)  )
  {
    core_id = findlowest_core(time);

    //TODO: make its own funiction
   job_t *test = s.running_jobs[core_id];
   if(!((s.Type == PSJF && newjob->run_time < (test->run_time - time + test->start_time))
   ||(s.Type == PPRI && newjob->priority < test->priority))){
     return -1;
   }

    test->run_time =test->run_time - time + test->start_time;
    newjob-> start_time = time;
    test->wait_time=test->start_time - test->sent_time;

    if(test->has_run == 0 ){
      s.total_responses= s.total_responses + test->wait_time;
    }
    s.total_wait= s.total_wait + test->wait_time;
    test->wait_time=0;
    s.total_turn_around= s.total_turn_around + (time - test->sent_time);
    if(test->start_time != time)
    {

      test->has_run = 1;

    }
    test->sent_time =time;
    newjob->wait_time= 0;
    s.running_jobs[core_id]= newjob;
  int help=  find_index(test->job_id); // remove it for PSJF, and RR
    priqueue_remove_at(&s.queue, help);
    priqueue_offer(&s.queue, test);
  }
   return core_id;

  // offer to correct queue



}
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
  //TODO::set up for multi cores


  s.job_nums++;
  return scheduler_job(job_number,time,running_time,priority, 0, 0);
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
 job_t* next_job()
 {
    job_t* test= NULL;
    int is_not_running = 1;
   for(int nex_job = 0; nex_job< priqueue_size(&s.queue); nex_job++)
   {
     is_not_running =1;
     test = priqueue_at(&s.queue,nex_job );
     if(test== NULL) {
     break; }

     for(int i=0 ; i< s.core_num ; i++)
     {
        if(s.running_jobs[i]==NULL)
        {

        }
        else if(s.running_jobs[i]->job_id == test->job_id)
       {
         is_not_running =0;
         break;
       }

     }
     if (is_not_running)
      return test;


   }
   return NULL;
 }
int scheduler_job_finished(int core_id, int job_number, int time)
{
	job_t *test= NULL;

	int size = priqueue_size(&s.queue);
	int i= find_index(job_number);
  if( i == -1)
    {return i; }
			test = priqueue_at(&s.queue, i);


          if((test->start_time + test-> run_time ) <=time)
          {
    				test = priqueue_remove_at(&s.queue, i);
            if(test->has_run == 0){
              //printf("Job_id:%d wait_time; %d", test->job_id, test->wait_time );
              s.total_responses= s.total_responses + test->wait_time;
            }
            s.total_wait= s.total_wait + test->wait_time;
            s.total_turn_around= s.total_turn_around + (time - test->sent_time);
            free(test);

          }else
          {
            return -1;
          }

  s.running_jobs[core_id] = NULL;
  test =next_job();
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
  //get the first element from the queue
  job_t *j= s.running_jobs[core_id];
  if(priqueue_size(&s.queue) <= s.core_num){
    return j->job_id;
  }
  int index= find_index(j->job_id);


    j->wait_time=  j->start_time - j->sent_time;
    s.total_wait= s.total_wait +j->wait_time;
    if((j-> has_run)==0){
      s.total_responses += j->wait_time;
    }
    j->wait_time =0;// j->start_time = time;
  j=priqueue_remove_at(&s.queue , index);

  s.total_turn_around = s.total_turn_around + (time - j->sent_time);



  j->has_run=1;
  j->sent_time = time;
  j->start_time= 0;
  j->wait_time=0;
  j->run_time= j->run_time - (time - j->start_time);
	priqueue_offer(&s.queue, j);

  //look at the next element
  job_t *test = next_job();
  test->start_time =time;
  test->wait_time=time - test->sent_time;
  s.running_jobs[core_id]= test;
  return (test->job_id);
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
  //printf("job_nums%d\n",s.job_nums );

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
  //printf("job_nums%d\n",s.job_nums );
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
  //printf("job_nums%d\n",s.job_nums );
  return (float)s.total_responses/ (float)s.job_nums;

}


/**
  Free any memory associated with your scheduler.

  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{

      priqueue_destroy(&s.queue);

    // free(s.running_jobs[i]);

     free(s.running_jobs);



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

    for(int i=0; i<priqueue_size(&s.queue); i++){
      job_t test = *(job_t *)priqueue_at(&s.queue,i);
      printf("%d ", test.job_id);
    }
  }
