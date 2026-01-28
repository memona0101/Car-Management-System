#ifndef ZONE_H
#define ZONE_H

#include <vector>
#include "ParkingArea.h"

class Zone {
private:
    int zoneId;
    std::vector<ParkingArea> areas;
    std::vector<int> adjacentZoneIds; // Adjacency list by ID

public:
    Zone(int id);

    void addParkingArea(const ParkingArea& area);
    void addAdjacentZone(int neighborId);
    
    int getZoneId() const;
    const std::vector<ParkingArea>& getParkingAreas() const;
    std::vector<ParkingArea>& getParkingAreasMutable();
    const std::vector<int>& getAdjacentZones() const;
};

#endif // ZONE_H
