# News Feed / Social Media Engine — Project Documentation

## 1. Title

**News Feed / Social Media Engine**
C++ Console-Based Data Structures-I Course Project

## 2. Abstract

This project is a small console application that models common social-media actions: users register and log in, publish posts, follow other users, like and comment on posts, receive notifications, undo recent actions, search content, and view a ranked feed. Its main purpose is to demonstrate practical use of manually implemented data structures and algorithms in C++.

The program is intentionally small and local. It uses a hash table for users, a singly linked list for posts, a directed graph for follow relationships, linked-node queues and stacks for notifications and undo, a max heap for feed ranking, and Merge Sort and Quick Sort for result ordering. Long-term data is written to simple text files when the user exits normally.

## 3. Problem Statement

A social-media system must keep track of different kinds of information that change over time:

- users must be stored and found by username;
- posts must be created, viewed, searched, and removed;
- follow relationships must show who follows whom;
- the system should find nearby social connections and connection paths;
- likes and comments should affect a post's engagement;
- notifications should be handled in arrival order;
- recent actions should be reversible in reverse order;
- feed candidates should be ranked;
- relevant state should remain after the program restarts.

Using one large array or one general-purpose container for all of these jobs would hide the DSA choices. The project instead uses a separate suitable structure for each requirement.

## 4. Objectives

- Build a clear menu-driven C++ application for DSA practice.
- Implement the main structures manually without substituting STL associative, queue, stack, list, or priority-queue containers.
- Use fast average-case username lookup.
- Store posts dynamically and keep creation order.
- Represent non-mutual follow relationships correctly.
- Use BFS and DFS for meaningful graph tasks.
- Support interactions, notifications, undo, feed ranking, and sorted search results.
- Preserve useful long-term state in simple files.
- Explain complexity and design decisions at an undergraduate level.

## 5. Scope

The project is a local, single-process console prototype. It supports one currently logged-in user at a time, but it retains records for many users. The implemented scope is user management, posts, social graph actions, interactions, notifications, undo, ranking, search/sorting, and file persistence.

It is not a client/server product and does not include a GUI, web API, database, real-time communication, secure credential storage, concurrency, or large-scale social-network features.

## 6. Functional Requirements

### User and session actions

- Register a user with username, password, and name.
- Log in, log out, view the current user, and search another username.
- Prevent duplicate usernames and reject the persistence delimiter `|` in user-entered fields that are saved.

### Post and search actions

- Create a post while logged in; post IDs begin at 101.
- View all posts or one post by ID.
- Delete only a post written by the logged-in user.
- Search post content case-insensitively and choose newest-first, engagement-first, or original creation order.

### Social actions

- Follow and unfollow another user.
- View the logged-in user's following and followers.
- Recommend users at directed graph distance two with BFS.
- Find one directed connection path to a selected user with DFS.

### Interaction and support actions

- Like or unlike an existing post, while preventing a duplicate active like by the same user.
- Add comments, view comments, and delete only one's own comments.
- View and process per-user notifications in FIFO order.
- Undo the logged-in user's latest supported action in LIFO order.
- Display a personalized feed for authors followed by the logged-in user.

### Persistence actions

- Load persistent records at startup.
- Save persistent records when option `0` exits the application.
- Preserve original IDs and avoid notification/undo creation during restoration.

## 7. System Architecture

```text
                      +-----------------------+
                      | Console menu / main() |
                      +-----------+-----------+
                                  |
       +--------------------------+--------------------------+
       |                          |                          |
       v                          v                          v
 UserManager                 PostManager                SocialManager
 HashTable                   PostLinkedList             Directed Graph
       |                          |                       /          \
       |                          |                     BFS          DFS
       |                          |
       +------------+-------------+
                    |
                    v
           InteractionManager
            /                 \
      NotificationManager     UndoManager
      NotificationQueue[]     ActionStack[]

           FeedManager                 PostSorting
             MaxHeap             Merge Sort / Quick Sort
                    \              /
                     v            v
                   FileManager -> data/*.txt
```

The managers coordinate the application rules. Their permanent data remains in the structures they own: the hash table owns `User` objects, the linked list owns post nodes, and the graph owns adjacency-list nodes. The heap and sorting routines use temporary non-owning pointers to linked-list posts.

## 8. Data Model

The project defines five simple model classes:

