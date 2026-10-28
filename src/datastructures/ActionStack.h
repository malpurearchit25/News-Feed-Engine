#ifndef ACTION_STACK_H
#define ACTION_STACK_H

#include <iostream>

#include "../models/Action.h"

// One node stores one undoable action in the manually managed LIFO stack.
class ActionStackNode {
public:
    Action data;
    ActionStackNode* next;

    ActionStackNode(const Action& action) : data(action), next(NULL) {
    }
};

// The top node is always the most recently performed action.
class ActionStack {
private:
    ActionStackNode* top;
    int actionCount;

public:
    ActionStack() : top(NULL), actionCount(0) {
    }

    ~ActionStack() {
        ActionStackNode* current = top;

        while (current != NULL) {
            ActionStackNode* nextNode = current->next;
            delete current;
            current = nextNode;
        }

        top = NULL;
        actionCount = 0;
    }

    // Push is O(1): insert a new node at the top.
    void push(const Action& action) {
        ActionStackNode* newNode = new ActionStackNode(action);
        newNode->next = top;
        top = newNode;
        actionCount++;
    }

    // Pop is O(1): remove and delete the top node.
    bool pop(Action& action) {
        if (top == NULL) {
            return false;
        }

        ActionStackNode* removedNode = top;
        action = removedNode->data;
        top = top->next;
        delete removedNode;
        actionCount--;
        return true;
    }

    // Peek is O(1) and does not remove the top action.
    const Action* peek() const {
        if (top == NULL) {
            return NULL;
        }

        return &top->data;
    }

    bool isEmpty() const {
        return top == NULL;
    }

    int getCount() const {
        return actionCount;
    }

    void display() const {
        if (isEmpty()) {
            std::cout << "No undoable actions." << std::endl;
            return;
        }

        const ActionStackNode* current = top;
        int number = 1;

        while (current != NULL) {
            std::cout << number << ". " << current->data.actionType
                      << " (target ID: " << current->data.targetID << ")"
                      << std::endl;
            current = current->next;
            number++;
        }
    }
};

#endif
