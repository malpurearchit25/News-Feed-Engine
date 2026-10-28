#ifndef FEED_MANAGER_H
#define FEED_MANAGER_H

#include <iostream>

#include "../datastructures/MaxHeap.h"
#include "PostManager.h"
#include "SocialManager.h"
#include "UserManager.h"

// Builds a temporary ranked feed from followed users' existing posts.
class FeedManager {
private:
    UserManager& userManager;
    SocialManager& socialManager;
    PostManager& postManager;

    int calculateScore(const Post& post, int recencyBonus) const {
        return (3 * post.likeCount) + (5 * post.commentCount) + recencyBonus;
    }

    void displayFeedItem(const FeedItem& item, int rank) const {
        const Post* post = item.post;
        User* author = userManager.findUserByID(post->authorID);

        std::cout << "#" << rank << std::endl;
        std::cout << "Post ID: " << post->postID << std::endl;
        if (author != NULL) {
            std::cout << "Author: " << author->username << " (ID: "
                      << post->authorID << ")" << std::endl;
        } else {
            std::cout << "Author: Unknown (ID: " << post->authorID << ")"
                      << std::endl;
        }
        std::cout << "Content: " << post->content << std::endl;
        std::cout << "Likes: " << post->likeCount << std::endl;
        std::cout << "Comments: " << post->commentCount << std::endl;
        std::cout << "Feed Score: " << item.score << std::endl;
        std::cout << "Timestamp: " << post->timestamp << std::endl;
    }

public:
    FeedManager(UserManager& users, SocialManager& social, PostManager& posts)
        : userManager(users), socialManager(social), postManager(posts) {
    }

    void displayPersonalizedFeed() const {
        User* currentUser = userManager.getCurrentUser();

        if (currentUser == NULL) {
            std::cout << "Please login to view your personalized feed." << std::endl;
            return;
        }

        // Posts are appended at the list tail, so this traversal is oldest to newest.
        const Post* candidates[MaxHeap::MAX_FEED_POSTS];
        int candidateCount = 0;
        int oldestCandidate = 0;
        bool capacityReached = false;
        const PostNode* current = postManager.getFirstPostNode();

        while (current != NULL) {
            const Post& post = current->data;

            // SocialManager asks the existing directed Graph; no second follow store exists.
            if (socialManager.isFollowing(currentUser->userID, post.authorID)) {
                if (candidateCount < MaxHeap::MAX_FEED_POSTS) {
                    candidates[candidateCount] = &post;
                    candidateCount++;
                } else {
                    // A circular buffer retains the latest eligible posts in O(1).
                    candidates[oldestCandidate] = &post;
                    oldestCandidate = (oldestCandidate + 1) % MaxHeap::MAX_FEED_POSTS;
                    capacityReached = true;
                }
            }

            current = current->next;
        }

        if (candidateCount == 0) {
            std::cout << "No posts available for your personalized feed." << std::endl;
            return;
        }

        MaxHeap feedHeap;

        // The final candidate is the newest. Its bonus is 10, then decreases to 0.
        for (int index = 0; index < candidateCount; index++) {
            int positionsFromNewest = candidateCount - 1 - index;
            int recencyBonus = 10 - positionsFromNewest;

            if (recencyBonus < 0) {
                recencyBonus = 0;
            }

            const Post* candidate = candidates[(oldestCandidate + index) % candidateCount];
            FeedItem item(candidate, calculateScore(*candidate, recencyBonus));
            feedHeap.insert(item);
        }

        std::cout << "\n==============================" << std::endl;
        std::cout << "Personalized Feed" << std::endl;
        std::cout << "==============================" << std::endl;

        FeedItem item;
        int rank = 1;

        while (feedHeap.extractMax(item)) {
            displayFeedItem(item, rank);
            std::cout << "------------------------------" << std::endl;
            rank++;
        }

        if (capacityReached) {
            std::cout << "Feed capacity reached; older candidate posts were not "
                         "included." << std::endl;
        }
    }
};

#endif
