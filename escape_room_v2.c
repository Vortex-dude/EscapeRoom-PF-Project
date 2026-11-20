#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define CLR_RESET   "\033[0m"
#define CLR_BOLD    "\033[1m"
#define CLR_DIM     "\033[2m"

#define CLR_RED     "\033[1;31m"
#define CLR_GREEN   "\033[1;32m"
#define CLR_YELLOW  "\033[1;33m"
#define CLR_BLUE    "\033[1;34m"
#define CLR_MAGENTA "\033[1;35m"
#define CLR_CYAN    "\033[1;36m"
#define CLR_WHITE   "\033[1;37m"
#define CLR_GRAY    "\033[0;37m"

#define INVENTORY_CAPACITY 3
#define MAZE_ROWS 11
#define MAZE_COLS 19
#define ITEM_COUNT 5

#define GLOVE 0
#define GLUE 1
#define OLD_KEY 2
#define SMALL_KEY 3
#define BRASS_KEY 4

#define AT_ORIGIN 0
#define IN_INVENTORY 1
#define USED 2

typedef struct
{
    int currentRoom;
    int gameOver;
    int escaped;
    int score;
    int timePenalty;
    time_t startTime;

    int itemStatus[ITEM_COUNT];
    int inventory[INVENTORY_CAPACITY];
    int inventoryCount;

    int room1Checks[7];
    int clueFound[4];
    int codeDigits[4];
    int room1Unlocked;

    int room2Checks[5];
    int wardrobePasswordFound;
    int computerRead;
    int statueKeyholeSeen;
    int room3Unlocked;
    int spiralVisited;

    int mazeIndex;
    int mazeRow;
    int mazeCol;
    int mazeExplored[MAZE_ROWS][MAZE_COLS];
    int mazeComplete;

    int room3Checks[3];
    int room3PuzzleSolved;
    int safeOpened;
    int glueUsed;
} GameState;

const char *mazeLayouts[4][MAZE_ROWS] =
{
    {
        "###################",
        "#S................#",
        "#################.#",
        "#.###############.#",
        "#.###############.#",
        "#.................#",
        "#.#################",
        "#.#################",
        "#.#################",
        "#.................E",
        "###################"
    },
    {
        "###################",
        "#S......#.........#",
        "#######.#.#######.#",
        "#.......#.....#...#",
        "#.###########.#.###",
        "#...........#.#...#",
        "###########.#.###.#",
        "#...........#.....#",
        "#.###############.#",
        "#.................E",
        "###################"
    },
    {
        "###################",
        "#S....#...........#",
        "#.##.##.#########.#",
        "#..#....#.........#",
        "##.######.#######.#",
        "#..............#..#",
        "#.############.#.##",
        "#..............#..#",
        "##############.#.##",
        "#.................#",
        "#################E#"
    },
    {
        "###################",
        "#S........#.......#",
        "#########.#.#####.#",
        "#.........#.....#.#",
        "#.#############.#.#",
        "#...............#.#",
        "#.###############.#",
        "#.................#",
        "#################.#",
        "#.................E",
        "###################"
    }
};

const char *itemNames[ITEM_COUNT] =
{
    "Rubber glove",
    "Glue tube",
    "Old key",
    "Small key",
    "Brass key"
};

const char *colourNames[4] = {"BLUE", "RED", "GREEN", "YELLOW"};
const char *colourAnsi[4]  = {CLR_BLUE, CLR_RED, CLR_GREEN, CLR_YELLOW};

const int doorColourOrder[4] = {2, 0, 3, 1};

const char *digitRiddles[10] =
{
    "\"I am a ring with nothing inside, the start of counting before one is tried.\"",
    "\"I stand alone, a solitary line, the very first step in any design.\"",
    "\"Eyes to see with, hands to hold, a pair of twins in stories told.\"",
    "\"Sides of a triangle, points on a crown, three blind mice running through town.\"",
    "\"Legs on a table, seasons in a year, corners of a square standing clear.\"",
    "\"Fingers on a hand, points on a star, half of ten, not very far.\"",
    "\"Faces on a cube, legs on a bee, turn me upside down and nine I will be.\"",
    "\"Days in a week, colours in the sky, lucky for some as the numbers go by.\"",
    "\"Legs on a spider, corners of a cube, lay me on my side for an endless loop.\"",
    "\"One less than ten, the highest single sign, turn me upside down and six is mine.\""
};

const char *digitWords[10] =
{
    "ZERO", "ONE", "TWO", "THREE", "FOUR",
    "FIVE", "SIX", "SEVEN", "EIGHT", "NINE"
};

void enableAnsiSupport(void);
void printBoxHeader(const char *title, const char *subtitle, const char *colour);
void printStoryBox(const char *heading, const char *lines[], int lineCount);
void printSuccess(const char *msg);
void printError(const char *msg);
void printClue(const char *msg);
void printInfo(const char *msg);
void initialiseGame(GameState *game);
void mainMenu(void);
void playGame(GameState *game);
void showStatus(const GameState *game);
void showInstructions(void);
void room1(GameState *game);
void room2(GameState *game);
void room3(GameState *game);
void searchRoom1Object(GameState *game, int object);
void searchRoom2Object(GameState *game, int object);
void searchRoom3Object(GameState *game, int object);
void attemptRoom1Door(GameState *game);
void attemptRoom2Computer(GameState *game);
void interactStatue(GameState *game);
void attemptRoom3Door(GameState *game);
void interactSafe(GameState *game);
void interactDamagedPanel(GameState *game);
void attemptFinalDoor(GameState *game);
void navigateMaze(GameState *game);
void revealNearby(GameState *game);
void displayMaze(const GameState *game);
void manageInventory(GameState *game);
int selectInventoryItem(const GameState *game, const char *promptText);
int hasItem(const GameState *game, int item);
int addItem(GameState *game, int item);
void consumeItem(GameState *game, int item);
void returnItemToOrigin(GameState *game, int item);
int readInteger(const char *prompt);
void readText(const char *prompt, char text[], int size);
void trimWhitespace(char *str);
int sameText(const char *a, const char *b);
int askYesNo(const char *prompt);
void addPenalty(GameState *game, int seconds, int points);
int elapsedSeconds(const GameState *game);
void pressEnter(void);

