# DSA Explanation Guide

This guide explains only the structures and algorithms that the current project actually uses. The names and examples match the code so that students can use it during a viva.

## 1. Hash Table

**What it is:** A hash table stores values in an array position calculated from a key. If two keys choose the same position, a collision-resolution method is needed.

**Where it is used here:** `HashTable` stores the application's `User` objects. `UserManager` uses it for registration, login, username search, and follow-target lookup.

**Why it was chosen:** Username lookup is common, and scanning every user for a normal username search would be unnecessary in the usual case.

**Basic working:** The program adds the unsigned byte values of the username and takes modulo 101. If that slot is occupied, it checks the next slot, wrapping from 100 to 0. This is called linear probing. Search follows the same probe sequence.

**Important complexity:** Insert and username search are average O(1), worst-case O(U) if many slots must be probed. Find by numeric user ID is a separate O(U) scan of the 101 slots.

**Likely viva question:** What happens if two usernames have the same hash index?

**Short answer:** The project uses linear probing. It checks the next array slot and continues with wraparound until it finds an empty slot for insertion or finds the required username during search.

## 2. Singly Linked List

**What it is:** A singly linked list is a chain of nodes. Each node stores data and a pointer to the next node.

**Where it is used here:** `PostLinkedList` owns permanent `PostNode` objects. `PostManager` creates, finds, displays, deletes, and keyword-searches posts through this list.

**Why it was chosen:** Posts are created and deleted dynamically. The list keeps a tail pointer, so a new post can be appended without moving old posts or reserving a fixed post array.

**Basic working:** The list stores `head`, `tail`, and a count. A new post node is linked after `tail`. To find or delete a post, the list walks node by node; deletion reconnects the previous node and updates head/tail when needed.

**Important complexity:** Tail insertion is O(1). Find by ID, keyword search, display, and deletion are O(P) because nodes may be traversed.

**Likely viva question:** Why not use an array for posts?

**Short answer:** Posts change during execution. The linked list lets the program append dynamically and delete a node without shifting all later posts. It also demonstrates a suitable custom linked structure for the main dynamic content store.

## 3. Directed Graph

**What it is:** A graph has vertices and edges. In a directed graph, an edge has one direction.

**Where it is used here:** `Graph` represents follow relationships. User IDs 1–101 are graph vertices, and `A -> B` means user A follows user B.

**Why it was chosen:** Follow relationships form a network: one person can follow many people and a person can be followed by many others. Following is not automatically mutual, so the direction matters.

**Basic working:** The graph has an adjacency-list head for each user ID. An `AdjNode` stores one followed user ID and points to the next edge in that source user's list. Duplicate and self edges are rejected.

**Important complexity:** Add, check, and remove an edge are O(degree(source)). Showing a user's following is O(degree(user)); showing followers can be O(V + E) because all adjacency lists are checked.

**Likely viva question:** Why use adjacency lists instead of an adjacency matrix?

**Short answer:** Most users will follow only a small part of all possible users. Adjacency lists store only existing follow edges, while a matrix would reserve space for every possible pair.

## 4. Breadth-First Search (BFS)

**What it is:** BFS explores a graph level by level from a starting vertex. It uses FIFO processing.

**Where it is used here:** `SocialManager::displayRecommendations()` starts from the logged-in user and recommends users at directed distance two.

**Why it was chosen:** Distance-two users mean “people followed by people I follow.” BFS naturally tracks the number of edges from the starting user.

**Basic working:** The code keeps a `distance` array initialized to -1 and a small local integer FIFO array. The current user starts at distance 0. An unvisited outgoing neighbor gets `distance[current] + 1` and is queued. Finally, valid users with distance 2 are displayed; direct follows and the current user are excluded.

**Important complexity:** O(V + E), because each reachable vertex and outgoing edge is processed at most once.

**Likely viva question:** Why is BFS used for recommendations instead of DFS?

**Short answer:** The implemented recommendation rule is based on exactly two hops. BFS records distance level by level, so identifying distance-two users is direct. DFS explores depth-first and is better suited here to finding one path.

## 5. Depth-First Search (DFS)

**What it is:** DFS follows one path as deeply as possible before backtracking. It can be implemented recursively.

**Where it is used here:** `SocialManager::findConnectionPath()` finds one directed path from the logged-in user to a selected username.

**Why it was chosen:** The feature needs to show that a connection path exists and print one discovered route. It does not need the shortest path.

**Basic working:** Recursive `findPathDFS()` marks the current user visited, adds that ID to a path array, and explores each unvisited followed user. If a branch fails, it reduces the path length and tries the next branch. The visited array makes cycles safe.

**Important complexity:** O(V + E) in the worst case.

**Likely viva question:** Does this DFS return the shortest connection path?

**Short answer:** No. It returns one path in the order adjacency nodes are explored. The project uses DFS for path exploration; BFS would be needed if the requirement were specifically a shortest unweighted path.

## 6. Queue

