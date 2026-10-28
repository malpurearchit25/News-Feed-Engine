# Presentation Outline

Use this as content for an 8–10 slide presentation. Keep slides visual and brief; use the verbal notes to explain the details.

## Slide 1 — Title

**Slide content**

- News Feed / Social Media Engine
- C++ Console-Based Data Structures-I Project
- Team Members: __________ / __________ / __________

**Explain verbally**

This is an educational mini social-media engine, not a production clone. The main aim is to show why particular data structures fit particular application tasks.

## Slide 2 — Problem Statement

**Slide content**

- Social platforms manage users, posts, follows, interactions, and feeds
- Need efficient lookup, dynamic storage, graph relationships, ordering, and persistence
- DSA choices should match each task

**Explain verbally**

We divided the social-media workflow into small requirements instead of using one structure for everything. This lets the application demonstrate practical reasons for hashing, linked lists, graphs, queues, stacks, heaps, and sorting.

## Slide 3 — Objectives

**Slide content**

- Manual core DSA implementations
- User and post operations
- Directed social graph with BFS/DFS
- Notifications, undo, ranked feed, sorted search
- Local text-file persistence

**Explain verbally**

The objective was clarity and correct DSA use. We kept the scope suitable for a console-based undergraduate project and did not add unrelated algorithms.

## Slide 4 — System Architecture

**Slide content**

```text
Menu -> Managers -> Custom DSA -> FileManager -> data/*.txt
                 -> Sorting / Feed ranking
```

- UserManager → HashTable
- PostManager → PostLinkedList
- SocialManager → Directed Graph → BFS / DFS
- FeedManager → MaxHeap

**Explain verbally**

Managers enforce application rules while custom structures hold data. Posts remain owned by the linked list; sorting and feed ranking only use temporary pointers to them.

## Slide 5 — Data Structures Used

**Slide content**

- Hash table: username lookup
- Singly linked list: posts
- Directed adjacency-list graph: follows
- Queue: notifications
- Stack: undo
- Max heap: personalized feed
- Arrays: bounded internal and temporary data

**Explain verbally**

Mention the natural match for each structure: FIFO notifications, LIFO undo, directed follows, and highest-score-first feed display. The capacities are deliberately fixed for this course project.

## Slide 6 — Algorithms and Personalized Feed

**Slide content**

- BFS: distance-two recommendations
- DFS: one directed connection path
- Merge Sort: newest-first search
- Quick Sort: engagement-first search
- Feed score = `3×likes + 5×comments + recency bonus`

**Explain verbally**

Only followed authors' posts become feed candidates. The newest selected candidate gets recency 10, then the bonus falls to 0. The Max Heap extracts highest score first; equal score uses higher post ID.

## Slide 7 — Application Features

**Slide content**

- Register, login, search users
- Create/view/delete/search posts
- Follow/unfollow, followers/following
- Likes, comments, notifications, undo
- Recommendations, connection path, ranked feed
- Save and restore state

**Explain verbally**

Point out that post deletion and comment deletion have ownership restrictions. Also distinguish keyword search ordering from personalized feed ranking.

## Slide 8 — Complexity and Performance

**Slide content**

- Hash username lookup: average O(1)
- Post append: O(1); post traversal/search: O(P)
- BFS / DFS: O(V + E)
- Queue / stack operations: O(1)
- Heap insert/extract: O(log C)
- Merge Sort: O(M log M); Quick Sort: average O(M log M)

**Explain verbally**

Explain that the actual feed candidate scan is O(P × degree(current user)) because each post checks a graph adjacency list. Quick Sort has O(M²) worst case because the implementation uses the last item as pivot.

## Slide 9 — Testing and Results

**Slide content**

- Phase 9 direct tests: 79 passed, 0 failed
- Edge cases: collisions, boundary deletion, cycles, FIFO/LIFO, ties/capacity
- Fresh-process persistence regression
- Zero-warning build and ownership review

**Explain verbally**

The direct test file validates the structures and managers. Mention that AddressSanitizer leak detection was unsupported in the test environment; do not claim it proved no leaks.

## Slide 10 — Conclusion and Future Scope

**Slide content**

- Each DSA has a practical role
- Local, readable, student-level implementation
- Persistent prototype with clear limitations
- Future: GUI/web, secure passwords, database, scalable capacity

**Explain verbally**

Conclude that the project demonstrates a complete DSA workflow without pretending to be production-ready. Future ideas are not part of the current implementation.
