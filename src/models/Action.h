#ifndef ACTION_H
#define ACTION_H

#include <string>

class Action {
public:
    std::string actionType;
    int userID;
    int targetID;

    Action(const std::string& type, int user, int target)
        : actionType(type), userID(user), targetID(target) {
    }
};

#endif
