#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#include "log.h"

/*
 * i/o functions - assumed to be called in a critical section
 */


/* Handle C++ namespaces, ignore if compiled in C 
 * C++ usually uses this #define to declare the C++ standard.
 * It will not be defined if a C compiler is used.
 */
#ifdef __cplusplus
using namespace std;
#endif

sem_t sem_print;  // Semaphore for ensuring mutex printing, 
                  // I know this is bad practice in terms of using Global, which is not ideal.
                  // Do NOT use the Global variables in your code! 

pthread_once_t init_once = PTHREAD_ONCE_INIT;  // Ensures one-time initialization

void initialize_semaphore() {
    sem_init(&sem_print, 0, 1);  // Initialize semaphore with value 1
}

/*
 * Data section - names must align with the enumerated types
 * defined in car_assembly.h
 */

/* Names of body producer threads and body types */
const char *body_producers[] = {"Sedan body", "SUV body"};
const char *body_producerNames[] = {"SEDAN", "SUV"};

/**
 * Names of body consumer threads (paint robots in two paint booths).
 * Consumers of the welded bodies from the body conveyor.
 * Also producers of painted bodies to the final line robot.
 */
const char *body_consumerNames[] = {"AquaCoat", "PrimeSpray"};

/**
 * Name of the final line robot consumer.
 * Consumer of painted bodies, installs the trim, inspects the car
 * and rolls it off the end of line.
 **/
const char *paintedbody_consumerName = "FinalLine";

/* double elapsed_s()
 * show seconds of wall clock time used by the process
 */

double elapsed_s() {
  const double ns_per_s = 1e9; /* nanoseconds per second */

  /* Initialize the first time we call this */
  static struct timespec start;
  static int firsttime = 1;

  struct timespec t;

  /* get elapsed wall clock time for this process
   * note:  use CLOCK_PROCESS_CPUTIME_ID for CPU time (not relevant here
   * as we will be sleeping a lot on the condition variables)
   */
  clock_gettime(CLOCK_REALTIME, &t);

  if (firsttime) {
    /* first time we've called the function, store the current
     * time.  This will not track the cost of the first item
     * produced, but is a reasonable approximation for the
     * whole process and avoids having to create an initialization
     * function.
     * (In C++, we'd just build a timer object and pass it around,
     *  but this approximation provides a simple interface for both
     *  C and C++.)
     */
    firsttime = 0;  /* don't do this again */
    start = t;  /* note when we started */
  }
  
  /* determine time delta from start and convert to s */
  double s = (t.tv_sec - start.tv_sec) + 
    (t.tv_nsec - start.tv_nsec) / ns_per_s ;
  return s;
}

/**
 * @brief Show that a body has been added to the body conveyor and 
 *        print the current status of the body conveyor.
 * 
 * @param bodyAdded      see header for this type
 * 
 * produced and inQueue reflect numbers *after* adding the current body.
 */
void log_added_body(BodyAdded bodyAdded) {
  int idx;
  int total;

  pthread_once(&init_once, initialize_semaphore);  // Ensure one-time initialization

  // lock for mutex printing
  sem_wait(&sem_print);

  /* Show what is in the body conveyor */
  printf("Body_conveyor: ");
  total = 0;  /* total produced */
  for (idx=0; idx < BodyTypeN; idx++) {
    if (idx > 0)
      printf(" + ");  /* separator */
    printf("%d %s", bodyAdded.inQueue[idx], body_producerNames[idx]);
    total += bodyAdded.inQueue[idx];
  }

  printf(" = %d. ", total);

  printf("Added %s.", body_producers[bodyAdded.type]);

  /* Show what has been produced */
  total = 0;
  printf(" Produced: ");
  for (idx=0; idx < BodyTypeN; idx++) {
    total += bodyAdded.produced[idx];  /* track total produced */
    if (idx > 0)
      printf(" + ");  /* separator */
    printf("%d %s", bodyAdded.produced[idx], body_producerNames[idx]);
  }
  /* total produced over how long */
  printf(" = %d in %.3f s.\n", total, elapsed_s());

  /* This is not really needed, but will be helpful for making sure that you
   * see output prior to a segmentation violation.  This is not usually a
   * good practice as we want to avoid ending the CPU burst prematurely which
   * this will do, but it is a helpful technique.
   */
  fflush(stdout);  

  sem_post(&sem_print); // release the semaphore for printing
};

/**
 * @brief   Show that a body has been removed from the body conveyor 
 *          and print the current status of the body conveyor.
 *
 * @param bodyRemoved    see header for this type
 * 
 * Counts reflect numbers *after* the body has been removed
 */
