# News Feed / Social Media Engine

## 1. Project Overview

News Feed / Social Media Engine is a C++ console application created for a Data Structures-I course. It is a small social-media model, built primarily to show how data structures and algorithms solve practical tasks such as finding users, storing posts, following people, ranking a feed, and preserving data between runs.

It is intentionally an educational application rather than an Instagram or Facebook clone. The core structures are implemented manually and the program keeps the interaction simple through a number-based console menu.

## 2. Problem Statement

A social platform needs efficient ways to store and search users, keep a changing collection of posts, represent follow relationships, find social connections, manage notifications, support undo, rank relevant feed posts, order search results, and retain state after exit.

This project applies appropriate Data Structures and Algorithms (DSA) to those requirements. Each structure has a real role in the program instead of being included only as a syllabus item.

## 3. Project Objectives

- Apply DSA concepts to a realistic console application.
- Manually implement the main structures used by the application.
- Support efficient username lookup and practical post operations.
- Model directed follow relationships and explore them with graph traversals.
- Generate a ranked feed from followed users' posts.
- Preserve the long-term application state in local text files.
- Explain the relevant algorithmic complexity clearly.

## 4. Main Features

- User registration, login, logout, username search, and current-user view.
- Post creation, viewing, keyword search, and author-only deletion.
- Directed follow/unfollow relationships with following and follower views.
- BFS distance-two user recommendations and DFS directed connection-path search.
- Likes, unlikes, comments, comment viewing, and author-only comment deletion.
- Per-user FIFO notifications for follows, likes, and comments; the oldest notification can be processed.
- Per-user LIFO undo for follow, unfollow, like, unlike, comment creation, and post creation.
- Personalized feed ranking for posts written by users the current user follows.
- Case-insensitive post search with newest-first, engagement-first, or original creation order.
- Text-file persistence for users, posts, relationships, likes, and comments.

## 5. Data Structures Used

| Data Structure | Used For | Why Chosen |
| --- | --- | --- |
| Custom hash table | User storage and username lookup | Usernames are searched frequently; open-address hashing gives average O(1) lookup. |
| Custom singly linked list | Permanent post storage | Posts can be added and removed dynamically; tail insertion preserves creation order. |
| Directed graph with adjacency lists | Follow relationships | `A -> B` naturally means A follows B, and adjacency lists store only existing follows. |
| Linked-node queue | Per-user notifications | Notifications are processed in the order they arrive (FIFO). |
| Linked-node stack | Per-user undo history | The newest supported action is reversed first (LIFO). |
| Fixed-array max heap | Temporary personalized-feed ranking | It repeatedly returns the highest-scoring candidate first. |
| Fixed arrays | Graph indexing, likes, comments, temporary search/feed data | The project has deliberately bounded capacities and arrays fit those small indexed or temporary stores. |

## 6. Algorithms Used

| Algorithm / Operation | Used For | Main Complexity |
| --- | --- | --- |
| Hashing with linear probing | Insert and find usernames | Average O(1), worst-case O(U) |
| BFS | Distance-two recommendations | O(V + E) |
| Recursive DFS | One directed connection path | O(V + E) worst case |
| Heap insertion / extraction | Build and display a ranked feed | O(log C) each |
| Merge Sort | Newest-first keyword results | O(M log M), O(M) auxiliary space |
| Quick Sort | Engagement-first keyword results | O(M log M) average; O(MÂ²) worst case |

`U` is the number of users, `V` the number of graph vertices, `E` follow edges, `C` feed candidates, and `M` matching search results.

## 7. Why Each Data Structure Was Chosen

- **Hash Table:** Username searches happen often during login, follow, and profile search. The 101-slot table uses a byte-sum hash and linear probing on collisions.
- **Singly Linked List:** Posts are stored as nodes with `head` and `tail` pointers. Appending is O(1), and deleting a post does not require shifting a post array.
- **Directed Graph:** Following is not necessarily mutual, so one directed edge represents one follow action. Adjacency lists avoid a mostly empty relationship matrix.
- **Queue:** A notification that arrives first should be processed first.
- **Stack:** Undo should reverse the most recent supported action first.
- **Max Heap:** Feed posts need to be presented by highest score, which is exactly what a max heap exposes at its root.
- **Arrays:** Fixed capacities are appropriate for this course-scale program: 101 users, 500 feed/search pointers, 1,000 like records, and 500 comments.

