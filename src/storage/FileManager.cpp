#include "FileManager.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace {

const char* const DATA_DIRECTORY = "data";
const char* const USERS_FILE = "data/users.txt";
const char* const POSTS_FILE = "data/posts.txt";
const char* const RELATIONSHIPS_FILE = "data/relationships.txt";
const char* const LIKES_FILE = "data/likes.txt";
const char* const COMMENTS_FILE = "data/comments.txt";

bool splitLine(const std::string& line, std::string fields[], int fieldCount) {
    int fieldIndex = 0;
    std::size_t start = 0;

    for (std::size_t index = 0; index <= line.length(); index++) {
        if (index == line.length() || line[index] == '|') {
            if (fieldIndex >= fieldCount) {
                return false;
            }

            fields[fieldIndex] = line.substr(start, index - start);
            fieldIndex++;
            start = index + 1;
        }
    }

    return fieldIndex == fieldCount;
}

bool readInteger(const std::string& text, int& value) {
    std::stringstream input(text);
    char extraCharacter;

    return (input >> value) && !(input >> extraCharacter);
}

void removeCarriageReturn(std::string& line) {
    if (!line.empty() && line[line.length() - 1] == '\r') {
        line.erase(line.length() - 1);
    }
}

int loadUsers(UserManager& userManager) {
    std::ifstream input(USERS_FILE);
    std::string line;
    int restoredCount = 0;

    while (std::getline(input, line)) {
        removeCarriageReturn(line);
        std::string fields[4];
        int userID;

        if (!splitLine(line, fields, 4) || !readInteger(fields[0], userID)) {
            continue;
        }

        if (userManager.restoreUser(userID, fields[1], fields[2], fields[3])) {
            restoredCount++;
        }
    }

    return restoredCount;
}

int loadPosts(const UserManager& userManager, PostManager& postManager) {
    std::ifstream input(POSTS_FILE);
    std::string line;
    int restoredCount = 0;

    while (std::getline(input, line)) {
        removeCarriageReturn(line);
        std::string fields[4];
        int postID;
        int authorID;

        if (!splitLine(line, fields, 4) || !readInteger(fields[0], postID) ||
            !readInteger(fields[1], authorID) ||
            userManager.findUserByID(authorID) == NULL) {
            continue;
        }

        if (postManager.restorePost(postID, authorID, fields[2], fields[3])) {
            restoredCount++;
        }
    }

    return restoredCount;
}

int loadRelationships(const UserManager& userManager,
                      SocialManager& socialManager) {
    std::ifstream input(RELATIONSHIPS_FILE);
    std::string line;
    int restoredCount = 0;

    while (std::getline(input, line)) {
        removeCarriageReturn(line);
        std::string fields[2];
        int followerID;
        int followedUserID;

        if (!splitLine(line, fields, 2) || !readInteger(fields[0], followerID) ||
            !readInteger(fields[1], followedUserID) ||
            userManager.findUserByID(followerID) == NULL ||
            userManager.findUserByID(followedUserID) == NULL) {
            continue;
        }

        if (socialManager.restoreFollow(followerID, followedUserID)) {
            restoredCount++;
        }
    }

    return restoredCount;
}

int loadLikes(const UserManager& userManager, const PostManager& postManager,
              InteractionManager& interactionManager) {
    std::ifstream input(LIKES_FILE);
    std::string line;
    int restoredCount = 0;

    while (std::getline(input, line)) {
        removeCarriageReturn(line);
        std::string fields[2];
        int userID;
        int postID;

        if (!splitLine(line, fields, 2) || !readInteger(fields[0], userID) ||
            !readInteger(fields[1], postID) ||
            userManager.findUserByID(userID) == NULL ||
            !postManager.postExists(postID)) {
            continue;
        }

        if (interactionManager.restoreLike(userID, postID)) {
            restoredCount++;
        }
    }

    return restoredCount;
}

