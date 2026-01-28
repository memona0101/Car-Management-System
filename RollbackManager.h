#ifndef ROLLBACK_MANAGER_H
#define ROLLBACK_MANAGER_H

#include <stack>
#include <vector>
#include "ParkingRequest.h"
#include "ParkingSlot.h"

// Command pattern for operations
struct Operation {
    enum Type { ALLOCATE, CANCEL };
    Type type;
    int requestId;
    int slotId;
    int zoneId;
};

class RollbackManager {
private:
    std::stack<Operation> history;

public:
    void logOperation(Operation op);
    // Returns the list of operations that were undone, so the System can assume state.
    // However, the Manager might just tell the System *what* to undo.
    // Better design: Manager takes the System context but System is too large.
    // Let's return the Operations to be inverted by the System.
    std::vector<Operation> rollback(int k);
};

#endif // ROLLBACK_MANAGER_H