## 8. System Architecture

```text
Console Menu (main.cpp)
        |
        +--> UserManager ----------> HashTable <---------- User
        |
        +--> PostManager ----------> PostLinkedList <---- Post
        |
        +--> SocialManager --------> Directed Graph
        |                                 |
        |                              BFS / DFS
        |
        +--> InteractionManager ----> like/comment arrays
        |        |
        |        +--> NotificationManager --> per-user NotificationQueue
        |        +--> UndoManager ---------> per-user ActionStack
        |
        +--> FeedManager ----------> MaxHeap (temporary FeedItems)
        |
        +--> PostSorting ----------> Merge Sort / Quick Sort
        |
        +--> FileManager ----------> data/*.txt
```

`PostLinkedList` owns the permanent posts. The feed and sorting modules only keep temporary non-owning pointers to those posts.

## 9. Application Flow

1. The application starts and calls `FileManager::loadAll()`.
2. Users, posts, relationships, likes, and comments are restored in dependency order.
3. The user chooses menu actions to register, log in, create content, follow users, interact, search, or view a feed.
4. Managers use their data structures to perform each operation.
5. Menu option `0` saves persistent state with `FileManager::saveAll()` and exits.

If standard input ends, the program also reaches the save-and-exit path. Login state, notifications, and undo histories begin empty on each program start.

## 10. Personalized Feed Ranking

The feed considers only posts whose author is followed by the logged-in user. It does not include the user's own posts or posts from unfollowed users.

For each selected candidate, the score is:

```text
Feed score = (3 Ã— likes) + (5 Ã— comments) + recency bonus
```

Candidates are collected in linked-list creation order, from oldest to newest. The newest selected candidate receives a recency bonus of 10, the previous one receives 9, and so on down to 0. Any candidate more than ten positions older than the newest receives 0. The score is calculated when the feed is requested, so it reflects current likes and comments.

Candidates are inserted into the custom `MaxHeap`, then repeatedly extracted for display. A higher score wins; if scores tie, the higher post ID wins, which favors the newer post. The feed retains the latest 500 eligible candidates using a circular buffer.

## 11. Search and Sorting

Keyword matching is case-insensitive. The program traverses the post linked list and stores up to 500 matching post pointers in a temporary array. The user then chooses one of these orderings:

- **Newest First â€” Merge Sort:** Descending post ID, because post IDs are assigned sequentially. The merge is stable when IDs are equal.
- **Most Engaging First â€” Quick Sort:** Descending engagement score, where `engagement = (3 Ã— likes) + (5 Ã— comments)`. Equal engagement scores use the higher post ID first.
- **Original Creation Order:** No sort; results remain in linked-list order.

Personalized feed ranking is separate from search sorting. Feed ranking uses the Max Heap and adds recency bonus; engagement-sorted search uses Quick Sort and does not add recency.

## 12. Persistence

`FileManager` stores plain-text, `|`-separated records in `data/`:

| File | Record Format |
| --- | --- |
| `data/users.txt` | `userID|username|password|name` |
| `data/posts.txt` | `postID|authorID|content|timestamp` |
| `data/relationships.txt` | `followerID|followedUserID` |
| `data/likes.txt` | `userID|postID` |
| `data/comments.txt` | `commentID|userID|postID|content|timestamp` |

Loading order is users, posts, relationships, likes, then comments. A record with invalid field count, invalid numeric data, duplicate identity, or a missing referenced user/post is skipped. Save rewrites all five files, so removed data is not brought back at the next restart.

Persistent state is users, posts, relationships, active likes, and comments. Session-only state is the logged-in user, notification queues, and undo stacks. Passwords are stored as plain text only because this is an academic prototype; that would not be acceptable in production software.

## 13. Complexity Analysis

