#ifndef PROBLEM_H
#define PROBLEM_H

#include <cstddef>
#include <string>

#include "customer.h"
#include "depot.h"

using namespace std;

class ProblemParameters {
    public:
        string name;
        
        Depot* depots = NULL;
        int depotCount;
        int vehicleCount = 1;
        
        Customer* customers = NULL;
        int customerCount;
};

#endif