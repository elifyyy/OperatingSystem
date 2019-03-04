#include<stdio.h>
#include<stdlib.h>
#include<time.h>

struct node
{
  	  int data;
    	struct node *next;
};
struct node *head=NULL;

struct task {
int task_num; //starting from 0
int task_type; // insert:0, delete:1, search:2
int value;
struct task* next;
};
typedef struct task task;

struct task* front = NULL;
struct task* rear = NULL;


void Insert(int data)
{
    	struct node *temp=head;
    	struct node *prev=NULL;
    	struct node *ptr;

    	ptr=(struct node*)malloc(sizeof(struct node));
    	ptr->data=data;
    	ptr->next=NULL;

    	if(temp==NULL)
    	{
        		//Executes when linked list is empty
        		ptr->next=NULL;
        		head=(struct node*)malloc(sizeof(struct node));
            head = ptr;
            printf("%d inserted in the list",data);
        		return;
    	}

    	if(data<temp->data)
    	{
        		//Executes if given data is less than data in first node of linked list
        		ptr->next=head;
        		head=ptr;
            printf("%d inserted in the list",data);
        		return;
    	}
    	else if(data>temp->data)
    	{
        		while(temp!=NULL)
        		{
            			if(data>temp->data )
            			{
                				//Traverse to location we want to insert the node + 1 node
                				prev=temp;
                				temp=temp->next;
                				continue;
            			}
            			else
            			{
                	      //Insert the node
                				prev->next=ptr;
                				ptr->next=temp;
                        printf("%d inserted in the list ",data);
                				return;
            			}
        		}
        		prev->next=ptr;
            printf("%d inserted in the list ",data);
        		//Insert node at last
    	}

      else{ //same value,no duplicates allowed.
        printf("%d not inserted ",data);
      }
}


void Search(int data){
  struct node *temp=head;
  struct node *prev=NULL;
  int is_found = 0 ;
  while(temp!=NULL)
  {
    if(temp->data == data){
      printf("%d found ",data);
      is_found = 1;
    }

    temp = temp->next;
  }
  if(is_found == 0){
    printf("%d not found ",data);
  }

}


void Delete(int data){

  if(head ==NULL){ //empty list
    printf("%d not deleted ",data);
   }

  else{

  struct node *current=head->next;
  struct node *prev=head;
  int is_found=0 ; // value is in the list or not.
  if(head->data == data){
    head=head->next;
    printf("%d deleted from list",data);
    is_found = 1;
  }

  while(current!=NULL)
  {
    if(current->data == data){
      prev->next = prev->next->next;
      printf("%d deleted from list",data);
      is_found = 1;
    }

    current = current->next;
    prev = prev->next;
  }
  free(current);
  if(is_found == 0){
    printf("%d not deleted ",data);
  }
}
}

void print() //print list that contains values of tasks.
{
    	struct node *temp=head;
    	printf("\nList:");
    	while(temp!=NULL)
    	{
        		printf("%d ",temp->data);
        		temp=temp->next;
    	}
}

void Enqueue(task *task) {


	if(front == NULL && rear == NULL){
		front = rear = task;
		return;
	}
	rear->next = task;
	rear = task;
}

void Dequeue() {
	struct task* temp = front;
	if(front == NULL) { //Queue is empty
		printf("Queue is Empty\n");
		return;
	}
	if(front == rear) { //Queue have one element
		front = rear = NULL;
	}
	else {
		front = front->next;
	}
  //check task types
  if(temp->task_type==0){ //insert task
    printf(" \ntask %d-insert ", temp->task_num);
    printf("%d : ",temp->value );
    Insert(temp->value);

  }

  else if(temp->task_type==1){ //delete task
    printf(" \ntask %d-delete ", temp->task_num);
    printf("%d : ",temp->value );
    Delete(temp->value);
  }


  else if(temp->task_type==2){ //search task
    printf(" \ntask %d-search ", temp->task_num);
    printf("%d : ",temp->value );
    Search(temp->value);
  }


	free(temp);
}

int main(int argc, char* argv[])
{

  int numberOfTasks = atoi(argv[1]); //number of task that will be generated, taken from console.
  int i;

  srand(time(NULL));
  for(i=0; i<numberOfTasks; i++){ //Generate random tasks
    task *task = malloc(sizeof(task));
    task->task_num = i;
    task->task_type = rand() % 3;
    task->value = rand() % 100 + 1;
    task->next=NULL;
    Enqueue(task);

}
printf("Generated %d random list tasks..", numberOfTasks);

for(i=0; i<numberOfTasks; i++){ //Dequeue all tasks inside the task queue and do insert,delete or search actions.
  Dequeue();
}

print();
return 0;
}
