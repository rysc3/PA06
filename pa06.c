#include <stdio.h>
#include <getopt.h>  // To parse command line arguments w/ `optarg` in main
#include <bits/pthreadtypes.h>  // for pthread_t



int waiting = 0;
int total_people = 0;
int total_cars = 0;
int max_people = 0;  // track max waiting at any time

#define SIM_DURATION 700  // 9 to 7pm in mins

pthread_mutex_t lock;

void* car(void* arg);
void* simulation(void* arg);



int main(int argc, char* argv[]){

  int option;
  while((option = getopt(argc, argv, "N:M:")) != -1){
    switch (option){
      case 'N':
        total_people = atoi(optarg);
        break;
      case 'M':
        total_cars = atoi(optarg);
        break;
      default:
        printf("Invalid option\n");
        return 1;
    }
  }
  
  // Check args are valid 
  if(total_people < 0 || total_cars < 0){
    printf("Give -M <int> -N <int>\n");
    return 1;
  }

  // pthread init 
  pthread_mutex_init(&lock, NULL);

  // do thread things
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