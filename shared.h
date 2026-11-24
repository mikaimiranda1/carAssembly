#ifndef SHARED_H
#define SHARED_H

#include <pthread.h>
#include <semaphore.h>
#include <queue> 
#include <string>
#include "car_assembly.h"


// Set the conveyor capacity limits
const int BODY_CONVEYOR_MAX = 16;
const int SEDAN_MAX = 12;
const int SUV_MAX = 6;
const int PAINT_CONVEYOR_MAX = 10;

const unsigned int BODY_TYPE_MAX[BodyTypeN] = {SEDAN_MAX, SUV_MAX};

// Default for optional command line argument
const int CAR_LIMIT = 100;
const int SLEEP_MS = 0;


// convert to microseconds (option originally in milliseconds)
const int MICROSECS_PER_MS = 1000;


// Exit codes
const int NORMAL_EXIT = 0;
const int BAD_FLAG = 1;
const int THREAD_ERROR = 2;

// For belt1 -> welded bodies waiting for a paint robot
struct BodyConveyor {

    std::queue<BodyType> bodies; // FIFO for bodies 
    unsigned int inQueue[BodyTypeN];  //count of each type currently on the belt
    unsigned int produced[BodyTypeN]; // count of each type that has been added
    unsigned int totalProduced; // keeps track of the car limit by adding the running total
    unsigned int carLimit; // at carLimit, welders stop

    pthread_mutex_t lock; // shield for the fields listed above
    pthread_cond_t roomForType[BodyTypeN]; // for each welder, it sleeps on its own type's variable
    pthread_cond_t notEmpty; // If belt 1 is empty then painters sleep here


};

// belt 2 -> painted bodies wait for final line
struct PaintConveyor {
    std::queue<std::string> paintedBodies;  // holds painted bodies in arrival order
    unsigned int totalFinished; // final line taken
    pthread_mutex_t lock; // shield for fields listed
    pthread_cond_t notFull; // Painters sleep while belt 2 is full
    pthread_cond_t notempty; // FinalLine sleeps when belt 2 is empty
};


// Every thread shares data and within main, pointers are made for all
struct SharedData { 
    BodyConveyor bodyConveyor;
    PaintConveyor paintConveyor;
    unsigned int carLimit; // total cars 
    sem_t allDone; // FinalLine posts but main waits on it
};

// Welder thread
struct WelderArgs {
    SharedData *shared; 
    BodyType type; // Type: Either SUV or Sedan 
    int weldTimeMs; // Pretend weld
};

// Painter thread
struct PainterArgs {
    SharedData *shared; 
    PaintRobotType robot; // Either PrimeSpray or AquaCoat
    int paintTimeMs; // pretend painting time
    unsigned int *consumed; // painter has its own count per body type

};

// FinalLine thread
struct FinalLineArgs {
    SharedData *shared; 
    int finishTimeMs; // Pretend trim and inspection time

};

#endif
