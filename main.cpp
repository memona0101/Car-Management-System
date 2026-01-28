#include <iostream>
#include <vector>
#include <cassert>
#include "ParkingSystem.h"
#include "Zone.h"
#include "ParkingArea.h"
#include "ParkingSlot.h"

// Helper to setup a small city
ParkingSystem setupCity() {
    ParkingSystem ps;

    // Zone 1: 1 Area, 2 Slots (ID 1, 2)
    Zone z1(1);
    ParkingArea a1(101);
    a1.addSlot(ParkingSlot(1, 1));
    a1.addSlot(ParkingSlot(2, 1));
    z1.addParkingArea(a1);
    z1.addAdjacentZone(2); // Connected to Zone 2

    // Zone 2: 1 Area, 1 Slot (ID 3)
    Zone z2(2);
    ParkingArea a2(201);
    a2.addSlot(ParkingSlot(3, 2));
    z2.addParkingArea(a2);
    z2.addAdjacentZone(1); // Connected to Zone 1

    // Zone 3: Isolated, 1 Slot (ID 4)
    Zone z3(3);
    ParkingArea a3(301);
    a3.addSlot(ParkingSlot(4, 3));
    z3.addParkingArea(a3);

    ps.addZone(z1);
    ps.addZone(z2);
    ps.addZone(z3);

    return ps;
}

void runTests() {
    std::cout << "Starting Test Suite..." << std::endl;
    ParkingSystem ps = setupCity();

    // Test 1: Normal Allocation (Zone 1)
    std::cout << "\nTest 1: Normal Allocation (Zone 1)\n";
    int r1 = ps.requestParking("V1", 1); 
    // Expect Slot 1 or 2
    
    // Test 2: Second Allocation (Zone 1)
    std::cout << "\nTest 2: Second Allocation (Zone 1)\n";
    int r2 = ps.requestParking("V2", 1);
    // Expect Slot 1 or 2 (whichever wasn't taken)
    
    // Test 3: Zone 1 Full, Cross-Zone Allocation to Zone 2
    std::cout << "\nTest 3: Cross-Zone Allocation (Zone 1 -> Zone 2)\n";
    int r3 = ps.requestParking("V3", 1);
    // Should go to Zone 2 (neighbor), Slot 3

    // Test 4: System Full for Connected Group (Request Zone 1, Zone 2 also full)
    std::cout << "\nTest 4: Allocation Failure (Zone 1 & 2 Full)\n";
    int r4 = ps.requestParking("V4", 1);
    // Should fail (print failure)

    // Test 5: Isolated Zone Allocation
    std::cout << "\nTest 5: Isolated Zone Allocation\n";
    int r5 = ps.requestParking("V5", 3);
    // Should get Slot 4

    // Test 6: Cancellation
    std::cout << "\nTest 6: Cancellation of Request 2\n";
    bool c6 = ps.cancelRequest(r2);
    assert(c6 == true);
    // Slot from r2 should be free now (Zone 1 has 1 free slot)

    // Test 7: Re-allocation after Cancellation
    std::cout << "\nTest 7: Re-allocation to Zone 1\n";
    int r7 = ps.requestParking("V6", 1);
    // Should succeed in Zone 1 now

    // Test 8: Rollback Last Operation (Undo r7 allocation)
    std::cout << "\nTest 8: Rollback Last Allocation\n";
    ps.rollbackOperations(1);
    // V6 allocation undone. Zone 1 has 1 free slot again.

    // Test 9: Rollback Cancellation (Undo r2 cancellation)
    // Note: r2 was Cancelled. This added a CANCEL op. 
    // Wait, did we add CANCEL op to history? Yes in cancelRequest.
    // Ops history: ... [ALLOC V1] [ALLOC V2] [ALLOC V3] [FAIL V4] [ALLOC V5] [CANCEL V2] [ALLOC V6]
    // After Test 8 rollback: [ALLOC V6] is gone. Top is [CANCEL V2].
    std::cout << "\nTest 9: Rollback Cancellation\n";
    ps.rollbackOperations(1); 
    // Undo Cancel of V2. V2 should be ALLOCATED again. Slot taken. Zone 1 Full again.

    // Test 10: Verify Zone 1 Full Again
    std::cout << "\nTest 10: Verify Zone 1 Full Again\n";
    int r10 = ps.requestParking("V7", 1);
    // Should go to Zone 2? Wait, Zone 2 is full (V3).
    // Zone 1 is full (V1, V2).
    // Should fail or go somewhere else? Zone 1 neighbors Zone 2. Zone 2 is full.
    // Should fail.
    
    // Test 11: Leave Parking (Duration analytic)
    std::cout << "\nTest 11: V1 Leaves Parking\n";
    ps.leaveParking(r1);
    
    ps.printAnalytics();
}

int main() {
    runTests();
    return 0;
}
