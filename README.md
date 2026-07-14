# Movies Data Manager

A C++ console application that loads IMDB movie metadata and organizes it with classic data structures including AVL trees, hash tables, graphs, stacks, and queues. Users can search films, explore actor networks, and get graph-based recommendations.

## Features

**Search**
- Movie by title (AVL tree)
- Movies by actor or genre (hash tables)
- Movies by release year or IMDB rating range

**Actors**
- Print an actor’s filmography
- List co-actors who appeared in the same films

**Recommendations & paths**
- BFS / DFS movie recommendations based on shared actors or genres
- Shortest path between two movies (BFS)
- Alternate path discovery (DFS)

**Management**
- Display all movies (alphabetical, AVL in-order traversal)
- Delete a movie
- Rebuild the recommendation graph after changes

## Data structures

| Structure | Role |
|-----------|------|
| **AVL tree** | Movies keyed by title — balanced insert, delete, and search |
| **Actor hash table** | Fast actor lookup, filmography, and co-actor queries |
| **Genre hash table** | Genre → list of movies |
| **Graph** | Movies as vertices; edges when films share actors or genres |
| **Stack / Queue** | Support DFS and BFS traversals |
| **Linked lists** | Genres per movie, actors per movie, adjacency lists, chaining |

## Dataset

Uses `movie_metadata.csv` (IMDB-style metadata: title, director, actors, genres, year, rating, and related fields). On startup the program parses the CSV into the AVL tree and both hash tables, then builds the recommendation graph.

## Project structure

```
Project/
├── main.cpp              # Implementation + interactive menu
├── main.h                # Class and function declarations
├── movie_metadata.csv    # Movie dataset
└── README.md
```

## Build & run

Requires a C++ compiler with C++11 or later (g++, Clang, or MSVC).

**g++ / MinGW**

```bash
g++ -o movies main.cpp -std=c++11
./movies
```

**MSVC (Developer Command Prompt)**

```bash
cl /EHsc /Fe:movies.exe main.cpp
movies.exe
```

Keep `movie_metadata.csv` in the same directory as the executable.

## Usage

Run the program and use the numbered menu:

```
========== MOVIES DATA MANAGER MENU ==========
--- SEARCH OPERATIONS ---
1.  Search movie by title
2.  Search movies by actor
3.  Search movies by genre
4.  Search movies by year
5.  Search movies by rating range
6.  Print actor filmography
7.  Get co-actors of an actor
8.  Display all movies
9.  Get movie recommendations (BFS)
10. Get movie recommendations (DFS)
11. Find shortest path between two movies (BFS)
12. Find shortest path between two movies (DFS)
13. Delete a movie
14. Rebuild graph
0.  Exit
```

Example searches: `Avatar`, `Johnny Depp`, `Action`, year `2009`, rating range `7.0`–`9.0`.

## How recommendations work

The graph connects two movies if they share at least one actor or one genre. BFS recommendations return nearby films by hop distance; DFS explores related films by depth-first traversal. After deleting a movie, choose option **14** to rebuild the graph so recommendations stay accurate.

## Requirements

- C++11-compatible compiler
- `movie_metadata.csv` beside the binary
- Console / terminal for the interactive menu

## License

Academic / coursework project. Use and adapt freely for learning.
