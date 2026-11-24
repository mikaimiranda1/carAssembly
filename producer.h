#ifndef PRODUCER_H
#define PRODUCER_H

#include "shared.h"

// Welder thread
// Sleeps to simulate welding -> adds a body of its type
// until car limit is reached (belt 1)
void *weldr(void *arg);

#endif