| Component | Operation | Complexity | Reason |
| --- | --- | --- |
| Hash table | Insert / username lookup | Average O(1), worst O(U) | Linear probing can scan the table when collisions are heavy. |
| Hash table | Find by user ID | O(U) | The 101 slots are scanned. |
| Linked list | Append post | O(1) | A tail pointer is maintained. |
| Linked list | Find, keyword search, delete post | O(P) | Nodes may need to be traversed. |
| Graph | Add, remove, or check a follow edge | O(degree(u)) | The source user's adjacency list is scanned. |
| Graph | Display followers | O(V + E) | All adjacency lists may be checked. |
| BFS | Distance-two recommendations | O(V + E) | Each reachable vertex/edge is processed at most once. |
| DFS | One connection-path search | O(V + E) worst case | Visited marks prevent repeated traversal in cycles. |
| Queue | Enqueue, dequeue, peek | O(1) | Front and rear pointers are maintained. |
| Stack | Push, pop, peek | O(1) | Operations are at the top node. |
| Max heap | Insert / extract maximum | O(log C) | Heapify moves along heap height. |
| Max heap | Peek | O(1) | Highest-priority item is at the root. |
| Feed generation | Select posts before heap operations | O(P Ã— degree(current user)) | Each post checks the current user's adjacency list. |
| Merge Sort | Newest-first results | O(M log M), O(M) auxiliary space | Recursive split and merge. |
| Quick Sort | Engagement-first results | O(M log M) average, O(MÂ²) worst | Final item is used as pivot. |

## 14. Project Folder Structure

```text
NewsFeedEngine/
â”œâ”€â”€ README.md
â”œâ”€â”€ .gitignore
â”œâ”€â”€ data/
â”‚   â”œâ”€â”€ comments.txt
â”‚   â”œâ”€â”€ likes.txt
â”‚   â”œâ”€â”€ posts.txt
â”‚   â”œâ”€â”€ relationships.txt
â”‚   â””â”€â”€ users.txt
â”œâ”€â”€ docs/
â”‚   â”œâ”€â”€ DEMO_GUIDE.md
â”‚   â”œâ”€â”€ DSA_EXPLANATION.md
â”‚   â”œâ”€â”€ PRESENTATION_OUTLINE.md
â”‚   â”œâ”€â”€ PROJECT_DOCUMENTATION.md
â”‚   â”œâ”€â”€ SUBMISSION_CHECKLIST.md
â”‚   â”œâ”€â”€ TEAM_CONTRIBUTIONS.md
â”‚   â””â”€â”€ VIVA_PREPARATION.md
â”œâ”€â”€ src/
â”‚   â”œâ”€â”€ algorithms/Sorting.h
â”‚   â”œâ”€â”€ datastructures/
â”‚   â”œâ”€â”€ models/
â”‚   â”œâ”€â”€ services/
â”‚   â”œâ”€â”€ storage/FileManager.cpp
â”‚   â”œâ”€â”€ storage/FileManager.h
â”‚   â””â”€â”€ main.cpp
â””â”€â”€ tests/
    â””â”€â”€ phase9_tests.cpp
```

## 15. Compilation and Execution

From the project root on macOS or Linux:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic \
    src/main.cpp src/storage/FileManager.cpp \
    -o news_feed

./news_feed
```

The project currently needs `FileManager.cpp` as the only separately compiled implementation file; the remaining project modules are header-based.

To run the direct Phase 9 test suite:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic \
    tests/phase9_tests.cpp \
    -o phase9_tests

./phase9_tests
```

## 16. Testing

Phase 9 direct regression tests report **79 passed, 0 failed**. They cover hash collision and wraparound behavior, linked-list boundary deletion, graph rules and cyclic BFS/DFS, queue FIFO order, stack LIFO order, heap ordering and capacity, both sorts, manager validation, and undo behavior.

Additional Phase 9 validation included console regression and edge-case checks, multi-process persistence regression, a complexity audit, and a manual resource-ownership review. The warning build completed with zero warnings, and `git diff --check` passed. AddressSanitizer leak detection was unsupported in the test environment, so this project does not claim an AddressSanitizer leak-proof result.

## 17. Limitations

- Console-only application with one local process at a time.
- Deliberately fixed capacities: 101 users, 500 feed/search candidates and comments, and 1,000 active-like records.
- Local text files are used instead of a database; state is saved on normal exit.
- Passwords are plain text for prototype simplicity.
- Feed ranking and recommendations are intentionally simplified for course demonstration.
- There is no concurrent multi-user server or production-grade security.

## 18. Future Scope

Possible future work, outside the implemented project, includes a GUI or web interface, secure password hashing, database storage, scalable/dynamic capacities, and richer recommendation or ranking logic.
