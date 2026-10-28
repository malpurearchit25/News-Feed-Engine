#ifndef UNDO_MANAGER_H
#define UNDO_MANAGER_H

#include <iostream>
#include <string>

#include "../datastructures/ActionStack.h"
#include "InteractionManager.h"
#include "PostManager.h"
#include "SocialManager.h"
#include "UserManager.h"

// Every user has a separate stack so one user's undo never changes another user's work.
class UndoManager {
public:
    static const int MAX_USERS = 101;

private:
    UserManager& userManager;
    SocialManager& socialManager;
    InteractionManager& interactionManager;
    PostManager& postManager;
    ActionStack actionStacks[MAX_USERS + 1];

    bool isValidUserID(int userID) const {
        return userID >= 1 && userID <= MAX_USERS;
    }

public:
    UndoManager(UserManager& users, SocialManager& social,
                InteractionManager& interactions, PostManager& posts)
        : userManager(users), socialManager(social), interactionManager(interactions),
          postManager(posts) {
    }

    void pushAction(int userID, const std::string& actionType, int targetID) {
        if (!isValidUserID(userID)) {
            return;
        }

        Action action(actionType, userID, targetID);
        actionStacks[userID].push(action);
    }

    bool undoLastAction() {
        User* currentUser = userManager.getCurrentUser();

        if (currentUser == NULL) {
            std::cout << "Please log in before undoing an action." << std::endl;
            return false;
        }

        int userID = currentUser->userID;

        if (!isValidUserID(userID)) {
            return false;
        }

        Action action("", -1, -1);

        if (!actionStacks[userID].pop(action)) {
            std::cout << "No actions to undo." << std::endl;
            return false;
        }

        bool undone = false;

        if (action.actionType == "FOLLOW") {
            undone = socialManager.unfollowUserByID(action.userID, action.targetID);
        } else if (action.actionType == "UNFOLLOW") {
            // Undo is a reversal, not a normal follow event, so no notification is made.
            undone = socialManager.followUserByID(action.userID, action.targetID, false);
        } else if (action.actionType == "LIKE") {
            undone = interactionManager.unlikePost(action.userID, action.targetID) ==
                     InteractionManager::SUCCESS;
        } else if (action.actionType == "UNLIKE") {
            undone = interactionManager.likePost(action.userID, action.targetID) ==
                     InteractionManager::SUCCESS;
        } else if (action.actionType == "COMMENT") {
            undone = interactionManager.deleteComment(action.userID, action.targetID) ==
                     InteractionManager::SUCCESS;
        } else if (action.actionType == "CREATE_POST") {
            undone = postManager.deletePost(action.targetID, action.userID);

            if (undone) {
                interactionManager.removeDataForPost(action.targetID);
            }
        }

        if (undone) {
            std::cout << "Undid " << action.actionType << "." << std::endl;
        } else {
            std::cout << "Could not undo " << action.actionType
                      << " because the related item no longer has the expected state."
                      << std::endl;
        }

        // Inverse actions are intentionally never pushed onto this stack.
        return undone;
    }
};

#endif
