# Escape Room: A Text-Based Adventure in C

A console-based escape room game written in pure C for the **Programming Fundamentals (CS-1002)** course at FAST-NUCES Karachi. You wake up on the floor of a strange laboratory with no memory and the door locked. Explore three connected areas, collect items, solve puzzles, cross a maze, and find your way out of the building.

---

## Table of Contents

- [Features](#features)
- [Story](#story)
- [Getting Started](#getting-started)
- [Gameplay Overview](#gameplay-overview)
- [Controls](#controls)
- [Scoring and Timer](#scoring-and-timer)
- [Project Structure](#project-structure)
- [C Concepts Demonstrated](#c-concepts-demonstrated)
- [Known Limitations](#known-limitations)
- [Authors](#authors)

---

## Features

- **Three explorable areas**: the Laboratory, the Statue Chamber and the Archive, with free movement back and forth between them.
- **Randomised four-digit door code**: the digits are regenerated every new game, so no two playthroughs are identical.
- **Four maze layouts**: one is picked at random each game, with fog of war that reveals nearby cells and remembers explored areas.
- **Limited inventory**: only 3 items can be carried at once. Discarded items return to where you found them.
- **Item-based puzzles**: five items (Rubber glove, Glue tube, Old key, Small key and Brass key) are used to unlock progress or earn bonus points.
- **Score and timer system**: mistakes and wasted actions cost points and time, and some puzzles give bonus points.
- **Robust input handling**: invalid numbers, extra characters and overlong text input are handled without crashing the game.
- **Menu system**: start a new game, read the instructions, or exit.
- **Coloured, boxed output** using ANSI escape codes.

---

## Story

You wake beside a shattered containment cylinder in an unfamiliar laboratory, with a throbbing head and a blank memory. The heavy steel door is sealed. Notes, a computer terminal and hidden files slowly reveal who you are and what happened here. Piece it together, and get out.

---

## Getting Started

**Requirements:** a C compiler such as GCC, and a terminal that supports ANSI colour codes. The game was compiled and played through with GCC on Linux. The code also contains a Windows-only line that switches on coloured text in the Windows console.

**Compile:**

```bash
gcc -o escape_room escape_room_v2.c
```

**Run:**

```bash
./escape_room
```

On Windows, run `escape_room.exe` instead.

---

## Gameplay Overview

| Area | Description |
|------|-------------|
| **Room 1: The Laboratory** | Search the experiment tubes, cupboard, picture frame, centre table, notebook on the desk and the trash bin. Gather clues to build the four-digit code for the door. |
| **Room 2: The Statue Chamber** | Search the washroom and wardrobe, use the old computer terminal, examine the statue and find the key to the next door. |
| **Room 3: The Archive** | Cross a randomly chosen maze, then use the table, wall safe, damaged panel and your items to reach the fingerprint-locked exit. |

**Items:** Rubber glove, Glue tube, Old key, Small key and Brass key. Some are needed to escape and some only earn bonus points, and you can carry only three at a time.

> **Tip:** Items you don't need right now can be discarded and picked up again later. Think carefully about what to carry, since you only have three slots.

---

## Controls

### Menus and Rooms

Enter the **number** shown next to an option and press **Enter**. For yes/no questions, type `y` or `n`. In Room 1, option `0` quits to the main menu (progress is lost).

### Maze Navigation

Keys are not case sensitive, and each move must be a single key.

| Key | Action |
|-----|--------|
| `W` | Move up |
| `A` | Move left |
| `S` | Move down |
| `D` | Move right |
| `B` | Go back to Room 2 (progress is saved) |

### Map Legend

| Symbol | Meaning |
|--------|---------|
| `@` | You |
| `?` | Unexplored |
| `#` | Wall |
| `.` | Path |
| `S` / `E` | Start / Exit |

---

## Scoring and Timer

- You start with **1000 points**, and your score never drops below 0.
- The timer shows real elapsed time plus time penalties. It is **not** a countdown in this version.
- Your final time and score are shown when the game ends.

| Event | Score | Time |
|-------|-------|------|
| Wrong riddle or sequence answer | -15 | +5 s |
| Wrong door code | -25 | +10 s |
| Wrong terminal password | -15 | +5 s |
| Spiral passage dead end (first visit) | -10 | +30 s |
| Searching the middle experiment tube | none | +15 s |
| Each maze step | none | +2 s |
| Archive table riddle solved | +25 | none |
| Wall safe opened | +50 | none |
| Damaged panel repaired | +50 | none |

---

## Project Structure

```
.
├── escape_room_v2.c   # complete game source code (single file)
└── README.md          # this file
```

Inside `escape_room_v2.c`, the code is split into small functions:

| Part | Main functions |
|------|----------------|
| Game state | `GameState`, `initialiseGame` |
| Menus and display | `mainMenu`, `showInstructions`, `showStatus`, `printBoxHeader`, `printStoryBox` |
| Rooms | `room1`, `room2`, `room3` |
| Room actions | `searchRoom1Object`, `attemptRoom1Door`, `interactStatue`, `attemptRoom2Computer`, `attemptRoom3Door`, `interactSafe`, `interactDamagedPanel`, `attemptFinalDoor` |
| Maze | `navigateMaze`, `revealNearby`, `displayMaze` |
| Inventory | `addItem`, `hasItem`, `consumeItem`, `returnItemToOrigin`, `manageInventory`, `selectInventoryItem` |
| Input | `readInteger`, `readText`, `trimWhitespace`, `sameText`, `askYesNo`, `pressEnter` |
| Score and timer | `addPenalty`, `elapsedSeconds` |

---

## C Concepts Demonstrated

- Structures (`struct`) and `typedef` for managing complete game state
- Passing structures by **pointer** to functions
- Arrays and **2D arrays** (maze layouts, explored-cell tracking)
- Arrays of strings and `const` data
- Modular programming with function prototypes
- Loops, `switch` statements and conditional logic
- String handling: `strcmp`, `strlen`, `toupper`, custom case-insensitive comparison
- Safe input using `fgets` and `sscanf` instead of raw `scanf`
- Random number generation with `rand()` and `srand()`
- Time handling with `time()` and `difftime()`
- Macros (`#define`) for readable constants

---

## Known Limitations

- Progress is lost when the game is closed or you quit to the main menu. There is no save or load.
- The game is console-only, and coloured output needs a terminal that supports ANSI escape codes.
- Only the four code digits, the riddle for the GREEN digit and the maze change between games. The other puzzles and item locations are the same every time.
- The four mazes are written out by hand in the code.
- The timer only counts upward. There is no time limit.
- A very long number typed at a numeric prompt is read as two separate entries. Text prompts discard the extra characters. Neither case crashes the game.
- There is no hint system, and the game supports one player only.

---

## Authors

**Ayan Gul Memon** (Roll No: 26K-2060)
**M. Arsalan Zafar** (Roll No: 26K-2036)
**Choudhry Zoraiz Qadir** (Roll No: 26K-2037)

BS Cyber Security, Section BS-CYS-1B
FAST-NUCES Karachi
