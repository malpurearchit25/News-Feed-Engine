# Demonstration Guide

This guide follows the real menu in `src/main.cpp`. It is designed for a rehearsal copy of the project so that registration names and post IDs are predictable. Do not erase or overwrite the submission's `data/` files for a demo; use a disposable copy if a clean start is needed.

Use four unused usernames below, or add a unique suffix if they already exist:

```text
alice_demo   password: a1   name: Alice Demo
bob_demo     password: b1   name: Bob Demo
charlie_demo password: c1   name: Charlie Demo
david_demo   password: d1   name: David Demo
```

On a clean rehearsal copy, note the post IDs that the program prints. They normally begin at 101, but the safest approach is always to use the actual displayed ID in later steps.

## 1. Start the Application

Compile and start from the project root:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic \
    src/main.cpp src/storage/FileManager.cpp \
    -o news_feed
./news_feed
```

Expected result: the program prints its title, says it is loading saved data, and shows the menu. A clean copy may say that no saved data was found.

DSA point: the program will reconstruct its data structures from the five text files before user actions begin.

## 2. Register Four Users — Hash Table

Repeat menu option **1. Register** four times. Enter the four user records above.

Expected result: each accepted record prints `Registration successful.`

DSA point: `UserManager` inserts each `User` into the custom 101-slot hash table. Usernames are its lookup key; collisions are handled with linear probing.

## 3. Search a User — Hash Table Lookup

Choose menu option **3. Search User**.

Enter `bob_demo`.

Expected result: Bob's ID, username, and name are displayed. This option does not require login.

DSA point: the same hash/probe sequence used during insertion is used to find the username. Average lookup is O(1).

## 4. Create Posts — Singly Linked List

Create posts in this order. Log in and out using option **2. Login** and option **5. Logout** as shown.

1. Choose **2**, log in as `alice_demo` / `a1`.
2. Choose **6. Create Post** and enter `Graph learning notes from Alice`.
3. Record the printed post ID as **Alice's post ID**.
4. Choose **5** to log out.
5. Choose **2**, log in as `bob_demo` / `b1`.
6. Choose **6** and enter `Graph data structures by Bob`.
7. Record the printed post ID as **Bob's post ID**.
8. Choose **5** to log out.
9. Choose **2**, log in as `charlie_demo` / `c1`.
10. Choose **6** and enter `Graph algorithms by Charlie`.
11. Record the printed post ID as **Charlie's post ID**.
12. Choose **5** to log out.
13. Choose **2**, log in as `david_demo` / `d1`.
14. Choose **6** and enter `Ranking graph content by David`.
15. Record the printed post ID as **David's post ID**.
16. Choose **5** to log out.

Optional check: choose option **7. View All Posts**. They appear in creation order.

DSA point: every post is appended to the tail of `PostLinkedList`, an O(1) operation. The displayed order is linked-list order.

## 5. Build Follow Relationships — Directed Graph

Create a directed chain `alice_demo -> bob_demo -> charlie_demo -> david_demo`.

1. Choose **2**, log in as `alice_demo` / `a1`.
2. Choose **11. Follow User**, enter `bob_demo`.
3. Choose **13. View Following**. Expected result: Bob is listed.
4. Choose **5**.
5. Choose **2**, log in as `bob_demo` / `b1`.
6. Choose **11**, enter `charlie_demo`.
7. Choose **13**. Expected result: Charlie is listed.
8. Choose **5**.
9. Choose **2**, log in as `charlie_demo` / `c1`.
10. Choose **11**, enter `david_demo`.
11. Choose **14. View Followers**. Expected result: Bob is listed as Charlie's follower.
12. Choose **5**.

DSA point: an edge `A -> B` means A follows B. The graph is directed: creating `alice_demo -> bob_demo` does not make Bob follow Alice. Each edge is stored in an adjacency list.

## 6. Show BFS Recommendations

1. Choose **2**, log in again as `alice_demo` / `a1`.
2. Choose **15. User Recommendations (BFS)**.

Expected result: `charlie_demo` is shown as a distance-two recommendation through Bob. David is distance three and is not a recommendation.

DSA point: BFS visits outward follow edges level by level and records distance. The program shows valid users at exactly distance two, excluding direct follows.

## 7. Show a DFS Connection Path

While still logged in as Alice:

1. Choose **16. Find Connection Path (DFS)**.
2. Enter `david_demo`.

Expected result: one directed path such as:

```text
alice_demo -> bob_demo -> charlie_demo -> david_demo
```

DSA point: recursive DFS explores a path through adjacency lists. A visited array prevents a follow cycle from causing infinite recursion. This is a path search, not a shortest-path feature.

## 8. Create Likes and Comments — Interaction Records

While still logged in as Alice:

1. Choose **17. Like Post** and enter **Bob's post ID**.
2. Choose **19. Add Comment**.
3. Enter **Bob's post ID** and then enter `Useful graph explanation`.
4. Record the printed comment ID.

Expected result: the like and comment succeed. Bob's post now has one like and one comment.

DSA point: likes use fixed `LikeRecord` storage and prevent a duplicate active like from the same user for the same post. Comments use the fixed comment array and update the post's count.

## 9. Undo the Latest Action — Stack

1. Choose **24. Undo Last Action (LIFO Stack)**.
2. Choose **20. View Comments** and enter **Bob's post ID**.

Expected result: the program prints `Undid COMMENT.` and then no longer shows Alice's comment for that post.

DSA point: the comment was the latest supported action, so it is at the top of Alice's per-user action stack. Undo pops it first and deletes that comment. The inverse action is not pushed back.

For a final feed with engagement, choose **19** once more, enter **Bob's post ID**, and enter `Useful graph explanation again`. This creates a new comment and does not change the fact that the previous action was successfully undone.

## 10. Complete Feed Candidates — Graph + Max Heap

Alice currently follows Bob. To include Charlie's post in Alice's feed:

1. Choose **11. Follow User**.
2. Enter `charlie_demo`.
3. Choose **25. View Personalized Feed (Max Heap Ranking)**.

Expected result: Bob's and Charlie's posts are candidates; David's post is excluded because Alice does not follow David. With the actions above, Bob's older post has a like and a comment, so it can outrank Charlie's newer post. The exact timestamps are dynamic.

In a clean run with only Bob and Charlie as candidates, the usual scores are:

```text
Bob:     3(1 like) + 5(1 comment) + 9 recency = 17
Charlie: 0 + 0 + 10 recency = 10
```

DSA point: the feed selects posts by checking the directed graph, calculates a temporary score, inserts candidates into the custom max heap, and extracts the highest priority first. Equal feed scores use higher post ID first.

## 11. Demonstrate Merge Sort Search

1. Choose **9. Search Posts**.
2. Enter the keyword `graph`.
3. At `Choose result order:`, enter **1** for `Newest First (Merge Sort)`.

Expected result: the matching posts appear in descending post-ID order. The program labels the results `Search Results - Newest First`.

DSA point: matching posts are collected as temporary pointers, then manual Merge Sort orders them by decreasing sequential post ID. The permanent linked list is not rearranged.

## 12. Demonstrate Quick Sort Search

1. Choose **9. Search Posts** again.
2. Enter `graph`.
3. At `Choose result order:`, enter **2** for `Most Engaging First (Quick Sort)`.

Expected result: Bob's post should appear above zero-engagement matching posts and the output shows each engagement score. Engagement is `3 × likes + 5 × comments`; it does not include feed recency bonus.

DSA point: Quick Sort partitions the temporary pointer array around the final element as pivot. A tie in engagement uses higher post ID first.

## 13. Demonstrate Notification FIFO Queue

Alice's normal follow, like, and comment actions created notifications for Bob. To inspect them:

1. Choose **5** to log out Alice.
2. Choose **2**, log in as `bob_demo` / `b1`.
3. Choose **22. View Notifications (FIFO Queue)**.
4. Choose **23. Process Oldest Notification (FIFO Queue)**.
5. Choose **22** again.

Expected result: the first display lists Bob's notifications in arrival order. Processing removes only the oldest one, normally the earlier `alice_demo followed you.` notification. The second display starts with the next remaining notification.

DSA point: each user's linked notification queue enqueues at the rear and dequeues from the front, so the behavior is FIFO.

## 14. Save and Prove Persistence

1. Choose **0. Exit**.
2. Expected result: the program prints `Saving data...` followed by `Data saved successfully.`
3. Start the program again with `./news_feed`.
4. At startup, expected result: `Data loaded successfully.`
5. Choose **3. Search User**, enter `alice_demo`, and confirm that the user exists.
6. Choose **2**, log in as `alice_demo` / `a1`.
7. Choose **13. View Following**. Bob and Charlie should still be listed.
8. Choose **8. View Single Post**, enter **Bob's post ID**, and confirm its likes/comments remain.
9. Choose **9**, search `graph`, choose **2**, and confirm persisted engagement can still be sorted.

To show session-only state, log out with **5**, log in as `bob_demo` / `b1`, and choose **22**. The queue is empty after restart even if notifications were pending before exit. Option **24** also reports that there are no actions to undo for a newly started session.

DSA point: users, posts, follows, likes, and comments are rebuilt from `data/` files. Login state, notifications, and undo stacks intentionally are not persisted.

## Short Demonstration Order

If time is limited, keep this order:

1. Register and search users — hash table.
2. Create posts — linked list.
3. Create a follow chain — directed graph.
4. Run recommendations and path search — BFS/DFS.
5. Like/comment, then undo a comment — arrays and stack.
6. Log in as the receiver and process notification — queue.
7. View feed — max heap.
8. Run both search orders — Merge Sort and Quick Sort.
9. Exit, restart, and verify a user, follow, and post — persistence.

Avoid promising exact timestamps or exact output ordering beyond the documented deterministic rules. Record the post and comment IDs printed during the live demonstration.
