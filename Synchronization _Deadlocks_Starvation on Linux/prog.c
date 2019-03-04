#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER; //for file numbers1.txt
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;// for file number2.txt
pthread_mutex_t lock3 = PTHREAD_MUTEX_INITIALIZER; // for file resource2.txt


typedef struct
{
	int threadNo;
	char* thread_type;
}THREAD_PARAMETERS;

void *ThreadA(void *param);
void *ThreadB(void *param);
void *ThreadC(void *param);

bool is_prime(int number)
{
   if (number > 2){
   int i;
   for (i=2; i<number; i++ ){
       if ((number % i) == 0){
         return false;
       }
       else{
       return true;
     }
     }
  }

	else{
		return false;
	}
}


int main(int argc, char* argv[])
{

	int threadCountA = 4;
	int threadCountB = 1;
	int threadCountC = 1;
	pthread_t tidA[threadCountA];
	pthread_t tidB[threadCountB];
	pthread_t tidC[threadCountC];

	pthread_attr_t attrA[threadCountA];
	pthread_attr_t attrB[threadCountB];
	pthread_attr_t attrC[threadCountC];

	THREAD_PARAMETERS* lpParameterA;
	THREAD_PARAMETERS* lpParameterB;
	THREAD_PARAMETERS* lpParameterC;



	lpParameterA = malloc(sizeof(THREAD_PARAMETERS)* threadCountA);
	lpParameterB = malloc(sizeof(THREAD_PARAMETERS)* threadCountB);
	lpParameterC = malloc(sizeof(THREAD_PARAMETERS)* threadCountC);



 	int  i=0;

	for(i=0; i<threadCountA; i++)
	{
		lpParameterA[i].thread_type = "A";
		lpParameterA[i].threadNo = i + 1;

		pthread_attr_init(&attrA[i]);
		pthread_create(&tidA[i],&attrA[i],ThreadA,&lpParameterA[i]);
	}

	for(i=0; i<threadCountA; i++)
	{
		pthread_join(tidA[i],NULL);
	}
	for(i=0; i<threadCountB; i++)
	{
		lpParameterB[i].thread_type = "B";
		lpParameterB[i].threadNo = i + 1;

		pthread_attr_init(&attrB[i]);
		pthread_create(&tidB[i],&attrB[i],ThreadB,&lpParameterB[i]);
	}

	for(i=0; i<threadCountB; i++)
	{
		pthread_join(tidB[i],NULL);
	}


	for(i=0; i<threadCountC; i++)
	{
		lpParameterC[i].thread_type = "C";
		lpParameterC[i].threadNo = i + 1;

		pthread_attr_init(&attrC[i]);
		pthread_create(&tidC[i],&attrC[i],ThreadC,&lpParameterC[i]);
	}

	for(i=0; i<threadCountC; i++)
	{
		pthread_join(tidC[i],NULL);
	}



	return 1;
}

void *ThreadA(void *parameters)
{
		int fileLen1= 1; //size of file1 (a non zero value initally)
		int fileLen2 = 1; // size of file1 (a non zero value initally)
		int size1; // size of array1
		int size2; // size of array2
		int a; // random number
		FILE* file1; //numbers1.txt R1
		FILE* file2; //numbers2.txt R1
		FILE* resource2; // R2 initally empty
		char  line1[255]; // line in numbers1.txt
		char  line2[255]; // line in numbers2.txt
		char arr1[1001][15]; // store contents of file1
		char arr2[1001][15]; // store contents of file2

		while(fileLen1>0 || fileLen2>0){

		srand(time(NULL));   // should only be called once
		a = (rand() % 10) +1;; //between [1,10]

		if (pthread_mutex_trylock(&lock3) == 0){ //try to lock R2,if it is locked then try to lock R1 files.
			resource2 = fopen("resource2.txt","a+"); //open R2 file to apped data.
			if(resource2 != NULL){
				if (pthread_mutex_trylock(&lock1) == 0){ // try to lock number1.txt file
						file1 = fopen("numbers1.txt", "r");
						if(file1 != NULL){
							int i = 0;
							int j= 0;
							size1 = 0;
							while (fgets(line1, sizeof(line1), file1) != NULL) {
		  				if(i<a){
		  					fprintf(resource2, "%s", line1); // paste first a (random number) line to resource2
		  					i++;
							}
		  				else {
		    				strcpy(arr1[j],line1); // store lines that comes after first a (random number) line in the arr1
		    				j++;
								size1++;
		  			}
					}
				}

				else{
		  		printf("Error opening file.\n" );
				}



		file1 = fopen("numbers1.txt", "w"); //open file,emptying it to write
		if(file1 != NULL){
			int k;
			for(k=0; k < (size1) ; k++) {
		  	fprintf(file1, "%s", arr1[k]); //write arr1 to file1 (we cut first a (random number) line from file1)

			}

			fileLen1=size1;

			fclose(file1);
		}
		else{
		  printf("Error opening file.\n");
		}
		pthread_mutex_unlock(&lock1); //release file1

}


if (pthread_mutex_trylock(&lock2) == 0){ //try to lock file2
			file2 = fopen("numbers2.txt", "r");
	if(file2 != NULL){
				int index = 0;
				int m = 0;
				size2=0;
				while (fgets(line2, sizeof(line2), file2) != NULL) {
		  		if(index<a){
		  			fprintf(resource2, "%s", line2); // paste first a (random number) line to resource2
		  			index++;

					}
		  	else {
		    	strcpy(arr2[m],line2); // store lines that comes after first a (random number) line in the arr2
		    	m++;
					size2++;
		  	}
			}
		}

		else{
		  printf("Error opening file.\n" );
		}



		file2 = fopen("numbers2.txt", "w");//open file2,emptying it to write
		if(file2 != NULL){
			int l;
			for(l=0; l < (size2) ; l++) {
		  	fprintf(file2, "%s", arr2[l]); // write arr2 to file2 (we cut first a (random number) line from file2)

			}

			fileLen2=size2;


			fclose(file2);
		}
		else{
		  printf("Error opening file.\n");
		}
 		pthread_mutex_unlock(&lock2); // release file2

}
		fclose(resource2);
		}

 else{ // if can not open resource2
		  printf("Error opening file.\n");
		}
pthread_mutex_unlock(&lock3); // release R2

	}
}
	pthread_exit(0);

}


