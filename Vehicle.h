#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>

class Vehicle {
private:
    std::string vehicleId;
    int preferredZoneId;

public:
    Vehicle(std::string id, int zoneId);
    
    std::string getVehicleId() const;
    int getPreferredZoneId() const;
};

#endif // VEHICLE_H
