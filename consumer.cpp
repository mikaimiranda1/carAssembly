#include "consumer.h"
#include "conveyor.h"

#include <unistd.h>
#include <string>



// The *painter function runs through both paint robots 
// args tell each one which robot it is and how long painting takes
void *painter(void *arg) {
    PainterArgs *args = (PainterArgs *) arg;

    while(true) {
        // next welded body is taken off belt 1
        BodyType type = removeBody(&args->shared->bodyConveyor, args->robot, args->consumed);

        usleep(args->paintTimeMs * MICROSECS_PER_MS); // sims painting outside the CS

        std::string paintedBody = std::string(body_consumerNames[args->robot])
            + body_producerNames[type];
        addPaintBody(&args->shared->paintConveyor, paintedBody);
    }

    return NULL;
}

// FinalLine thread -> finishes painted bodies until carLimit is reached
void *finalLine(void *arg) {

    FinalLineArgs *args = (FinalLineArgs *) arg;
    bool allFinished = false;

    while(!allFinished) {
        unsigned int finished = removePaintedBody(&args->shared->paintConveyor);

        // sims trim/inspection OUTSIDE the CS
        usleep(args->finishTimeMs * MICROSECS_PER_MS);
        allFinished = (finished >= args->shared->carLimit);
    }

    // Lets main know every car is finished
    sem_post(&args->shared->allDone);
    return NULL;

}