void *ThreadB(void *parameters)
{


	FILE* file1; // numbers1.txt R1
	FILE* file2; // numbers2.txt R1
	FILE* resource2; // resource2.txt R2
	char  line1[15]; // line in numbers1.tx
	char  line2[15]; // line in number2.txt
	char  line3[15]; // line in resource2.txt
	char arr1[1001][15]; // store contents of file1
	char arr2[1001][15]; // store contents of file2
	char arr3[1001][15]; // store contents of resource2
	int size1; //size of arr1
	int size2; //size of arr2
	int size3; //size of arr3
	bool flag1 = true ; //to decide if thread is done or not
	bool flag2 = true ; //to decide if thread is done or not
	bool flag3 = true ; //to decide if thread is done or not

 while(flag1 == true || flag2 == true ||flag3 == true ){
 if (pthread_mutex_trylock(&lock1) == 0){ // try to lock file1
	file1 = fopen("numbers1.txt", "r");
	if(file1 != NULL){
		int i = 0;
		size1 = 0;
		while (fgets(line1, sizeof(line1), file1) != NULL) {
	        if (is_prime(atoi(line1)) == true){
						flag1=true;
						continue;
	      }
	      	else{
	        	strcpy(arr1[i],line1); // store lines in file1 that are not prime in arr1
	        	i++;
						size1++;
	      }
	}
	flag1=false;
	fclose(file1);
	}
	else{
	  printf("Error openinig file.\n" );
	}


	file1 = fopen("numbers1.txt", "w"); // emptying file1 to write
	if(file1 != NULL){
		int m;
		for(m=0; m < (size1); m++) {
	  	fprintf(file1, "%s", arr1[m]); //write arr1 to file1 (without prime numbers)
		}
	fclose(file1);
	}
 else{
	 printf("Error opening file.\n");
	}
pthread_mutex_unlock(&lock1); // release file1.
}

	if (pthread_mutex_trylock(&lock2) == 0){ //try to lock file2
		file2 = fopen("numbers2.txt", "r");
		if(file2 != NULL){
			int index = 0;
			size2 = 0;
			while (fgets(line2, sizeof(line2), file2) != NULL) {
	        if (is_prime(atoi(line2)) == true){
						flag2 = true;
						continue;
	      }
	      	else{
	        	strcpy(arr2[index],line2);// store lines in file2 that are not prime in arr2
	        	index++;
						size2++;
	      }
	}
	flag2 = false;
	fclose(file2);
	}
	else{
	  printf("Error openinig file.\n" );
	}


	file2 = fopen("numbers2.txt", "w"); //open and emptying file2 to write
	if(file2 != NULL){
		int j;
		for(j=0; j < (size2); j++) {
	  	fprintf(file2, "%s", arr2[j]); // write arr2 to file2 (without prime numbers)
	}
	fclose(file2);
	}
	else{
	  printf("Error opening file.\n");
	}
pthread_mutex_unlock(&lock2); // release file2
}
	if (pthread_mutex_trylock(&lock3) == 0){ // try to lock resource2
		resource2 = fopen("resource2.txt", "r");
		if(resource2 != NULL){
			int l = 0;
			size3 = 0;
			while (fgets(line3, sizeof(line3), resource2) != NULL) {
	        if (is_prime(atoi(line3)) == true){
						flag3 = true;
						continue;
	      }
	      else{
	        strcpy(arr3[l],line3); // store lines in resource2 that are not prime in arr3
	        l++;
					size3++;
	      }
	}
	flag3 = false;

	fclose(resource2);
	}
	else{
	  printf("Error openinig file.\n" );
	}


	resource2 = fopen("resource2.txt", "w");// emptying file to write
	if(resource2 != NULL){
		int k;
		for(k=0; k < (size3); k++) {
	  	fprintf(resource2, "%s", arr3[k]); // write arr3 to resource2 (without prime numbers)
	}
	fclose(resource2);
	}
	else{
	  printf("Error opening file.\n");
	}
pthread_mutex_unlock(&lock3); //release resource2

}
}
	pthread_exit(0);
}


