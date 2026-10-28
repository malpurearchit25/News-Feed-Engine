#ifndef POST_LINKED_LIST_H
#define POST_LINKED_LIST_H

#include <iostream>
#include <string>

#include "../models/Post.h"

// Each node stores one Post and points to the next post in the list.
class PostNode {
public:
    Post data;
    PostNode* next;

    PostNode(const Post& post) : data(post), next(NULL) {
    }
};

class PostLinkedList {
private:
    PostNode* head;
    PostNode* tail;
    int postCount;

    void displayPost(const Post& post) const {
        std::cout << "Post ID: " << post.postID << std::endl;
        std::cout << "Author ID: " << post.authorID << std::endl;
        std::cout << "Content: " << post.content << std::endl;
        std::cout << "Timestamp: " << post.timestamp << std::endl;
        std::cout << "Likes: " << post.likeCount << std::endl;
        std::cout << "Comments: " << post.commentCount << std::endl;
    }

public:
    PostLinkedList() : head(NULL), tail(NULL), postCount(0) {
    }

    ~PostLinkedList() {
        PostNode* current = head;

        while (current != NULL) {
            PostNode* nextNode = current->next;
            delete current;
            current = nextNode;
        }

        head = NULL;
        tail = NULL;
        postCount = 0;
    }

    // Adds at the tail so posts are displayed in creation order.
    void addPost(const Post& post) {
        PostNode* newNode = new PostNode(post);

        if (head == NULL) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }

        postCount++;
    }

    Post* findPostByID(int postID) {
        PostNode* current = head;

        while (current != NULL) {
            if (current->data.postID == postID) {
                return &current->data;
            }

            current = current->next;
        }

        return NULL;
    }

    const Post* findPostByID(int postID) const {
        const PostNode* current = head;

        while (current != NULL) {
            if (current->data.postID == postID) {
                return &current->data;
            }

            current = current->next;
        }

        return NULL;
    }

    bool postExists(int postID) const {
        return findPostByID(postID) != NULL;
    }

    bool deletePostByID(int postID) {
        PostNode* current = head;
        PostNode* previous = NULL;

        while (current != NULL && current->data.postID != postID) {
            previous = current;
            current = current->next;
        }

        if (current == NULL) {
            return false;
        }

        if (previous == NULL) {
            head = current->next;
        } else {
            previous->next = current->next;
        }

        if (current == tail) {
            tail = previous;
        }

        delete current;
        postCount--;

        if (head == NULL) {
            tail = NULL;
        }

        return true;
    }

    int getPostCount() const {
        return postCount;
    }

    // Read-only traversal access for temporary feed candidate collection.
    const PostNode* getHead() const {
        return head;
    }

    void displayAllPosts() const {
        if (head == NULL) {
            std::cout << "No posts available." << std::endl;
            return;
        }

        const PostNode* current = head;

        while (current != NULL) {
            displayPost(current->data);
            std::cout << "--------------------" << std::endl;
            current = current->next;
        }
    }

    void displayPostsWithKeyword(const std::string& keyword) const {
        const PostNode* current = head;
        bool foundMatch = false;

        while (current != NULL) {
            if (current->data.content.find(keyword) != std::string::npos) {
                displayPost(current->data);
                std::cout << "--------------------" << std::endl;
                foundMatch = true;
            }

            current = current->next;
        }

        if (!foundMatch) {
            std::cout << "No posts matched the keyword." << std::endl;
        }
    }

    void displaySinglePost(const Post& post) const {
        displayPost(post);
    }
};

#endif
