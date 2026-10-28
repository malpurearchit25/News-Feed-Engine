#include <iostream>
#include <sstream>
#include <string>

#include "../src/algorithms/Sorting.h"
#include "../src/datastructures/ActionStack.h"
#include "../src/datastructures/Graph.h"
#include "../src/datastructures/HashTable.h"
#include "../src/datastructures/MaxHeap.h"
#include "../src/datastructures/NotificationQueue.h"
#include "../src/datastructures/PostLinkedList.h"
#include "../src/services/FeedManager.h"
#include "../src/services/InteractionManager.h"
#include "../src/services/NotificationManager.h"
#include "../src/services/SocialManager.h"
#include "../src/services/UndoManager.h"
#include "../src/services/UserManager.h"

int passedTests = 0;
int failedTests = 0;

void expect(bool condition, const std::string& name) {
    if (condition) {
        passedTests++;
    } else {
        failedTests++;
        std::cout << "FAIL: " << name << std::endl;
    }
}

bool hasPostIDs(const Post* posts[], int count, const int expectedIDs[]) {
    for (int index = 0; index < count; index++) {
        if (posts[index] == NULL || posts[index]->postID != expectedIDs[index]) {
            return false;
        }
    }

    return true;
}

bool isEngagementOrder(const Post* posts[], int count) {
    for (int index = 0; index < count - 1; index++) {
        if (PostSorting::hasHigherEngagement(posts[index + 1], posts[index])) {
            return false;
        }
    }

    return true;
}

void testHashTable() {
    HashTable table;
    User* first = new User(1, "d", "p", "First");
    User* wrapped = new User(2, "de", "p", "Wrapped");
    std::string nonAsciiName("\xC3\xA9", 2);
    User* nonAscii = new User(3, nonAsciiName, "p", "Non ASCII");

    // "d" and "de" both hash to slot 100, so the second insert wraps to 0.
    expect(table.insert(first), "Hash table inserts first user");
    expect(table.insert(wrapped), "Hash table resolves collision and wraparound");
    expect(table.findByUsername("de") == wrapped,
           "Hash table finds linearly probed user");
    expect(table.insert(nonAscii), "Hash table accepts non-ASCII username bytes safely");
    expect(table.findByUsername(nonAsciiName) == nonAscii,
           "Hash table finds non-ASCII username");

    User* duplicate = new User(4, "d", "p", "Duplicate");
    expect(!table.insert(duplicate), "Hash table rejects duplicate username");
    delete duplicate;

    int number = 0;
    while (!table.isFull() && number < 200) {
        User* user = new User(10 + number, "filled" + std::to_string(number),
                              "p", "Filled");
        if (!table.insert(user)) {
            delete user;
            break;
        }
        number++;
    }

    expect(table.isFull(), "Hash table reports full capacity");
    User* overflow = new User(999, "overflow", "p", "Overflow");
    expect(!table.insert(overflow), "Hash table rejects insertion when full");
    delete overflow;
}

void testPostLinkedList() {
    PostLinkedList posts;
    Post first(101, 1, "First", "time");
    Post middle(102, 1, "Middle", "time");
    Post last(103, 1, "Last", "time");

    expect(posts.getPostCount() == 0 && posts.getHead() == NULL,
           "Linked list starts empty");
    posts.addPost(first);
    posts.addPost(middle);
    posts.addPost(last);
    expect(posts.getPostCount() == 3, "Linked list stores multiple posts");
    expect(posts.findPostByID(102) != NULL, "Linked list searches a middle node");

    const PostNode* current = posts.getHead();
    bool creationOrder = current != NULL && current->data.postID == 101;
    current = creationOrder ? current->next : NULL;
    creationOrder = creationOrder && current != NULL && current->data.postID == 102;
    current = creationOrder ? current->next : NULL;
    creationOrder = creationOrder && current != NULL && current->data.postID == 103;
    expect(creationOrder, "Linked list traverses in insertion order");

    expect(posts.deletePostByID(102), "Linked list deletes middle node");
    expect(posts.deletePostByID(101), "Linked list deletes first node");
    expect(posts.deletePostByID(103), "Linked list deletes last and only node");
    expect(posts.getPostCount() == 0 && posts.getHead() == NULL,
           "Linked list resets head after deleting only node");
    expect(!posts.deletePostByID(999), "Linked list rejects nonexistent deletion");

    Post replacement(104, 1, "Replacement", "time");
    posts.addPost(replacement);
    expect(posts.findPostByID(104) != NULL && posts.getPostCount() == 1,
           "Linked list tail remains valid after deletion and insertion");
}

