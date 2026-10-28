#ifndef INTERACTION_MANAGER_H
#define INTERACTION_MANAGER_H

#include <ctime>
#include <iostream>
#include <string>

#include "../models/Comment.h"
#include "PostManager.h"

// Small fixed records are enough to remember which users liked which posts.
class LikeRecord {
public:
    int userID;
    int postID;
    bool active;

    LikeRecord() : userID(-1), postID(-1), active(false) {
    }
};

class InteractionManager {
public:
    static const int MAX_LIKE_RECORDS = 1000;
    static const int MAX_COMMENTS = 500;

    enum Result {
        SUCCESS,
        POST_NOT_FOUND,
        ALREADY_LIKED,
        NOT_LIKED,
        LIKE_STORAGE_FULL,
        COMMENT_STORAGE_FULL,
        COMMENT_NOT_FOUND,
        NOT_COMMENT_AUTHOR,
        EMPTY_COMMENT
    };

private:
    PostManager& postManager;
    LikeRecord likeRecords[MAX_LIKE_RECORDS];
    Comment comments[MAX_COMMENTS];
    int storedCommentCount;
    int nextCommentID;

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

    int findLikeRecord(int userID, int postID) const {
        for (int i = 0; i < MAX_LIKE_RECORDS; i++) {
            if (likeRecords[i].active && likeRecords[i].userID == userID &&
                likeRecords[i].postID == postID) {
                return i;
            }
        }

        return -1;
    }

    int findFreeLikeRecord() const {
        for (int i = 0; i < MAX_LIKE_RECORDS; i++) {
            if (!likeRecords[i].active) {
                return i;
            }
        }

        return -1;
    }

    int findCommentIndex(int commentID) const {
        for (int i = 0; i < storedCommentCount; i++) {
            if (comments[i].commentID == commentID) {
                return i;
            }
        }

        return -1;
    }

public:
    InteractionManager(PostManager& manager)
        : postManager(manager), storedCommentCount(0), nextCommentID(1) {
    }

    Result likePost(int userID, int postID) {
        Post* post = postManager.findPostByID(postID);

        if (post == NULL) {
            return POST_NOT_FOUND;
        }

        if (findLikeRecord(userID, postID) != -1) {
            return ALREADY_LIKED;
        }

        int freeRecord = findFreeLikeRecord();

        if (freeRecord == -1) {
            return LIKE_STORAGE_FULL;
        }

        likeRecords[freeRecord].userID = userID;
        likeRecords[freeRecord].postID = postID;
        likeRecords[freeRecord].active = true;
        post->likeCount++;
        return SUCCESS;
    }

    Result unlikePost(int userID, int postID) {
        Post* post = postManager.findPostByID(postID);

        if (post == NULL) {
            return POST_NOT_FOUND;
        }

        int recordIndex = findLikeRecord(userID, postID);

        if (recordIndex == -1) {
            return NOT_LIKED;
        }

        likeRecords[recordIndex].active = false;

        if (post->likeCount > 0) {
            post->likeCount--;
        }

        return SUCCESS;
    }

    bool hasLikedPost(int userID, int postID) const {
        return findLikeRecord(userID, postID) != -1;
    }

    // Restores a saved active like. This method creates no notification or undo action.
    bool restoreLike(int userID, int postID) {
        return likePost(userID, postID) == SUCCESS;
    }

    Result addComment(int userID, int postID, const std::string& content,
                      int& createdCommentID) {
        createdCommentID = -1;
        Post* post = postManager.findPostByID(postID);

        if (post == NULL) {
            return POST_NOT_FOUND;
        }

        if (content.empty() || content.find('|') != std::string::npos) {
            return EMPTY_COMMENT;
        }

        if (storedCommentCount == MAX_COMMENTS) {
            return COMMENT_STORAGE_FULL;
        }

        comments[storedCommentCount] = Comment(nextCommentID, postID, userID,
                                                content, getCurrentTimestamp());
        createdCommentID = nextCommentID;
        nextCommentID++;
        storedCommentCount++;
        post->commentCount++;
        return SUCCESS;
    }

    // Restores a saved comment with its original ID and timestamp.
    // FileManager validates referenced users before calling this method.
    bool restoreComment(int commentID, int userID, int postID,
                        const std::string& content, const std::string& timestamp) {
        Post* post = postManager.findPostByID(postID);

        if (commentID < 1 || userID < 1 || post == NULL || content.empty() ||
            timestamp.empty() || storedCommentCount == MAX_COMMENTS ||
            findCommentIndex(commentID) != -1) {
            return false;
        }

        comments[storedCommentCount] = Comment(commentID, postID, userID, content,
                                               timestamp);
        storedCommentCount++;
        post->commentCount++;

        if (commentID >= nextCommentID) {
            nextCommentID = commentID + 1;
        }

        return true;
    }

    Result deleteComment(int userID, int commentID) {
        int commentIndex = findCommentIndex(commentID);

        if (commentIndex == -1) {
            return COMMENT_NOT_FOUND;
        }

        if (comments[commentIndex].userID != userID) {
            return NOT_COMMENT_AUTHOR;
        }

        Post* post = postManager.findPostByID(comments[commentIndex].postID);

        if (post != NULL && post->commentCount > 0) {
            post->commentCount--;
        }

        for (int i = commentIndex; i < storedCommentCount - 1; i++) {
            comments[i] = comments[i + 1];
        }

        storedCommentCount--;
        comments[storedCommentCount] = Comment();
        return SUCCESS;
    }

    bool displayComments(int postID) const {
        if (!postManager.postExists(postID)) {
            return false;
        }

        bool foundComment = false;
        std::cout << "Comments for Post " << postID << ":" << std::endl;

        for (int i = 0; i < storedCommentCount; i++) {
            if (comments[i].postID == postID) {
                std::cout << "Comment ID: " << comments[i].commentID << std::endl;
                std::cout << "User ID: " << comments[i].userID << std::endl;
                std::cout << "Content: " << comments[i].content << std::endl;
                std::cout << "Timestamp: " << comments[i].timestamp << std::endl;
                std::cout << "--------------------" << std::endl;
                foundComment = true;
            }
        }

        if (!foundComment) {
            std::cout << "No comments for this post." << std::endl;
        }

        return true;
    }

    // Read-only access used by FileManager when writing the current state.
    const LikeRecord* getActiveLikeRecordAt(int index) const {
        if (index < 0 || index >= MAX_LIKE_RECORDS || !likeRecords[index].active) {
            return NULL;
        }

        return &likeRecords[index];
    }

    int getCommentCount() const {
        return storedCommentCount;
    }

    const Comment* getCommentAt(int index) const {
        if (index < 0 || index >= storedCommentCount) {
            return NULL;
        }

        return &comments[index];
    }

    // Called after a post is deleted so stale interaction records do not remain.
    void removeDataForPost(int postID) {
        for (int i = 0; i < MAX_LIKE_RECORDS; i++) {
            if (likeRecords[i].active && likeRecords[i].postID == postID) {
                likeRecords[i].active = false;
            }
        }

        int index = 0;

        while (index < storedCommentCount) {
            if (comments[index].postID == postID) {
                for (int i = index; i < storedCommentCount - 1; i++) {
                    comments[i] = comments[i + 1];
                }

                storedCommentCount--;
                comments[storedCommentCount] = Comment();
            } else {
                index++;
            }
        }
    }
};

#endif
