#include "conveyor.h"
#include "log.h"
#include "shared.h"

#include <pthread.h>



// Functions monitor two conveyors

//  initBodyConveyor manages locking, waiting and signaling
// Robot threads never touch a mutex
void initBodyConveyor(BodyConveyor *conveyor, unsigned int carLimit) {
    // start: empty conveyor -> set up lock and condition variables
    for(int type = 0; type < BodyTypeN; type++) {
        conveyor->inQueue[type] = 0;
        conveyor->produced[type] = 0;
        pthread_cond_init(&conveyor->roomForType[type], NULL);
    }

    conveyor->carLimit = carLimit;
    conveyor->totalProduced = 0;
    pthread_mutex_init(&conveyor->lock, NULL);
    pthread_cond_init(&conveyor->notEmpty, NULL);
    pthread_cond_init(&conveyor->notEmpty, NULL);

}

void initPaintConveyor(PaintConveyor *conveyor) {
    conveyor->totalFinished = 0; // Sets up lock and condition variables for an empty conveyor
    pthread_mutex_init(&conveyor->lock, NULL);
    pthread_cond_init(&conveyor->notFull, NULL);
    pthread_cond_init(&conveyor->notempty, NULL);
}

bool addBody(BodyConveyor *conveyor, BodyType type) {

    pthread_mutex_lock(&conveyor->lock);
    
    // Waiting when caras are needed; Belt/type is full
    while(conveyor->totalProduced < conveyor->carLimit && 
        (conveyor->bodies.size() > BODY_CONVEYOR_MAX || 
        conveyor->inQueue[type] >= BODY_TYPE_MAX[type])) {
        pthread_cond_wait(&conveyor->roomForType[type], &conveyor->lock);
    }

    if(conveyor->totalProduced >= conveyor->carLimit) {
        pthread_mutex_unlock(&conveyor->lock);

        return false;
    }

    // Update counts and body goes on belt
    conveyor->bodies.push(type);
    conveyor->inQueue[type] ++;
    conveyor->produced[type] ++;
    conveyor->totalProduced ++;

    BodyAdded added;
    added.type = type;
    added.produced = conveyor->produced;
    added.inQueue = conveyor->inQueue;
    log_added_body(added);  // Once belt changes, log inside the lock

    // wake a painter that could be waiting for a body
    pthread_cond_signal(&conveyor->notEmpty);
    pthread_mutex_unlock(&conveyor->lock);

    return true;


}

BodyType removeBody(BodyConveyor *conveyor, PaintRobotType robot, unsigned int *consumed) {

    pthread_mutex_lock(&conveyor->lock);

    // waiting until a body shows
    while(conveyor->bodies.empty()) {
        pthread_cond_wait(&conveyor->notEmpty, &conveyor->lock);
    }

    // FIFO: Take the oldest body and update counts 
    BodyType type = conveyor->bodies.front();
    conveyor->bodies.pop();
    conveyor->inQueue[type] --;
    consumed[type] ++;

    // Log body removed, inside the lock after belt changes
    BodyRemoved removed;
    removed.consumer = robot;
    removed.type = type;
    removed.consumed = consumed;
    removed.inQueue = conveyor->inQueue;
    log_removed_body(removed);

    // After removal, there is room both welders
    for(int weldr = 0; weldr < BodyTypeN; weldr++) {
        pthread_cond_signal(&conveyor->roomForType[weldr]);
    }

    pthread_mutex_unlock(&conveyor->lock);
    return type;
}

void addPaintBody(PaintConveyor *conveyor, const std::string &paintedBody) {
        pthread_mutex_lock(&conveyor->lock);

        // waiting for belt 2 to open 
        while(conveyor->paintedBodies.size() >= PAINT_CONVEYOR_MAX) {
            pthread_cond_wait(&conveyor->notFull, &conveyor->lock);
        }

        conveyor->paintedBodies.push(paintedBody);
        log_added_paintedbody(paintedBody.c_str(), conveyor->paintedBodies.size());

        pthread_cond_signal(&conveyor->notempty);
        pthread_mutex_unlock(&conveyor->lock);  // wake up FinalLine if it is waiting

}

unsigned int removePaintedBody(PaintConveyor *conveyor) {

    pthread_mutex_lock(&conveyor->lock);

    // Waiting until there is a painted body to finish
    while(conveyor->paintedBodies.empty()) {
        pthread_cond_wait(&conveyor->notempty, &conveyor->lock);    
    }

    // FIFO: take oldest paintedBody and count it as finished
    std::string paintedBody = conveyor->paintedBodies.front(); 
    conveyor->paintedBodies.pop();
    conveyor->totalFinished++;
    unsigned int finished = conveyor->totalFinished;
    log_removed_paintedbody(paintedBody.c_str(), conveyor->paintedBodies.size(), finished);

    // Wake up a painter that could be waiting for room on belt2
    pthread_cond_signal(&conveyor->notFull);
    pthread_mutex_unlock(&conveyor->lock);


    return finished;

}


