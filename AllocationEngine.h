#ifndef ALLOCATION_ENGINE_H
#define ALLOCATION_ENGINE_H

#include <vector>
#include "Zone.h"
#include "ParkingSlot.h"

struct AllocationResult {
    bool success;
    int slotId;
    int zoneId; // Could be different from requested if cross-zone
    bool isCrossZone;
};

class AllocationEngine {
public:
    // Tries to allocate a slot for the given zone preference in the list of all Zones
    // Returns AllocationResult
    static AllocationResult allocateSlot(int requestedZoneId, std::vector<Zone>& zones);
};

#endif // ALLOCATION_ENGINE_H