int main()
{
    enableAnsiSupport();
    srand((unsigned int)time(NULL));
    mainMenu();
    return 0;
}

void enableAnsiSupport()
{
#ifdef _WIN32
    system("");
#endif
}

void printBoxHeader(const char *title, const char *subtitle, const char *colour)
{
    int width = 56;
    int titleLen = (int)strlen(title);
    int leftPad = (width - titleLen) / 2;
    int rightPad = width - titleLen - leftPad;
    int i;

    printf("\n%s+", colour);
    for (i = 0; i < width; i++) printf("-");
    printf("+\n|");
    for (i = 0; i < leftPad; i++) printf(" ");
    printf("%s", title);
    for (i = 0; i < rightPad; i++) printf(" ");
    printf("|\n");

    if (subtitle != NULL && subtitle[0] != '\0')
    {
        int subLen = (int)strlen(subtitle);
        int sLeft = (width - subLen) / 2;
        int sRight = width - subLen - sLeft;
        printf("|");
        for (i = 0; i < sLeft; i++) printf(" ");
        printf("%s", subtitle);
        for (i = 0; i < sRight; i++) printf(" ");
        printf("|\n");
    }

    printf("+");
    for (i = 0; i < width; i++) printf("-");
    printf("+%s\n", CLR_RESET);
}

void printStoryBox(const char *heading, const char *lines[], int lineCount)
{
    int i;
    printf("\n%s+--------------------------------------------------------+\n", CLR_MAGENTA);
    printf("| %-54s |\n", heading);
    printf("+--------------------------------------------------------+%s\n", CLR_RESET);
    for (i = 0; i < lineCount; i++)
    {
        printf("%s  %s%s\n", CLR_WHITE, lines[i], CLR_RESET);
    }
    printf("%s+--------------------------------------------------------+%s\n", CLR_MAGENTA, CLR_RESET);
}

void printSuccess(const char *msg)
{
    printf("%s[+] %s%s\n", CLR_GREEN, msg, CLR_RESET);
}

void printError(const char *msg)
{
    printf("%s[!] %s%s\n", CLR_RED, msg, CLR_RESET);
}

void printClue(const char *msg)
{
    printf("%s[*] %s%s\n", CLR_YELLOW, msg, CLR_RESET);
}

void printInfo(const char *msg)
{
    printf("%s[-] %s%s\n", CLR_CYAN, msg, CLR_RESET);
}

void mainMenu()
{
    int choice;
    GameState game;

    do
    {
        printBoxHeader("PROJECT ECHO: ESCAPE ROOM", "VERSION 2.0  |  MEMORY RECOVERY PROTOCOL", CLR_CYAN);
        printf("  %s1.%s Start a new game\n", CLR_BOLD, CLR_RESET);
        printf("  %s2.%s Instructions & Rules\n", CLR_BOLD, CLR_RESET);
        printf("  %s3.%s Exit\n", CLR_BOLD, CLR_RESET);

        choice = readInteger("\nEnter your choice (1-3): ");

        if (choice == 1)
        {
            initialiseGame(&game);
            playGame(&game);
        }
        else if (choice == 2)
        {
            showInstructions();
        }
        else if (choice == 3)
        {
            printf("\n%sSession terminated. Thank you for playing.%s\n", CLR_CYAN, CLR_RESET);
        }
        else
        {
            printError("Invalid choice. Please select 1, 2, or 3.");
        }

    } while (choice != 3);
}

void initialiseGame(GameState *game)
{
    int i;
    int r;
    int c;

    game->currentRoom = 1;
    game->gameOver = 0;
    game->escaped = 0;
    game->score = 1000;
    game->timePenalty = 0;
    game->startTime = time(NULL);

    game->inventoryCount = 0;

    for (i = 0; i < ITEM_COUNT; i++)
    {
        game->itemStatus[i] = AT_ORIGIN;
    }

    for (i = 0; i < INVENTORY_CAPACITY; i++)
    {
        game->inventory[i] = -1;
    }

    for (i = 0; i < 7; i++)
    {
        game->room1Checks[i] = 0;
    }

    for (i = 0; i < 4; i++)
    {
        game->clueFound[i] = 0;
        game->codeDigits[i] = rand() % 10;
    }

    game->room1Unlocked = 0;

    for (i = 0; i < 5; i++)
    {
        game->room2Checks[i] = 0;
    }

    game->wardrobePasswordFound = 0;
    game->computerRead = 0;
    game->statueKeyholeSeen = 0;
    game->room3Unlocked = 0;
    game->spiralVisited = 0;

    game->mazeIndex = rand() % 4;
    game->mazeRow = 1;
    game->mazeCol = 1;
    game->mazeComplete = 0;

    for (r = 0; r < MAZE_ROWS; r++)
    {
        for (c = 0; c < MAZE_COLS; c++)
        {
            game->mazeExplored[r][c] = 0;
        }
    }

    for (i = 0; i < 3; i++)
    {
        game->room3Checks[i] = 0;
    }

    game->room3PuzzleSolved = 0;
    game->safeOpened = 0;
    game->glueUsed = 0;
}

void playGame(GameState *game)
{
    printf("\n%sCold fluorescent light flickers overhead as you regain consciousness.%s\n", CLR_WHITE, CLR_RESET);
    printf("You are lying beside a shattered containment cylinder inside an unfamiliar\n");
    printf("laboratory. Your head throbs, and your memory is blank. The heavy steel\n");
    printf("door ahead is sealed. You need to piece together who you are and escape.\n");

    while (!game->gameOver && !game->escaped)
    {
        if (game->currentRoom == 1)
        {
            room1(game);
        }
        else if (game->currentRoom == 2)
        {
            room2(game);
        }
        else if (game->currentRoom == 3)
        {
            room3(game);
        }
    }

    if (game->escaped)
    {
        const char *endingLines[] = {
            "The security bulkhead slides open into the cool night air.",
            "Clutching the recovered Project Echo logs, Dr. Elias Voss",
            "steps clear of the subterranean facility. His memories may",
            "still be fractured, but the truth is finally out."
        };
        printBoxHeader("ESCAPE SUCCESSFUL", "SUBJECT E-01 HAS EXITED THE FACILITY", CLR_GREEN);
        printStoryBox("EPILOGUE", endingLines, 4);
    }

    printf("\n%s+--------------------------------------------------------+%s\n", CLR_CYAN, CLR_RESET);
    printf("  Final Time  : %02d minute(s), %02d second(s)\n",
           elapsedSeconds(game) / 60, elapsedSeconds(game) % 60);
    printf("  Final Score : %s%d%s\n", CLR_YELLOW, game->score, CLR_RESET);
    printf("%s+--------------------------------------------------------+%s\n", CLR_CYAN, CLR_RESET);
    pressEnter();
}

