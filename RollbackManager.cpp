#include "RollbackManager.h"

void RollbackManager::logOperation(Operation op) {
    history.push(op);
}

std::vector<Operation> RollbackManager::rollback(int k) {
    std::vector<Operation> opsToUndo;
    while (k > 0 && !history.empty()) {
        opsToUndo.push_back(history.top());
        history.pop();
        k--;
    }
    return opsToUndo;
}
