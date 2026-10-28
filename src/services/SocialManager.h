#ifndef SOCIAL_MANAGER_H
#define SOCIAL_MANAGER_H

#include <iostream>
#include <string>

#include "../datastructures/Graph.h"
#include "NotificationManager.h"
#include "UserManager.h"

// Connects UserManager lookups with the directed social graph.
class SocialManager {
private:
    UserManager& userManager;
    NotificationManager* notificationManager;
    Graph socialGraph;

    void displayUserSummary(const User* user) const {
        std::cout << user->name << " (" << user->username
                  << ", ID: " << user->userID << ")" << std::endl;
    }

    bool findPathDFS(int currentUserID, int targetUserID,
                     bool visited[], int path[], int& pathLength) const {
        visited[currentUserID] = true;
        path[pathLength] = currentUserID;
        pathLength++;

        if (currentUserID == targetUserID) {
            return true;
        }

        const AdjNode* current = socialGraph.getFollowingList(currentUserID);

        while (current != NULL) {
            if (!visited[current->userID] &&
                findPathDFS(current->userID, targetUserID, visited, path,
                            pathLength)) {
                return true;
            }

            current = current->next;
        }

        pathLength--;
        return false;
    }

public:
    // Kept for Phase 4-style use where notifications are not connected yet.
    SocialManager(UserManager& manager)
        : userManager(manager), notificationManager(NULL) {
    }

    SocialManager(UserManager& manager, NotificationManager& notifications)
        : userManager(manager), notificationManager(&notifications) {
    }

    // This ID-based form is also used by UndoManager. It does not print output.
    bool followUserByID(int followerID, int targetUserID,
                        bool createNotification) {
        User* follower = userManager.findUserByID(followerID);
        User* targetUser = userManager.findUserByID(targetUserID);

        if (follower == NULL || targetUser == NULL ||
            followerID == targetUserID ||
            !socialGraph.addEdge(followerID, targetUserID)) {
            return false;
        }

        if (createNotification && notificationManager != NULL) {
            notificationManager->addNotification(
                targetUserID, follower->username + " followed you.");
        }

        return true;
    }

    // This ID-based form is also used by UndoManager. Unfollowing has no notification.
    bool unfollowUserByID(int followerID, int targetUserID) {
        return socialGraph.removeEdge(followerID, targetUserID);
    }

    bool followUser(const std::string& targetUsername, int& followedUserID) {
        followedUserID = -1;
        User* currentUser = userManager.getCurrentUser();

        if (currentUser == NULL) {
            std::cout << "Please log in before following a user." << std::endl;
            return false;
        }

        User* targetUser = userManager.searchUser(targetUsername);

        if (targetUser == NULL) {
            std::cout << "User not found." << std::endl;
        } else if (targetUser->userID == currentUser->userID) {
            std::cout << "You cannot follow yourself." << std::endl;
        } else if (socialGraph.hasEdge(currentUser->userID, targetUser->userID)) {
            std::cout << "You already follow " << targetUser->username << "."
                      << std::endl;
        } else if (followUserByID(currentUser->userID, targetUser->userID, true)) {
            followedUserID = targetUser->userID;
            std::cout << "You are now following " << targetUser->username << "."
                      << std::endl;
            return true;
        }

        return false;
    }

    // Compatibility wrapper for the Phase 4 menu-style call.
    void followUser(const std::string& targetUsername) {
        int followedUserID = -1;
        followUser(targetUsername, followedUserID);
    }

    bool unfollowUser(const std::string& targetUsername, int& unfollowedUserID) {
        unfollowedUserID = -1;
        User* currentUser = userManager.getCurrentUser();

        if (currentUser == NULL) {
            std::cout << "Please log in before unfollowing a user." << std::endl;
            return false;
        }

        User* targetUser = userManager.searchUser(targetUsername);

        if (targetUser == NULL) {
            std::cout << "User not found." << std::endl;
        } else if (unfollowUserByID(currentUser->userID, targetUser->userID)) {
            unfollowedUserID = targetUser->userID;
            std::cout << "You unfollowed " << targetUser->username << "."
                      << std::endl;
            return true;
        } else {
            std::cout << "You do not follow " << targetUser->username << "."
                      << std::endl;
        }

        return false;
    }

    // Compatibility wrapper for the Phase 4 menu-style call.
    void unfollowUser(const std::string& targetUsername) {
        int unfollowedUserID = -1;
        unfollowUser(targetUsername, unfollowedUserID);
    }

    void displayFollowing() const {
        User* currentUser = userManager.getCurrentUser();

        if (currentUser == NULL) {
            std::cout << "Please log in to view following users." << std::endl;
            return;
        }

        const AdjNode* current = socialGraph.getFollowingList(currentUser->userID);

        if (current == NULL) {
            std::cout << "You are not following any users." << std::endl;
            return;
        }

        std::cout << "Following (" << socialGraph.getFollowingCount(currentUser->userID)
                  << "):" << std::endl;

        while (current != NULL) {
            User* followedUser = getUserByID(current->userID);

            if (followedUser != NULL) {
                displayUserSummary(followedUser);
            }

            current = current->next;
        }
    }

