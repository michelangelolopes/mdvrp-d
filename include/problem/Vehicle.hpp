#ifndef VEHICLE_HPP
#define VEHICLE_HPP

class Vehicle {
    public:
        double speed;
        double capacity;
        double serviceTime;

        inline bool checkWeightConstraint(double demand) const {
            return (capacity >= demand);
        }
};

#endif