int loadComments(const UserManager& userManager, const PostManager& postManager,
                 InteractionManager& interactionManager) {
    std::ifstream input(COMMENTS_FILE);
    std::string line;
    int restoredCount = 0;

    while (std::getline(input, line)) {
        removeCarriageReturn(line);
        std::string fields[5];
        int commentID;
        int userID;
        int postID;

        if (!splitLine(line, fields, 5) || !readInteger(fields[0], commentID) ||
            !readInteger(fields[1], userID) || !readInteger(fields[2], postID) ||
            userManager.findUserByID(userID) == NULL ||
            !postManager.postExists(postID)) {
            continue;
        }

        if (interactionManager.restoreComment(commentID, userID, postID,
                                              fields[3], fields[4])) {
            restoredCount++;
        }
    }

    return restoredCount;
}

bool saveUsers(const UserManager& userManager) {
    std::ofstream output(USERS_FILE, std::ios::trunc);

    if (!output) {
        return false;
    }

    for (int userID = 1; userID <= UserManager::MAX_USERS; userID++) {
        const User* user = userManager.findUserByID(userID);

        if (user != NULL) {
            output << user->userID << '|' << user->username << '|'
                   << user->password << '|' << user->name << '\n';
        }
    }

    return static_cast<bool>(output);
}

bool savePosts(const PostManager& postManager) {
    std::ofstream output(POSTS_FILE, std::ios::trunc);

    if (!output) {
        return false;
    }

    const PostNode* current = postManager.getFirstPostNode();

    while (current != NULL) {
        const Post& post = current->data;
        output << post.postID << '|' << post.authorID << '|' << post.content
               << '|' << post.timestamp << '\n';
        current = current->next;
    }

    return static_cast<bool>(output);
}

bool saveRelationships(const SocialManager& socialManager) {
    std::ofstream output(RELATIONSHIPS_FILE, std::ios::trunc);

    if (!output) {
        return false;
    }

    for (int userID = 1; userID <= UserManager::MAX_USERS; userID++) {
        const AdjNode* current = socialManager.getFollowingListForUser(userID);

        while (current != NULL) {
            output << userID << '|' << current->userID << '\n';
            current = current->next;
        }
    }

    return static_cast<bool>(output);
}

bool saveLikes(const InteractionManager& interactionManager) {
    std::ofstream output(LIKES_FILE, std::ios::trunc);

    if (!output) {
        return false;
    }

    for (int index = 0; index < InteractionManager::MAX_LIKE_RECORDS; index++) {
        const LikeRecord* like = interactionManager.getActiveLikeRecordAt(index);

        if (like != NULL) {
            output << like->userID << '|' << like->postID << '\n';
        }
    }

    return static_cast<bool>(output);
}

bool saveComments(const InteractionManager& interactionManager) {
    std::ofstream output(COMMENTS_FILE, std::ios::trunc);

    if (!output) {
        return false;
    }

    for (int index = 0; index < interactionManager.getCommentCount(); index++) {
        const Comment* comment = interactionManager.getCommentAt(index);

        if (comment != NULL) {
            output << comment->commentID << '|' << comment->userID << '|'
                   << comment->postID << '|' << comment->content << '|'
                   << comment->timestamp << '\n';
        }
    }

    return static_cast<bool>(output);
}

}  // namespace

bool FileManager::loadAll(UserManager& userManager, PostManager& postManager,
                          SocialManager& socialManager,
                          InteractionManager& interactionManager) {
    int restoredCount = 0;
    restoredCount += loadUsers(userManager);
    restoredCount += loadPosts(userManager, postManager);
    restoredCount += loadRelationships(userManager, socialManager);
    restoredCount += loadLikes(userManager, postManager, interactionManager);
    restoredCount += loadComments(userManager, postManager, interactionManager);
    return restoredCount > 0;
}

bool FileManager::saveAll(const UserManager& userManager,
                          const PostManager& postManager,
                          const SocialManager& socialManager,
                          const InteractionManager& interactionManager) {
    std::error_code directoryError;
    std::filesystem::create_directories(DATA_DIRECTORY, directoryError);

    if (directoryError) {
        return false;
    }

    bool usersSaved = saveUsers(userManager);
    bool postsSaved = savePosts(postManager);
    bool relationshipsSaved = saveRelationships(socialManager);
    bool likesSaved = saveLikes(interactionManager);
    bool commentsSaved = saveComments(interactionManager);

    return usersSaved && postsSaved && relationshipsSaved && likesSaved &&
           commentsSaved;
}
