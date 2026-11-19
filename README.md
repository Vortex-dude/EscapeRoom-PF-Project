#  Escape Room — A Text-Based Adventure in C
 
A console-based escape room game written in pure C for the **Programming Fundamentals (CS-1002)** course. You wake up on the floor of a strange laboratory with the door locked. Explore three connected areas, collect items, solve puzzles, cross a maze, and find your way out of the building.
 
---
 
##  Table of Contents
 
- [Features](#-features)
- [Gameplay Overview](#-gameplay-overview)
- [Getting Started](#-getting-started)
- [Controls](#-controls)
- [Scoring and Timer](#-scoring-and-timer)
- [Project Structure](#-project-structure)
- [C Concepts Demonstrated](#-c-concepts-demonstrated)
- [Known Limitations](#-known-limitations)
- [Author](#-author)
---
 
##  Features
 
- **Three explorable areas**: Laboratory, Statue Room and Final Area, with free movement back and forth between them.
- **Randomised four-digit door code**: the digits are regenerated every new game, so no two playthroughs are identical.
- **Four maze layouts**: one is picked at random each game, with fog-of-war that reveals nearby cells and remembers explored areas.
- **Limited inventory**: only 3 items can be carried at once. Discarded items return to where you found them.
- **Item-based puzzles**: five items (rubber glove, glue tube, useless key, statue key, Room 3 key) used to unlock progress.
- **Score and timer system**: mistakes and wasted actions cost points and time.
- **Robust input handling**: invalid numbers, extra characters and overlong input are all handled safely.
- **Menu system**: start a new game, read the instructions, or exit.
---
 
##  Gameplay Overview
 
| Area | Description |
|------|-------------|
| **Room 1: Laboratory** | Search tubes, a cupboard, a picture frame, a table, a desk and the trash. Gather clues to build the four-digit code for the door. |
| **Room 2: Statue Room** | Search the washroom and wardrobe, use an old computer, inspect the statue and find the key to the next door. |
| **Room 3: Maze + Final Area** | Cross a randomly chosen maze, then solve the remaining puzzles to reach the fingerprint-locked exit. |
 
>  **Tip:** Items you don't need right now can be discarded and picked up again later. Think carefully about what to carry, since you only have three slots.
 
---
 
##  Controls
 
### Menus and Rooms
Enter the **number** shown next to an option and press **Enter**.
 
### Maze Navigation
 
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
 
##  Scoring and Timer
 
- You start with **1000 points**.
- Wrong answers, wrong door codes and dead-end detours deduct points and add **time penalties**.
- Every maze step and some searches add a small time penalty.
- Solving certain puzzles earns **bonus points**.
- The timer tracks elapsed time (it is **not** a countdown in this version). Your final time and score are shown when the game ends.
---
 
## 📚 C Concepts Demonstrated
 
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
 
##  Authors

 **Ayan Gul Memon**
 Roll No: 26K-2060
 **M.Arsalan Zafar**
 Roll No: 26K-2036
**Choudhry Zoraiz Qadir**
Roll No: 26K-2037

BS Cyber Security, Section BS-CYS-1B
FAST-NUCES Karachi
