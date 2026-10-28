#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <string>

#include "../models/User.h"

// This table owns the User objects that are inserted into it.
class HashTable {
private:
    static const int TABLE_SIZE = 101;
    User* table[TABLE_SIZE];
    int userCount;

    // Adds byte values and keeps the result inside the table range.
    int hash(const std::string& username) const {
        int total = 0;

        for (int i = 0; i < static_cast<int>(username.length()); i++) {
            // unsigned char prevents non-ASCII input bytes from making the
            // array index negative. Reducing each step also avoids overflow.
            total = (total + static_cast<unsigned char>(username[i])) % TABLE_SIZE;
        }

        return total;
    }

public:
    HashTable() : userCount(0) {
        for (int i = 0; i < TABLE_SIZE; i++) {
            table[i] = NULL;
        }
    }

    ~HashTable() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            delete table[i];
        }
    }

    bool isFull() const {
        return userCount == TABLE_SIZE;
    }

    // Inserts by linear probing: after a collision, try the next array slot.
    bool insert(User* user) {
        if (user == NULL || isFull() || findByUsername(user->username) != NULL) {
            return false;
        }

        int index = hash(user->username);

        for (int attempts = 0; attempts < TABLE_SIZE; attempts++) {
            if (table[index] == NULL) {
                table[index] = user;
                userCount++;
                return true;
            }

            index = (index + 1) % TABLE_SIZE;
        }

        return false;
    }

    User* findByUsername(const std::string& username) const {
        int index = hash(username);

        // Search using the same probe sequence used during insertion.
        for (int attempts = 0; attempts < TABLE_SIZE; attempts++) {
            if (table[index] == NULL) {
                return NULL;
            }

            if (table[index]->username == username) {
                return table[index];
            }

            index = (index + 1) % TABLE_SIZE;
        }

        return NULL;
    }

    // User IDs are sequential, so this simple scan is sufficient for now.
    User* findByUserID(int userID) const {
        for (int i = 0; i < TABLE_SIZE; i++) {
            if (table[i] != NULL && table[i]->userID == userID) {
                return table[i];
            }
        }

        return NULL;
    }

    bool usernameExists(const std::string& username) const {
        return findByUsername(username) != NULL;
    }
};

#endif
