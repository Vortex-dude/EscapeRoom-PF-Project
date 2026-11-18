#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define INVENTORY_CAPACITY 3
#define MAZE_ROWS 11
#define MAZE_COLS 19
#define ITEM_COUNT 5

#define GLOVE 0
#define GLUE 1
#define USELESS_KEY 2
#define STATUE_KEY 3
#define ROOM3_KEY 4

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

/* Four predefined maze layouts. S = start, E = exit, # = wall. */
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
    "Useless key",
    "Statue key",
    "Room 3 key"
};

const char *colourNames[4] = {"BLUE", "RED", "GREEN", "YELLOW"};
/* The door asks for the colours in this order: GREEN, BLUE, YELLOW, RED. */
const int doorColourOrder[4] = {2, 0, 3, 1};

/* Function declarations */
void initialiseGame(GameState *game);
void mainMenu(void);
void playGame(GameState *game);
void showStatus(const GameState *game);
void showInstructions(void);
void room1(GameState *game);
void room2(GameState *game);
void room3(GameState *game);
void navigateMaze(GameState *game);
void manageInventory(GameState *game);
void revealNearby(GameState *game);
void displayMaze(const GameState *game);
void searchRoom1Object(GameState *game, int object);
void searchRoom2Object(GameState *game, int object);
void searchRoom3Object(GameState *game, int object);
void attemptRoom1Door(GameState *game);
void attemptRoom2Computer(GameState *game);
void useStatueKey(GameState *game);
void useRoom3Key(GameState *game);
void useUselessKey(GameState *game);
void useGlove(GameState *game);
void useGlue(GameState *game);
void attemptFinalDoor(GameState *game);
int hasItem(const GameState *game, int item);
int addItem(GameState *game, int item);
void consumeItem(GameState *game, int item);
void returnItemToOrigin(GameState *game, int item);
int readInteger(const char *prompt);
void readText(const char *prompt, char text[], int size);
int sameText(const char *a, const char *b);
int askYesNo(const char *prompt);
void addPenalty(GameState *game, int seconds, int points);
int elapsedSeconds(const GameState *game);
void pressEnter(void);

/* Main program */
int main(void)
{
    srand((unsigned int)time(NULL));
    mainMenu();
    return 0;
}

void mainMenu(void)
{
    int choice;
    GameState game;

    do
    {
        printf("\n====================================\n");
        printf("             ESCAPE ROOM\n");
        printf("              VERSION 1.0\n");
        printf("====================================\n");
        printf("1. Start a new game\n");
        printf("2. Instructions\n");
        printf("3. Exit\n");

        choice = readInteger("\nEnter your choice: ");

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
            printf("\nThank you for playing!\n");
        }
        else
        {
            printf("\nInvalid choice. Please select 1, 2 or 3.\n");
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
    printf("\nYou wake up on the floor of a strange laboratory.\n");
    printf("The door is locked. You need to find a way out.\n");

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
        printf("\n====================================\n");
        printf("          YOU ESCAPED!\n");
        printf("====================================\n");
        printf("You made it out of the building.\n");
    }

    printf("\nFinal time: %d minute(s), %d second(s)\n",
           elapsedSeconds(game) / 60, elapsedSeconds(game) % 60);
    printf("Final score: %d\n", game->score);
    pressEnter();
}

void showInstructions(void)
{
    printf("\n============== INSTRUCTIONS ==============\n");
    printf("- Explore rooms and investigate objects.\n");
    printf("- Collect useful items and solve puzzles.\n");
    printf("- Your inventory can hold only three items.\n");
    printf("- You may discard an item and collect it again\n");
    printf("  from its original location.\n");
    printf("- Successfully used items disappear.\n");
    printf("- You can revisit earlier rooms.\n");
    printf("- In the maze, use W, A, S and D to move.\n");
    printf("- Previously explored maze areas stay visible.\n");
    printf("- The timer records elapsed time; it is not a\n");
    printf("  countdown in this version.\n");
    printf("==========================================\n");
    pressEnter();
}

