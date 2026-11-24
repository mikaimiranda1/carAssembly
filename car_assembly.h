#ifndef CARASSEMBLY_H
#define CARASSEMBLY_H

/*
 * Arrays with body producer and body consumer names
 * These can be indexed with the enumerated types below
 * and are defined in log.c
 */

 /* Names of body producers (two welding robots) and body types */
extern const char *body_producers[];
extern const char *body_producerNames[];

/**
 * Names of body consumers (two paint robots) that paint each welded body,
 * producing painted bodies. So they are also the producers of
 * the painted bodies that will be consumed by the final line robot
 * in the next station.
 *
 * Consumers of the welded bodies from the body conveyor.
 * Also producers of painted bodies onto the paint conveyor.
 */
extern const char *body_consumerNames[];

/**
 * Name of the consumer of the painted bodies (final line robot), one thread
 * that installs the trim, inspects the car and rolls it off the end of line.
 **/
extern const char *paintedbody_consumerName;

/*
 * Enumerated types to be used by the producers and consumers
 * These are expected in the input/output functions (log.h)
 * should be useful in your producer and consumer code.
 */

/**
 * The body conveyor can hold up to a maximum of 16 unconsumed bodies
 * at any given time.
 *
 * There can be no more than 12 Sedan bodies and no more than 6 SUV bodies
 * on the body conveyor at any given time.
*/

/*
 * Two producers of bodies (threads) are offered:
 *   one welding robot for producing Sedan bodies,
 *   one welding robot for producing SUV bodies.
 * Each body producer only produces one type of body,
 * so BodyType is synonymous with the producer type
 */
typedef enum Bodies {
  Sedan = 0,          // Sedan body (four-door, low roof line)
  SUV = 1,            // SUV body (tall body, larger frame)
  BodyTypeN = 2,      // number of body types
} BodyType;

/*
 * Two consumers of bodies (threads) are paint robots
 * working in two paint booths.
 * These threads take the bodies off from the body conveyor
 * in the order that they are produced and placed on the conveyor (FIFO).
*/
typedef enum PaintRobots {
  AquaCoat = 0,            // AquaCoat paint robot (water-based coat)
  PrimeSpray = 1,          // PrimeSpray paint robot (primer + spray coat)
  PaintRobotTypeN = 2,     // Number of body consumers / painted body producers
} PaintRobotType;


typedef struct {
  // What kind of body was produced?
  BodyType type;
  // Array of number of bodies of each type that have been produced
  unsigned int *produced;
  // Array of number of bodies of each type that are
  // on the body conveyor and have not yet been consumed.
  // (inQueue[Sedan] and inQueue[SUV])
  unsigned int *inQueue;
} BodyAdded;

typedef struct {
  // Who removed and processed the body?
  PaintRobotType consumer;
  // What kind of body was removed?  As each consumer consumes
  // one type of request, this is the same as the producer.
  BodyType type;
  // Array of number of bodies of each type that have been
  // consumed.
  unsigned int *consumed;
  // Array of number of bodies of each type that are
  // on the body conveyor and have not yet been consumed.
  // (inQueue[Sedan] and inQueue[SUV])
  unsigned int *inQueue;
} BodyRemoved;

#endif
