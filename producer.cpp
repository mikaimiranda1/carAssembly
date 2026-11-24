#include "producer.h"
#include "conveyor.h"

#include <unistd.h>

void *weldr(void *arg) {
    WelderArgs *args = (WelderArgs *) arg;
    bool keepWelding = true;

    while(keepWelding) {
        usleep(args->weldTimeMs * MICROSECS_PER_MS); // sims welding outside the CS

        keepWelding = addBody(&args->shared->bodyConveyor, args->type); // addBody returns False at carLimit
    }
    return NULL;
}