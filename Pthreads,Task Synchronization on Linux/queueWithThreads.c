//Elif Yılmaz 230201054

/* File:
 *     queueWithThreads.c
 *
 * Purpose:
 *     Implement a task queue using pthreads.
 *     The main thread begins by starting a user-specified
 *     number of threads that immediately go to sleep in a
 *     condition wait. The main thread generates new tasks
 *     to be carried out by other threads. When main thread
 *     completes generating tasks, it sets a global variable
 *     to indicate no more tasks and awakens all threads with
 *     a condition broadcast.
 *
 * Input:
 *     number of threads
 *     number of tasks to be generated
 *
 * Notes:
 *     -  tasks are Linked list operations
 *     -  task options:
 *        0: insert
 *        1: delete
 *        2: check if the data is in the list
 *        default: print the Linkedlist
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define BILLION 1E9

const int max_title = 1000;
const int task_count = 3;
const int max_val = 50;

/* Struct for list nodes */
struct list_node_s {
   int    data;
   struct list_node_s* next;
};

/* Struct for task nodes */
struct task_node_s {
   int which_task;
   int option;
   int data;
   struct task_node_s* next;
};

/* Shared variables , I also explained them in report. */ 
int thread_count;
pthread_cond_t cond_queue; // to use cond wait sleeps for threads
pthread_mutex_t mutex,queue_mutex; //for dequeu and enqueue,// to use cond wait sleeps for threads
int n; //number of tasks
int flag; //to use for broadcast signal
int isOver; // executing of task is over or not.
pthread_mutex_t list_mutex; //to print final list
pthread_cond_t cond_list;//print final list
pthread_mutex_t work_mutex; //task executing,critical section for worker threads
/* Head of linked list */
struct list_node_s* head = NULL;

/* Head of task queue */
struct task_node_s* tasks_head = NULL;

/* Tail of task queue */
struct task_node_s* tasks_tail = NULL;

/* Usage */
void Usage(char* prog_name);

/* Thread function */
void* Thread_work(void* rank);

/* Task queue functions */
int Empty_queue(void);
int Terminate(long my_rank, int* which_task_p, int* option_p, int* data_p);
void Task_queue(int n);
void Task_enqueue(int which_task, int option, int data);
int Task_dequeue(long my_rank, int* which_task_p, int* option_p, int* data_p);

/* List operations */
int Insert(int value);
void Print(char title[]);
int Search(int value);
int Delete(int value);
void Free_list(void);
int Is_empty(void);

/*-----------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   long thread;
   pthread_t* thread_handles;


   if(argc != 3) Usage(argv[0]);

   thread_count = strtol(argv[1], NULL, 10); //get number of threads from command line.
   n = strtol(argv[2], NULL, 10); //number of tasks

   printf("Hello from main thread.\n" );
   /* Allocate array for threads */

   thread_handles = malloc(thread_count*sizeof(pthread_t));

   /* Initialize mutexes and condition variables */

   pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //enqueue and dequeue task not at the same time
   pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;// use to sleep threads in cond wai
   pthread_cond_t cond_queue = PTHREAD_COND_INITIALIZER; // cond variable for threads' sleep condition
   pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;//to print final list
   pthread_cond_t cond_list = PTHREAD_COND_INITIALIZER; //to print final list
   pthread_mutex_t work_mutex = PTHREAD_MUTEX_INITIALIZER;//for threads' critical section
   flag = 0 ; //to use broadcast signal
   isOver = 0 ; //dequeue all task or not

   printf("Main thread start thread creation.\n" );

   /* Start threads */
   for(thread = 0; thread < thread_count; thread++){
     pthread_create(&thread_handles[thread],NULL,Thread_work,(void*) thread);

   }


      printf("Main thread start task creation.\n" );
      /* Generate tasks */
      Task_queue(n); //main thread generate ediyor.

     printf("Main thread waits for threads to complete.\n" );
   /* Wait for threads to complete */

     struct timespec requestStart, requestEnd;
   	//Start timer

     	clock_t begin = clock();
     	clock_gettime(CLOCK_REALTIME, &requestStart);


      for(thread = 0; thread < thread_count; thread++){
        pthread_join(thread_handles[thread],NULL);
      }
      //End timer
    	clock_gettime(CLOCK_REALTIME, &requestEnd);

    // Calculate time it took
    double time_spent = ( requestEnd.tv_sec - requestStart.tv_sec ) + ( requestEnd.tv_nsec - requestStart.tv_nsec ) / BILLION;
    printf("Threads are completed.\n" );
    printf("Total time spent: %lf\n", time_spent);
    Free_list();

   return 0;
}  /* main */


/*-------------------------------------------------------------------
 * Function:    Task_queue
 * Purpose:     generate random tasks for the task queue, and
 *              notify a thread to wake up from condition
 *              wait and get a task
 * In arg:      n: number of tasks
 */

