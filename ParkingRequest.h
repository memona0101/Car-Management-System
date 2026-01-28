#ifndef PARKING_REQUEST_H
#define PARKING_REQUEST_H

#include <string>
#include <ctime>

enum class RequestState {
    REQUESTED,
    ALLOCATED,
    OCCUPIED,
    RELEASED,
    CANCELLED
};

class ParkingRequest {
private:
    static int idCounter;
    int requestId;
    std::string vehicleId;
    int requestedZoneId;
    int assignedSlotId; //-1 if not assigned
    time_t requestTime;
    time_t endTime; // For duration
    RequestState state;

public:
    ParkingRequest(std::string vId, int zoneId);

    int getRequestId() const;
    std::string getVehicleId() const;
    int getRequestedZoneId() const;
    int getAssignedSlotId() const;
    RequestState getState() const;
    time_t getRequestTime() const;
    time_t getEndTime() const;
    double getDuration() const; // In seconds

    void assignSlot(int slotId);
    void setEndTime(time_t time);
    bool transitionTo(RequestState newState); // Returns false if invalid
    void forceState(RequestState newState); // For rollback/admin use
    
    // Helper to print state string
    std::string getStateString() const;
};

#endif // PARKING_REQUEST_H