void *ThreadC(void *parameters)
{

	FILE* file1; // numbers1.txt R1
	FILE* file2; // numbers2.txt R1
	FILE* resource2; // resource2.txt R2
	char  line1[15]; // line in numbers1.txt
	char  line2[15]; // line in numbers2.txt
	char  line3[15]; // line in resource2.txt
	char arr1[1001][15]; // store contents of file1
	char arr2[1001][15];// store contents of file2
	char arr3[1001][15];// store contents of resource2
	int size1; // size of arr1
	int size2; //size of arr2
	int size3; //size of arr3
	bool flag1 = true; //to decided if thread is done or not
	bool flag2 = true; //to decided if thread is done or not
	bool flag3 = true; //to decided if thread is done or not


	while(flag1 == true || flag2 == true || flag3 == true){
	if (pthread_mutex_trylock(&lock3) == 0) { // try to lock resource2
		resource2 = fopen("resource2.txt", "r");
		if(resource2 != NULL){
			int i = 0;
			size3 = 0;
			while (fgets(line3, sizeof(line3), resource2) != NULL) {
	        if (atoi(line3) < 0){
						flag3 = true;
	        	continue;
	      }
	      	else{
	        	strcpy(arr3[i],line3); // store lines that are greater than zero in arr3
	        	i++;
						size3++;

	      }
	}
	flag3 = false;
	fclose(resource2);
	}

	else{
	  printf("Error openinig file.\n" );
	}


	resource2 = fopen("resource2.txt", "w"); // emptying file to write
	if(resource2 != NULL){
		int n;
		for(n=0; n < (size3); n++) {
	  	fprintf(resource2, "%s", arr3[n]); // write arr3 to resource2 (without negative numbers)
	}
	fclose(resource2);
	}
	else{
	  printf("Error openining file.\n" );
	}
	pthread_mutex_unlock(&lock3);

}


	if (pthread_mutex_trylock(&lock1) == 0) { // try to lock file1
	file1 = fopen("numbers1.txt", "r");
	if(file1 != NULL){
	int i = 0;
	size1=0;
	while (fgets(line1, sizeof(line1), file1) != NULL) {
	        if (atoi(line1) < 0){
					flag1=true;
					continue;

	      }
	      else{
	        strcpy(arr1[i],line1); // store lines that are greater than zero in arr1
	        i++;
					size1++;
	      }
	}
	flag1=false;

	fclose(file1);
	}

	else{
	  printf("Error openinig file.\n" );
	}


	file1 = fopen("numbers1.txt", "w"); // emptying file1 to write
	if(file1 != NULL){
		int j;
		for(j=0; j < (size1); j++) {
			fprintf(file1, "%s", arr1[j]); // write arr1 to file1 (without negative numbers)
		}
	fclose(file1);
	}
	else{
		printf("Error opening file.\n");
	}
pthread_mutex_unlock(&lock1); //release file1
}

	if (pthread_mutex_trylock(&lock2) == 0) { //try to lock file2
	file2 = fopen("numbers2.txt", "r");
		if(file2 != NULL){
			int index = 0;
			size2 = 0;
			while (fgets(line2, sizeof(line2), file2) != NULL) {
	        if (atoi(line2) < 0){
						flag2 = true;
						continue;
	      }
	      	else{
	        	strcpy(arr2[index],line2); // store lines that are greater than zero in arr2
	        	index++;
						size2++;
	      }
	}
	flag2 = false;
	fclose(file2);
	}
	else{
	    printf("Error openinig file.\n");
	}

	file2 = fopen("numbers2.txt", "w"); // emptying file to write
	if(file2 != NULL){
		int k;
		for(k=0; k < (size2) ; k++) {
	  	fprintf(file2, "%s", arr2[k]); // write arr2 to file2 (without negative numbers)
	}
	fclose(file2);
	}
	else{
	  printf("Error openining file bura.\n" );
	}
pthread_mutex_unlock(&lock2); //release file2
}

}
pthread_exit(0);
}
