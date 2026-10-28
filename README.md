# NewsFeedEngine

A C++17 console social media engine built with custom data structures and algorithms. Register users, publish posts, follow people, and explore a personalized feed ranked by engagement and recency.

## Features

- User registration, login, and username lookup using an open-addressed hash table.
- Post creation, search, and deletion using a singly linked list.
- Follow relationships modeled as a directed adjacency-list graph.
- BFS friend recommendations and DFS connection paths.
- Likes, comments, FIFO notifications, and per-user LIFO undo.
- Personalized feed ranking with a custom max heap; retains the latest 500 eligible posts.
- Case-insensitive search with merge sort and quicksort ordering.
- Local text-file persistence, restored on startup and saved on normal exit.

## Build and run

Requirements: a C++17 compiler and CMake 3.16 or newer. Supports Windows, Linux, and macOS.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

Run from the repository root so the application uses the expected `data/` folder:

```sh
# Linux / macOS / single-configuration builds
./build/news_feed
# Windows with Visual Studio
.\build\Debug\news_feed.exe
```

Without CMake (GCC):

```sh
g++ -std=c++17 -Wall -Wextra -pedantic src/main.cpp src/storage/FileManager.cpp -o news_feed
./news_feed
```

## Try it

1. Register two fictional users, then log in as the first and create a post.
2. Log out and log in as the second user.
3. Follow the first user, like their post, and add a comment.
4. Choose **25** to see the ranked feed, **24** to undo, or **15** for recommendations.
5. Choose **0** to save and exit. Restart to restore the saved state.

The first run starts empty. Local user data is ignored by Git and is never needed to build the project.

## How ranking works

`score = 3 × likes + 5 × comments + recency bonus`

The newest eligible candidate gets a bonus of 10, decreasing to zero for older candidates. A max heap displays the highest score first; ties favor the higher post ID. A circular buffer keeps the newest 500 candidates when more are available.

## Project layout

| Path | Contents |
| --- | --- |
| `src/datastructures/` | Hash table, linked list, graph, queue, stack, heap |
| `src/algorithms/` | Merge sort and quicksort |
| `src/services/` | User, post, social, interaction, feed, notification, undo managers |
| `src/storage/` | Text-file persistence |
| `tests/` | Regression coverage, including feed overflow behavior |
| `docs/` | Architecture, complexity analysis, demo and course notes |

See [the technical guide](docs/TECHNICAL_GUIDE.md) for architecture, persistence formats, and complexity, or [the demo guide](docs/DEMO_GUIDE.md) for a walkthrough. GitHub Actions builds and tests the project on Linux, Windows, and macOS.

## Scope and limitations

This is an academic Data Structures project, intended for local demonstrations. Passwords are stored in plaintext: use fictional accounts and disposable passwords only. It has no production authentication or network service.

Capacities are intentionally bounded: 101 users, 500 feed/search candidates and comments, and 1,000 active likes. Notifications and undo histories last for one session. Data is saved on normal exit; unexpected termination can lose changes. Persistence rewrites multiple files and is not transactional. Run only one instance per data directory.

The original course documentation is retained as reference material; historical testing statements describe that earlier version. Run the included suite to verify your current build.
