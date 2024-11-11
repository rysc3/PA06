#include <stdio.h>
#include <getopt.h>  // To parse command line arguments w/ `optarg` in main
#include <bits/pthreadtypes.h>  // for pthread_t



int waiting = 0;
int total_people = 0;
int total_cars = 0;

pthread_mutex_t lock;



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

}