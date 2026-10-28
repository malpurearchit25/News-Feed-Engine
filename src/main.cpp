#include <iostream>
#include <limits>
#include <string>

#include "algorithms/Sorting.h"
#include "models/User.h"
#include "services/InteractionManager.h"
#include "services/FeedManager.h"
#include "services/NotificationManager.h"
#include "services/PostManager.h"
#include "services/SocialManager.h"
#include "services/UndoManager.h"
#include "services/UserManager.h"
#include "storage/FileManager.h"

bool containsPersistenceDelimiter(const std::string& text) {
    return text.find('|') != std::string::npos;
}

void displayUser(const User* user) {
    std::cout << "User ID: " << user->userID << std::endl;
    std::cout << "Username: " << user->username << std::endl;
    std::cout << "Name: " << user->name << std::endl;
}

void showMenu() {
    std::cout << "\n1. Register" << std::endl;
    std::cout << "2. Login" << std::endl;
    std::cout << "3. Search User" << std::endl;
    std::cout << "4. View Current User" << std::endl;
    std::cout << "5. Logout" << std::endl;
    std::cout << "6. Create Post" << std::endl;
    std::cout << "7. View All Posts" << std::endl;
    std::cout << "8. View Single Post" << std::endl;
    std::cout << "9. Search Posts" << std::endl;
    std::cout << "10. Delete Post" << std::endl;
    std::cout << "11. Follow User" << std::endl;
    std::cout << "12. Unfollow User" << std::endl;
    std::cout << "13. View Following" << std::endl;
    std::cout << "14. View Followers" << std::endl;
    std::cout << "15. User Recommendations (BFS)" << std::endl;
    std::cout << "16. Find Connection Path (DFS)" << std::endl;
    std::cout << "17. Like Post" << std::endl;
    std::cout << "18. Unlike Post" << std::endl;
    std::cout << "19. Add Comment" << std::endl;
    std::cout << "20. View Comments" << std::endl;
    std::cout << "21. Delete Comment" << std::endl;
    std::cout << "22. View Notifications (FIFO Queue)" << std::endl;
    std::cout << "23. Process Oldest Notification (FIFO Queue)" << std::endl;
    std::cout << "24. Undo Last Action (LIFO Stack)" << std::endl;
    std::cout << "25. View Personalized Feed (Max Heap Ranking)" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "Enter your choice: ";
}

void showLikeResult(InteractionManager::Result result, bool isLike) {
    if (result == InteractionManager::POST_NOT_FOUND) {
        std::cout << "Post not found." << std::endl;
    } else if (result == InteractionManager::ALREADY_LIKED) {
        std::cout << "You have already liked this post." << std::endl;
    } else if (result == InteractionManager::NOT_LIKED) {
        std::cout << "You have not liked this post." << std::endl;
    } else if (result == InteractionManager::LIKE_STORAGE_FULL) {
        std::cout << "Like storage is full." << std::endl;
    } else if (isLike) {
        std::cout << "Post liked successfully." << std::endl;
    } else {
        std::cout << "Post unliked successfully." << std::endl;
    }
}

void displaySearchResult(const Post* post, const UserManager& userManager,
                         int rank, bool showEngagementScore) {
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
    if (showEngagementScore) {
        std::cout << "Engagement Score: "
                  << PostSorting::calculateEngagementScore(post) << std::endl;
    }
    std::cout << "Timestamp: " << post->timestamp << std::endl;
}

void showCommentResult(InteractionManager::Result result) {
    if (result == InteractionManager::POST_NOT_FOUND) {
        std::cout << "Post not found." << std::endl;
    } else if (result == InteractionManager::EMPTY_COMMENT) {
        std::cout << "Comment content cannot be empty." << std::endl;
    } else if (result == InteractionManager::COMMENT_STORAGE_FULL) {
        std::cout << "Comment storage is full." << std::endl;
    } else if (result == InteractionManager::COMMENT_NOT_FOUND) {
        std::cout << "Comment not found." << std::endl;
    } else if (result == InteractionManager::NOT_COMMENT_AUTHOR) {
        std::cout << "You can delete only your own comments." << std::endl;
    }
}

