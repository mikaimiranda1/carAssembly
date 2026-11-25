#ifndef CONSUMER_H
#define CONSUMER_H

#include "shared.h"

// Painter thread
// Removes body from belt1, sleeping helps simulate painting
// continues to add painted body to belt 2
// Takes in a pointer to PainterArgs and outputs nothing
void *painter(void *arg);

// FinalLine thread
// Removes painted bodies from belt 2 and sleeping sims trim/inspection
// Takes in pointier to FinalLineArgs and outputs NULL after finishing
void *finalLine(void *arg);

#endif