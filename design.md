# Design Document: Smart Parking Allocation & Zone Management System

## Design Overview
The system is designed as a modular solution to simulate a city's parking management. It uses a clean separation of concerns:
- **Entities**: Passive data holders like `Vehicle`, `ParkingSlot`, `ParkingArea`, `Zone`.
- **Logic**: Active managers like `AllocationEngine` (strategy), `RollbackManager` (undo), `ParkingSystem` (facade).

## Data Structure Choices
- **Arrays/Vectors (`std::vector`)**: Used for storing Lists of Zones, Areas, and Slots. Chosen for O(1) random access and cache locality, fulfilling the "understanding of arrays" objective.
- **Adjancency List (in `Zone`)**: Implemented as a `vector<int>` of neighbor IDs. This models the graph connectivity explicitly without using complex STL Graph libraries.
- **Stack (`RollbackManager`)**: Used `std::stack` for the undo history, perfectly matching the LIFO requirement for rolling back `k` operations.

## Allocation Strategy (`AllocationEngine`)
1. **Prefer Same Zone**: Iterate through all areas in the requested Zone. Return first free slot.
2. **Cross-Zone**: If failed, iterate through the adjacency list of the requested Zone. Check each neighbor Zone for free slots.
3. **Failure**: If both fail, return failure. 

## Request Lifecycle (State Machine)
`ParkingRequest` strictly enforces state transitions:
- `REQUESTED` -> `ALLOCATED` or `CANCELLED`
- `ALLOCATED` -> `OCCUPIED` or `CANCELLED`
- `OCCUPIED` -> `RELEASED`

Invalid transitions return `false`. A special `forceState` was added to support Rollback operations that need to revert state against the natural flow.

## Rollback Design
- **Command Pattern**: Every state-changing action (Allocate, Cancel) is logged as an `Operation` struct containing the type and parameters (Slot ID, Request ID).
- **Undo Logic**:
    - **Undo Allocate**: Releases the slot and resets Request to `REQUESTED`.
    - **Undo Cancel**: Re-occupies the slot and resets Request to `ALLOCATED`.

## Complexity
- **Time**: 
    - Allocation: O(Slots in Zone + Slots in Neighbors). In worst case O(Total Slots) if highly connected.
    - Rollback: O(k).
- **Space**: O(N) where N is number of slots/requests.
