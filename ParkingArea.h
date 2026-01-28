#ifndef PARKING_AREA_H
#define PARKING_AREA_H

#include <vector>
#include "ParkingSlot.h"

class ParkingArea {
private:
    int areaId;
    std::vector<ParkingSlot> slots;

public:
    ParkingArea(int id);
    
    void addSlot(const ParkingSlot& slot);
    const std::vector<ParkingSlot>& getSlots() const;
    std::vector<ParkingSlot>& getSlotsMutable(); // Helper for modification
    int getAreaId() const;
};

#endif // PARKING_AREA_H