void showInstructions(void)
{
    printBoxHeader("INSTRUCTIONS & GAMEPLAY", "HOW TO SURVIVE PROJECT ECHO", CLR_CYAN);
    printf("  * Explore each room and examine objects carefully.\n");
    printf("  * Collect items and solve puzzles to unlock new areas.\n");
    printf("  * Your inventory holds a maximum of %s3 items%s at once.\n", CLR_YELLOW, CLR_RESET);
    printf("  * Discarded items return to their original locations so\n");
    printf("    you can always retrieve them later.\n");
    printf("  * Successfully used items are consumed; wrong attempts\n");
    printf("    leave your items intact.\n");
    printf("  * You may revisit earlier rooms at any time.\n");
    printf("  * In the Archive maze, move with %sW, A, S, D%s. Explored\n", CLR_GREEN, CLR_RESET);
    printf("    corridors remain mapped even if you turn back.\n");
    printf("  * Wrong answers and hazardous detours add time and\n");
    printf("    reduce your score.\n");
    pressEnter();
}

void showStatus(const GameState *game)
{
    int seconds = elapsedSeconds(game);
    const char *roomName = "THE LABORATORY";

    if (game->currentRoom == 2) roomName = "THE STATUE CHAMBER";
    else if (game->currentRoom == 3) roomName = "THE ARCHIVE";

    printf("\n%s+--------------------------------------------------------+%s\n", CLR_GRAY, CLR_RESET);
    printf("  %sROOM %d:%s %-20s  %sTIME:%s %02d:%02d   %sSCORE:%s %d\n",
           CLR_CYAN, game->currentRoom, CLR_WHITE, roomName,
           CLR_YELLOW, CLR_RESET, seconds / 60, seconds % 60,
           CLR_GREEN, CLR_RESET, game->score);
    printf("%s+--------------------------------------------------------+%s\n", CLR_GRAY, CLR_RESET);
}

void room1(GameState *game)
{
    int choice;

    while (game->currentRoom == 1 && !game->gameOver && !game->escaped)
    {
        showStatus(game);
        printBoxHeader("ROOM 1: THE LABORATORY", "SUB-LEVEL BIOMETRIC RESEARCH WING", CLR_BLUE);
        printf("  1. Inspect the upper broken experiment tube\n");
        printf("  2. Search the middle broken experiment tube\n");
        printf("  3. Open the metal cupboard\n");
        printf("  4. Examine the wall picture frame\n");
        printf("  5. Inspect the centre laboratory table\n");
        printf("  6. Read the notebook on the desk\n");
        printf("  7. Search the trash bin\n");
        printf("  8. Approach the heavy door to Room 2\n");
        printf("  9. View / manage inventory (%d/%d)\n", game->inventoryCount, INVENTORY_CAPACITY);
        printf("  0. Quit to main menu\n");

        choice = readInteger("\nChoose an action: ");

        switch (choice)
        {
            case 1: searchRoom1Object(game, 0); break;
            case 2: searchRoom1Object(game, 1); break;
            case 3: searchRoom1Object(game, 2); break;
            case 4: searchRoom1Object(game, 3); break;
            case 5: searchRoom1Object(game, 4); break;
            case 6: searchRoom1Object(game, 5); break;
            case 7: searchRoom1Object(game, 6); break;
            case 8: attemptRoom1Door(game); break;
            case 9: manageInventory(game); break;
            case 0:
                if (askYesNo("Return to the main menu? Current progress will be lost (y/n): "))
                {
                    game->gameOver = 1;
                }
                break;
            default:
                printError("Invalid choice. Please enter a number from 0 to 9.");
        }
    }
}

