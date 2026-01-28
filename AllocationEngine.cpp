#include "AllocationEngine.h"
#include <iostream>

AllocationResult AllocationEngine::allocateSlot(int requestedZoneId, std::vector<Zone>& zones) {
    AllocationResult result = {false, -1, -1, false};
    Zone* targetZone = nullptr;

    // 1. Find the requested zone object
    for (auto& z : zones) {
        if (z.getZoneId() == requestedZoneId) {
            targetZone = &z;
            break;
        }
    }

    if (!targetZone) {
        // Requested zone does not exist
        return result; 
    }

    // 2. Try to find slot in requested zone
    for (auto& area : targetZone->getParkingAreasMutable()) {
        for (auto& slot : area.getSlotsMutable()) {
            if (!slot.isOccupied()) {
                slot.occupy(); // Mark as occupied temporarily (reservation) or caller does it?
                               // Ideally Engine just finds it, but usually allocation "reserves" it.
                               // Requirement: "Allocate parking slots". Let's mark it here.
                result.success = true;
                result.slotId = slot.getSlotId();
                result.zoneId = requestedZoneId;
                return result;
            }
        }
    }

    // 3. If full, check neighbors (Cross-zone)
    // Constraint: "Cross-zone allocation ... incurs extra cost/penalty" 
    // We just find a slot here.
    const std::vector<int>& neighbors = targetZone->getAdjacentZones();
    for (int neighborId : neighbors) {
        for (auto& z : zones) {
            if (z.getZoneId() == neighborId) {
                // Search this neighbor
                for (auto& area : z.getParkingAreasMutable()) {
                    for (auto& slot : area.getSlotsMutable()) {
                        if (!slot.isOccupied()) {
                            slot.occupy();
                            result.success = true;
                            result.slotId = slot.getSlotId();
                            result.zoneId = neighborId;
                            result.isCrossZone = true;
                            return result;
                        }
                    }
                }
            }
        }
    }

    // 4. Failed to allocate
    return result;
}
