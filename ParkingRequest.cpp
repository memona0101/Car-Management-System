#include "ParkingRequest.h"
#include <iostream>

int ParkingRequest::idCounter = 1;

ParkingRequest::ParkingRequest(std::string vId, int zoneId) 
    : vehicleId(vId), requestedZoneId(zoneId), assignedSlotId(-1), state(RequestState::REQUESTED), endTime(0) {
    requestId = idCounter++;
    requestTime = std::time(nullptr);
}

int ParkingRequest::getRequestId() const { return requestId; }
std::string ParkingRequest::getVehicleId() const { return vehicleId; }
int ParkingRequest::getRequestedZoneId() const { return requestedZoneId; }
int ParkingRequest::getAssignedSlotId() const { return assignedSlotId; }
RequestState ParkingRequest::getState() const { return state; }
time_t ParkingRequest::getRequestTime() const { return requestTime; }
time_t ParkingRequest::getEndTime() const { return endTime; }

double ParkingRequest::getDuration() const {
    if (endTime == 0) return 0;
    return difftime(endTime, requestTime);
}

void ParkingRequest::assignSlot(int slotId) {
    assignedSlotId = slotId;
}

void ParkingRequest::setEndTime(time_t time) {
    endTime = time;
}

bool ParkingRequest::transitionTo(RequestState newState) {
    bool valid = false;
    switch (state) {
        case RequestState::REQUESTED:
            if (newState == RequestState::ALLOCATED || newState == RequestState::CANCELLED) valid = true;
            break;
        case RequestState::ALLOCATED:
            if (newState == RequestState::OCCUPIED || newState == RequestState::CANCELLED) valid = true;
            break;
        case RequestState::OCCUPIED:
            if (newState == RequestState::RELEASED) valid = true;
            break;
        case RequestState::RELEASED:
            // Terminal state
            break;
        case RequestState::CANCELLED:
            // Terminal state
            break;
    }

    if (valid) {
        state = newState;
    }
    return valid;
}

void ParkingRequest::forceState(RequestState newState) {
    state = newState;
}

std::string ParkingRequest::getStateString() const {
    switch (state) {
        case RequestState::REQUESTED: return "REQUESTED";
        case RequestState::ALLOCATED: return "ALLOCATED";
        case RequestState::OCCUPIED: return "OCCUPIED";
        case RequestState::RELEASED: return "RELEASED";
        case RequestState::CANCELLED: return "CANCELLED";
        default: return "UNKNOWN";
    }
}
