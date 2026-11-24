#ifndef CONVEYOR_H
#define CONVEYOR_H

#include "shared.h"
#include <string>

// To set up belt 1, initBodyConveoyr zeroes the counts, stores carLimit, and initializes
// mutex and condition variables. 
// Input takes the conveyor, total number of cars and outputs nothing
void initBodyConveyor(BodyConveyor *conveyor, unsigned int carLimit);


// For belt 2, the finish count is zeroed and mutex/condition variables are initialized
// Takes in the conveyor -> outputs nothing
void initPaintConveyor(PaintConveyor *conveyor);

// Welder adds a body to belt 1 and waits while the belt or type is full
// Input takes conveyor and body type -> Outputs true if added and false if carLimit was reached
bool addBody(BodyConveyor *conveyor, BodyType type);

// Painter removes oldest body in belt 1 and waits while belt is empty
// removeBody takes in the conveyor, painter, and its per-type counts
// Output -> BodyType that is being removed
BodyType removeBody(BodyConveyor *conveyor, PaintRobotType robot, unsigned int *consumed);


// Painter produces painted bodies to belt 2, waiting if full
// Input takes the conveyor, and paintedBody name -> output nothing
void addPaintBody(PaintConveyor *conveyor, const std::string &paintedBody);

// FinalLine is tasked to remove the oldest paintedBody on belt 2, wait if empty
// Using conveyor, removePaintedBody returns the number of painted bodies finished + the current one
unsigned int removePaintedBody(PaintConveyor *conveyor);

#endif