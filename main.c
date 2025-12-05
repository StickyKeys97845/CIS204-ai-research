/* main.c
 * Main entry point for the CIS204 text-based dungeon RPG.
 *
 * This file controls the overall program flow: initializing the player,
 * displaying the intro, managing the dungeon traversal loop, and handling
 * game-over conditions.
 */

#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Forward declarations (optional, for clarity) */
void display_intro(void);
void initialize_player(Player *p);
void display_player_status(Player *p);
void game_loop(Player *player);


/* display_intro
 * Algorithm:
 * 1. Print a welcome message and brief explanation of the game.
 * 2. Describe the goal (reach the top of the 5-level tower).
 * 3. Explain the main mechanics: exploration, combat, items, leveling.
 * 4. Print a separator and wait for user input (e.g., "Press Enter to begin").
 */
void display_intro(void) {
    /* TODO: Print intro text to guide the player. */
    printf("=== DUNGEON TOWER RPG ===\n");
    printf("Welcome brave traveller! You have been locked away at the bottom of this dungeon and you need to escape. Make your way through the maze-like dungeon, finding stairs to move to the next floor. Be careful, though. There are dangerous monsters everywhere. You'll need to get some equipment if you have any hope of leaving. Good luck to you!\n");
    printf("Press Enter to continue...\n");
    /* TODO: Read a line of input to wait for user. */
    getchar();
}


/* initialize_player
 * Algorithm:
 * 1. Prompt the player to enter their character name.
 * 2. Set initial stats (level=1, maxHP=100, attack=10, defense=5, etc.).
 * 3. Set currentHP = maxHP and exp = 0.
 * 4. Populate the Player struct with these values.
 */
void initialize_player(Player *p) {
    /* TODO: Read player name from stdin. */
    printf("Enter your character name: ");
    /* Use fgets to read the name (allow spaces) and strip newline */
    if (fgets(p->name, NAME_LEN, stdin) != NULL) {
        /* strip newline */
        char *nl = strchr(p->name, '\n');
        if (nl) *nl = '\0';
    } else {
        /* fallback */
        strncpy(p->name, "Hero", NAME_LEN-1);
        p->name[NAME_LEN-1] = '\0';
    }

    /* TODO: Initialize all stats as described above. */
    p->level = 1;
    p->maxHP = 100;
    p->currentHP = 100;
    p->attack = 10;
    p->defense = 5;
    p->magic = 8;
    p->speed = 6;
    p->exp = 0;
    p->stairs_bonus = 0;
    /* Initialize inventory and equipped items to safe defaults to avoid
     * using uninitialized memory later. */
    p->inv_count = 0;
    for (int i = 0; i < INVENTORY_CAP; ++i) {
        p->inventory[i].type = ITEM_NONE;
        p->inventory[i].name[0] = '\0';
        p->inventory[i].power = 0;
    }
    p->equipped_weapon.type = ITEM_NONE;
    p->equipped_weapon.name[0] = '\0';
    p->equipped_weapon.power = 0;
    p->equipped_armor.type = ITEM_NONE;
    p->equipped_armor.name[0] = '\0';
    p->equipped_armor.power = 0;
}


/* display_player_status
 * Algorithm:
 * 1. Print the player's current stats in a readable format: name, level, HP,
 *    attack, defense, etc.
 * 2. Optionally show XP progress toward next level.
 * 3. Use clear formatting (e.g., a status bar or tabular layout).
 */
void display_player_status(Player *p) {
    /* TODO: Print player status with clear formatting. */
    printf("\n--- Player Status ---\n");
    printf("Name: %s\n", p->name);
    printf("Level: %d | HP: %d/%d\n", p->level, p->currentHP, p->maxHP);
    printf("Attack: %d | Defense: %d | Magic: %d | Speed: %d\n",
           p->attack, p->defense, p->magic, p->speed);
    printf("EXP: %d\n", p->exp);
    printf("---\n\n");
}


/* game_loop
 * Algorithm:
 * 1. Initialize the player's starting position on floor 1.
 * 2. Enter a loop that continues while the player is alive and hasn't reached
 *    the top floor.
 * 3. On each iteration:
 *    a) Display the current floor and nearby environment.
 *    b) Show available actions (move in a direction, check inventory, etc.).
 *    c) Prompt the player for input (e.g., "Choose action: ").
 *    d) Parse the input and execute the corresponding action:
 *       - Movement (up/down/left/right): update position, check for random
 *         encounter or chest.
 *       - Random encounter generation: call generate_monster() and then
 *         combat() if a monster spawns.
 *       - Chest discovery: show items, manage inventory.
 *       - Stairs: if found, move to next floor.
 *    e) After each action, check win/loss conditions.
 * 4. Handle game-over: display appropriate message if player wins or loses.
 */
