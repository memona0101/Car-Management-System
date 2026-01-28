#include "ParkingSlot.h"

ParkingSlot::ParkingSlot(int sId, int zId) : slotId(sId), zoneId(zId), occupied(false) {}

int ParkingSlot::getSlotId() const {
    return slotId;
}

int ParkingSlot::getZoneId() const {
    return zoneId;
}

bool ParkingSlot::isOccupied() const {
    return occupied;
}

void ParkingSlot::occupy() {
    occupied = true;
}

void ParkingSlot::release() {
    occupied = false;
}
