#include "Vehicle.h"

Vehicle::Vehicle(std::string id, int zoneId) : vehicleId(id), preferredZoneId(zoneId) {}

std::string Vehicle::getVehicleId() const {
    return vehicleId;
}

int Vehicle::getPreferredZoneId() const {
    return preferredZoneId;
}
