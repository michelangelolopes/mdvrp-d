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
        void print() const;
        void print(int* visitedCustomersIndexes) const;

    private:
        void initialize();
};

#endif