#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H

#include <ctime>
#include <iostream>
#include <string>

#include "../datastructures/NotificationQueue.h"

// Each user ID has its own FIFO notification queue.
class NotificationManager {
public:
    static const int MAX_USERS = 101;

private:
    NotificationQueue notificationQueues[MAX_USERS + 1];
    int nextNotificationID;

    std::string getCurrentTimestamp() const {
        std::time_t currentTime = std::time(NULL);
        std::tm* localTime = std::localtime(&currentTime);

        if (localTime == NULL) {
            return "Timestamp unavailable";
        }

        char timeText[80];
        std::strftime(timeText, sizeof(timeText), "%Y-%m-%d %H:%M:%S", localTime);
        return timeText;
    }

    bool isValidUserID(int userID) const {
        return userID >= 1 && userID <= MAX_USERS;
    }

public:
    NotificationManager() : nextNotificationID(1) {
    }

    bool addNotification(int receiverID, const std::string& message) {
        if (!isValidUserID(receiverID) || message.empty()) {
            return false;
        }

        Notification notification(nextNotificationID, receiverID, message,
                                  getCurrentTimestamp());
        notificationQueues[receiverID].enqueue(notification);
        nextNotificationID++;
        return true;
    }

    void viewNotifications(int userID) const {
        if (!isValidUserID(userID)) {
            std::cout << "No notifications." << std::endl;
            return;
        }

        std::cout << "Notifications:" << std::endl;
        notificationQueues[userID].display();
    }

    bool processOldestNotification(int userID) {
        if (!isValidUserID(userID)) {
            return false;
        }

        Notification removedNotification(0, 0, "", "");

        if (!notificationQueues[userID].dequeue(removedNotification)) {
            return false;
        }

        std::cout << "Processed notification: " << removedNotification.message
                  << std::endl;
        return true;
    }

    bool hasNotifications(int userID) const {
        return isValidUserID(userID) && !notificationQueues[userID].isEmpty();
    }
};

#endif