void testGraphAndTraversals() {
    Graph graph;
    expect(graph.addEdge(1, 2), "Graph adds a directed edge");
    expect(!graph.addEdge(1, 2), "Graph rejects duplicate edge");
    expect(!graph.addEdge(1, 1), "Graph rejects self edge");
    expect(graph.hasEdge(1, 2) && graph.getFollowingCount(1) == 1,
           "Graph reports following relationship");
    expect(graph.removeEdge(1, 2), "Graph removes edge");
    expect(!graph.removeEdge(1, 2), "Graph rejects missing edge removal");

    UserManager users;
    NotificationManager notifications;
    SocialManager social(users, notifications);
    expect(users.registerUser("alice", "a1", "Alice"), "Registers Alice for graph test");
    expect(users.registerUser("bob", "b1", "Bob"), "Registers Bob for graph test");
    expect(users.registerUser("charlie", "c1", "Charlie"),
           "Registers Charlie for graph test");
    expect(users.registerUser("david", "d1", "David"), "Registers David for graph test");
    expect(users.login("alice", "a1"), "Logs in Alice for graph test");

    expect(social.followUserByID(1, 2, false), "Social graph adds Alice to Bob");
    expect(social.followUserByID(2, 3, false), "Social graph adds Bob to Charlie");
    expect(social.followUserByID(3, 1, false), "Social graph accepts cycle edge safely");
    expect(social.followUserByID(3, 4, false), "Social graph adds path to David");

    std::ostringstream recommendationOutput;
    std::streambuf* originalBuffer = std::cout.rdbuf(recommendationOutput.rdbuf());
    social.displayRecommendations();
    std::cout.rdbuf(originalBuffer);
    expect(recommendationOutput.str().find("charlie") != std::string::npos,
           "BFS recommends distance-two user with graph cycle present");

    std::ostringstream pathOutput;
    originalBuffer = std::cout.rdbuf(pathOutput.rdbuf());
    social.findConnectionPath("david");
    std::cout.rdbuf(originalBuffer);
    expect(pathOutput.str().find("alice -> bob -> charlie -> david") !=
               std::string::npos,
           "DFS finds directed path without looping on cycle");
}

void testQueueAndStack() {
    NotificationQueue queue;
    Notification removed(0, 0, "", "");
    expect(!queue.dequeue(removed), "Queue handles dequeue from empty queue");
    NotificationQueue singleQueue;
    singleQueue.enqueue(Notification(1, 1, "Only", "t"));
    expect(singleQueue.dequeue(removed) && removed.message == "Only" &&
               singleQueue.isEmpty(),
           "Queue handles one element");
    queue.enqueue(Notification(1, 1, "A", "t1"));
    queue.enqueue(Notification(2, 1, "B", "t2"));
    queue.enqueue(Notification(3, 1, "C", "t3"));
    expect(queue.dequeue(removed) && removed.message == "A", "Queue processes A first");
    expect(queue.dequeue(removed) && removed.message == "B", "Queue processes B second");
    queue.enqueue(Notification(4, 1, "D", "t4"));
    expect(queue.dequeue(removed) && removed.message == "C", "Queue preserves FIFO after reuse");
    expect(queue.dequeue(removed) && removed.message == "D" && queue.isEmpty(),
           "Queue resets after final dequeue");

    ActionStack stack;
    Action action("", -1, -1);
    expect(!stack.pop(action), "Stack handles pop from empty stack");
    ActionStack singleStack;
    singleStack.push(Action("Only", 1, 1));
    expect(singleStack.pop(action) && action.actionType == "Only" &&
               singleStack.isEmpty(),
           "Stack handles one element");
    stack.push(Action("A", 1, 1));
    stack.push(Action("B", 1, 2));
    stack.push(Action("C", 1, 3));
    expect(stack.pop(action) && action.actionType == "C", "Stack pops C first");
    expect(stack.pop(action) && action.actionType == "B", "Stack pops B second");
    stack.push(Action("D", 1, 4));
    expect(stack.pop(action) && action.actionType == "D", "Stack reuses top correctly");
    expect(stack.pop(action) && action.actionType == "A" && stack.isEmpty(),
           "Stack preserves LIFO after reuse");
}