void showStatus(const GameState *game)
{
    int seconds = elapsedSeconds(game);

    printf("\n------------------------------------------\n");
    printf("ROOM: %d    TIME: %02d:%02d    SCORE: %d\n",
           game->currentRoom, seconds / 60, seconds % 60, game->score);
    printf("------------------------------------------\n");
}

void room1(GameState *game)
{
    int choice;

    while (game->currentRoom == 1 && !game->gameOver && !game->escaped)
    {
        showStatus(game);
        printf("\n============== ROOM 1: LABORATORY ==============\n");
        printf("1. Investigate the top broken experiment tube\n");
        printf("2. Search the middle broken experiment tube\n");
        printf("3. Investigate the cupboard\n");
        printf("4. Investigate the picture frame\n");
        printf("5. Investigate the centre table\n");
        printf("6. Inspect the desk\n");
        printf("7. Search the trash\n");
        printf("8. Try the door to Room 2\n");
        printf("9. View / manage inventory\n");
        printf("0. Quit to main menu\n");

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
                if (askYesNo("Return to the main menu? Your progress will be lost (y/n): "))
                {
                    game->gameOver = 1;
                }
                break;
            default: printf("\nInvalid choice.\n");
        }
    }
}

void searchRoom1Object(GameState *game, int object)
{
    game->room1Checks[object]++;

    if (object == 0)
    {
        printf("\nYou inspect the top experiment tube.\n");
        if (!game->clueFound[0])
        {
            printf("Inside, you find a digit marked BLUE: %d\n", game->codeDigits[0]);
            game->clueFound[0] = 1;
        }
        else
        {
            printf("You have already found the BLUE digit: %d\n", game->codeDigits[0]);
        }
    }
    else if (object == 1)
    {
        printf("\nYou carefully search through the broken glass.\n");
        addPenalty(game, 15, 0);

        if (game->itemStatus[GLUE] == AT_ORIGIN)
        {
            printf("You find a tube of glue among the debris.\n");
            if (addItem(game, GLUE))
            {
                printf("Glue tube added to your inventory.\n");
            }
        }
        else
        {
            printf("You find nothing else of use here.\n");
        }
    }
    else if (object == 2)
    {
        printf("\nYou open the cupboard. A digit is written inside.\n");
        if (!game->clueFound[1])
        {
            printf("The digit is marked RED: %d\n", game->codeDigits[1]);
            game->clueFound[1] = 1;
        }
        else
        {
            printf("You already recorded the RED digit: %d\n", game->codeDigits[1]);
        }

        if (game->itemStatus[GLOVE] == AT_ORIGIN)
        {
            printf("There is also a rubber glove.\n");
            if (askYesNo("Take the rubber glove? (y/n): "))
            {
                if (addItem(game, GLOVE))
                {
                    printf("Rubber glove added to your inventory.\n");
                }
            }
            else
            {
                printf("You leave the glove in the cupboard. You can return for it.\n");
            }
        }
    }
    else if (object == 3)
    {
        printf("\nA message is written on the frame:\n");
        printf("\"I follow you in the light, but leave you in the dark.\"\n");
        if (!game->clueFound[2])
        {
            char answer[100];
            readText("Your answer (placeholder riddle): ", answer, 100);
            if (sameText(answer, "SHADOW"))
            {
                printf("Correct. The GREEN digit is %d\n", game->codeDigits[2]);
                game->clueFound[2] = 1;
            }
            else
            {
                printf("Not quite. This is a placeholder riddle.\n");
                addPenalty(game, 5, 15);
            }
        }
        else
        {
            printf("You already found the GREEN digit: %d\n", game->codeDigits[2]);
        }
    }
    else if (object == 4)
    {
        printf("\nThe centre table has a puzzle written on it.\n");
        if (!game->clueFound[3])
        {
            char answer[100];
            printf("PLACEHOLDER PUZZLE: enter the word LAB to solve it.\n");
            readText("Answer: ", answer, 100);
            if (sameText(answer, "LAB"))
            {
                printf("Correct. The YELLOW digit is %d\n", game->codeDigits[3]);
                game->clueFound[3] = 1;
            }
            else
            {
                printf("Incorrect answer.\n");
                addPenalty(game, 5, 15);
            }
        }
        else
        {
            printf("You already found the YELLOW digit: %d\n", game->codeDigits[3]);
        }
    }
    else if (object == 5)
    {
        printf("\nYou inspect the desk. A name is written in an old notebook.\n");
        printf("PLACEHOLDER BACKSTORY: The person's name is [MAIN CHARACTER NAME].\n");
        printf("There is nothing else useful on the desk.\n");
    }
    else if (object == 6)
    {
        printf("\nYou search the trash and find a small key.\n");
        if (game->itemStatus[USELESS_KEY] == AT_ORIGIN)
        {
            printf("It looks unimportant, but you decide whether to keep it.\n");
            if (askYesNo("Take the key? (y/n): "))
            {
                if (addItem(game, USELESS_KEY))
                {
                    printf("Key added to your inventory.\n");
                }
            }
        }
        else
        {
            printf("There is nothing else here.\n");
        }
    }
}

