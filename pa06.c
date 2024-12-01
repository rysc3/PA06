#include <stdio.h>
#include <getopt.h>            // To parse command line arguments w/ `optarg` in main
#include <bits/pthreadtypes.h> // for pthread_t
#include "random437.h"

int waiting = 0;
int total_people = 0;
int total_cars = 0;
int total_rides_taken = 0;
int total_rejected = 0;
int max_people = 0; // track max waiting at any time
int max_waiting_time = 0;

#define SIM_DURATION 700 // 9 to 7pm in mins
#define MAXWAITPEOPLE 800

void *car(void *arg);
void *simulation(void *arg);

pthread_mutex_t lock;
// Global clock
pthread_mutex_t mutex_clock;
pthread_cond_t sync;
int time = 0;

int main(int argc, char *argv[])
{

  // parse options
  int option;
  while ((option = getopt(argc, argv, "N:M:")) != -1)
  {
    switch (option)
    {
    case 'N':
      total_cars = atoi(optarg);
      if (total_cars < 0)
      {
        printf("Need a positive N\n");
        return 1;
      }
      break;
    case 'M':
      max_people = atoi(optarg);
      if (max_people < 0)
      {
        printf("Need a positive M\n");
        return 1;
      }
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
  for (int i = 0; i < total_cars; i++)
  {
    pthread_create(&cars[i], NULL, car, NULL);
  }

  /*
    These two loops need to stay separate. I tried putting them together at first and it breaks everyting.

    When we do the join, it waits for the thread to finish before continuing.

    If you add the cars right as you create them without waiting for main to finish first, you just serialize all the threads and make then run entirely sequentially.
  */
  pthread_join(sim, NULL);
  for (int i = 0; i < total_cars; i++)
  {
    pthread_join(cars[i], NULL);
  }

  // Donezo
  pthread_mutex_destroy(&lock);
  pthread_mutex_destroy(&mutex_clock);
  pthread_cond_destroy(&sync);

  // Print final stats
  printf("-----------------------------\n");
  printf("-----------------------------\n");
  printf("Results:\n");
  printf("Total people arrived: %d\n", total_people);
  printf("Total people rejected: %d\n", total_rejected);
  printf("Total people who rode: %d\n", total_people);
  printf("Total rides taken: %d\n", total_rides_taken);
  printf("Max queue size: %d was at %d mins.\n", max_people, max_waiting_time);
  printf("-----------------------------\n");
  printf("-----------------------------\n");

  return 0;
}

/*
  Function to run the entire simulation of shuffling people through cars until we reach
  the max amount of time.
*/
void *simulation(void *arg)
{
  for (int i = 0; i < SIM_DURATION; i++)
  {
    int people = poissonRandom(mean_arrivals(i));
    int rejected = 0;
    pthread_mutex_lock(&lock);

    /*
      Inside the safe section, we handle tracking the number of people arriving, waititng, and comparing to the max.

      Start by checking we haven't went over our defined MAX. If so, track how many we've rejected
      If not, track all the other things and send them on the rides.
    */
    if (waiting + people > MAXWAITPEOPLE) // Case where we need to reject someone
    {
      // Track max waiting people
      if (waiting > max_people)
      {
        max_people = waiting;
        max_waiting_time = i;
      }
      rejected += (waiting + people) - MAXWAITPEOPLE;
      printf("(sim) Step: %d, Rejected %d bums.\n", i, rejected);
    }
    else // Case where we are below the MAX cap
    {
      waiting += people;
    }
    pthread_mutex_unlock(&lock);
    /*
      End of safe section
    */

    /*
      Create a global clock running in the simulation thread. We send a broadcast message to all threads
      on set intervals so they can syncronize their clocks against the global.

      Counting time to be the i value, so with a SIM_DURATION of 700 each i is a minute.
    */
    pthread_mutex_lock(&mutex_clock);
    time = i;
    pthread_cond_broadcast(&sync);
    pthread_mutex_unlock(&mutex_clock);
  }
  return NULL;
}

/*
  Function to track each car as their own threads, each carrying a number of people

  They need to start and leave at set intervals, and also track that they don't
  take more than a certain number of people.
*/
void *car(void *arg)
{
  int riders = 0;

  // Each thread loops until the sim ends
  while (time < SIM_DURATION)
  {
    /*
      Cars sync with the global clock
      each waits for a message from the mutex_clock to start. This is broadcast to every thread
      in simulation with the pthread_cond_broadcast.
    */
    pthread_mutex_lock(&mutex_clock);
    pthread_cond_wait(&sync, &mutex_clock);
    pthread_mutex_unlock(&mutex_clock);

    // We wait before loading since we can accept new passengers even if they show up during loading period
    usleep(7000000); // 7 seconds

    /*
      Inside of safe section here, we manage dealing with the shared number of people things.
    */
    pthread_mutex_lock(&lock);
    if (waiting > 0)
    {
      riders = (waiting >= max_people) ? max_people : waiting;

      // Save total riders
      total_people += riders;

      waiting -= riders;

      // Track total rides taken
      total_rides_taken += riders;

      printf("  (car) Took %d riders", riders);
    }
    pthread_mutex_unlock(&lock);

    // ride time
    usleep(53000000); // 53 seconds
  }

  return NULL;
}

/*
  From project overview.
*/
int mean_arrivals(int time)
{
  // 09:00 - 10:59
  if (time < 120)
    return 25;
  // 11:00 - 13:59
  if (time < 240)
    return 45;
  // 14:00 - 15:59
  if (time < 360)
    return 35;
  // 16:00 - 18:59
  return 25;
}