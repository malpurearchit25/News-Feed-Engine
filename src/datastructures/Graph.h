#ifndef GRAPH_H
#define GRAPH_H

// One adjacency-list node represents one directed follow relationship.
class AdjNode {
public:
    int userID;
    AdjNode* next;

    AdjNode(int id) : userID(id), next(NULL) {
    }
};

// A directed graph where A -> B means that user A follows user B.
class Graph {
public:
    // User IDs begin at 1. Index 0 is kept unused for simpler indexing.
    static const int MAX_USERS = 101;

private:
    AdjNode* adjacency[MAX_USERS + 1];

    bool isValidUserID(int userID) const {
        return userID >= 1 && userID <= MAX_USERS;
    }

public:
    Graph() {
        for (int i = 0; i <= MAX_USERS; i++) {
            adjacency[i] = NULL;
        }
    }

    ~Graph() {
        for (int i = 1; i <= MAX_USERS; i++) {
            AdjNode* current = adjacency[i];

            while (current != NULL) {
                AdjNode* nextNode = current->next;
                delete current;
                current = nextNode;
            }

            adjacency[i] = NULL;
        }
    }

    // Adds a directed edge. Duplicate edges and self-edges are rejected.
    bool addEdge(int fromUserID, int toUserID) {
        if (!isValidUserID(fromUserID) || !isValidUserID(toUserID) ||
            fromUserID == toUserID || hasEdge(fromUserID, toUserID)) {
            return false;
        }

        AdjNode* newNode = new AdjNode(toUserID);
        newNode->next = adjacency[fromUserID];
        adjacency[fromUserID] = newNode;
        return true;
    }

    // Removes the directed edge and deletes its adjacency-list node.
    bool removeEdge(int fromUserID, int toUserID) {
        if (!isValidUserID(fromUserID) || !isValidUserID(toUserID)) {
            return false;
        }

        AdjNode* current = adjacency[fromUserID];
        AdjNode* previous = NULL;

        while (current != NULL && current->userID != toUserID) {
            previous = current;
            current = current->next;
        }

        if (current == NULL) {
            return false;
        }

        if (previous == NULL) {
            adjacency[fromUserID] = current->next;
        } else {
            previous->next = current->next;
        }

        delete current;
        return true;
    }

    bool hasEdge(int fromUserID, int toUserID) const {
        if (!isValidUserID(fromUserID) || !isValidUserID(toUserID)) {
            return false;
        }

        const AdjNode* current = adjacency[fromUserID];

        while (current != NULL) {
            if (current->userID == toUserID) {
                return true;
            }

            current = current->next;
        }

        return false;
    }

    const AdjNode* getFollowingList(int userID) const {
        if (!isValidUserID(userID)) {
            return NULL;
        }

        return adjacency[userID];
    }

    int getFollowingCount(int userID) const {
        if (!isValidUserID(userID)) {
            return 0;
        }

        int count = 0;
        const AdjNode* current = adjacency[userID];

        while (current != NULL) {
            count++;
            current = current->next;
        }

        return count;
    }

    int getFollowerCount(int userID) const {
        if (!isValidUserID(userID)) {
            return 0;
        }

        int count = 0;

        for (int i = 1; i <= MAX_USERS; i++) {
            if (hasEdge(i, userID)) {
                count++;
            }
        }

        return count;
    }
};

#endif