void Task_queue(int n) {
   int i;

   for(i = 0; i < n; i++) {

      pthread_mutex_lock(&mutex); //enqueue and dequeue should not be done at the same time.

      Task_enqueue(i, random() % task_count, random() % max_val);

      pthread_mutex_lock(&queue_mutex);
      pthread_cond_signal(&cond_queue);//signal to awake a thread.
      pthread_mutex_unlock(&queue_mutex);

      pthread_mutex_unlock(&mutex);

   }
  pthread_mutex_lock(&queue_mutex);
  pthread_cond_broadcast(&cond_queue);//signal to awake all threads.
  flag = 1;
  pthread_mutex_unlock(&queue_mutex);

   /* Now the queue is empty:  wait for threads to terminate */

   pthread_mutex_lock(&list_mutex);
   pthread_cond_wait(&cond_list,&list_mutex); //waits for threads to finish execute tasks.
   pthread_mutex_unlock(&list_mutex);

   Print("main:  Final list");

}  /* Task_queue */


/*-------------------------------------------------------------------
 * Function:    Empty_queue
 * Purpose:     Determine whether the task queue is empty
 * Return val:  0 task queue not empty
 *              1 otherwise
 */
int Empty_queue(void) {
   if (tasks_head == NULL)
      return 1;
   else
      return 0;
}  /* Empty_queue */


/*-------------------------------------------------------------------
 * Function:    Task_enqueue
 * Purpose:     insert a new task into task queue
 * In arg:      option, data
 * Global var:  tasks_head, tasks_tail
 */
/*NOTHING TO CHANGE HERE*/
void Task_enqueue(int which_task, int option, int data){

   struct task_node_s* tmp_task = NULL;

   tmp_task = malloc(sizeof(struct task_node_s));
   tmp_task->which_task = which_task;
   tmp_task->option = option;
   tmp_task->data = data;
   tmp_task->next = NULL;

   if (tasks_tail == NULL) { //task list is empty
      tasks_head = tmp_task;
      tasks_tail = tmp_task;
   } else {
      tasks_tail->next = tmp_task;
      tasks_tail = tmp_task;
   }

//# ifdef DEBUG
   switch(option) {
      case 0:
         printf("Main:  enqueued task %d: Insert %d\n", which_task, data);
         break;
      case 1:
         printf("Main:  enqueued task %d: Delete %d\n", which_task, data);
         break;
      case 2:
         printf("Main:  enqueued task %d: Search %d\n", which_task, data);
         break;
      default:
         printf("Main:  enqueued task %d: Print list\n", which_task);
   }
//#  endif

}  /* Task_enqueue */


/*-------------------------------------------------------------------
 * Function:    Task_dequeue
 * Purpose:     take a task from task queue
 * In arg:      my_rank
 * Out arg:     which_task_p, option_p, data_p
 * Global var:  tasks_head, tasks_tail
 * Return val:  0 if queue is empty, 1 otherwise
 */
/*NOTHING TO CHANGE HERE*/
int Task_dequeue(long my_rank, int* which_task_p, int* option_p,
      int* data_p){
   struct task_node_s* tmp_tasks_head = tasks_head;

   if (tmp_tasks_head == NULL) {
      #  ifdef DEBUG
        printf("Th %ld > Queue empty\n", my_rank);
      #  endif
      return 0;
   }

   *which_task_p = tmp_tasks_head->which_task;
   *option_p = tmp_tasks_head->option;
   *data_p = tmp_tasks_head->data;

   if (tasks_tail == tasks_head) //last task
      tasks_tail = tasks_tail->next;

   tasks_head = tasks_head->next;
   free(tmp_tasks_head);

   return 1;
}  /* Task_dequeue */

/*-------------------------------------------------------------------
 * Function:    Thread_work
 * Purpose:     When main thread signals a thread
 *              carry out a linked list operation
 * In arg:      rank
 */
void *Thread_work(void* rank) {


   long my_rank = (long) rank;
   char title[max_title];

   pthread_mutex_lock(&queue_mutex);
   pthread_cond_wait(&cond_queue,&queue_mutex);//Sleep immediately
   pthread_mutex_unlock(&queue_mutex);

   int option = 0, data = 0, which_task;

   while(Terminate(my_rank, &which_task, &option, &data)) // terminate = 0
   {
    pthread_mutex_lock(&work_mutex); //critical section for worker threads
      switch (option) {
         case 0:
            if (Insert(data))
               printf("Thread %ld: task %d: %d is inserted\n",
                     my_rank, which_task, data);
            else
               printf("Thread %ld: task %d: %d cannot be inserted\n",
                     my_rank, which_task, data);
            break;
         case 1:
            if (Delete(data))
               printf("Thread %ld: task %d:  %d is deleted\n",
                     my_rank, which_task, data);
            else
               printf("Thread %ld: task %d:  %d cannot be deleted\n",
                     my_rank, which_task, data);
            break;
         case 2:
            if (Search(data))
               printf("Thread %ld: task %d:  %d is in the list\n",
                     my_rank, which_task, data);
            else
               printf("Thread %ld: task %d:  %d is not in the list\n",
                     my_rank, which_task, data);
            break;
         default:
            sprintf(title, "Th %ld:  task %d:  print list",
                  my_rank, which_task);
            Print(title);
            break;
      }
      isOver++ ; // one more task executed.
      pthread_mutex_unlock(&work_mutex); //end of critical section for worker threads
      if(flag==0){ //no broadcast
        pthread_mutex_lock(&queue_mutex);
        pthread_cond_wait(&cond_queue,&queue_mutex);//thread finishes executing its task and return
                                                    //condition wait but if there is a condution
                                                    // broadcast signal, then all threads awakens
                                                    //do not return condition wait,complete Thread_work.
        pthread_mutex_unlock(&queue_mutex);
      }
   }


   if(isOver == n){ // All task executed, n=number of tasks.
     pthread_mutex_lock(&list_mutex);
     pthread_cond_signal(&cond_list);//signal to print final list.
     pthread_mutex_unlock(&list_mutex);
     return NULL;
   }

   return NULL;





}