void searchRoom1Object(GameState *game, int object)
{
    game->room1Checks[object]++;

    if (object == 0)
    {
        printf("\nYou peer into the upper shattered experiment tube.\n");
        printf("Etched onto the inner glass casing is a glowing numeral.\n");
        printf("%s[*] Clue Discovered - BLUE digit: %d%s\n", CLR_BLUE, game->codeDigits[0], CLR_RESET);
        game->clueFound[0] = 1;
    }
    else if (object == 1)
    {
        printf("\nYou carefully sift through jagged glass in the middle tube.\n");
        addPenalty(game, 15, 0);
        printInfo("Searching carefully takes an extra 15 seconds.");

        if (game->itemStatus[GLUE] == AT_ORIGIN)
        {
            printf("Beneath a cracked sensor housing, you spot a small tube of industrial adhesive.\n");
            if (askYesNo("Take the Glue tube? (y/n): "))
            {
                if (addItem(game, GLUE))
                {
                    printSuccess("Glue tube added to your inventory.");
                }
            }
            else
            {
                printInfo("You leave the Glue tube in the middle tube.");
            }
        }
        else
        {
            printf("Only shattered glass remains inside.\n");
        }
    }
    else if (object == 2)
    {
        printf("\nYou pull open the dented metal cupboard.\n");
        printf("Stenciled on the inside of the door is a crimson digit.\n");
        printf("%s[*] Clue Discovered - RED digit: %d%s\n", CLR_RED, game->codeDigits[1], CLR_RESET);
        game->clueFound[1] = 1;

        if (game->itemStatus[GLOVE] == AT_ORIGIN)
        {
            printf("Resting on the bottom shelf is a single thick rubber glove.\n");
            if (askYesNo("Take the Rubber glove? (y/n): "))
            {
                if (addItem(game, GLOVE))
                {
                    printSuccess("Rubber glove added to your inventory.");
                }
            }
            else
            {
                printInfo("You leave the Rubber glove in the cupboard.");
            }
        }
    }
    else if (object == 3)
    {
        int targetDigit = game->codeDigits[2];
        printf("\nA brass plaque beneath the picture frame displays a riddle in green ink:\n");
        printf("  %s%s%s\n", CLR_GREEN, digitRiddles[targetDigit], CLR_RESET);

        if (!game->clueFound[2])
        {
            char answer[100];
            char expectedDigitStr[4];
            sprintf(expectedDigitStr, "%d", targetDigit);

            readText("Enter the single digit (0-9) that solves the riddle: ", answer, 100);
            trimWhitespace(answer);

            if (sameText(answer, expectedDigitStr) || sameText(answer, digitWords[targetDigit]))
            {
                printSuccess("A small latch clicks on the frame, confirming your answer!");
                printf("%s[*] Clue Discovered - GREEN digit: %d%s\n", CLR_GREEN, targetDigit, CLR_RESET);
                game->clueFound[2] = 1;
            }
            else
            {
                printError("That does not fit the riddle.");
                addPenalty(game, 5, 15);
            }
        }
        else
        {
            printf("%s[*] Already solved - GREEN digit: %d%s\n", CLR_GREEN, targetDigit, CLR_RESET);
        }
    }
    else if (object == 4)
    {
        printf("\nEngraved onto a brass plate on the centre table is a number sequence:\n");
        printf("  %s3,  7,  15,  31,  ?%s\n", CLR_YELLOW, CLR_RESET);
        printf("  Beneath it reads: \"Double the previous step and add one.\"\n");

        if (!game->clueFound[3])
        {
            int ans = readInteger("Enter the missing number in the sequence: ");
            if (ans == 63)
            {
                printSuccess("Correct! An illuminated panel reveals the final laboratory digit.");
                printf("%s[*] Clue Discovered - YELLOW digit: %d%s\n", CLR_YELLOW, game->codeDigits[3], CLR_RESET);
                game->clueFound[3] = 1;
            }
            else
            {
                printError("Incorrect number. Check the pattern again.");
                addPenalty(game, 5, 15);
            }
        }
        else
        {
            printf("%s[*] Already solved - YELLOW digit: %d%s\n", CLR_YELLOW, game->codeDigits[3], CLR_RESET);
        }
    }
    else if (object == 5)
    {
        const char *story1[] = {
            "Property of: Dr. Elias Voss - Lead Neuro-Architect",
            "Facility Division: Project Echo",
            "",
            "Handwritten margin note:",
            "\"If you are reading this without remembering your own",
            "name, do not panic. The neural wipe is temporary, but",
            "time is short. Find what Vale buried in the archive.\""
        };
        printf("\nYou open a leather-bound research notebook resting on the desk.\n");
        printStoryBox("RECOVERED LOG - LAB NOTEBOOK", story1, 7);
    }
    else if (object == 6)
    {
        printf("\nYou rummage through crumpled diagnostic printouts in the trash bin.\n");
        if (game->itemStatus[OLD_KEY] == AT_ORIGIN)
        {
            printf("At the bottom of the bin lies a tarnished old key.\n");
            if (askYesNo("Take the Old key? (y/n): "))
            {
                if (addItem(game, OLD_KEY))
                {
                    printSuccess("Old key added to your inventory.");
                }
            }
            else
            {
                printInfo("You leave the Old key in the trash bin.");
            }
        }
        else
        {
            printf("Nothing remains in the bin except shredded paper.\n");
        }
    }
}

void attemptRoom1Door(GameState *game)
{
    char entered[100];
    char expected[5];
    int i;

    printf("\nThe electronic keypad on the door displays four coloured indicators:\n  ");
    for (i = 0; i < 4; i++)
    {
        int idx = doorColourOrder[i];
        printf("%s[%s]%s", colourAnsi[idx], colourNames[idx], CLR_RESET);
        if (i < 3) printf(" -> ");
        expected[i] = (char)('0' + game->codeDigits[idx]);
    }
    expected[4] = '\0';
    printf("\n");

    if (game->room1Unlocked)
    {
        printSuccess("The door is already unlocked. You step into Room 2.");
        game->currentRoom = 2;
        return;
    }

    readText("Enter the four-digit code (or press Enter to cancel): ", entered, 100);
    trimWhitespace(entered);

    if (entered[0] == '\0')
    {
        printInfo("You step back from the keypad.");
        return;
    }

    if (strcmp(entered, expected) == 0)
    {
        printSuccess("Access code accepted! The magnetic bolts retract.");
        printf("You push open the heavy door and enter the Statue Chamber.\n");
        game->room1Unlocked = 1;
        game->currentRoom = 2;
    }
    else
    {
        printError("Invalid code. The keypad emits a sharp warning tone.");
        addPenalty(game, 10, 25);
    }
}

void room2(GameState *game)
{
    int choice;

    while (game->currentRoom == 2 && !game->gameOver && !game->escaped)
    {
        showStatus(game);
        printBoxHeader("ROOM 2: THE STATUE CHAMBER", "CENTRAL ATRIUM & OBSERVATION TERMINAL", CLR_MAGENTA);
        printf("  1. Enter the washroom and inspect near the toilet\n");
        printf("  2. Search the wooden wardrobe\n");
        printf("  3. Examine the stone statue\n");
        printf("  4. Access the 1990s computer terminal\n");
        printf("  5. Explore the narrow spiral passage\n");
        printf("  6. Try the reinforced door to Room 3\n");
        printf("  7. Return to Room 1 (The Laboratory)\n");
        printf("  8. View / manage inventory (%d/%d)\n", game->inventoryCount, INVENTORY_CAPACITY);

        choice = readInteger("\nChoose an action: ");

        switch (choice)
        {
            case 1: searchRoom2Object(game, 0); break;
            case 2: searchRoom2Object(game, 1); break;
            case 3: interactStatue(game); break;
            case 4: attemptRoom2Computer(game); break;
            case 5: searchRoom2Object(game, 3); break;
            case 6: attemptRoom3Door(game); break;
            case 7:
                printInfo("You walk back into the Laboratory.");
                game->currentRoom = 1;
                break;
            case 8: manageInventory(game); break;
            default:
                printError("Invalid choice. Please enter a number from 1 to 8.");
        }
    }
}

