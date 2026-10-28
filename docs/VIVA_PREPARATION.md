# Viva Preparation

These questions are based on the implemented project. Answers are intentionally concise; students should also be ready to point to the relevant source file during a viva.

## Project Basics

### 1. What problem does this project solve?

It models a small social-media/news-feed workflow in a console program. It manages users, posts, follow relationships, interactions, notifications, undo, feed ranking, search ordering, and local persistence while demonstrating suitable DSA choices.

### 2. Is this intended to be a full social-media product?

No. It is an undergraduate Data Structures-I project with deliberately limited local scope. It is not a client/server system, GUI, database application, or production platform.

### 3. Which data structures are implemented manually?

The project manually implements a hash table, singly linked list, directed adjacency-list graph, linked-node queue, linked-node stack, and fixed-array max heap. It also uses bounded arrays for interaction records and temporary working data.

### 4. Why are IDs used in many records instead of pointers?

IDs are stable values that can be saved in text files and used to reconnect data after restart. Pointers are memory addresses and would not be meaningful in a later process.

## Hash Table and User Management

### 5. Why did you choose a hash table for users?

The common user key is the username, used for login, search, and following someone by name. Hashing provides average O(1) username lookup, which is better than linearly checking every user in the usual case.

### 6. What collision-resolution technique is used?

The hash table uses linear probing. When the initial slot is occupied, it checks the next array slot and wraps around at the end.

### 7. What happens during a collision?

The program does not discard either user. It moves forward one slot at a time until it finds an empty position; later lookup uses the same probe sequence, so the collided user can still be found.

### 8. What is the hash function in this project?

It adds the unsigned byte values of the username and reduces the running sum modulo 101. Using `unsigned char` prevents non-ASCII bytes from producing a negative array index.

### 9. What is the table capacity and why is it important?

The user hash table has 101 slots, and graph/per-user managers also support IDs 1 through 101. Registration is rejected when the next user ID would exceed that established capacity.

### 10. What is the complexity of username lookup?

It is average O(1) when probing is short. In the worst case it is O(U), because linear probing may inspect many occupied table slots.

## Linked List and Posts

### 11. Why use a linked list for posts?

Posts are created and deleted while the program runs. The custom singly linked list lets the program append at the tail in O(1) and delete a matching node without shifting an array of later posts.

### 12. What does each post node contain?

A `PostNode` stores one `Post` object and a pointer to the next node. The list also maintains head, tail, and a post count.

### 13. What is the complexity of finding or deleting a post?

Both can be O(P), where P is the number of posts, because the singly linked list may need to visit nodes sequentially. Deletion also keeps track of the previous node so it can relink the list.

### 14. Who can delete a post?

Only the currently logged-in user who is the post's author can delete it. When deletion succeeds, related active like records and comments are also cleared by `InteractionManager`.

## Graph, BFS, and DFS

### 15. Why is the social graph directed?

Following is not automatically mutual. The edge `A -> B` means A follows B; it does not mean B follows A.

### 16. How is the graph stored?

It uses manual adjacency lists. Each source user ID has a linked list of followed user IDs, so only real follow relationships are stored.

### 17. What is the difference between BFS and DFS?

BFS explores outward level by level and naturally tracks distance from a start vertex. DFS follows one branch deeply before backtracking; this project uses it to find one connection path.

### 18. Why use BFS for recommendations?

The recommendation rule is specifically users at directed distance two: people followed by people the current user follows. BFS records these distances directly and excludes direct follows and the current user.

### 19. Why use DFS for connection-path exploration?

The feature needs to determine whether a directed route exists and print one route. DFS is simple to implement recursively with a path array and does not claim to find the shortest path.

### 20. How does the program avoid infinite traversal in graph cycles?

Both traversals keep visited information. BFS uses its distance array to avoid revisiting a discovered user, and DFS marks a user before recursively exploring its outgoing edges.

### 21. What are the complexities of BFS and DFS here?

Both are O(V + E) in the usual graph-traversal analysis. Each reachable user and follow edge is processed at most once, with bounded arrays sized for the project's graph capacity.

## Queue, Stack, and Interactions

### 22. Why is a queue suitable for notifications?

Notifications are handled in the order they arrive. The linked-node queue enqueues at the rear and processes the oldest node from the front, giving FIFO behavior.

### 23. What notification events exist in this project?

