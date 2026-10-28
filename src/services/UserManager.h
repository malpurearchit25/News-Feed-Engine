#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <string>

#include "../datastructures/HashTable.h"

class UserManager {
private:
    HashTable users;
    int nextUserID;
    int currentLoggedInUserID;

public:
    static const int MAX_USERS = 101;

    UserManager() : nextUserID(1), currentLoggedInUserID(-1) {
    }

    bool registerUser(const std::string& username, const std::string& password,
                      const std::string& name) {
        if (username.empty() || password.empty() || name.empty() ||
            username.find('|') != std::string::npos ||
            password.find('|') != std::string::npos ||
            name.find('|') != std::string::npos ||
            users.usernameExists(username) || isTableFull()) {
            return false;
        }

        User* newUser = new User(nextUserID, username, password, name);

        if (!users.insert(newUser)) {
            delete newUser;
            return false;
        }

        nextUserID++;
        return true;
    }

    // Restores an existing user without assigning a new ID or logging anyone in.
    bool restoreUser(int userID, const std::string& username,
                     const std::string& password, const std::string& name) {
        if (userID < 1 || userID > MAX_USERS || username.empty() ||
            password.empty() || name.empty() || users.usernameExists(username) ||
            users.findByUserID(userID) != NULL || users.isFull()) {
            return false;
        }

        User* restoredUser = new User(userID, username, password, name);

        if (!users.insert(restoredUser)) {
            delete restoredUser;
            return false;
        }

        if (userID >= nextUserID) {
            nextUserID = userID + 1;
        }

        return true;
    }

    User* searchUser(const std::string& username) const {
        return users.findByUsername(username);
    }

    User* findUserByID(int userID) const {
        return users.findByUserID(userID);
    }

    bool usernameExists(const std::string& username) const {
        return users.usernameExists(username);
    }

    bool isTableFull() const {
        // The Graph and per-user managers support IDs 1 through MAX_USERS.
        // This also keeps a malformed sparse save file from producing ID 102.
        return users.isFull() || nextUserID > MAX_USERS;
    }

    bool isLoggedIn() const {
        return currentLoggedInUserID != -1;
    }

    bool login(const std::string& username, const std::string& password) {
        if (isLoggedIn()) {
            return false;
        }

        User* user = users.findByUsername(username);

        if (user == NULL || user->password != password) {
            return false;
        }

        currentLoggedInUserID = user->userID;
        return true;
    }

    bool logout() {
        if (!isLoggedIn()) {
            return false;
        }

        currentLoggedInUserID = -1;
        return true;
    }

    User* getCurrentUser() const {
        if (!isLoggedIn()) {
            return NULL;
        }

        return users.findByUserID(currentLoggedInUserID);
    }
};

#endif
