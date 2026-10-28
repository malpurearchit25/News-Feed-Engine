#ifndef COMMENT_H
#define COMMENT_H

#include <string>

class Comment {
public:
    int commentID;
    int postID;
    int userID;
    std::string content;
    std::string timestamp;

    Comment()
        : commentID(-1), postID(-1), userID(-1), content(""), timestamp("") {
    }

    Comment(int id, int post, int user, const std::string& text,
            const std::string& time)
        : commentID(id), postID(post), userID(user), content(text),
          timestamp(time) {
    }
};

#endif
