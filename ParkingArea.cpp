#include "ParkingArea.h"

ParkingArea::ParkingArea(int id) : areaId(id) {}

void ParkingArea::addSlot(const ParkingSlot& slot) {
    slots.push_back(slot);
}

const std::vector<ParkingSlot>& ParkingArea::getSlots() const {
    return slots;
}

std::vector<ParkingSlot>& ParkingArea::getSlotsMutable() {
    return slots;
}

int ParkingArea::getAreaId() const {
    return areaId;
}