void attemptRoom1Door(GameState *game)
{
    char entered[100];
    char expected[5];
    int i;

    printf("\nThe door has four coloured slots in this order:\n");
    for (i = 0; i < 4; i++)
    {
        printf("%s ", colourNames[doorColourOrder[i]]);
        expected[i] = (char)('0' + game->codeDigits[doorColourOrder[i]]);
    }
    expected[4] = '\0';
    printf("\n");

    if (game->room1Unlocked)
    {
        printf("The door is already unlocked. You enter Room 2.\n");
        game->currentRoom = 2;
        return;
    }

    readText("Enter the four-digit code: ", entered, 100);

    if (strcmp(entered, expected) == 0)
    {
        printf("The lock clicks open. You enter Room 2.\n");
        game->room1Unlocked = 1;
        game->currentRoom = 2;
    }
    else
    {
        printf("Wrong code. The lock flashes red.\n");
        addPenalty(game, 10, 25);
    }
}

void room2(GameState *game)
{
    int choice;

    while (game->currentRoom == 2 && !game->gameOver && !game->escaped)
    {
        showStatus(game);
        printf("\n============== ROOM 2: STATUE ROOM ==============\n");
        printf("1. Enter the washroom and inspect the toilet\n");
        printf("2. Search the wardrobe\n");
        printf("3. Investigate the statue / use its key\n");
        printf("4. Use the 1990s computer\n");
        printf("5. Enter the spiral passage\n");
        printf("6. Try the door to Room 3\n");
        printf("7. Return to Room 1\n");
        printf("8. View / manage inventory\n");
        choice = readInteger("\nChoose an action: ");

        switch (choice)
        {
            case 1: searchRoom2Object(game, 0); break;
            case 2: searchRoom2Object(game, 1); break;
            case 3:
                searchRoom2Object(game, 2);
                if (hasItem(game, STATUE_KEY))
                {
                    if (askYesNo("Use the statue key on the keyhole? (y/n): "))
                    {
                        useStatueKey(game);
                    }
                }
                break;
            case 4: attemptRoom2Computer(game); break;
            case 5: searchRoom2Object(game, 3); break;
            case 6: useRoom3Key(game); break;
            case 7:
                game->currentRoom = 1;
                break;
            case 8: manageInventory(game); break;
            default: printf("\nInvalid choice.\n");
        }
    }
}

