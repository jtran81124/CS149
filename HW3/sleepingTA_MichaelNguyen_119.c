#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define MAX_SLEEP_TIME 3
#define NUM_OF_STUDENTS 4
#define NUM_OF_HELPS 2
#define NUM_OF_SEATS 2

pthread_mutex_t mutex_lock; //controls the waiting room; when locked, # waitingstudents can be inc/dec

/* semaphore declarations */
sem_t students_sem; //ta waits for student to show up, student notifies TA of arrival
sem_t ta_sem; //student waits for ta to help, ta notifies student they are ready to help


int waiting_students;
unsigned int seed;
//function prototypes
void* student(void* number);
void* ta(void* param);

int main(){
	pthread_t students[NUM_OF_STUDENTS];
	pthread_t taThread;
	int i;
	int studentID[4] = {0, 1, 2, 3};
	seed = time(NULL);

	printf("CS149 SleepingTA from Michael Nguyen\n");
	
	//initialize the waiting room lock
	pthread_mutex_init(&mutex_lock, NULL);

	//initialize semaphores
	sem_init(&students_sem, 0, 0);
	sem_init(&ta_sem, 0, 0);

	//create TA thread
	pthread_create(&taThread, NULL, ta, NULL);

	//create student threads
	for(i = 0; i < NUM_OF_STUDENTS; i++)	/* initialize the random seed */
		pthread_create(&students[i], NULL, student,  &studentID[i]); //pass the student number

	//join student threads
	for(i = 0; i < NUM_OF_STUDENTS; i++)
		pthread_join(students[i], NULL);

	pthread_cancel(taThread);

	return 1;
}

void* student(void* number){
	int sleepTime;
	int studentNumber = *(int*) number;
	int helpCount;
	
	for(helpCount = 0; helpCount < NUM_OF_HELPS; ++helpCount){
		seed += 1;
		sleepTime = (int) rand_r(&seed) % MAX_SLEEP_TIME + 1;
		printf("\tStudent %d programming for %d seconds\n", studentNumber, sleepTime);
		sleep(sleepTime);

		pthread_mutex_lock(&mutex_lock);
		//if there are available seats in the waiting room
		if(waiting_students < NUM_OF_SEATS){
			waiting_students += 1;
			printf("\t\tStudent %d takes a seat, # of waiting students = %d\n", studentNumber, waiting_students);
			sem_post(&students_sem); //notifies the TA that student is ready
			pthread_mutex_unlock(&mutex_lock); //release the lock on the waiting room
			sem_wait(&ta_sem); //wait for TA to help
			printf("Student %d receiving help\n", studentNumber);
		}
		else{ //student comes back if the seats are all filled
			printf("\t\t\tStudent %d will try later\n", studentNumber);
			helpCount -= 1;
			pthread_mutex_unlock(&mutex_lock);
		}
	}
}
void* ta(void* param){
	int sleepTime;
	int helpCount = 0;

	while(helpCount != (NUM_OF_STUDENTS * NUM_OF_HELPS)){
		seed += 1;
		sem_wait(&students_sem); //attempt to get student
		pthread_mutex_lock(&mutex_lock); //attempt to get access of hallway waiting room
		waiting_students -= 1; //one less student is in the waiting room
		sem_post(&ta_sem); //TA helps the student
		pthread_mutex_unlock(&mutex_lock); //unlock waiting room
		sleepTime = (int) rand_r(&seed) % MAX_SLEEP_TIME + 1;
		printf("Helping a student for %d seconds, # of waiting students = %d\n", sleepTime, waiting_students);
		sleep(sleepTime);
		helpCount += 1;
	}
}

