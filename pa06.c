#include <stdio.h>
#include <getopt.h>  // To parse command line arguments w/ `optarg` in main
#include <bits/pthreadtypes.h>  // for pthread_t
#include "random437.h"



int waiting = 0;
int total_people = 0;
int total_cars = 0;
int max_people = 0;  // track max waiting at any time

#define SIM_DURATION 700  // 9 to 7pm in mins
#define MAXWAITPEOPLE 800 

void* car(void* arg);
void* simulation(void* arg);

pthread_mutex_t lock;
// Global clock 
pthread_mutex_t mutex_clock; 
pthread_cond_t sync;
int time = 0;


int main(int argc, char* argv[]){

  // parse options
  int option;
  while((option = getopt(argc, argv, "N:M:")) != -1){
    switch (option){
      case 'N':
        total_cars = atoi(optarg);
        break;
      case 'M':
        max_people = atoi(optarg);
        break;
      default:
        printf("Invalid option\n");
        return 1;
    }
  }

  /*
    Initialize all the thread variables. 

    lock is what we use as a critical section among the threads to track when things are waiting.

    clock is what we use as a global clock so we can syncronize all the processes. We then use c for the waiting variable. Make the processes wait until c is ready before they can continue, and c is set based off of when the clock is at the right time so that everything will be syncronized.
  */
  pthread_mutex_init(&lock, NULL);
  pthread_mutex_init(&mutex_clock, NULL);
  pthread_cond_init(&sync, NULL);
  
  // Check args are valid 
  if(total_people < 0 || total_cars < 0){
    printf("Give -M <int> -N <int>\n");
    return 1;
  }

  pthread_t cars[total_cars]; // track all of our cars
  pthread_t sim;

  /*
    for pthread_create args; 
    1. pointer to thread ID 
    2. thread attr. 
    3. function to execute 
    4. args for function in 3. 
  */
  pthread_create(&sim, NULL, simulation, NULL);
  for (int i = 0; i < total_cars; i++){
    pthread_create(&cars[i], NULL, car, NULL);
  }

  /*
    These two loops need to stay separate. I tried putting them together at first and it breaks everyting. 

    When we do the join, it waits for the thread to finish before continuing. 

    If you add the cars right as you create them without waiting for main to finish first, you just serialize all the threads and make then run entirely sequentially.
  */
  pthread_join(sim, NULL);
  for(int i = 0; i < total_cars; i++){
    pthread_join(cars[i], NULL);
  }



  // Donezo 
  pthread_mutex_destroy(&lock);
  pthread_mutex_destroy(&mutex_clock);
  pthread_cond_destroy(&sync);

  return 0;
}


/*
  Function to track each car as their own threads, each carrying a number of people
  
  They need to start and leave at set intervals, and also track that they don't
  take more than a certain number of people.
*/
void* car(void* arg){

  // We wait before loading since we can accept new passengers even if they show up during loading period
  usleep(7000000);  // 7 seconds
  
  pthead_mutex_lock(&lock);
  // TODO accept less than or equal to max passengers
  pthead_mutex_lock(&lock);

  // ride time
  usleep(53000000);  // 53 seconds
}

/*
  Function to run the entire simulation of shuffling people through cars until we reach 
  the max amount of time. 
*/
void* simulation(void* arg){

  // int time = 0;
  // while(time < SIM_DURATION){
  //   pthread_mutex_lock(&lock);
  //   pthread_mutex_lock(&lock);
  // }

  for(int i=0; i<SIM_DURATION; i++){
    pthread_mutex_lock(&lock);

    total_people += 1;

    if(total_people > max_people){
      max_people = total_people;
    }

    pthread_mutex_lock(&lock);
  }
}