void searchRoom2Object(GameState *game, int object)
{
    game->room2Checks[object]++;

    if (object == 0)
    {
        printf("\nYou inspect the toilet in the washroom.\n");
        if (game->itemStatus[STATUE_KEY] == AT_ORIGIN)
        {
            printf("You find a small key hidden inside.\n");
            if (addItem(game, STATUE_KEY))
            {
                printf("Statue key added to your inventory.\n");
            }
        }
        else
        {
            printf("You have already searched here.\n");
        }
    }
    else if (object == 1)
    {
        printf("\nYou search the wardrobe (search %d).\n", game->room2Checks[1]);
        if (!game->wardrobePasswordFound && game->room2Checks[1] == 1)
        {
            printf("You find nothing on the first search.\n");
        }
        else if (!game->wardrobePasswordFound)
        {
            printf("Behind a loose panel, you find a paper with a password: 1997\n");
            game->wardrobePasswordFound = 1;
        }
        else
        {
            printf("You already found the paper.\n");
        }
    }
    else if (object == 2)
    {
        printf("\nYou inspect the statue and notice a small keyhole.\n");
        game->statueKeyholeSeen = 1;
        if (!hasItem(game, STATUE_KEY))
        {
            printf("Something may fit into the keyhole.\n");
        }
    }
    else if (object == 3)
    {
        printf("\nYou enter a narrow spiral passage.\n");
        if (!game->spiralVisited)
        {
            printf("It winds around and leads nowhere. You return to the statue room.\n");
            game->spiralVisited = 1;
            addPenalty(game, 30, 10);
        }
        else
        {
            printf("You recognise this dead end and turn back.\n");
        }
    }
}

void attemptRoom2Computer(GameState *game)
{
    char password[100];

    game->room2Checks[4]++;

    printf("\nThe old computer asks for a password.\n");
    if (!game->wardrobePasswordFound)
    {
        printf("You do not know the password yet. Search the wardrobe.\n");
        return;
    }

    if (game->computerRead)
    {
        printf("The computer's files have already been read.\n");
        return;
    }

    readText("Enter password: ", password, 100);

    if (strcmp(password, "1997") == 0)
    {
        printf("\nACCESS GRANTED.\n");
        printf("PLACEHOLDER BACKSTORY PART 2:\n");
        printf("The records suggest that the laboratory's work was not what\n");
        printf("the staff had claimed. Replace this text with your story.\n");
        game->computerRead = 1;
    }
    else
    {
        printf("Incorrect password.\n");
        addPenalty(game, 5, 15);
    }
}

void useStatueKey(GameState *game)
{
    if (!game->statueKeyholeSeen)
    {
        printf("You need to inspect the statue first.\n");
        return;
    }

    if (hasItem(game, STATUE_KEY))
    {
        printf("\nThe key fits. A compartment opens in the statue.\n");
        printf("Inside is the key to Room 3.\n");
        consumeItem(game, STATUE_KEY);
        if (addItem(game, ROOM3_KEY))
        {
            game->room3Unlocked = 1;
            printf("Room 3 key added to your inventory.\n");
        }
        else
        {
            /* This should not normally occur because the statue key used a slot. */
            printf("Your inventory is full. The Room 3 key remains in the statue.\n");
            game->itemStatus[ROOM3_KEY] = AT_ORIGIN;
        }
    }
    else
    {
        printf("You do not have the statue key.\n");
    }
}

