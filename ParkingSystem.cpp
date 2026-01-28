#include "ParkingSystem.h"
#include <iostream>
#include <iomanip>

ParkingSystem::ParkingSystem() {}

void ParkingSystem::addZone(const Zone& zone) {
    zones.push_back(zone);
}

const std::vector<Zone>& ParkingSystem::getZones() const {
    return zones;
}

const std::vector<ParkingRequest>& ParkingSystem::getRequests() const {
    return requests;
}

ParkingSlot* ParkingSystem::findSlotById(int slotId, int zoneId) {
    for (auto& z : zones) {
        if (z.getZoneId() == zoneId) {
            for (auto& area : z.getParkingAreasMutable()) {
                for (auto& slot : area.getSlotsMutable()) {
                    if (slot.getSlotId() == slotId) return &slot;
                }
            }
        }
    }
    return nullptr;
}

int ParkingSystem::requestParking(std::string vehicleId, int preferredZoneId) {
    ParkingRequest req(vehicleId, preferredZoneId);
    
    // Transition to ALLOCATED via Engine
    AllocationResult res = AllocationEngine::allocateSlot(preferredZoneId, zones);
    
    if (res.success) {
        req.transitionTo(RequestState::ALLOCATED);
        req.assignSlot(res.slotId);
        
        Operation op;
        op.type = Operation::ALLOCATE;
        op.requestId = req.getRequestId();
        op.slotId = res.slotId;
        op.zoneId = res.zoneId;
        rollbackManager.logOperation(op);
        
        std::cout << "[System] Vehicle " << vehicleId << " allocated to Slot " << res.slotId 
                  << " in Zone " << res.zoneId 
                  << (res.isCrossZone ? " (Cross-zone)" : "") << std::endl;
    } else {
        std::cout << "[System] Failed to allocate parking for Vehicle " << vehicleId << std::endl;
    }

    requests.push_back(req);
    return req.getRequestId();
}

bool ParkingSystem::cancelRequest(int requestId) {
    for (auto& req : requests) {
        if (req.getRequestId() == requestId) {
            if (req.getState() == RequestState::ALLOCATED || req.getState() == RequestState::REQUESTED) {
                 if (req.transitionTo(RequestState::CANCELLED)) {
                    // Start rollback logic specific to single cancel?
                    // "Cancelling a request must: Restore slot availability... Support rollback of last k operations"
                    // If we cancel *now*, it's an operation itself. 
                    // But if this is "undo", that's rollback.
                    // Let's treat "cancelRequest" as a new user action.
                    
                    if (req.getAssignedSlotId() != -1) {
                         // Find slot and release
                         // We need zone ID. We didn't store assigned Zone ID in Request directly, 
                         // but we can search or assuming we know it.
                         // Actually Request doesn't store assigned Zone, only requested. 
                         // To enable generic release, we might need to search or store AssignedZoneId in Request.
                         // Let's brute force find for now or check current zones.
                         // But we didn't store AssignedZone in Request class. 
                         // Let's assume we can search all zones for the SlotID if IDs are unique, or change Request to store it.
                         // For simplicity: Search global zones for slot ID.
                         // Better: In real app, SlotID should be unique or Tuple. 
                         // Our Slot class has ZoneID.
                         
                         // We need to look up which zone this slot belongs to.
                         // Iterating all zones...
                         bool found = false;
                         for(auto& z : zones) {
                             for(auto& a : z.getParkingAreasMutable()) {
                                 for(auto& s : a.getSlotsMutable()) {
                                     if(s.getSlotId() == req.getAssignedSlotId()) {
                                         s.release();
                                         found = true;
                                         
                                         Operation op;
                                         op.type = Operation::CANCEL;
                                         op.requestId = requestId;
                                         op.slotId = s.getSlotId();
                                         op.zoneId = z.getZoneId();
                                         rollbackManager.logOperation(op);
                                         break;
                                     }
                                 }
                                 if(found) break;
                             }
                             if(found) break;
                         }
                    }
                    std::cout << "[System] Request " << requestId << " Cancelled." << std::endl;
                    return true;
                 }
            }
        }
    }
    return false;
}

