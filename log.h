#ifndef LOG_H
#define LOG_H

#include "car_assembly.h"

/**
 * @brief Show that a body has been added to the body conveyor and 
 *        print the current status of the body conveyor.
 * 
 * @param bodyAdded      see header for this type
 * 
 * produced and inQueue reflect numbers *after* adding the current body.
 */
void log_added_body (BodyAdded bodyAdded);

/**
 * @brief   Show that a body has been removed from the body conveyor 
 *          and print the current status of the body conveyor.
 *
 * @param bodyRemoved    see header for this type
 * 
 * Counts reflect numbers *after* the body has been removed
 */
void log_removed_body(BodyRemoved bodyRemoved);

/**
 * @brief   Show that an item has been added to the paint conveyor 
 *          and print the current size of the paint conveyor.
 *
 * @param paintedBodyAdded       a string combining body consumer name and 
 *                              body type, e.g., AquaCoatSUV, see log.c
 * @param paintConveyorSize   Size *after* the painted body has been added
 */
void log_added_paintedbody(const char* paintedBodyAdded,
                              unsigned int paintConveyorSize);

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
                                unsigned int totalFinished);

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
void log_body_history(unsigned int produced[], unsigned int *consumed[]);

#endif
