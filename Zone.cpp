#include "Zone.h"

Zone::Zone(int id) : zoneId(id) {}

void Zone::addParkingArea(const ParkingArea& area) {
    areas.push_back(area);
}

void Zone::addAdjacentZone(int neighborId) {
    // Avoid duplicates if necessary, but for now simple push
    adjacentZoneIds.push_back(neighborId);
}

int Zone::getZoneId() const {
    return zoneId;
}

const std::vector<ParkingArea>& Zone::getParkingAreas() const {
    return areas;
}

std::vector<ParkingArea>& Zone::getParkingAreasMutable() {
    return areas;
}

const std::vector<int>& Zone::getAdjacentZones() const {
    return adjacentZoneIds;
}
