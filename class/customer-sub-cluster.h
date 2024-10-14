#ifndef CUSTOMER_SUB_CLUSTER_H
#define CUSTOMER_SUB_CLUSTER_H

#include "customer.h"

class CustomerSubCluster {
    Customer* neighborCustomers;

    public:
        Customer* getNeighborCustomers() {
            return neighborCustomers;
        }

        void setNeighborCustomers(Customer* neighborCustomersInformed) {
            neighborCustomers = neighborCustomersInformed;
        }
};

#endif