A successful normal follow notifies the followed user. A successful like or comment notifies the post author unless the action is on that user's own post; normal unfollow does not notify anyone.

### 24. What is the complexity of queue operations?

Enqueue, dequeue, and peek are O(1) because the queue maintains both front and rear pointers. Dequeue deletes the removed node and resets the rear when the queue becomes empty.

### 25. Why is a stack suitable for undo?

Undo should normally reverse the most recently supported action first. A stack's LIFO rule matches that behavior exactly.

### 26. Which actions can be undone?

The implementation supports undo for follow, unfollow, like, unlike, comment creation, and post creation. Failed operations are not pushed onto the user's stack.

### 27. Why are undo stacks separate for each user?

`UndoManager` owns one stack per valid user ID. This prevents one logged-in user from reversing another user's action history.

### 28. What happens when undo reverses an action?

The action is popped and its inverse is attempted. The inverse is not pushed back onto the same stack, which avoids endless follow/unfollow or like/unlike toggling on repeated undo.

### 29. How are duplicate likes prevented?

The interaction manager scans its active `LikeRecord` array for the same user ID and post ID before adding a like. If a matching active record exists, it returns `ALREADY_LIKED`.

## Heap and Feed Ranking

### 30. Why is a max heap used instead of a min heap?

The feed needs the highest-scoring candidate first. A max heap keeps that candidate at its root, while a min heap would prioritize the lowest score instead.

### 31. What posts are eligible for a personalized feed?

Only posts whose authors are followed by the logged-in user are candidates. The user's own posts and posts from unfollowed authors are not included.

### 32. How is feed score calculated?

`feed score = 3 × likes + 5 × comments + recency bonus`. The newest selected candidate gets bonus 10, then the bonus decreases by one toward 0; older candidates stay at 0.

### 33. How are equal feed scores handled?

The max heap gives priority to the higher post ID when scores are equal. Since post IDs increase as posts are created, this favors the newer post deterministically.

### 34. What is heapify?

Heapify restores the heap-order rule after a change. Heapify-up moves a newly inserted item toward its parent; heapify-down moves the replacement root toward its higher-priority child after extraction.

### 35. What are the heap operation complexities?

Insertion and extraction are O(log C), where C is the number of feed candidates. Peeking at the maximum item is O(1).

## Sorting and Search

### 36. What is the difference between feed ranking and engagement sorting?

Feed ranking uses a max heap for followed users' posts and includes recency bonus. Engagement sorting uses Quick Sort on keyword-search matches and uses only `3 × likes + 5 × comments`.

### 37. Why does Merge Sort represent newest-first search results?

The project assigns increasing post IDs when posts are created. Merge Sort orders matching result pointers by descending post ID, so higher IDs appear first.

### 38. Why does the project use Quick Sort for engagement order?

It demonstrates a second relevant sorting algorithm and its partitioning approach. Quick Sort orders temporary result pointers so high-engagement posts appear first; equal scores use higher post ID first.

### 39. What are the average and worst-case complexities of Quick Sort here?

Average time is O(M log M), but worst-case time is O(M²). The implementation always selects the final element as pivot, which can create poor partitions for some input orders.

## Persistence, Complexity, and Design Choices

### 40. What data survives application restart?

Users, posts, follow relationships, active likes, and comments are saved. The five files are `users.txt`, `posts.txt`, `relationships.txt`, `likes.txt`, and `comments.txt` under `data/`.

### 41. What remains session-only?

The current login state, notification queues, and undo stacks are not saved. A restart requires login again and begins with empty notifications and undo history.

### 42. Why is load order important?

The program loads users first, then posts, relationships, likes, and comments. Later records refer to users or posts, so the referenced objects must already exist before the record is restored.

### 43. Why isn't a database used?

The project is a Data Structures-I prototype focused on manual structures and basic file persistence. Plain text files make the saved IDs and relationships easy to inspect, while database design is outside this course scope.

### 44. What are important project limitations?

It has small fixed capacities, local text-file storage, console-only interaction, no concurrent server, and plain-text prototype passwords. Recommendations and feed scoring are simplified intentionally for clear DSA explanation.

### 45. What would change for a production application?

A production system would need secure password hashing, a database, input/security hardening, scalable/dynamic storage, concurrent server handling, and more robust persistence. Those changes are outside the implemented course project and would not replace the current DSA demonstration without further design work.
