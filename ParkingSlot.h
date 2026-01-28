#ifndef PARKING_SLOT_H
#define PARKING_SLOT_H

#include <string>

class ParkingSlot {
private:
    int slotId;
    int zoneId;
    bool occupied;

public:
    ParkingSlot(int sId, int zId);

    int getSlotId() const;
    int getZoneId() const;
    bool isOccupied() const;
    
    void occupy();
    void release();
};

#endif // PARKING_SLOT_H
