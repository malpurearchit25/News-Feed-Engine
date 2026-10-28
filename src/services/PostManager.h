#ifndef POST_MANAGER_H
#define POST_MANAGER_H

#include <cctype>
#include <ctime>
#include <string>

#include "../datastructures/PostLinkedList.h"

class PostManager {
private:
    PostLinkedList posts;
    int nextPostID;

    std::string toLowerCase(const std::string& text) const {
        std::string lowerText = text;

        for (int index = 0; index < static_cast<int>(lowerText.length()); index++) {
            lowerText[index] = static_cast<char>(
                std::tolower(static_cast<unsigned char>(lowerText[index])));
        }

        return lowerText;
    }

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

public:
    static const int MAX_SEARCH_RESULTS = 500;

    PostManager() : nextPostID(101) {
    }

    bool createPost(int authorID, const std::string& content) {
        int createdPostID = -1;
        return createPost(authorID, content, createdPostID);
    }

    bool createPost(int authorID, const std::string& content, int& createdPostID) {
        createdPostID = -1;

        if (authorID < 1 || content.empty() ||
            content.find('|') != std::string::npos) {
            return false;
        }

        Post newPost(nextPostID, authorID, content, getCurrentTimestamp());
        posts.addPost(newPost);
        createdPostID = nextPostID;
        nextPostID++;
        return true;
    }

    // Restores a saved post with its original ID and timestamp.
    bool restorePost(int postID, int authorID, const std::string& content,
                     const std::string& timestamp) {
        if (postID < 101 || authorID < 1 || content.empty() ||
            timestamp.empty() || posts.postExists(postID)) {
            return false;
        }

        Post restoredPost(postID, authorID, content, timestamp);
        posts.addPost(restoredPost);

        if (postID >= nextPostID) {
            nextPostID = postID + 1;
        }

        return true;
    }

    Post* findPostByID(int postID) {
        return posts.findPostByID(postID);
    }

    const Post* findPostByID(int postID) const {
        return posts.findPostByID(postID);
    }

    bool postExists(int postID) const {
        return posts.postExists(postID);
    }

    bool deletePost(int postID, int currentUserID) {
        Post* post = posts.findPostByID(postID);

        if (post == NULL || post->authorID != currentUserID) {
            return false;
        }

        return posts.deletePostByID(postID);
    }

    int getPostCount() const {
        return posts.getPostCount();
    }

    // Keeps PostLinkedList as the single post store while allowing read-only traversal.
    const PostNode* getFirstPostNode() const {
        return posts.getHead();
    }

    // Traverses the linked list and stores non-owning pointers temporarily.
    // Matching is case-insensitive so "Data" and "data" both match.
    int collectPostsWithKeyword(const std::string& keyword,
                                const Post* results[], int maxResults,
                                bool& capacityReached) const {
        capacityReached = false;

        if (keyword.empty() || maxResults <= 0) {
            return 0;
        }

        std::string lowerKeyword = toLowerCase(keyword);
        const PostNode* current = posts.getHead();
        int resultCount = 0;

        while (current != NULL) {
            std::string lowerContent = toLowerCase(current->data.content);

            if (lowerContent.find(lowerKeyword) != std::string::npos) {
                if (resultCount < maxResults) {
                    results[resultCount] = &current->data;
                    resultCount++;
                } else {
                    capacityReached = true;
                }
            }

            current = current->next;
        }

        return resultCount;
    }

    void displayAllPosts() const {
        posts.displayAllPosts();
    }

    void displaySinglePost(int postID) const {
        const Post* post = findPostByID(postID);

        if (post == NULL) {
            std::cout << "Post not found." << std::endl;
            return;
        }

        posts.displaySinglePost(*post);
    }

    void searchPosts(const std::string& keyword) const {
        posts.displayPostsWithKeyword(keyword);
    }
};

#endif