void searchRoom2Object(GameState *game, int object)
{
    game->room2Checks[object]++;

    if (object == 0)
    {
        printf("\nYou step into the cramped tiled washroom and check behind the toilet tank.\n");
        if (game->itemStatus[SMALL_KEY] == AT_ORIGIN)
        {
            printf("Taped beneath the ceramic rim is a small key.\n");
            if (askYesNo("Take the Small key? (y/n): "))
            {
                if (addItem(game, SMALL_KEY))
                {
                    printSuccess("Small key added to your inventory.");
                }
            }
            else
            {
                printInfo("You leave the Small key where you found it.");
            }
        }
        else
        {
            printf("There is nothing else hidden in the washroom.\n");
        }
    }
    else if (object == 1)
    {
        printf("\nYou open the tall wooden wardrobe and rummage through old lab coats.\n");
        if (!game->wardrobePasswordFound && game->room2Checks[1] == 1)
        {
            printInfo("Your first search turns up nothing besides dusty coats.");
            printf("Something about the wooden back panel looks slightly uneven, though.\n");
        }
        else if (!game->wardrobePasswordFound)
        {
            printSuccess("Probing deeper behind the coats, you pry open a false back panel!");
            printClue("Inside is a faded slip of paper reading: TERMINAL PASSWORD - 1997");
            game->wardrobePasswordFound = 1;
        }
        else
        {
            printClue("You already found the slip of paper: TERMINAL PASSWORD - 1997");
        }
    }
    else if (object == 3)
    {
        printf("\nYou duck into a dimly lit spiral passage curving into the concrete.\n");
        if (!game->spiralVisited)
        {
            printError("After winding downward for several minutes, you hit a sealed brick wall.");
            printf("You are forced to retrace your steps back to the Statue Chamber.\n");
            game->spiralVisited = 1;
            addPenalty(game, 30, 10);
        }
        else
        {
            printInfo("You remember that the spiral passage is a dead end and stay put.");
        }
    }
}

void interactStatue(GameState *game)
{
    game->room2Checks[2]++;
    game->statueKeyholeSeen = 1;

    printf("\nYou examine the marble statue standing in the centre of the chamber.\n");

    if (game->itemStatus[SMALL_KEY] == USED)
    {
        printf("The hidden compartment at its base stands open.\n");
        if (game->itemStatus[BRASS_KEY] == AT_ORIGIN)
        {
            printf("Inside rests a heavy brass key.\n");
            if (askYesNo("Take the Brass key? (y/n): "))
            {
                if (addItem(game, BRASS_KEY))
                {
                    printSuccess("Brass key added to your inventory.");
                }
            }
        }
        else
        {
            printf("The compartment is empty.\n");
        }
        return;
    }

    printf("Built into the pedestal is a concealed seam with a tiny keyhole.\n");

    if (game->inventoryCount == 0)
    {
        printInfo("You have nothing in your inventory to try on the keyhole.");
        return;
    }

    if (askYesNo("Would you like to try an item from your inventory on the keyhole? (y/n): "))
    {
        int chosenItem = selectInventoryItem(game, "Select an item to use on the pedestal keyhole: ");
        if (chosenItem == -1)
        {
            return;
        }

        if (chosenItem == SMALL_KEY)
        {
            printSuccess("The Small key turns smoothly! A hidden compartment slides open.");
            consumeItem(game, SMALL_KEY);
            printf("Inside the recess lies a heavy Brass key.\n");
            if (addItem(game, BRASS_KEY))
            {
                printSuccess("Brass key added to your inventory.");
            }
            else
            {
                game->itemStatus[BRASS_KEY] = AT_ORIGIN;
            }
        }
        else
        {
            printError("That item does not fit the keyhole. Nothing happens.");
        }
    }
}

void attemptRoom2Computer(GameState *game)
{
    char password[100];
    const char *story2[] = {
        "PROJECT ECHO - CLASSIFIED INCIDENT BRIEFING",
        "Director: Adrian Vale",
        "",
        "Project Echo was built to copy and reconstruct human",
        "memories. Instead, neural extraction caused severe",
        "cognitive trauma across early test subjects.",
        "",
        "When Dr. Elias Voss uncovered the casualties and",
        "threatened to expose the program, Director Vale ordered",
        "the records buried and scheduled Voss for suppression."
    };

    game->room2Checks[4]++;

    printf("\nThe CRT monitor hums to life, displaying a login prompt.\n");

    if (game->computerRead)
    {
        printInfo("The terminal is already unlocked. Displaying cached archive file:");
        printStoryBox("PROJECT ECHO - ARCHIVE LOG 02", story2, 10);
        return;
    }

    readText("Enter terminal password (or press Enter to step back): ", password, 100);
    trimWhitespace(password);

    if (password[0] == '\0')
    {
        printInfo("You step away from the terminal.");
        return;
    }

    if (strcmp(password, "1997") == 0)
    {
        printSuccess("PASSWORD ACCEPTED. Decrypting local drive...");
        game->computerRead = 1;
        printStoryBox("PROJECT ECHO - ARCHIVE LOG 02", story2, 10);
    }
    else
    {
        printError("ACCESS DENIED. Incorrect password.");
        addPenalty(game, 5, 15);
    }
}

void attemptRoom3Door(GameState *game)
{
    if (game->currentRoom != 2)
    {
        return;
    }

    if (game->room3Unlocked)
    {
        printSuccess("The door to Room 3 is already unlocked. You step inside.");
        game->currentRoom = 3;
        return;
    }

    printf("\nThe heavy oak-and-steel door to Room 3 is secured by a large mechanical lock.\n");

    if (game->inventoryCount == 0)
    {
        printInfo("You have no items in your inventory to unlock this door.");
        return;
    }

    if (askYesNo("Try an item from your inventory on the door lock? (y/n): "))
    {
        int chosenItem = selectInventoryItem(game, "Select an item to use on the door lock: ");
        if (chosenItem == -1)
        {
            return;
        }

        if (chosenItem == BRASS_KEY)
        {
            printSuccess("The Brass key engages the tumblers with a heavy clunk!");
            consumeItem(game, BRASS_KEY);
            game->room3Unlocked = 1;
            game->currentRoom = 3;
        }
        else
        {
            printError("That item cannot unlock this door. Nothing happens.");
        }
    }
}

