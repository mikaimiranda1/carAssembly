#include <cstdlib>
#include <unistd.h> 
#include <iostream>
#include "shared.h"

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

    // test for parsing
    cout << "n=" << carLimit << " s=" << sedanWeldTime << " u=" << suvWeldTime 
        << " a=" << aquaCoatTime << " p=" << primeSprayTime 
        << " f=" << finalLineTime << endl;


    exit(NORMAL_EXIT);

}