- **User:** `userID`, `username`, `password`, and `name`.
- **Post:** `postID`, `authorID`, `content`, `timestamp`, `likeCount`, and `commentCount`.
- **Comment:** `commentID`, `postID`, `userID`, `content`, and `timestamp`.
- **Notification:** `notificationID`, `receiverID`, `message`, and `timestamp`.
- **Action:** `actionType`, `userID`, and `targetID`, used by undo.

The application stores IDs rather than pointers in relationships, likes, comments, and action records. This makes it possible to save the meaningful relationships to files without writing memory addresses.

## 9. Data Structures Used

### Custom hash table

`HashTable` uses 101 `User*` slots and owns the inserted users. The hash is the sum of unsigned username bytes reduced modulo 101. A collision is handled by linear probing with wraparound. The same probe sequence is used when searching.

### Custom singly linked list

`PostLinkedList` stores a `Post` in each `PostNode`. It keeps `head`, `tail`, and a count. Posts are appended at the tail, so the list represents creation order. It deletes its nodes in its destructor.

### Directed graph with adjacency lists

`Graph` holds an adjacency-list head for user IDs 1 through 101. An edge `A -> B` means A follows B. Edges are inserted at the head of A's adjacency list; self-edges and duplicates are rejected.

### Queue and stack

Each user has a linked-node `NotificationQueue` and an `ActionStack`. Queue nodes are linked at the rear and removed from the front. Stack nodes are pushed and popped at the top.

### Max heap

`MaxHeap` stores up to 500 temporary `FeedItem` values in an array. Each `FeedItem` contains a non-owning post pointer and its calculated score. It prioritizes higher score, then higher post ID.

### Arrays

Arrays are intentionally used for the graph's indexed adjacency heads, BFS work arrays, per-user queues/stacks, 1,000 like records, 500 comments, and up to 500 temporary feed or search pointers. These capacities are an explicit design limit of the project.

## 10. Algorithms Used

### BFS recommendations

The program starts at the logged-in user and uses a local fixed FIFO integer array with a distance array. It displays existing users at distance exactly two, excluding the current user and direct follows. It does not rank recommendations.

### DFS connection path

The program recursively explores outgoing follow edges and stores the current path. A visited array prevents cycles from causing infinite recursion. It prints one discovered directed path, not necessarily a shortest path.

### Merge Sort

After keyword matching, Merge Sort orders the temporary post pointers by decreasing post ID for newest-first results. Higher post IDs are newer because IDs are sequential. The merge keeps the left item first on equal IDs, so its merge step is stable.

### Quick Sort

Quick Sort orders search results by decreasing engagement:

```text
Engagement score = (3 × likes) + (5 × comments)
```

The final element is the pivot. Equal engagement is resolved by higher post ID first.

### Heap operations

Feed candidates are inserted with heapify-up and removed with heapify-down. The root is always the candidate that should be shown next.

## 11. Module Description

- **`src/main.cpp`:** Shows the menu, validates console input, connects managers, loads data at start, and saves at exit.
- **`models/`:** Defines `User`, `Post`, `Comment`, `Notification`, and `Action` data objects.
- **`datastructures/HashTable.h`:** Owns and finds users using hashing and linear probing.
- **`datastructures/PostLinkedList.h`:** Owns posts in a custom singly linked list.
- **`datastructures/Graph.h`:** Stores directed follow edges in manual adjacency lists.
- **`datastructures/NotificationQueue.h`:** Implements a linked-node FIFO queue.
- **`datastructures/ActionStack.h`:** Implements a linked-node LIFO stack.
- **`datastructures/MaxHeap.h`:** Implements a fixed-array max heap for temporary feed items.
- **`services/UserManager.h`:** Applies registration, login, restore, and current-user rules.
- **`services/PostManager.h`:** Creates, restores, finds, deletes, and keyword-collects posts.
- **`services/SocialManager.h`:** Applies follow rules and runs BFS/DFS graph operations.
- **`services/InteractionManager.h`:** Manages likes, comments, counters, and cleanup after post deletion.
- **`services/NotificationManager.h`:** Adds, shows, and processes per-user notifications.
- **`services/UndoManager.h`:** Reverses the logged-in user's latest supported action.
- **`services/FeedManager.h`:** Selects followed-user posts, calculates feed scores, and uses the heap to display ranks.
- **`algorithms/Sorting.h`:** Implements Merge Sort and Quick Sort for temporary search-result pointers.
- **`storage/FileManager.cpp`:** Reads and rewrites the five persistence files.

## 12. Feed Ranking Logic

Feed generation begins only if a user is logged in. `FeedManager` traverses all permanent post nodes and includes a post only if the social graph contains `currentUserID -> post.authorID`. The logged-in user's own posts are not candidates because the graph rejects self edges.