**What it is:** A queue follows First In, First Out (FIFO): the earliest inserted item leaves first.

**Where it is used here:** `NotificationManager` owns one `NotificationQueue` for each user ID. Follow, like, and comment notifications are enqueued for the receiver.

**Why it was chosen:** Notifications should be handled in arrival order. Processing the oldest notification should not skip newer ones.

**Basic working:** A queue node contains a `Notification` and a `next` pointer. `enqueue()` links a new node at `rear`; `dequeue()` removes the node at `front`. When the final node is removed, both pointers are reset.

**Important complexity:** Enqueue, dequeue, and peek are O(1).

**Likely viva question:** Why does the project use a queue rather than a stack for notifications?

**Short answer:** A queue preserves arrival order, so the oldest notification is processed first. A stack would show the newest notification first, which is not the behavior chosen for this application.

## 7. Stack

**What it is:** A stack follows Last In, First Out (LIFO): the most recently pushed item is popped first.

**Where it is used here:** `UndoManager` keeps one `ActionStack` per user. Supported actions are follow, unfollow, like, unlike, comment creation, and post creation.

**Why it was chosen:** Undo normally reverses the latest action before earlier actions.

**Basic working:** A node contains one `Action` and points to the older top. `push()` links a new node at `top`; `pop()` removes that top node. The inverse action is performed but is not pushed onto the same stack.

**Important complexity:** Push, pop, and peek are O(1).

**Likely viva question:** Why is an inverse action not pushed during undo?

**Short answer:** Pushing the inverse could cause an endless toggle such as LIKE then UNLIKE then LIKE on repeated undo. The implementation removes the newest stored action and only restores the earlier state.

## 8. Max Heap

**What it is:** A max heap is a complete binary tree usually stored in an array. Every parent has priority at least as high as its children, so the maximum-priority item is at the root.

**Where it is used here:** `FeedManager` uses `MaxHeap` to order temporary `FeedItem` candidates for the personalized feed.

**Why it was chosen:** The feed must repeatedly display the currently highest-scoring post. A max heap gives direct access to that next item.

**Basic working:** A `FeedItem` contains a non-owning post pointer and calculated score. Insertion puts an item at the end and uses heapify-up. Extraction takes the root, moves the last item to the root, and uses heapify-down. Higher score wins; equal score uses higher post ID.

**Important complexity:** Insert and extract maximum are O(log C); peek is O(1). `C` is the number of candidates, up to 500.

**Likely viva question:** What is heapify in this project?

**Short answer:** Heapify is the swapping process that restores heap order. Heapify-up compares an inserted item with its parent, while heapify-down compares the new root with its children after extraction.

## 9. Merge Sort

**What it is:** Merge Sort recursively divides a collection into halves, sorts the halves, and merges them into sorted order.

**Where it is used here:** `PostSorting::mergeSortByNewest()` orders keyword-search results by descending post ID when the user chooses newest-first.

**Why it was chosen:** The project needs a clear manual sorting algorithm for result ordering. Merge Sort gives predictable O(M log M) time and naturally demonstrates divide-and-conquer.

**Basic working:** The temporary result pointer array is divided until subarrays have one item. The `merge()` function compares post IDs and writes the larger ID first to a temporary pointer buffer, then copies the merged range back.

**Important complexity:** O(M log M) time and O(M) auxiliary pointer space for M matching results. The implementation's result capacity is 500.

**Likely viva question:** Why does a higher post ID mean a newer result?

**Short answer:** `PostManager` assigns increasing IDs to newly created posts, starting at 101. The implementation uses that sequence as its newest-first ordering rule rather than parsing timestamps.

## 10. Quick Sort

**What it is:** Quick Sort selects a pivot, partitions items around it, and recursively sorts the two resulting parts.

**Where it is used here:** `PostSorting::quickSortByEngagement()` orders keyword-search results by engagement when the user selects most-engaging-first.

**Why it was chosen:** It provides a second practical sorting strategy for a different order and illustrates partitioning. The temporary result set is bounded to 500 pointers.

**Basic working:** The final array item is the pivot. During partitioning, posts with higher engagement are moved before it. Engagement is `3 × likeCount + 5 × commentCount`; equal scores use higher post ID first. The two partitions are then sorted recursively.

**Important complexity:** Average O(M log M), worst O(M²) because the fixed final-item pivot can be poor. The average recursive call stack is O(log M); worst case can be O(M).

**Likely viva question:** Why can Quick Sort become O(M²) here?

**Short answer:** The pivot is always the final item. For some input orders, that pivot can split the data into one empty part and one almost-full part repeatedly, leading to quadratic work.

## Important Distinction: Feed Score vs. Engagement Score

The project intentionally uses two related but different formulas:

```text
Feed score       = 3 × likes + 5 × comments + recency bonus
Engagement score = 3 × likes + 5 × comments
```

The max heap uses feed score only for posts by followed users. Quick Sort uses engagement score only for keyword search results. The recency bonus is not used in engagement sorting.