    void displayFollowers() const {
        User* currentUser = userManager.getCurrentUser();

        if (currentUser == NULL) {
            std::cout << "Please log in to view followers." << std::endl;
            return;
        }

        int followerCount = socialGraph.getFollowerCount(currentUser->userID);

        if (followerCount == 0) {
            std::cout << "You have no followers." << std::endl;
            return;
        }

        std::cout << "Followers (" << followerCount << "):" << std::endl;

        for (int userID = 1; userID <= Graph::MAX_USERS; userID++) {
            if (socialGraph.hasEdge(userID, currentUser->userID)) {
                User* follower = getUserByID(userID);

                if (follower != NULL) {
                    displayUserSummary(follower);
                }
            }
        }
    }

    void displayRecommendations() const {
        User* currentUser = userManager.getCurrentUser();

        if (currentUser == NULL) {
            std::cout << "Please log in to view user recommendations." << std::endl;
            return;
        }

        bool directlyFollowed[Graph::MAX_USERS + 1] = {false};
        int distance[Graph::MAX_USERS + 1];
        int bfsQueue[Graph::MAX_USERS + 1];

        for (int i = 0; i <= Graph::MAX_USERS; i++) {
            distance[i] = -1;
        }

        const AdjNode* directUser = socialGraph.getFollowingList(currentUser->userID);

        while (directUser != NULL) {
            directlyFollowed[directUser->userID] = true;
            directUser = directUser->next;
        }

        int front = 0;
        int rear = 0;
        bfsQueue[rear] = currentUser->userID;
        rear++;
        distance[currentUser->userID] = 0;

        while (front < rear) {
            int userID = bfsQueue[front];
            front++;

            const AdjNode* neighbor = socialGraph.getFollowingList(userID);

            while (neighbor != NULL) {
                if (distance[neighbor->userID] == -1) {
                    distance[neighbor->userID] = distance[userID] + 1;
                    bfsQueue[rear] = neighbor->userID;
                    rear++;
                }

                neighbor = neighbor->next;
            }
        }

        bool foundRecommendation = false;
        std::cout << "User recommendations:" << std::endl;

        for (int userID = 1; userID <= Graph::MAX_USERS; userID++) {
            if (distance[userID] == 2 && !directlyFollowed[userID] &&
                userID != currentUser->userID) {
                User* recommendedUser = getUserByID(userID);

                if (recommendedUser != NULL) {
                    displayUserSummary(recommendedUser);
                    foundRecommendation = true;
                }
            }
        }

        if (!foundRecommendation) {
            std::cout << "No distance-2 user recommendations found." << std::endl;
        }
    }

    void findConnectionPath(const std::string& targetUsername) const {
        User* currentUser = userManager.getCurrentUser();

        if (currentUser == NULL) {
            std::cout << "Please log in before finding a connection path." << std::endl;
            return;
        }

        User* targetUser = userManager.searchUser(targetUsername);

        if (targetUser == NULL) {
            std::cout << "User not found." << std::endl;
            return;
        }

        if (targetUser->userID == currentUser->userID) {
            std::cout << "You are already the selected user." << std::endl;
            return;
        }

        bool visited[Graph::MAX_USERS + 1] = {false};
        int path[Graph::MAX_USERS + 1];
        int pathLength = 0;

        if (!findPathDFS(currentUser->userID, targetUser->userID, visited, path,
                         pathLength)) {
            std::cout << "No connection path found." << std::endl;
            return;
        }

        std::cout << "Connection path: ";

        for (int i = 0; i < pathLength; i++) {
            User* pathUser = getUserByID(path[i]);

            if (pathUser != NULL) {
                std::cout << pathUser->username;
            } else {
                std::cout << "User " << path[i];
            }

            if (i < pathLength - 1) {
                std::cout << " -> ";
            }
        }

        std::cout << std::endl;
    }

    // Used by FeedManager to consult the existing directed Graph directly.
    bool isFollowing(int followerID, int followedUserID) const {
        return socialGraph.hasEdge(followerID, followedUserID);
    }

    // Restores a saved relationship without generating a notification.
    bool restoreFollow(int followerID, int followedUserID) {
        return followUserByID(followerID, followedUserID, false);
    }

    // Read-only graph traversal access used when saving relationships.
    const AdjNode* getFollowingListForUser(int userID) const {
        return socialGraph.getFollowingList(userID);
    }

private:
    User* getUserByID(int userID) const {
        return userManager.findUserByID(userID);
    }
};

#endif
