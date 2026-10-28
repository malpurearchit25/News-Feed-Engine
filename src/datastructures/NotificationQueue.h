#ifndef NOTIFICATION_QUEUE_H
#define NOTIFICATION_QUEUE_H

#include <iostream>

#include "../models/Notification.h"

// One node stores one notification in the manually managed FIFO queue.
class NotificationQueueNode {
public:
    Notification data;
    NotificationQueueNode* next;

    NotificationQueueNode(const Notification& notification)
        : data(notification), next(NULL) {
    }
};

// Notifications are added at the rear and processed from the front.
class NotificationQueue {
private:
    NotificationQueueNode* front;
    NotificationQueueNode* rear;
    int notificationCount;

public:
    NotificationQueue() : front(NULL), rear(NULL), notificationCount(0) {
    }

    ~NotificationQueue() {
        NotificationQueueNode* current = front;

        while (current != NULL) {
            NotificationQueueNode* nextNode = current->next;
            delete current;
            current = nextNode;
        }

        front = NULL;
        rear = NULL;
        notificationCount = 0;
    }

    // Enqueue is O(1): the new node is linked after the rear node.
    void enqueue(const Notification& notification) {
        NotificationQueueNode* newNode = new NotificationQueueNode(notification);

        if (rear == NULL) {
            front = newNode;
            rear = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }

        notificationCount++;
    }

    // Dequeue is O(1): remove and delete the front node.
    bool dequeue(Notification& notification) {
        if (front == NULL) {
            return false;
        }

        NotificationQueueNode* removedNode = front;
        notification = removedNode->data;
        front = front->next;

        if (front == NULL) {
            rear = NULL;
        }

        delete removedNode;
        notificationCount--;
        return true;
    }

    // Peek is O(1) and does not remove the front notification.
    const Notification* peek() const {
        if (front == NULL) {
            return NULL;
        }

        return &front->data;
    }

    bool isEmpty() const {
        return front == NULL;
    }

    int getCount() const {
        return notificationCount;
    }

    void display() const {
        if (isEmpty()) {
            std::cout << "No notifications." << std::endl;
            return;
        }

        const NotificationQueueNode* current = front;
        int number = 1;

        while (current != NULL) {
            std::cout << number << ". " << current->data.message
                      << " (" << current->data.timestamp << ")" << std::endl;
            current = current->next;
            number++;
        }
    }
};

#endif