bool ParkingSystem::leaveParking(int requestId) {
    for (auto& req : requests) {
        if (req.getRequestId() == requestId) {
             long long duration = 0;
             // If ALLOCATED, move to OCCUPIED first (Assuming vehicle arrived)
             if (req.getState() == RequestState::ALLOCATED) {
                 req.transitionTo(RequestState::OCCUPIED);
             }

             if (req.getState() == RequestState::OCCUPIED) {
                 if (req.transitionTo(RequestState::RELEASED)) {
                     // Release slot logic...
                     if (req.getAssignedSlotId() != -1) {
                         bool found = false;
                         for(auto& z : zones) {
                             for(auto& a : z.getParkingAreasMutable()) {
                                 for(auto& s : a.getSlotsMutable()) {
                                     if(s.getSlotId() == req.getAssignedSlotId()) {
                                         s.release();
                                         found = true;
                                         break;
                                     }
                                 }
                                 if(found) break;
                             }
                             if(found) break;
                         }
                     }
                     req.setEndTime(std::time(nullptr));
                     std::cout << "[System] Vehicle " << req.getVehicleId() << " left parking. Duration: " << req.getDuration() << "s" << std::endl;
                     return true;
                 }
             }
        }
    }
    return false;
}

void ParkingSystem::rollbackOperations(int k) {
    std::vector<Operation> ops = rollbackManager.rollback(k);
    std::cout << "[Rollback] Rolling back " << ops.size() << " operations..." << std::endl;
    
    for (const auto& op : ops) {
        if (op.type == Operation::ALLOCATE) {
            // Undo Allocation -> Release Slot, set Request to REQUESTED
            
            // 1. Release slot
            ParkingSlot* s = findSlotById(op.slotId, op.zoneId);
            if (s) {
                s->release();
                std::cout << " -> Released Slot " << op.slotId << std::endl;
            }
            
            // 2. Revert Request state
            for (auto& req : requests) {
                if (req.getRequestId() == op.requestId) {
                    req.forceState(RequestState::REQUESTED);
                    req.assignSlot(-1); // Clear slot assignment
                    std::cout << " -> Reverted Request " << op.requestId << " to REQUESTED" << std::endl;
                }
            }
        } else if (op.type == Operation::CANCEL) {
            // Undo Cancel -> Re-occupy slot, set Request back to ALLOCATED
            ParkingSlot* s = findSlotById(op.slotId, op.zoneId);
            if (s) {
                s->occupy();
                std::cout << " -> Re-occupied Slot " << op.slotId << std::endl;
            }
            
            for (auto& req : requests) {
                 if (req.getRequestId() == op.requestId) {
                     req.forceState(RequestState::ALLOCATED);
                     std::cout << " -> Reverted Request " << op.requestId << " to ALLOCATED" << std::endl;
                 }
            }
        }
    }
}

void ParkingSystem::printAnalytics() const {
    int total = requests.size();
    int cancelled = 0;
    int completed = 0; // RELEASED
    double totalDuration = 0;
    
    // Zone Utilization
    std::cout << "\n--- Analytics ---\n";
    std::cout << "Zone Utilization:\n";
    int maxUsage = -1;
    int peakZoneId = -1;

    for (const auto& z : zones) {
        int capacity = 0;
        int occupied = 0;
        for (const auto& a : z.getParkingAreas()) {
             capacity += a.getSlots().size();
             for (const auto& s : a.getSlots()) {
                 if(s.isOccupied()) occupied++;
             }
        }
        double rate = capacity > 0 ? (double)occupied / capacity * 100.0 : 0.0;
        std::cout << "  Zone " << z.getZoneId() << ": " << std::fixed << std::setprecision(1) << rate << "% (" << occupied << "/" << capacity << ")\n";
        
        if (occupied > maxUsage) {
            maxUsage = occupied;
            peakZoneId = z.getZoneId();
        }
    }

    for(const auto& r : requests) {
        if(r.getState() == RequestState::CANCELLED) cancelled++;
        else if (r.getState() == RequestState::RELEASED) {
            completed++;
            totalDuration += r.getDuration();
        }
    }
    
    double avgDuration = completed > 0 ? totalDuration / completed : 0.0;

    std::cout << "\nGeneral Stats:\n";
    std::cout << "Total Requests: " << total << "\n";
    std::cout << "Cancelled: " << cancelled << "\n";
    std::cout << "Completed (Left): " << completed << "\n";
    std::cout << "Average Duration: " << avgDuration << " seconds\n";
    std::cout << "Peak Usage Zone: Zone " << peakZoneId << "\n";
    std::cout << "-----------------\n";
}