void log_removed_body(BodyRemoved bodyRemoved) {
  int idx;
  int total;

  pthread_once(&init_once, initialize_semaphore);  // Ensure one-time initialization

  // lock for mutex printing
  sem_wait(&sem_print);

  /* Show what is in the body conveyor */
  total = 0;
  printf("Body_conveyor: ");
  for (idx=0; idx < BodyTypeN; idx++) {
    if (idx > 0)
      printf(" + ");  /* separator */
    printf("%d %s", bodyRemoved.inQueue[idx], body_producerNames[idx]);
    total += bodyRemoved.inQueue[idx];
  }
  printf(" = %d. ", total);
  
  /* Show what has been consumed by consumer */
  printf("%s consumed %s.  %s totals: ",
    body_consumerNames[bodyRemoved.consumer],
    body_producers[bodyRemoved.type],
    body_consumerNames[bodyRemoved.consumer]);
  total = 0;
  for (idx = 0; idx < BodyTypeN; idx++) {
    if (idx > 0)
      printf(" + ");  /* separator */
    total += bodyRemoved.consumed[idx];  /* track total consumed */
    printf("%d %s", bodyRemoved.consumed[idx], body_producerNames[idx]);
  }
  /* total consumed over how long */
  printf(" = %d consumed in %.3f s.\n", total, elapsed_s());

  /* This is not really needed, but will be helpful for making sure that you
   * see output prior to a segmentation violation.  This is not usually a
   * good practice as we want to avoid ending the CPU burst prematurely which
   * this will do, but it is a helpful technique.
   */
  fflush(stdout);

  sem_post(&sem_print); // release the semaphore for printing
};

/**
 * @brief   Show that an item has been added to the paint conveyor 
 *          and print the current size of the paint conveyor.
 *
 * @param paintedBodyAdded       a string combining body consumer name and 
 *                              body type, e.g., AquaCoatSUV, see log.c
 * @param paintConveyorSize   Size *after* the painted body has been added
 */
void log_added_paintedbody(const char* paintedBodyAdded,
                              unsigned int paintConveyorSize) {

  pthread_once(&init_once, initialize_semaphore);  // Ensure one-time initialization

  // lock for mutex printing
  sem_wait(&sem_print);

  /* Show what is in the paint conveyor */
  printf("Paint_conveyor: produced and added %s in %.3f s, queue size: %d\n", 
         paintedBodyAdded, elapsed_s(), paintConveyorSize);

  sem_post(&sem_print); // release the semaphore for printing
}

/**
 * @brief   Show that an item has been removed from the paint conveyor 
 *          and print the current size of the paint conveyor.
 *
 * @param paintedBodyRemoved     a string combining body consumer name and 
 *                              body type, e.g., AquaCoatSUV, see log.c
 * @param paintConveyorSize   Size *after* the painted body has been removed
 * @param totalFinished         total number of finished painted bodies so far
 */
void log_removed_paintedbody(const char* paintedBodyRemoved,
                                unsigned int paintConveyorSize,
                                unsigned int totalFinished) {

  pthread_once(&init_once, initialize_semaphore);  // Ensure one-time initialization

  // lock for mutex printing
  sem_wait(&sem_print);

  /* Show what is in the paint conveyor */
  printf("Paint_conveyor: removed and finished %s in %.3f s, queue size: %d, total finished: %d\n", 
    paintedBodyRemoved, elapsed_s(), paintConveyorSize, totalFinished);

  sem_post(&sem_print); // release the semaphore for printing
}

/**
 * @brief   Show how many bodies of each type produced.  
 *          Show how many bodies consumed by each consumer.
 * 
 * @param produced   count for each BodyType produced
 * @param consumed   array of pointers to consumed arrays for each consumer
 *                   e.g. consumed[AquaCoat] points to an array that 
 *                   is indexed by body type
 *                   (it is a 2-D array, consumed[AquaCoat][Sedan] is 
 *                    the number for Sedan bodies that were 
 *                    consumed by AquaCoat)
 */
void log_body_history(unsigned int produced[], 
                            unsigned int *consumed[]) {
  int p, c;  /* array indices */
  int total;

  printf("\nCAR ASSEMBLY REPORT\n----------------------------------------\n");
   
  /* show number produced for each body type */
  for (p = 0; p < BodyTypeN; p++) {
    printf("%s producer generated %d bodies\n",
	   body_producers[p], produced[p]);
  }
  /* show number consumed by each consumer */
  for (c=0; c < PaintRobotTypeN; c++) {
    printf("%s consumed ", body_consumerNames[c]);
    total = 0;
    for (p = 0; p < BodyTypeN; p++) {
      if (p > 0)
	printf(" + ");
    
      total += consumed[c][p];
      printf("%d %s", consumed[c][p], body_producerNames[p]);
    }
    printf(" = %d total\n", total);
  }

  printf("Elapsed time %.3f s\n", elapsed_s());
}