int main() {
    std::cout << "News Feed / Social Media Engine" << std::endl;
    std::cout << "Custom data structures. Personalized feeds."
              << std::endl;

    UserManager userManager;
    PostManager postManager;
    NotificationManager notificationManager;
    InteractionManager interactionManager(postManager);
    SocialManager socialManager(userManager, notificationManager);
    UndoManager undoManager(userManager, socialManager, interactionManager, postManager);
    FeedManager feedManager(userManager, socialManager, postManager);

    std::cout << "Loading saved data..." << std::endl;
    if (FileManager::loadAll(userManager, postManager, socialManager,
                             interactionManager)) {
        std::cout << "Data loaded successfully." << std::endl;
    } else {
        std::cout << "No saved data found. Starting with empty application."
                  << std::endl;
    }

    int choice;

    do {
        showMenu();

        if (!(std::cin >> choice)) {
            if (std::cin.eof()) {
                std::cout << "\nInput ended." << std::endl;
                choice = 0;
            } else {
                std::cout << "Please enter a number from the menu." << std::endl;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::string username;
        std::string password;
        std::string name;
        std::string content;
        int postID;
        int commentID;

        switch (choice) {
            case 1: {
                std::cout << "Enter username: ";
                std::getline(std::cin, username);
                std::cout << "Enter password: ";
                std::getline(std::cin, password);
                std::cout << "Enter name: ";
                std::getline(std::cin, name);

                if (username.empty() || password.empty() || name.empty()) {
                    std::cout << "Username, password, and name cannot be empty."
                              << std::endl;
                } else if (containsPersistenceDelimiter(username) ||
                           containsPersistenceDelimiter(password) ||
                           containsPersistenceDelimiter(name)) {
                    std::cout << "Username, password, and name cannot contain '|'."
                              << std::endl;
                } else if (userManager.usernameExists(username)) {
                    std::cout << "Username already exists. Please choose another one."
                              << std::endl;
                } else if (userManager.isTableFull()) {
                    std::cout << "User table is full. Registration is unavailable."
                              << std::endl;
                } else if (userManager.registerUser(username, password, name)) {
                    std::cout << "Registration successful." << std::endl;
                }
                break;
            }

            case 2: {
                if (userManager.isLoggedIn()) {
                    std::cout << "A user is already logged in. Please log out first."
                              << std::endl;
                    break;
                }

                std::cout << "Enter username: ";
                std::getline(std::cin, username);
                std::cout << "Enter password: ";
                std::getline(std::cin, password);

                if (userManager.login(username, password)) {
                    std::cout << "Login successful." << std::endl;
                } else {
                    std::cout << "Invalid username or password." << std::endl;
                }
                break;
            }

            case 3: {
                std::cout << "Enter username to search: ";
                std::getline(std::cin, username);

                if (username.empty()) {
                    std::cout << "Username cannot be empty." << std::endl;
                    break;
                }

                User* foundUser = userManager.searchUser(username);

                if (foundUser == NULL) {
                    std::cout << "User not found." << std::endl;
                } else {
                    displayUser(foundUser);
                }
                break;
            }

            case 4: {
                User* currentUser = userManager.getCurrentUser();

                if (currentUser == NULL) {
                    std::cout << "No user is currently logged in." << std::endl;
                } else {
                    displayUser(currentUser);
                }
                break;
            }

            case 5:
                if (userManager.logout()) {
                    std::cout << "Logout successful." << std::endl;
                } else {
                    std::cout << "No user is currently logged in." << std::endl;
                }
                break;

            case 6: {
                User* currentUser = userManager.getCurrentUser();

                if (currentUser == NULL) {
                    std::cout << "Please log in before creating a post." << std::endl;
                    break;
                }

                std::cout << "Enter post content: ";
                std::getline(std::cin, content);
                int createdPostID = -1;

                if (containsPersistenceDelimiter(content)) {
                    std::cout << "Post content cannot contain '|'." << std::endl;
                } else if (postManager.createPost(currentUser->userID, content,
                                                  createdPostID)) {
                    undoManager.pushAction(currentUser->userID, "CREATE_POST", createdPostID);
                    std::cout << "Post created successfully. Post ID: " << createdPostID
                              << std::endl;
                } else {
                    std::cout << "Post content cannot be empty." << std::endl;
                }
                break;
            }

            case 7:
                postManager.displayAllPosts();
                break;

            case 8: {
                std::cout << "Enter post ID: ";
                if (!(std::cin >> postID)) {
                    std::cout << "Post ID must be a number." << std::endl;
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    break;
                }

                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                postManager.displaySinglePost(postID);
                break;
            }

            case 9: {
                std::cout << "Enter keyword to search: ";
                std::getline(std::cin, content);

                if (content.empty()) {
                    std::cout << "Keyword cannot be empty." << std::endl;
                    break;
                }

                const Post* results[PostManager::MAX_SEARCH_RESULTS];
                bool capacityReached = false;
                int resultCount = postManager.collectPostsWithKeyword(
                    content, results, PostManager::MAX_SEARCH_RESULTS, capacityReached);

                if (resultCount == 0) {
                    std::cout << "No matching posts found." << std::endl;
                    break;
                }

                std::cout << "\n1. Newest First (Merge Sort)" << std::endl;
                std::cout << "2. Most Engaging First (Quick Sort)" << std::endl;
                std::cout << "3. Original Creation Order" << std::endl;
                std::cout << "0. Back" << std::endl;
                std::cout << "Choose result order: ";

                int searchOrder;
                if (!(std::cin >> searchOrder)) {
                    std::cout << "Search order must be a number." << std::endl;
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    break;
                }

                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (searchOrder == 0) {
                    break;
                }

                bool showEngagementScore = false;
                if (searchOrder == 1) {
                    PostSorting::mergeSortByNewest(results, resultCount);
                    std::cout << "\nSearch Results - Newest First" << std::endl;
                } else if (searchOrder == 2) {
                    PostSorting::quickSortByEngagement(results, resultCount);
                    showEngagementScore = true;
                    std::cout << "\nSearch Results - Most Engaging" << std::endl;
                } else if (searchOrder == 3) {
                    std::cout << "\nSearch Results - Original Creation Order"
                              << std::endl;
                } else {
                    std::cout << "Invalid search order." << std::endl;
                    break;
                }

                std::cout << "==============================" << std::endl;
                for (int index = 0; index < resultCount; index++) {
                    displaySearchResult(results[index], userManager, index + 1,
                                        showEngagementScore);
                    std::cout << "------------------------------" << std::endl;
                }

                if (capacityReached) {
                    std::cout << "Search result capacity reached; additional matching "
                                 "posts were not included." << std::endl;
                }
                break;
            }

            case 10: {
                User* currentUser = userManager.getCurrentUser();

                if (currentUser == NULL) {
                    std::cout << "Please log in before deleting a post." << std::endl;
                    break;
                }

                std::cout << "Enter post ID to delete: ";
                if (!(std::cin >> postID)) {
                    std::cout << "Post ID must be a number." << std::endl;
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    break;
                }

                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                const Post* post = postManager.findPostByID(postID);

                if (post == NULL) {
                    std::cout << "Post not found." << std::endl;
                } else if (post->authorID != currentUser->userID) {
                    std::cout << "You can delete only your own posts." << std::endl;
                } else if (postManager.deletePost(postID, currentUser->userID)) {
                    interactionManager.removeDataForPost(postID);
                    std::cout << "Post deleted successfully." << std::endl;
                }
                break;
            }

            case 11: {
                std::cout << "Enter username to follow: ";
                std::getline(std::cin, username);

                if (username.empty()) {
                    std::cout << "Username cannot be empty." << std::endl;
                } else {
                    User* currentUser = userManager.getCurrentUser();
                    int followedUserID = -1;

                    if (socialManager.followUser(username, followedUserID) &&
                        currentUser != NULL) {
                        undoManager.pushAction(currentUser->userID, "FOLLOW", followedUserID);
                    }
                }
                break;
            }

            case 12: {
                std::cout << "Enter username to unfollow: ";
                std::getline(std::cin, username);

                if (username.empty()) {
                    std::cout << "Username cannot be empty." << std::endl;
                } else {
                    User* currentUser = userManager.getCurrentUser();
                    int unfollowedUserID = -1;

                    if (socialManager.unfollowUser(username, unfollowedUserID) &&
                        currentUser != NULL) {
                        undoManager.pushAction(currentUser->userID, "UNFOLLOW",
                                               unfollowedUserID);
                    }
                }
                break;
            }

            case 13:
                socialManager.displayFollowing();
                break;

            case 14:
                socialManager.displayFollowers();
                break;

            case 15:
                socialManager.displayRecommendations();
                break;

            case 16: {
                std::cout << "Enter username to find a connection path: ";
                std::getline(std::cin, username);

                if (username.empty()) {
                    std::cout << "Username cannot be empty." << std::endl;
                } else {
                    socialManager.findConnectionPath(username);
                }
                break;
            }

            case 17: {
                User* currentUser = userManager.getCurrentUser();

                if (currentUser == NULL) {
                    std::cout << "Please log in before liking a post." << std::endl;
                    break;
                }

                std::cout << "Enter post ID to like: ";
                if (!(std::cin >> postID)) {
                    std::cout << "Post ID must be a number." << std::endl;
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    break;
                }

                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                InteractionManager::Result result =
                    interactionManager.likePost(currentUser->userID, postID);

                if (result == InteractionManager::SUCCESS) {
                    const Post* post = postManager.findPostByID(postID);

                    if (post != NULL && post->authorID != currentUser->userID) {
                        notificationManager.addNotification(
                            post->authorID,
                            currentUser->username + " liked your post.");
                    }

                    undoManager.pushAction(currentUser->userID, "LIKE", postID);
                }

                showLikeResult(result, true);
                break;
            }

            case 18: {
                User* currentUser = userManager.getCurrentUser();

                if (currentUser == NULL) {
                    std::cout << "Please log in before unliking a post." << std::endl;
                    break;
                }

                std::cout << "Enter post ID to unlike: ";
                if (!(std::cin >> postID)) {
                    std::cout << "Post ID must be a number." << std::endl;
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    break;
                }

                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                InteractionManager::Result result =
                    interactionManager.unlikePost(currentUser->userID, postID);

                if (result == InteractionManager::SUCCESS) {
                    undoManager.pushAction(currentUser->userID, "UNLIKE", postID);
                }

                showLikeResult(result, false);
                break;
            }

            case 19: {
                User* currentUser = userManager.getCurrentUser();

                if (currentUser == NULL) {
                    std::cout << "Please log in before adding a comment." << std::endl;
                    break;
                }

                std::cout << "Enter post ID to comment on: ";
                if (!(std::cin >> postID)) {
                    std::cout << "Post ID must be a number." << std::endl;
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    break;
                }

                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Enter comment content: ";
                std::getline(std::cin, content);
                int createdCommentID = -1;

                if (containsPersistenceDelimiter(content)) {
                    std::cout << "Comment content cannot contain '|'." << std::endl;
                    break;
                }

                InteractionManager::Result result = interactionManager.addComment(
                    currentUser->userID, postID, content, createdCommentID);

                if (result == InteractionManager::SUCCESS) {
                    const Post* post = postManager.findPostByID(postID);

                    if (post != NULL && post->authorID != currentUser->userID) {
                        notificationManager.addNotification(
                            post->authorID,
                            currentUser->username + " commented on your post.");
                    }

                    undoManager.pushAction(currentUser->userID, "COMMENT",
                                           createdCommentID);
                    std::cout << "Comment added successfully. Comment ID: "
                              << createdCommentID << std::endl;
                } else {
                    showCommentResult(result);
                }
                break;
            }

            case 20: {
                std::cout << "Enter post ID to view comments: ";
                if (!(std::cin >> postID)) {
                    std::cout << "Post ID must be a number." << std::endl;
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    break;
                }

                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (!interactionManager.displayComments(postID)) {
                    std::cout << "Post not found." << std::endl;
                }
                break;
            }

            case 21: {
                User* currentUser = userManager.getCurrentUser();

                if (currentUser == NULL) {
                    std::cout << "Please log in before deleting a comment." << std::endl;
                    break;
                }

                std::cout << "Enter comment ID to delete: ";
                if (!(std::cin >> commentID)) {
                    std::cout << "Comment ID must be a number." << std::endl;
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    break;
                }

                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                InteractionManager::Result result =
                    interactionManager.deleteComment(currentUser->userID, commentID);

                if (result == InteractionManager::SUCCESS) {
                    std::cout << "Comment deleted successfully." << std::endl;
                } else {
                    showCommentResult(result);
                }
                break;
            }

            case 22: {
                User* currentUser = userManager.getCurrentUser();

                if (currentUser == NULL) {
                    std::cout << "Please log in to view notifications." << std::endl;
                } else {
                    notificationManager.viewNotifications(currentUser->userID);
                }
                break;
            }

            case 23: {
                User* currentUser = userManager.getCurrentUser();

                if (currentUser == NULL) {
                    std::cout << "Please log in to process notifications." << std::endl;
                } else if (!notificationManager.processOldestNotification(
                               currentUser->userID)) {
                    std::cout << "No notifications to process." << std::endl;
                }
                break;
            }

            case 24:
                undoManager.undoLastAction();
                break;

            case 25:
                feedManager.displayPersonalizedFeed();
                break;

            case 0:
                std::cout << "Saving data..." << std::endl;
                if (FileManager::saveAll(userManager, postManager, socialManager,
                                         interactionManager)) {
                    std::cout << "Data saved successfully." << std::endl;
                } else {
                    std::cerr << "Could not save all data." << std::endl;
                    return 1;
                }
                std::cout << "Exiting News Feed / Social Media Engine." << std::endl;
                break;

            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
    } while (choice != 0);

    return 0;
}