void room3(GameState *game)
{
    int choice;

    if (!game->mazeComplete)
    {
        navigateMaze(game);
        if (game->currentRoom != 3 || game->gameOver)
        {
            return;
        }
    }

    while (game->currentRoom == 3 && !game->gameOver && !game->escaped)
    {
        showStatus(game);
        printBoxHeader("ROOM 3: THE ARCHIVE", "RESTRICTED VAULT & SURFACE EXIT", CLR_YELLOW);
        printf("  1. Examine the large archive table\n");
        printf("  2. Search the trash pile and wall safe\n");
        printf("  3. Inspect the damaged wall panel\n");
        printf("  4. Approach the fingerprint-locked exit door\n");
        printf("  5. Return to Room 2 (The Statue Chamber)\n");
        printf("  6. View / manage inventory (%d/%d)\n", game->inventoryCount, INVENTORY_CAPACITY);

        choice = readInteger("\nChoose an action: ");

        switch (choice)
        {
            case 1: searchRoom3Object(game, 0); break;
            case 2: interactSafe(game); break;
            case 3: interactDamagedPanel(game); break;
            case 4: attemptFinalDoor(game); break;
            case 5:
                printInfo("You bypass the mapped archive maze and return to Room 2.");
                game->currentRoom = 2;
                break;
            case 6: manageInventory(game); break;
            default:
                printError("Invalid choice. Please enter a number from 1 to 6.");
        }
    }
}

void navigateMaze(GameState *game)
{
    char command[100];
    int newRow;
    int newCol;
    char cell;

    printBoxHeader("ROOM 3: ARCHIVE SECURITY MAZE", "NAVIGATE THE STACKS TO REACH THE EXIT WING", CLR_YELLOW);
    printf("  Controls: %sW%s = Up, %sS%s = Down, %sA%s = Left, %sD%s = Right, %sB%s = Return to Room 2\n",
           CLR_GREEN, CLR_RESET, CLR_GREEN, CLR_RESET, CLR_GREEN, CLR_RESET,
           CLR_GREEN, CLR_RESET, CLR_CYAN, CLR_RESET);
    printf("  Visibility is limited, but explored corridors stay mapped.\n");

    while (!game->mazeComplete && game->currentRoom == 3 && !game->gameOver)
    {
        showStatus(game);
        revealNearby(game);
        displayMaze(game);

        printf("\nCurrent coordinates: Row %d, Column %d\n", game->mazeRow + 1, game->mazeCol + 1);
        readText("Move (W/A/S/D) or B to go back: ", command, 100);
        trimWhitespace(command);

        if (sameText(command, "B"))
        {
            game->currentRoom = 2;
            printInfo("You retreat to Room 2. Your maze map progress is saved.");
            return;
        }

        if (strlen(command) != 1)
        {
            printError("Enter a single key (W, A, S, D, or B).");
            continue;
        }

        newRow = game->mazeRow;
        newCol = game->mazeCol;
        command[0] = (char)toupper((unsigned char)command[0]);

        if (command[0] == 'W') newRow--;
        else if (command[0] == 'S') newRow++;
        else if (command[0] == 'A') newCol--;
        else if (command[0] == 'D') newCol++;
        else
        {
            printError("Unknown movement key. Use W, A, S, D, or B.");
            continue;
        }

        if (newRow < 0 || newRow >= MAZE_ROWS || newCol < 0 || newCol >= MAZE_COLS)
        {
            printError("A reinforced perimeter wall blocks that direction.");
            continue;
        }

        cell = mazeLayouts[game->mazeIndex][newRow][newCol];

        if (cell == '#')
        {
            printError("Archive shelving blocks your path.");
            continue;
        }

        game->mazeRow = newRow;
        game->mazeCol = newCol;
        addPenalty(game, 2, 0);

        if (cell == 'E')
        {
            game->mazeComplete = 1;
            printSuccess("You emerge from the archive stacks into the vault chamber!");
            printInfo("The route is now clear and will be bypassed on future visits.");
            pressEnter();
        }
    }
}

void revealNearby(GameState *game)
{
    int r = game->mazeRow;
    int c = game->mazeCol;

    game->mazeExplored[r][c] = 1;

    if (r > 0) game->mazeExplored[r - 1][c] = 1;
    if (r < MAZE_ROWS - 1) game->mazeExplored[r + 1][c] = 1;
    if (c > 0) game->mazeExplored[r][c - 1] = 1;
    if (c < MAZE_COLS - 1) game->mazeExplored[r][c + 1] = 1;
}

void displayMaze(const GameState *game)
{
    int r;
    int c;
    char cell;

    printf("\n  +---------------------------------------+\n");
    for (r = 0; r < MAZE_ROWS; r++)
    {
        printf("  | ");
        for (c = 0; c < MAZE_COLS; c++)
        {
            if (r == game->mazeRow && c == game->mazeCol)
            {
                printf("%s@%s ", CLR_CYAN, CLR_RESET);
            }
            else if (!game->mazeExplored[r][c])
            {
                printf("%s?%s ", CLR_GRAY, CLR_RESET);
            }
            else
            {
                cell = mazeLayouts[game->mazeIndex][r][c];

                if (cell == 'S')
                {
                    printf("%sS%s ", CLR_BLUE, CLR_RESET);
                }
                else if (cell == 'E')
                {
                    printf("%sE%s ", CLR_GREEN, CLR_RESET);
                }
                else if (cell == '#')
                {
                    printf("%s#%s ", CLR_YELLOW, CLR_RESET);
                }
                else
                {
                    printf(". ");
                }
            }
        }
        printf("|\n");
    }
    printf("  +---------------------------------------+\n");
    printf("  Legend: %s@%s=You  %s?%s=Unexplored  %s#%s=Wall  .=Path  %sE%s=Exit\n",
           CLR_CYAN, CLR_RESET, CLR_GRAY, CLR_RESET, CLR_YELLOW, CLR_RESET, CLR_GREEN, CLR_RESET);
}