/*-------------------------------------------------------------------
 * Function:    Terminate
 * Purpose:     Wake up from main thread call and take a task
 *              from task queue
 * In arg:      my_rank
 * Out args:    which_task_p, option_p, data_p
 */

int Terminate(long my_rank, int* which_task_p, int* option_p, int* data_p) { //oooo patates

      pthread_mutex_lock(&mutex);//enqueue and dequeue should not be done at the same time.
      int dequeue_task = Task_dequeue(my_rank, which_task_p,option_p,data_p);
      pthread_mutex_unlock(&mutex);

  return dequeue_task;
}  /* Terminate */


/*--------------------------------------------------------------------
 * Function:    Usage
 * Purpose:     Print command line for function and terminate
 * In arg:      prog_name
 */
void Usage(char* prog_name) {

   fprintf(stderr, "usage: %s <number of threads> <number of tasks>\n",
         prog_name);
   exit(0);
}  /* Usage */


/*-----------------------------------------------------------------*/
/* Insert value in correct numerical location into list */
/* If value is not in list, return 1, else return 0 */
int Insert(int value) {
   struct list_node_s* curr = head;
   struct list_node_s* pred = NULL;
   struct list_node_s* temp;
   int rv = 1;

   while (curr != NULL && curr->data < value) {
      pred = curr;
      curr = curr->next;
   }

   if (curr == NULL || curr->data > value) {
      temp = malloc(sizeof(struct list_node_s));
      temp->data = value;
      temp->next = curr;
      if (pred == NULL)
         head = temp;
      else
         pred->next = temp;
   } else { /* value in list */
      rv = 0;
   }

   return rv;
}  /* Insert */

/*-----------------------------------------------------------------*/
void Print(char title[]) {
   struct list_node_s* temp;

   printf("%s:\n   ", title);

   temp = head;
   while (temp != (struct list_node_s*) NULL) {
      printf("%d ", temp->data);
      temp = temp->next;
   }
   printf("\n");
}  /* Print */


/*-----------------------------------------------------------------*/
int  Search(int value) {
   struct list_node_s* temp;

   temp = head;
   while (temp != NULL && temp->data < value)
      temp = temp->next;

   if (temp == NULL || temp->data > value) {
#     ifdef DEBUG
      printf("%d is not in the list\n", value);
#     endif
      return 0;
   } else {
#     ifdef DEBUG
      printf("%d is in the list\n", value);
#     endif
      return 1;
   }
}  /* Search */

/*-----------------------------------------------------------------*/
/* Deletes value from list */
/* If value is in list, return 1, else return 0 */
int Delete(int value) {
   struct list_node_s* curr = head;
   struct list_node_s* pred = NULL;
   int rv = 1;

   /* Find value */
   while (curr != NULL && curr->data < value) {
      pred = curr;
      curr = curr->next;
   }

   if (curr != NULL && curr->data == value) {
      if (pred == NULL) { /* first element in list */
         head = curr->next;
#        ifdef DEBUG
         printf("Freeing %d\n", value);
#        endif
         free(curr);
      } else {
         pred->next = curr->next;
#        ifdef DEBUG
         printf("Freeing %d\n", value);
#        endif
         free(curr);
      }
   } else { /* Not in list */
      rv = 0;
   }

   return rv;
}  /* Delete */

/*-----------------------------------------------------------------*/
void Free_list(void) {
   struct list_node_s* current;
   struct list_node_s* following;

   if (Is_empty()) return;
   current = head;
   following = current->next;
   while (following != NULL) {
#     ifdef DEBUG
      printf("Freeing %d\n", current->data);
#     endif
      free(current);
      current = following;
      following = current->next;
   }
#  ifdef DEBUG
   printf("Freeing %d\n", current->data);
#  endif
   free(current);
}  /* Free_list */

/*-----------------------------------------------------------------*/
int  Is_empty(void) {
   if (head == NULL)
      return 1;
   else
      return 0;
}  /* Is_empty */
