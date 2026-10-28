#ifndef USER_H
#define USER_H

#include <string>

class User {
public:
    int userID;
    std::string username;
    std::string password;
    std::string name;

    User(int id, const std::string& user, const std::string& pass,
         const std::string& fullName)
        : userID(id), username(user), password(pass), name(fullName) {
    }
};

#endif