void testMaxHeap() {
    Post post10(110, 1, "", "");
    Post post30(130, 1, "", "");
    Post post20(120, 1, "", "");
    Post post40(140, 1, "", "");
    Post post15(115, 1, "", "");
    MaxHeap heap;
    FeedItem removed;

    expect(!heap.extractMax(removed), "Max heap handles empty extraction");
    expect(heap.insert(FeedItem(&post10, 10)), "Max heap inserts first item");
    expect(heap.insert(FeedItem(&post30, 30)), "Max heap inserts second item");
    expect(heap.insert(FeedItem(&post20, 20)), "Max heap inserts third item");
    expect(heap.insert(FeedItem(&post40, 40)), "Max heap inserts fourth item");
    expect(heap.insert(FeedItem(&post15, 15)), "Max heap inserts fifth item");

    const int expectedScores[] = {40, 30, 20, 15, 10};
    bool expectedOrder = true;
    for (int index = 0; index < 5; index++) {
        if (!heap.extractMax(removed) || removed.score != expectedScores[index]) {
            expectedOrder = false;
        }
    }
    expect(expectedOrder && heap.isEmpty(), "Max heap extracts 40, 30, 20, 15, 10");

    MaxHeap singleHeap;
    singleHeap.insert(FeedItem(&post10, 10));
    expect(singleHeap.extractMax(removed) && removed.score == 10 &&
               singleHeap.isEmpty(),
           "Max heap handles one item");

    Post olderTie(200, 1, "", "");
    Post newerTie(201, 1, "", "");
    heap.insert(FeedItem(&olderTie, 25));
    heap.insert(FeedItem(&newerTie, 25));
    expect(heap.extractMax(removed) && removed.post->postID == 201,
           "Max heap uses higher post ID for equal-score tie");

    MaxHeap capacityHeap;
    for (int index = 0; index < MaxHeap::MAX_FEED_POSTS; index++) {
        capacityHeap.insert(FeedItem(&post10, index));
    }
    expect(capacityHeap.size() == MaxHeap::MAX_FEED_POSTS &&
               !capacityHeap.insert(FeedItem(&post10, 999)),
           "Max heap enforces fixed capacity");
}

void testSorting() {
    Post post105(105, 1, "", "");
    Post post101(101, 1, "", "");
    Post post108(108, 1, "", "");
    Post post103(103, 1, "", "");
    const Post* newest[] = {&post105, &post101, &post108, &post103};
    const int newestExpected[] = {108, 105, 103, 101};
    PostSorting::mergeSortByNewest(newest, 4);
    expect(hasPostIDs(newest, 4, newestExpected),
           "Merge Sort orders 105, 101, 108, 103 newest first");

    const Post* emptyResults[1] = {NULL};
    const Post* oneResult[1] = {&post105};
    const Post* alreadySorted[] = {&post108, &post105, &post103, &post101};
    const Post* reverseSorted[] = {&post101, &post103, &post105, &post108};
    PostSorting::mergeSortByNewest(emptyResults, 0);
    PostSorting::mergeSortByNewest(oneResult, 1);
    PostSorting::mergeSortByNewest(alreadySorted, 4);
    PostSorting::mergeSortByNewest(reverseSorted, 4);
    expect(oneResult[0] == &post105 && hasPostIDs(alreadySorted, 4, newestExpected) &&
               hasPostIDs(reverseSorted, 4, newestExpected),
           "Merge Sort handles empty, one, sorted, and reverse inputs");

    Post score10(210, 1, "", "");
    Post score30(230, 1, "", "");
    Post score20(220, 1, "", "");
    Post score40(240, 1, "", "");
    Post score15(215, 1, "", "");
    score10.commentCount = 2;
    score30.likeCount = 10;
    score20.commentCount = 4;
    score40.commentCount = 8;
    score15.likeCount = 5;
    const Post* engagement[] = {&score10, &score30, &score20, &score40, &score15};
    const int engagementExpected[] = {240, 230, 220, 215, 210};
    PostSorting::quickSortByEngagement(engagement, 5);
    expect(hasPostIDs(engagement, 5, engagementExpected),
           "Quick Sort orders engagement scores 40, 30, 20, 15, 10");

    Post equalOlder(300, 1, "", "");
    Post equalNewer(301, 1, "", "");
    equalOlder.likeCount = 2;
    equalNewer.likeCount = 2;
    const Post* equalScores[] = {&equalOlder, &equalNewer};
    const Post* alreadySortedEngagement[] = {
        &score40, &score30, &score20, &score15, &score10
    };
    const Post* reverseEngagement[] = {&score10, &score15, &score20, &score30, &score40};
    PostSorting::quickSortByEngagement(emptyResults, 0);
    PostSorting::quickSortByEngagement(oneResult, 1);
    PostSorting::quickSortByEngagement(equalScores, 2);
    PostSorting::quickSortByEngagement(alreadySortedEngagement, 5);
    PostSorting::quickSortByEngagement(reverseEngagement, 5);
    expect(equalScores[0]->postID == 301 &&
               isEngagementOrder(alreadySortedEngagement, 5) &&
               isEngagementOrder(reverseEngagement, 5),
           "Quick Sort handles empty, one, equal, sorted, and reverse inputs");
}