For selected candidates in creation order, the recency bonus is based on position among candidates rather than parsed timestamp text:

- newest candidate: 10;
- next newest: 9;
- continues downward to 0;
- older candidates remain at 0.

The final score is:

```text
Feed score = 3 × likeCount + 5 × commentCount + recencyBonus
```

All candidates are inserted into `MaxHeap`; repeated `extractMax()` displays the ranked feed. Equal scores use the higher post ID first. The score is temporary and is not stored inside `Post` or written to a file.

## 13. Search and Sorting Logic

`PostManager::collectPostsWithKeyword()` makes a case-insensitive scan of the post linked list. It adds at most 500 non-owning `const Post*` values to a temporary result array and reports if more matching posts were omitted because of that capacity.

The menu then gives three meaningful result orders:

- option 1: Merge Sort, newest post ID first;
- option 2: Quick Sort, highest engagement first, with engagement score shown;
- option 3: original linked-list creation order.

Search ordering has a different purpose from feed ordering. Search may include any matching post and never uses recency bonus. The personalized feed includes only followed authors and is ranked with a heap.

## 14. Persistence Design

### File formats

```text
users.txt          userID|username|password|name
posts.txt          postID|authorID|content|timestamp
relationships.txt  followerID|followedUserID
likes.txt          userID|postID
comments.txt       commentID|userID|postID|content|timestamp
```

All files are under `data/`. User-entered username, password, name, post content, and comment content cannot contain `|`, which keeps each record easy to split.

### Loading

The required load order is users, posts, relationships, likes, and comments. This ensures each later record can verify the user or post it references. Restoration preserves IDs and timestamps, moves next-ID counters forward, and does not create notifications or undo actions. Malformed or orphaned lines are skipped.

### Saving and session boundary

Exit rewrites each of the five files from the current in-memory structures. This means a delete, unlike, comment deletion, or unfollow is preserved rather than being re-added by old appended data.

The current login, notification queues, and undo stacks are deliberately session-only. They are useful for a running demonstration but are not saved.

## 15. Complexity Analysis

Let `P` be posts, `V` vertices, `E` edges, `C` feed candidates, and `M` matching search results.

- Hash insert/username find: average O(1), worst O(U); find by ID is O(U).
- Linked-list tail insert: O(1); post find, search, display, and deletion: O(P).
- Graph edge add/check/remove: O(degree(source)); following display: O(degree(user)); followers display: O(V + E).
- BFS recommendations: O(V + E).
- DFS path search: O(V + E) worst case.
- Queue enqueue, dequeue, peek: O(1).
- Stack push, pop, peek: O(1).
- Heap insert/extract: O(log C); heap peek: O(1); fully inserting and extracting C candidates: O(C log C).
- Feed candidate selection: O(P × degree(current user)) because each post checks the current user's adjacency list.
- Merge Sort: O(M log M) time and O(M) auxiliary pointer space (bounded by the 500-result array).
- Quick Sort: O(M log M) average time, O(M²) worst-case time; average recursive call stack O(log M).

## 16. Testing Summary

The direct Phase 9 program in `tests/phase9_tests.cpp` completed with **79 passed, 0 failed**. It directly checks collision/wraparound handling, linked-list boundary cases, graph edge rules and cycle-safe traversals, queue FIFO, stack LIFO, heap priority/capacity, sorting cases, manager validation, and undo.

Phase 9 also included full regression and edge-case testing, fresh-process persistence testing, a complexity audit, manual resource-ownership review, zero-warning compilation, and `git diff --check`. The test environment could not support AddressSanitizer leak detection, so no claim is made that that tool proved the absence of leaks.

## 17. Limitations

- The program is console-based and local.
- Capacity is intentionally fixed for course scope: 101 users, 500 comments and temporary feed/search items, and 1,000 like records.
- Data is saved in plain local text files on normal exit, not continuously or transactionally.
- Passwords are plain text because security is outside this academic prototype.
- Recommendations are limited to distance-two users; feed scoring uses only likes, comments, candidate position, and post-ID tiebreaking.

## 18. Conclusion

The project demonstrates that a social-media-style workflow can be broken into clear DSA responsibilities. The hash table supports user lookup, the linked list owns changing post data, the graph models follows, BFS and DFS explore the social network, queue and stack support natural action order, the heap ranks feed items, and sorting gives useful search views. The implementation remains deliberately small enough for students to explain its choices, limits, and complexity in a demonstration or viva.
