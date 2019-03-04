//Elif Yılmaz 230201054

/* File:
 *     queueSequential.c
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

/* Shared variables */
int thread_count;



/* Head of linked list */
struct list_node_s* head = NULL;

/* Head of task queue */
struct task_node_s* tasks_head = NULL;

/* Tail of task queue */
struct task_node_s* tasks_tail = NULL;

/* Usage */
void Usage(char* prog_name);

/* Thread function */
void Thread_work();

/* Task queue functions */
int Empty_queue(void);
int Terminate(int* which_task_p, int* option_p, int* data_p);
void Task_queue(int n);
void Task_enqueue(int which_task, int option, int data);
int Task_dequeue(int* which_task_p, int* option_p, int* data_p);

/* List operations */
int Insert(int value);
void Print(char title[]);
int Search(int value);
int Delete(int value);
void Free_list(void);
int Is_empty(void);

/*-----------------------------------------------------------------*/
int main(int argc, char* argv[]) {

   int n;

   if(argc != 2) Usage(argv[0]);
   n = strtol(argv[1], NULL, 10);
   /* Generate tasks */
   Task_queue(n);

   struct timespec requestStart, requestEnd;
  //Start timer

  clock_t begin = clock();
  clock_gettime(CLOCK_REALTIME, &requestStart);

   Thread_work();
   Print("main:  Final list");
   //End timer
   clock_gettime(CLOCK_REALTIME, &requestEnd);

   // Calculate time it took
   double time_spent = ( requestEnd.tv_sec - requestStart.tv_sec ) + ( requestEnd.tv_nsec - requestStart.tv_nsec ) / BILLION;
   printf("Total time spent: %lf\n", time_spent);
   Free_list();


   return 0;
}  /* main */


void Task_queue(int n) {
   int i;
   for(i = 0; i < n; i++) {
      Task_enqueue(i, random() % task_count, random() % max_val);

   }

}  /* Task_queue */

int Empty_queue(void) {
   if (tasks_head == NULL)
      return 1;
   else
      return 0;
}  /* Empty_queue */



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

//#  ifdef DEBUG
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

int Task_dequeue(int* which_task_p, int* option_p,
      int* data_p){
   struct task_node_s* tmp_tasks_head = tasks_head;

   if (tmp_tasks_head == NULL) {
     #     ifdef DEBUG
      printf("Queue empty\n");
    #     endif
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

void Thread_work() {

   char title[max_title];

   int option = 0, data = 0, which_task;

   while(Terminate( &which_task, &option, &data)) // terminate = 0
   {

      switch (option) {
         case 0:
            if (Insert(data))
               printf("task %d: %d is inserted\n",which_task,data);
            else
               printf("task %d: %d cannot be inserted\n",which_task,data);
            break;
         case 1:
            if (Delete(data))
               printf("task %d:  %d is deleted\n",which_task,data);
            else
               printf("task %d:  %d cannot be deleted\n",which_task,data);
            break;
         case 2:
            if (Search(data))
               printf("task %d:  %d is in the list\n", which_task,data);
            else
               printf("task %d:  %d is not in the list\n",which_task,data);
            break;
         default:
            sprintf(title, "task %d:  print list",which_task);
            Print(title);
            break;
      }

   }

}


int Terminate(int* which_task_p, int* option_p, int* data_p) {

  int dequeue_task = Task_dequeue(which_task_p,option_p,data_p);
  return dequeue_task;

}  /* Terminate */


/*--------------------------------------------------------------------
 * Function:    Usage
 * Purpose:     Print command line for function and terminate
 * In arg:      prog_name
 */
void Usage(char* prog_name) {

   fprintf(stderr, "usage: %s <number of tasks>\n",
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
