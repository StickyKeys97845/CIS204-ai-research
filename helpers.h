/* helpers.h
 * Declarations for structs and helper function prototypes
 * for the CIS204 text-based dungeon RPG.
 */

#ifndef HELPERS_H
#define HELPERS_H

#include <stdbool.h>

#define NAME_LEN 25
/* Inventory capacity (10 as requested) */
#define INVENTORY_CAP 10

/* Item system */
typedef enum {
    ITEM_NONE = 0,
    ITEM_POTION,
    ITEM_WEAPON,
    ITEM_ARMOR
} ItemType;

typedef struct Item {
    ItemType type;
    char name[NAME_LEN];
    int power; /* heal amount for potions, stat bonus for gear */
} Item;

typedef struct {
    char name[NAME_LEN];
    int level;
    int maxHP;
    int currentHP;
    int attack;
    int defense;
    int magic;
    int speed;
    int expReward;
} Monster;

typedef struct {
    char name[NAME_LEN];
    int level;
    int maxHP;
    int currentHP;
    int attack;
    int defense;
    int magic;
    int speed;
    int exp; /* current XP */
    /* Inventory: simple fixed-size array */
    int inv_count;
    Item inventory[INVENTORY_CAP];
    /* Equipped items (weapon and armor) */
    Item equipped_weapon;
    Item equipped_armor;
    /* Chance bonus (in percent) added to stairs discovery per monster defeated */
    int stairs_bonus;
} Player;

/* Function prototypes */

/* Generate a Monster appropriate for the given floor (1..5). */
Monster generate_monster(int floor);

/* Calculate damage from attacker to defender; returns damage dealt. */
int attackCalc(int attacker_attack, int defender_defense);

/* Heal the caller by `amount`; returns actual amount healed. */
int healing(Player *p, int amount);

/* Run a combat loop between the player and a monster. Uses pointers to
 * modify the player's state. */
void combat(Player *player, Monster *monster);

/* Attempt to run from battle; returns true on successful escape. */
bool try_run(Player *player, Monster *monster);

/* Item / Inventory helpers */
Item create_potion(const char *name, int heal_amount);
Item create_weapon(const char *name, int attack_bonus);
Item create_armor(const char *name, int defense_bonus);
/* Add item to player's inventory. Returns true on success, false if full. */
bool add_item(Player *p, Item it);
/* Replace an item at index with a new item. Returns true on success. */
bool replace_item(Player *p, int index, Item it);
/* Print inventory to stdout. */
void print_inventory(Player *p);

/* Use or equip an inventory item outside of combat. Returns true if an
 * action occurred (potion used or item equipped), false otherwise. */
bool use_inventory_item(Player *p, int index);

/* Random tile/event generation */
typedef enum {EV_NONE=0, EV_MONSTER, EV_CHEST, EV_STAIRS} EventType;
/* Return an EventType for a moved-to tile. */
EventType random_event(void);

/* Random event that takes the player's stairs bonus into account. */
EventType random_event_for_player(const Player *p);

/* Safe input helper (returns -1 on invalid) */
int read_int_input(void);

#endif /* HELPERS_H */
