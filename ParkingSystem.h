#ifndef PARKING_SYSTEM_H
#define PARKING_SYSTEM_H

#include <vector>
#include <string>
#include "Zone.h"
#include "ParkingRequest.h"
#include "AllocationEngine.h"
#include "RollbackManager.h"

class ParkingSystem {
private:
    std::vector<Zone> zones;
    std::vector<ParkingRequest> requests;
    RollbackManager rollbackManager;

    ParkingSlot* findSlotById(int slotId, int zoneId);

public:
    ParkingSystem();

    void addZone(const Zone& zone);
    
    // Core capabilities
    int requestParking(std::string vehicleId, int preferredZoneId); // Returns requestId
    bool leaveParking(int requestId); // New: Complete the lifecycle
    bool cancelRequest(int requestId);
    void rollbackOperations(int k);
    
    // Getters for API/GUI
    const std::vector<Zone>& getZones() const;
    const std::vector<ParkingRequest>& getRequests() const;
    
    // Analytics
    void printAnalytics() const;
    void printSystemStatus() const;
};

#endif // PARKING_SYSTEM_H