void testManagersAndUndo() {
    UserManager users;
    expect(users.registerUser("alice", "a1", "Alice"), "User manager registers user");
    expect(!users.registerUser("alice", "a2", "Duplicate"),
           "User manager rejects duplicate username");
    expect(!users.login("alice", "wrong"), "User manager rejects invalid login");
    expect(users.login("alice", "a1") && users.getCurrentUser() != NULL,
           "User manager logs in registered user");
    expect(users.logout() && !users.isLoggedIn(), "User manager logs out safely");

    UserManager sparseUsers;
    expect(sparseUsers.restoreUser(101, "last", "p", "Last") &&
               sparseUsers.isTableFull() &&
               !sparseUsers.registerUser("outOfRange", "p", "Out of Range"),
           "User manager keeps restored IDs inside graph capacity");

    PostManager posts;
    int createdPostID = -1;
    expect(!posts.createPost(0, "Invalid", createdPostID),
           "Post manager rejects invalid author ID");
    expect(posts.createPost(1, "Data structures", createdPostID) && createdPostID == 101,
           "Post manager creates sequential post");

    InteractionManager interactions(posts);
    expect(interactions.likePost(2, 101) == InteractionManager::SUCCESS,
           "Interactions add like");
    expect(interactions.likePost(2, 101) == InteractionManager::ALREADY_LIKED,
           "Interactions reject duplicate like");
    expect(interactions.unlikePost(3, 101) == InteractionManager::NOT_LIKED,
           "Interactions reject invalid unlike");
    int commentID = -1;
    expect(interactions.addComment(2, 101, "Useful post", commentID) ==
               InteractionManager::SUCCESS,
           "Interactions add comment");
    expect(interactions.deleteComment(3, commentID) ==
               InteractionManager::NOT_COMMENT_AUTHOR,
           "Interactions reject deletion by another user");
    expect(interactions.deleteComment(2, commentID) == InteractionManager::SUCCESS,
           "Interactions delete own comment");

    UserManager undoUsers;
    undoUsers.registerUser("owner", "o1", "Owner");
    undoUsers.registerUser("reader", "r1", "Reader");
    undoUsers.login("owner", "o1");
    PostManager undoPosts;
    NotificationManager undoNotifications;
    SocialManager undoSocial(undoUsers, undoNotifications);
    InteractionManager undoInteractions(undoPosts);
    UndoManager undo(undoUsers, undoSocial, undoInteractions, undoPosts);
    int ownerPostID = -1;
    undoPosts.createPost(1, "Undo post", ownerPostID);

    undoSocial.followUserByID(1, 2, false);
    undo.pushAction(1, "FOLLOW", 2);
    expect(undo.undoLastAction() && !undoSocial.isFollowing(1, 2),
           "Undo reverses follow without adding inverse action");
    expect(!undo.undoLastAction(), "Undo handles empty user stack safely");

    undoInteractions.likePost(1, ownerPostID);
    undo.pushAction(1, "LIKE", ownerPostID);
    expect(undo.undoLastAction() && !undoInteractions.hasLikedPost(1, ownerPostID),
           "Undo reverses like state");

    undo.pushAction(1, "CREATE_POST", ownerPostID);
    expect(undo.undoLastAction() && !undoPosts.postExists(ownerPostID),
           "Undo removes created post and related interaction data");
}

void testFeedCapacity() {
    UserManager users;
    PostManager posts;
    NotificationManager notifications;
    SocialManager social(users, notifications);
    users.registerUser("reader", "demo", "Reader");
    users.registerUser("author", "demo", "Author");
    users.login("reader", "demo");
    social.followUserByID(1, 2, false);
    for (int i = 0; i < 501; ++i) {
        posts.createPost(2, "Candidate");
    }
    FeedManager feed(users, social, posts);
    std::ostringstream output;
    std::streambuf* previous = std::cout.rdbuf(output.rdbuf());
    feed.displayPersonalizedFeed();
    std::cout.rdbuf(previous);
    const std::string text = output.str();
    expect(text.find("#1\nPost ID: 601\n") != std::string::npos,
           "Feed retains and ranks the newest post when capacity overflows");
    expect(text.find("Post ID: 101\n") == std::string::npos,
           "Feed drops the oldest eligible post when capacity overflows");
    expect(text.find("#500\n") != std::string::npos &&
           text.find("#501\n") == std::string::npos,
           "Feed displays exactly its bounded capacity");
}

int main() {
    testHashTable();
    testPostLinkedList();
    testGraphAndTraversals();
    testQueueAndStack();
    testMaxHeap();
    testSorting();
    testManagersAndUndo();
    testFeedCapacity();

    std::cout << "Phase 9 direct tests: " << passedTests << " passed, "
              << failedTests << " failed." << std::endl;
    return failedTests == 0 ? 0 : 1;
}
