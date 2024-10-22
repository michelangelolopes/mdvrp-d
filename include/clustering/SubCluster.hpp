#ifndef SUB_CLUSTER_HPP
#define SUB_CLUSTER_HPP

class SubCluster {
    public:
        SubCluster(int size) : size(size) {
            initialize();
        }

        int size;
        int* elements;

        void finalize();
        void print();
        void print(int* visitedCustomersIndexes);

    private:
        void initialize();
};

#endif