void searchRoom3Object(GameState *game, int object)
{
    game->room3Checks[object]++;

    if (object == 0)
    {
        printf("\nSpread across the large archive table is a biometric calibration sheet:\n");
        printf("  %s\"Optical scanner rejection triggers when dermal ridges are masked\n", CLR_CYAN);
        printf("   by a non-porous insulating barrier. What biological pattern does\n");
        printf("   the exit sensor attempt to read?\"%s\n", CLR_RESET);

        if (!game->room3PuzzleSolved)
        {
            char answer[100];
            readText("Enter your one-word answer: ", answer, 100);
            trimWhitespace(answer);

            if (sameText(answer, "FINGERPRINT") || sameText(answer, "FINGERPRINTS"))
            {
                printSuccess("Correct! A calibration note slides out from beneath the sheet.");
                printClue("Note: \"The exit sensor is faulty-it fails to lock out when dermal ridges are obscured by an insulating barrier.\"");
                game->room3PuzzleSolved = 1;
                game->score += 25;
            }
            else
            {
                printError("Incorrect answer. Read the calibration sheet carefully.");
                addPenalty(game, 5, 15);
            }
        }
        else
        {
            printClue("Already solved - Note: \"The exit sensor fails to lock out when dermal ridges are obscured by an insulating barrier.\"");
        }
    }
}

void interactSafe(GameState *game)
{
    const char *story3[] = {
        "SUBJECT FILE: E-01 - DR. ELIAS VOSS",
        "",
        "Voss was not an unwilling victim of the memory wipe.",
        "Realising Vale would erase his mind regardless, Voss",
        "voluntarily entered the Project Echo chair first-hiding",
        "the victim logs inside this vault and planting clues",
        "throughout the wing so his future self could recover",
        "the evidence and bring Project Echo into the light."
    };

    game->room3Checks[1]++;
    printf("\nBehind a pile of discarded archive boxes sits a compact steel wall safe.\n");

    if (game->safeOpened)
    {
        printInfo("The wall safe stands open. Its recovered dossier reads:");
        printStoryBox("FINAL REVELATION - SUBJECT E-01", story3, 8);
        return;
    }

    printf("Its door is secured by a small, tarnished keyhole.\n");

    if (game->inventoryCount == 0)
    {
        printInfo("You have no items in your inventory to try on the safe.");
        return;
    }

    if (askYesNo("Try an item from your inventory on the safe? (y/n): "))
    {
        int chosenItem = selectInventoryItem(game, "Select an item to use on the wall safe: ");
        if (chosenItem == -1)
        {
            return;
        }

        if (chosenItem == OLD_KEY)
        {
            printSuccess("The Old key clicks inside the lock! The safe door swings open.");
            game->safeOpened = 1;
            consumeItem(game, OLD_KEY);
            game->score += 50;
            printStoryBox("FINAL REVELATION - SUBJECT E-01", story3, 8);
        }
        else
        {
            printError("That item does not open the safe. Nothing happens.");
        }
    }
}

void interactDamagedPanel(GameState *game)
{
    game->room3Checks[2]++;
    printf("\nAlong the eastern wall, a junction panel hangs loose from a fractured bracket.\n");

    if (game->glueUsed)
    {
        printInfo("The repaired panel holds steady, displaying an auxiliary override diagnostic.");
        printClue("Diagnostic Log: \"Emergency power restored to archive vault (+50 bonus points).\"");
        return;
    }

    printf("Exposed copper contacts are split cleanly down the middle-if bonded back\n");
    printf("together, the auxiliary circuit might complete.\n");

    if (game->inventoryCount == 0)
    {
        printInfo("You have no items in your inventory to use here.");
        return;
    }

    if (askYesNo("Try an item from your inventory on the damaged panel? (y/n): "))
    {
        int chosenItem = selectInventoryItem(game, "Select an item to use on the damaged panel: ");
        if (chosenItem == -1)
        {
            return;
        }

        if (chosenItem == GLUE)
        {
            printSuccess("You apply the adhesive and press the fractured bracket into place.");
            printf("A small status LED turns green as an auxiliary archive log prints out:\n");
            printClue("Encrypted Memo: \"Vale's biometric override excludes lab staff-only unverified readings trigger the fail-safe release.\"");
            game->glueUsed = 1;
            consumeItem(game, GLUE);
            game->score += 50;
            printSuccess("Bonus +50 points awarded for restoring the auxiliary circuit!");
        }
        else
        {
            printError("That item has no effect on the damaged panel.");
        }
    }
}

void attemptFinalDoor(GameState *game)
{
    printf("\nYou stand before the heavy surface exit door. Beside it is a glowing\n");
    printf("optical fingerprint scanner.\n");

    if (game->inventoryCount == 0)
    {
        printError("You press your bare hand to the scanner: \"ACCESS DENIED - STAFF LOCKOUT.\"");
        printInfo("You have no items in your inventory. Remember you can revisit earlier rooms.");
        return;
    }

    if (askYesNo("Would you like to use an item from your inventory at the exit scanner? (y/n): "))
    {
        int chosenItem = selectInventoryItem(game, "Select an item to use at the scanner: ");
        if (chosenItem == -1)
        {
            return;
        }

        if (chosenItem == GLOVE)
        {
            printSuccess("You pull on the Rubber glove and press your hand to the glass.");
            printf("Unable to read a blacklisted staff print, the faulty sensor triggers\n");
            printf("its emergency fail-safe release!\n");
            consumeItem(game, GLOVE);
            game->escaped = 1;
        }
        else
        {
            printError("That item does nothing against the biometric scanner.");
        }
    }
    else
    {
        printError("Touching the scanner bare-handed triggers: \"ACCESS DENIED - STAFF LOCKOUT.\"");
    }
}