void useRoom3Key(GameState *game)
{
    if (game->currentRoom != 2)
    {
        return;
    }

    if (game->room3Unlocked)
    {
        printf("\nThe Room 3 door is unlocked. You enter Room 3.\n");
        game->currentRoom = 3;
        return;
    }

    if (hasItem(game, ROOM3_KEY))
    {
        printf("\nYou unlock the door to Room 3.\n");
        consumeItem(game, ROOM3_KEY);
        game->room3Unlocked = 1;
        game->currentRoom = 3;
    }
    else
    {
        printf("\nThe door is locked. You need its key.\n");
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
        printf("\n============== ROOM 3: FINAL AREA ==============\n");
        printf("You have crossed the maze. The remaining area is accessible.\n");
        printf("1. Investigate the large table puzzle\n");
        printf("2. Search the trash / safe\n");
        printf("3. Use glue on the cracked panel (placeholder)\n");
        printf("4. Try the fingerprint-locked exit\n");
        printf("5. Return to Room 2\n");
        printf("6. View / manage inventory\n");

        choice = readInteger("\nChoose an action: ");

        switch (choice)
        {
            case 1: searchRoom3Object(game, 0); break;
            case 2:
                searchRoom3Object(game, 1);
                if (hasItem(game, USELESS_KEY) && !game->safeOpened)
                {
                    if (askYesNo("Use the small key on the safe? (y/n): "))
                    {
                        useUselessKey(game);
                    }
                }
                break;
            case 3: useGlue(game); break;
            case 4: attemptFinalDoor(game); break;
            case 5:
                game->currentRoom = 2;
                break;
            case 6: manageInventory(game); break;
            default: printf("\nInvalid choice.\n");
        }
    }
}

