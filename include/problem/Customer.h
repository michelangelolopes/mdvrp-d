#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "Position2D.h"

class Customer : public Position2D {
    public:
        double demand;
};

#endif