void manageInventory(GameState *game)
{
    int i;
    int choice;
    int item;

    printBoxHeader("INVENTORY", "CARRY CAPACITY: 3 ITEMS MAX", CLR_CYAN);

    if (game->inventoryCount == 0)
    {
        printInfo("Your inventory is currently empty.");
        pressEnter();
        return;
    }

    for (i = 0; i < game->inventoryCount; i++)
    {
        printf("  %s%d.%s %s\n", CLR_BOLD, i + 1, CLR_RESET, itemNames[game->inventory[i]]);
    }

    printf("\n  Slots used: %s%d/%d%s\n", CLR_YELLOW, game->inventoryCount, INVENTORY_CAPACITY, CLR_RESET);
    printf("  0. Return\n");
    choice = readInteger("\nSelect an item number to discard it, or 0 to return: ");

    if (choice == 0)
    {
        return;
    }

    if (choice < 1 || choice > game->inventoryCount)
    {
        printError("Invalid selection.");
        return;
    }

    item = game->inventory[choice - 1];
    returnItemToOrigin(game, item);
    printInfo("Item discarded and returned to its original location.");
    printf("  Returned: %s%s%s\n", CLR_YELLOW, itemNames[item], CLR_RESET);
}

int selectInventoryItem(const GameState *game, const char *promptText)
{
    int i;
    int choice;

    if (game->inventoryCount == 0)
    {
        printInfo("Your inventory is empty.");
        return -1;
    }

    printf("\n  %sAvailable Inventory:%s\n", CLR_CYAN, CLR_RESET);
    for (i = 0; i < game->inventoryCount; i++)
    {
        printf("    %d. %s\n", i + 1, itemNames[game->inventory[i]]);
    }
    printf("    0. Cancel\n");

    choice = readInteger(promptText);

    if (choice == 0)
    {
        return -1;
    }

    if (choice < 1 || choice > game->inventoryCount)
    {
        printError("Invalid item selection.");
        return -1;
    }

    return game->inventory[choice - 1];
}

int hasItem(const GameState *game, int item)
{
    int i;

    for (i = 0; i < game->inventoryCount; i++)
    {
        if (game->inventory[i] == item)
        {
            return 1;
        }
    }

    return 0;
}

int addItem(GameState *game, int item)
{
    if (hasItem(game, item))
    {
        return 1;
    }

    if (game->inventoryCount >= INVENTORY_CAPACITY)
    {
        printError("Your inventory is full (3/3). Discard an item first to pick this up.");
        return 0;
    }

    game->inventory[game->inventoryCount] = item;
    game->inventoryCount++;
    game->itemStatus[item] = IN_INVENTORY;
    return 1;
}

void consumeItem(GameState *game, int item)
{
    int i;
    int j;

    for (i = 0; i < game->inventoryCount; i++)
    {
        if (game->inventory[i] == item)
        {
            for (j = i; j < game->inventoryCount - 1; j++)
            {
                game->inventory[j] = game->inventory[j + 1];
            }

            game->inventoryCount--;
            game->inventory[game->inventoryCount] = -1;
            game->itemStatus[item] = USED;
            return;
        }
    }
}

void returnItemToOrigin(GameState *game, int item)
{
    int i;
    int j;

    for (i = 0; i < game->inventoryCount; i++)
    {
        if (game->inventory[i] == item)
        {
            for (j = i; j < game->inventoryCount - 1; j++)
            {
                game->inventory[j] = game->inventory[j + 1];
            }

            game->inventoryCount--;
            game->inventory[game->inventoryCount] = -1;
            game->itemStatus[item] = AT_ORIGIN;
            return;
        }
    }
}

int readInteger(const char *prompt)
{
    char line[100];
    char extra;
    int value;

    while (1)
    {
        printf("%s", prompt);

        if (fgets(line, sizeof(line), stdin) == NULL)
        {
            return -1;
        }

        if (sscanf(line, "%d %c", &value, &extra) == 1)
        {
            return value;
        }

        printError("Please enter a valid whole number.");
    }
}

void readText(const char *prompt, char text[], int size)
{
    int length;

    printf("%s", prompt);

    if (fgets(text, size, stdin) == NULL)
    {
        text[0] = '\0';
        return;
    }

    length = (int)strlen(text);

    if (length > 0 && text[length - 1] == '\n')
    {
        text[length - 1] = '\0';
    }
    else
    {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF)
        {
            // Discard excess input
        }
    }
}

void trimWhitespace(char *str)
{
    int start = 0;
    int end = (int)strlen(str) - 1;
    int i = 0;

    while (str[start] != '\0' && isspace((unsigned char)str[start]))
    {
        start++;
    }

    while (end >= start && isspace((unsigned char)str[end]))
    {
        end--;
    }

    while (start <= end)
    {
        str[i++] = str[start++];
    }
    str[i] = '\0';
}

int sameText(const char *a, const char *b)
{
    while (*a != '\0' && *b != '\0')
    {
        if (toupper((unsigned char)*a) != toupper((unsigned char)*b))
        {
            return 0;
        }

        a++;
        b++;
    }

    return (*a == '\0' && *b == '\0');
}

int askYesNo(const char *prompt)
{
    char answer[20];

    while (1)
    {
        readText(prompt, answer, 20);
        trimWhitespace(answer);

        if (sameText(answer, "Y") || sameText(answer, "YES"))
        {
            return 1;
        }

        if (sameText(answer, "N") || sameText(answer, "NO"))
        {
            return 0;
        }

        printError("Please enter Y or N.");
    }
}

void addPenalty(GameState *game, int seconds, int points)
{
    game->timePenalty += seconds;
    game->score -= points;

    if (game->score < 0)
    {
        game->score = 0;
    }
}

int elapsedSeconds(const GameState *game)
{
    int elapsed = (int)difftime(time(NULL), game->startTime);
    elapsed += game->timePenalty;

    if (elapsed < 0)
    {
        elapsed = 0;
    }

    return elapsed;
}

void pressEnter(void)
{
    char line[10];

    printf("\nPress Enter to continue...");
    if (fgets(line, sizeof(line), stdin) == NULL)
    {
        return;
    }
}