void navigateMaze(GameState *game)
{
    char command[100];
    int newRow;
    int newCol;
    char cell;

    printf("\n============== ROOM 3: MAZE ==============\n");
    printf("You must navigate through the maze to reach the far side.\n");
    printf("W = up, S = down, A = left, D = right, B = return to Room 2.\n");
    printf("You can see nearby cells. Explored areas remain visible.\n");

    while (!game->mazeComplete && game->currentRoom == 3 && !game->gameOver)
    {
        showStatus(game);
        revealNearby(game);
        displayMaze(game);

        printf("\nPosition: row %d, column %d\n", game->mazeRow + 1, game->mazeCol + 1);
        readText("Move (W/A/S/D) or B to go back: ", command, 100);

        if (sameText(command, "B"))
        {
            game->currentRoom = 2;
            printf("You return to Room 2. Your maze progress is saved.\n");
            return;
        }

        if (strlen(command) != 1)
        {
            printf("Enter one movement key at a time.\n");
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
            printf("Unknown movement key.\n");
            continue;
        }

        if (newRow < 0 || newRow >= MAZE_ROWS || newCol < 0 || newCol >= MAZE_COLS)
        {
            printf("You cannot move outside the maze.\n");
            continue;
        }

        cell = mazeLayouts[game->mazeIndex][newRow][newCol];

        if (cell == '#')
        {
            printf("A wall blocks your way.\n");
            continue;
        }

        game->mazeRow = newRow;
        game->mazeCol = newCol;
        addPenalty(game, 2, 0);

        if (cell == 'E')
        {
            game->mazeComplete = 1;
            printf("\nYou have reached the far side of the maze!\n");
            printf("You will be able to bypass the maze on future visits.\n");
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

    printf("\n");
    for (r = 0; r < MAZE_ROWS; r++)
    {
        for (c = 0; c < MAZE_COLS; c++)
        {
            if (r == game->mazeRow && c == game->mazeCol)
            {
                printf("@ ");
            }
            else if (!game->mazeExplored[r][c])
            {
                printf("? ");
            }
            else
            {
                cell = mazeLayouts[game->mazeIndex][r][c];

                if (cell == 'S')
                {
                    printf("S ");
                }
                else if (cell == 'E')
                {
                    printf("E ");
                }
                else if (cell == '#')
                {
                    printf("# ");
                }
                else
                {
                    printf(". ");
                }
            }
        }
        printf("\n");
    }
    printf("@ = You, ? = Unexplored, # = Wall, . = Path, E = Exit\n");
}

void searchRoom3Object(GameState *game, int object)
{
    game->room3Checks[object]++;

    if (object == 0)
    {
        printf("\nYou examine the large table.\n");
        if (!game->room3PuzzleSolved)
        {
            char answer[100];
            printf("PLACEHOLDER PUZZLE: enter GLOVE to receive a hint.\n");
            readText("Answer: ", answer, 100);

            if (sameText(answer, "GLOVE"))
            {
                printf("Correct. Hint: a thin barrier can prevent the scanner\n");
                printf("from reading the fingerprint directly.\n");
                game->room3PuzzleSolved = 1;
                game->score += 25;
            }
            else
            {
                printf("Incorrect answer.\n");
                addPenalty(game, 5, 15);
            }
        }
        else
        {
            printf("You have already solved this puzzle and found its hint.\n");
        }
    }
    else if (object == 1)
    {
        printf("\nYou search the trash and discover a small safe.\n");
        if (game->safeOpened)
        {
            printf("The safe is already open.\n");
        }
        else if (!hasItem(game, USELESS_KEY))
        {
            printf("The safe is locked. A small keyhole is visible.\n");
        }
        else
        {
            printf("The key you found earlier might fit.\n");
        }
    }
}

void useUselessKey(GameState *game)
{
    if (!hasItem(game, USELESS_KEY))
    {
        printf("You do not have the key.\n");
        return;
    }

    if (!game->safeOpened)
    {
        printf("\nThe key fits the safe. It opens with a click.\n");
        printf("PLACEHOLDER BACKSTORY PART 3 / FULL TRUTH:\n");
        printf("Replace this paragraph with the final revelation of your story.\n");
        game->safeOpened = 1;
        consumeItem(game, USELESS_KEY);
    }
}

void useGlue(GameState *game)
{
    if (game->glueUsed)
    {
        printf("\nYou have already used the glue on the cracked panel.\n");
        return;
    }

    if (!hasItem(game, GLUE))
    {
        printf("\nYou do not have the glue tube.\n");
        return;
    }

    printf("\nPLACEHOLDER GLUE INTERACTION:\n");
    printf("You repair a cracked panel. A short message appears behind it.\n");
    printf("\"The truth was hidden in plain sight.\"\n");
    game->glueUsed = 1;
    consumeItem(game, GLUE);
    game->score += 25;
}

void attemptFinalDoor(GameState *game)
{
    printf("\nThe exit has a fingerprint scanner.\n");

    if (!hasItem(game, GLOVE))
    {
        printf("You cannot operate the scanner without the rubber glove.\n");
        printf("You may return to Room 1 and collect it from the cupboard.\n");
        return;
    }

    if (askYesNo("Use the rubber glove on the scanner? (y/n): "))
    {
        printf("\nThe scanner fails to read the fingerprint through the glove.\n");
        printf("The lock releases. You step outside.\n");
        consumeItem(game, GLOVE);
        game->escaped = 1;
    }
}

void manageInventory(GameState *game)
{
    int i;
    int choice;
    int item;

    printf("\n============== INVENTORY ==============\n");

    if (game->inventoryCount == 0)
    {
        printf("Your inventory is empty.\n");
        pressEnter();
        return;
    }

    for (i = 0; i < game->inventoryCount; i++)
    {
        printf("%d. %s\n", i + 1, itemNames[game->inventory[i]]);
    }

    printf("Slots used: %d/%d\n", game->inventoryCount, INVENTORY_CAPACITY);
    printf("0. Return\n");
    choice = readInteger("Select an item to discard, or 0 to return: ");

    if (choice == 0)
    {
        return;
    }

    if (choice < 1 || choice > game->inventoryCount)
    {
        printf("Invalid selection.\n");
        return;
    }

    item = game->inventory[choice - 1];
    returnItemToOrigin(game, item);
    printf("%s returned to its original location.\n", itemNames[item]);
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
        printf("Your inventory is full. Discard an item before collecting this one.\n");
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

        printf("Please enter a valid whole number.\n");
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
            /* Discard excess input. */
        }
    }
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

        if (sameText(answer, "Y") || sameText(answer, "YES"))
        {
            return 1;
        }

        if (sameText(answer, "N") || sameText(answer, "NO"))
        {
            return 0;
        }

        printf("Please enter Y or N.\n");
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
    fgets(line, sizeof(line), stdin);
}
