#include <cstdlib>
#include <unistd.h> 
#include <iostream>

#include "shared.h"
#include "conveyor.h"
#include "producer.h"
#include "consumer.h"
#include "log.h"

using namespace std;

// The main thread parses CL options,  
int main(int argc, char **argv) {

    int option; // this is the current flag returned by getopt
    int carLimit = CAR_LIMIT; // # of cars to produce (-n)
    // all in ms
    int sedanWeldTime = SLEEP_MS; // SedanWelder time (-s)
    int suvWeldTime = SLEEP_MS; // SUVWelder time (-u)
    int aquaCoatTime = SLEEP_MS; // AquaCoat paint time (-a)
    int primeSprayTime = SLEEP_MS; // PrimeSpray paint time (-p)
    int finalLineTime = SLEEP_MS; // FinalLine trim + inspect time (-f)

    while ((option = getopt(argc, argv, "n:s:u:a:p:f:")) != -1) {

        switch(option) {

            case 'n':
                carLimit = atoi(optarg);
                break;

            case 's':
                sedanWeldTime = atoi(optarg);
                break;

            case 'u':
                suvWeldTime = atoi(optarg);
                break;

            case 'a':
                aquaCoatTime = atoi(optarg);
                break;

            case 'p':
                primeSprayTime = atoi(optarg);
                break;

            case 'f':
                finalLineTime = atoi(optarg);
                break;

            default: 
                // if flag is missing value or even unknown
                exit(BAD_FLAG);

        }

    }

    // Shared data is set up before any threading begins

    SharedData sharedData; 
    sharedData.carLimit = carLimit;

    initBodyConveyor(&sharedData.bodyConveyor, carLimit);
    initPaintConveyor(&sharedData.paintConveyor);
    sem_init(&sharedData.allDone, 0, 0);

    // For each painter, takes count of each body type consumed
    unsigned int *consumed[PaintRobotTypeN];
    for(int robot = 0; robot < PaintRobotTypeN; robot++) {

        consumed[robot] = new unsigned int [BodyTypeN];

        for(int type = 0; type < BodyTypeN; type ++) {
            consumed[robot][type] = 0;
        }
    }

    // For each thread -> it gets shared data + settings
    WelderArgs sedanArgs = {&sharedData, Sedan, sedanWeldTime};
    WelderArgs suvArgs = {&sharedData, SUV, suvWeldTime};

    PainterArgs aquaCoatArgs = {&sharedData, AquaCoat, aquaCoatTime, consumed[AquaCoat]};
    PainterArgs primeSprayArgs = {&sharedData, PrimeSpray, primeSprayTime, consumed[PrimeSpray]};

    FinalLineArgs finalArgs = {&sharedData, finalLineTime};

    pthread_t sedanThread, suvThread, aquaThread, primeThread, finalThread;

    // starting all threads condition
    if(pthread_create(&sedanThread, NULL, &weldr, &sedanArgs) ||
        pthread_create(&suvThread, NULL, &weldr, &suvArgs) ||
        pthread_create(&aquaThread, NULL, &painter, &aquaCoatArgs) ||
        pthread_create(&primeThread, NULL, &painter, &primeSprayArgs) ||
        pthread_create(&finalThread, NULL, &finalLine, &finalArgs)) {

            exit(THREAD_ERROR);
    }

    // waiting for FinalLine to finish the last car
    sem_wait(&sharedData.allDone);
    log_body_history(sharedData.bodyConveyor.produced, consumed);

    for(int robot = 0; robot < PaintRobotTypeN; robot++) {
        delete [] consumed[robot]; // free the count arrays (per painter)
    }

    exit(NORMAL_EXIT); // Hooray

}