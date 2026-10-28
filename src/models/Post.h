#ifndef POST_H
#define POST_H

#include <string>

class Post {
public:
    int postID;
    int authorID;
    std::string content;
    std::string timestamp;
    int likeCount;
    int commentCount;

    Post(int id, int author, const std::string& text,
         const std::string& time)
        : postID(id), authorID(author), content(text), timestamp(time),
          likeCount(0), commentCount(0) {
    }
};

#endif