void game_loop(Player *player) {
    /* TODO: Implement main game loop. */
    int current_floor = 1;
    int game_over = 0;

    printf("\nStarting game on floor %d...\n", current_floor);
    printf("TODO: Implement full game loop with exploration, combat, items.\n");

    while (!game_over && current_floor <= 5) {
        /* TODO: Display current dungeon state. */
        printf("\n--- Floor %d ---\n", current_floor);
        display_player_status(player);

        /* TODO: Prompt for player action. */
        printf("Choose an action:\n");
        printf("1. Move\n2. Check Inventory\n3. Status\n4. Quit\n");
        int choice = 0;
        printf("Enter the corresponding number to choose your action: ");
        choice = read_int_input();
        if (choice == -1) choice = 0;

        /* TODO: Execute action based on choice. */
        if (choice < 1 || choice > 4) {
            printf("Invalid choice. Please select a valid action.\n");
            continue;
        } else if (choice == 4) {
            printf("Quitting the game...\n");
            break;
        } else if (choice == 1) {
            printf("Which direction do you wish to go in?\n\n1. Up\n2. Down\n3. Left\n4. Right\n");
            int direction = 0;
            printf("Enter the corresponding number to choose your direction: ");
            direction = read_int_input();
            if (direction == -1) direction = 0;
            if (direction < 1 || direction > 4) {
                printf("Invalid direction. Please select a valid direction.\n");
                continue;
            }
            printf("You move in the chosen direction.\n\n");
            /* Determine what happens on this tile; use player's stairs bonus */
            EventType ev = random_event_for_player(player);
            if (ev == EV_NONE) {
                printf("You search the area but find nothing of interest.\n");
            } else if (ev == EV_MONSTER) {
                Monster m = generate_monster(current_floor);
                printf("A wild %s (level %d) appears!\n", m.name[0] ? m.name : "monster", m.level);
                /* Call combat (not yet implemented fully) */
                combat(player, &m);
                if (player->currentHP <= 0) {
                    printf("You have been defeated in battle.\n");
                    game_over = 1;
                    break;
                }
            } else if (ev == EV_CHEST) {
                /* Generate a random item and offer it to the player */
                int r = rand() % 3; /* 0..2 */
                Item found;
                if (r == 0) {
                    found = create_potion("Small Potion", 25);
                } else if (r == 1) {
                    found = create_weapon("Rusty Sword", 3);
                } else {
                    found = create_armor("Leather Vest", 2);
                }
                printf("You found a chest! Inside is: %s\n", found.name);
                printf("Use this item? (y/n): ");
                char ansbuf[16] = {0};
                if (fgets(ansbuf, sizeof(ansbuf), stdin) == NULL) ansbuf[0] = 'n';
                if (ansbuf[0] == 'y' || ansbuf[0] == 'Y') {
                    if (add_item(player, found)) {
                        printf("Added %s to inventory.\n", found.name);
                    } else {
                        printf("Inventory full. Choose an item to replace or 0 to cancel:\n");
                        print_inventory(player);
                        int slot = -1;
                        printf("Enter slot number to replace (0 to cancel): ");
                        slot = read_int_input();
                        if (slot == -1) slot = 0;
                        if (slot > 0 && slot <= player->inv_count) {
                            replace_item(player, slot-1, found);
                            printf("Replaced slot %d with %s.\n", slot, found.name);
                        } else {
                            printf("Canceled. You leave the %s in the chest.\n", found.name);
                        }
                    }
                } else {
                    printf("You leave the %s in the chest.\n", found.name);
                }
            } else if (ev == EV_STAIRS) {
                printf("You found stairs leading up! You ascend to the next floor.\n");
                current_floor++;
                if (current_floor > 5) {
                    break; /* player reached top */
                }
            }
        }
        else if (choice == 2) {
            /* Inventory menu: list and optionally use/equip items */
            print_inventory(player);
            if (player->inv_count == 0) continue;
            printf("Enter slot number to use/equip (0 to go back): ");
            int slot = read_int_input();
            if (slot <= 0) continue;
            if (slot > player->inv_count) {
                printf("Invalid slot.\n");
                continue;
            }
            /* use the selected item outside combat */
            if (!use_inventory_item(player, slot-1)) {
                printf("Couldn't use that item.\n");
            }
        } else if (choice == 3) {
            display_player_status(player);
        }
        /* TODO: Check for level-up after combat. */
        /* TODO: Check for floor advance beyond stairs handling. */
        /* TODO: Check for death after combat (handled above). */
    }

    if (current_floor > 5) {
        printf("\nCongratulations! You've reached the top of the tower!\n");
        printf("TODO: Boss fight sequence.\n");
    } else {
        printf("\nGame Over. You were defeated.\n");
    }
}


/* main
 * Algorithm:
 * 1. Display the intro/welcome message.
 * 2. Initialize the player struct (prompt for name, set base stats).
 * 3. Call game_loop() to begin the main game flow.
 * 4. Clean up and exit.
 */
int main(void) {
    /* Seed RNG once at program start for randomized events */
    srand((unsigned int)time(NULL));

    display_intro();

    Player player;
    initialize_player(&player);

    printf("\nWelcome, %s!\n\n", player.name);
    display_player_status(&player);

    game_loop(&player);

    printf("\nThanks for playing!\n");
    return EXIT_SUCCESS;
}
