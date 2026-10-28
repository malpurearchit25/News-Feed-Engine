#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <string>

class Notification {
public:
    int notificationID;
    int receiverID;
    std::string message;
    std::string timestamp;

    Notification(int id, int receiver, const std::string& text,
                 const std::string& time)
        : notificationID(id), receiverID(receiver), message(text),
          timestamp(time) {
    }
};

#endif
