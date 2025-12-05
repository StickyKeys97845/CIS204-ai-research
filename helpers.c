/* helpers.c
 * Stubs for helper functions used by the CIS204 text-based dungeon RPG.
 *
 * Each function below contains an algorithmic description as comments but
 * intentionally does not implement the logic. These stubs return minimal
 * placeholder values where required so the file is syntactically complete.
 */

#include "helpers.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>

/* generate_monster
 * Algorithm (high-level):
 * 1. Use `floor` (1..5) to determine a difficulty multiplier.
 * 2. Choose a monster template/name from a short list appropriate to the
 *    floor range (e.g. weak creatures on floor 1, stronger on higher floors).
 * 3. Set the monster's level to be approximately the floor or floor +/- 1.
 * 4. Compute base stats (maxHP, attack, defense, magic, speed) from the
 *    template multiplied by the difficulty multiplier. Optionally add a small
 *    random variation to each stat so monsters of the same floor aren't
 *    identical.
 * 5. Set currentHP = maxHP and choose an expReward proportional to the
 *    monster's level and stats.
 * 6. Return the constructed Monster.
 */
Monster generate_monster(int floor) {
    Monster m;
    memset(&m, 0, sizeof(Monster));

    /* clamp floor */
    if (floor < 1) floor = 1;
    if (floor > 5) floor = 5;

    /* simple name pools per floor */
    const char *names_floor1[] = {"Goblin", "Rat", "Cave Bat"};
    const char *names_floor2[] = {"Skeleton", "Orc", "Giant Spider"};
    const char *names_floor3[] = {"Zombie", "Bandit", "Warg"};
    const char *names_floor4[] = {"Mage Acolyte", "Ogre", "Wraith"};
    const char *names_floor5[] = {"Dread Knight", "Fire Drake", "Tower Guardian"};

    const char *chosen = "Monster";
    if (floor == 1) chosen = names_floor1[rand() % (sizeof(names_floor1)/sizeof(names_floor1[0]))];
    else if (floor == 2) chosen = names_floor2[rand() % (sizeof(names_floor2)/sizeof(names_floor2[0]))];
    else if (floor == 3) chosen = names_floor3[rand() % (sizeof(names_floor3)/sizeof(names_floor3[0]))];
    else if (floor == 4) chosen = names_floor4[rand() % (sizeof(names_floor4)/sizeof(names_floor4[0]))];
    else if (floor == 5) chosen = names_floor5[rand() % (sizeof(names_floor5)/sizeof(names_floor5[0]))];

    strncpy(m.name, chosen, NAME_LEN-1);
    m.name[NAME_LEN-1] = '\0';

    /* level roughly equals floor +/- 0..1 */
    m.level = floor + (rand() % 2);

    /* base stats scale with floor and level */
    m.maxHP = 20 + m.level * 10 + (rand() % 6); /* small rand */
    m.currentHP = m.maxHP;
    m.attack = 5 + m.level * 2 + (rand() % 4);
    m.defense = 2 + m.level + (rand() % 3);
    m.magic = 2 + (m.level / 2) + (rand() % 3);
    m.speed = 3 + (m.level / 2) + (rand() % 3);
    m.expReward = 10 * m.level + (rand() % 6);

    return m;
}


/* attackCalc
 * Algorithm (high-level):
 * 1. Determine a base damage value from the attacker's attack stat and the
 *    defender's defense stat. A common formula is:
 *       base = attacker_attack - (defender_defense / 2)
 * 2. Ensure base is at least 1 (or some minimum) so attacks always can
 *    deal damage unless fully mitigated.
 * 3. Apply random variance to the damage (e.g., +/- 10-20%) to avoid fully
 *    predictable results.
 * 4. Optionally apply critical hit chance (e.g., based on attacker's speed
 *    or separate critical stat): if critical, multiply damage by 1.5 or 2.0.
 * 5. Return the final integer damage value.
 */
int attackCalc(int attacker_attack, int defender_defense) {
    /* Minimal usable implementation:
     * base = attacker_attack - (defender_defense / 2)
     * ensure at least 1
     * add small random variance +/-10%
     */
    int base = attacker_attack - (defender_defense / 2);
    if (base < 1) base = 1;
    int variance = (rand() % 21) - 10; /* -10..10 */
    int dmg = base + (base * variance) / 100;
    if (dmg < 1) dmg = 1;
    return dmg;
}


/* healing
 * Algorithm (high-level):
 * 1. Compute how much the player can be healed without exceeding maxHP:
 *       healable = p->maxHP - p->currentHP
 * 2. actual_heal = min(amount, healable)
 * 3. Increase p->currentHP by actual_heal.
 * 4. Return actual_heal so callers know how much HP was restored.
 */
int healing(Player *p, int amount) {
    if (p == NULL || amount <= 0) return 0;
    int healable = p->maxHP - p->currentHP;
    if (healable <= 0) return 0;
    int actual = amount;
    if (actual > healable) actual = healable;
    p->currentHP += actual;
    return actual;
}


/* combat
 * Algorithm (high-level):
 * 1. Present a short description of the encounter to the player.
 * 2. Enter a loop that continues while both combatants have HP > 0:
 *    a) Determine turn order (compare player->speed and monster->speed).
 *    b) On the active unit's turn, prompt or decide an action:
 *       - Player options: Attack, Magic (choose spell), Item (use item),
 *         Guard (reduce incoming damage), Run (attempt to escape).
 *       - Monster options: choose from simple AI (attack, cast, special).
 *    c) Resolve the action: call `attackCalc`, apply damage, handle healing,
 *       update HP, show messages to the player.
 *    d) After each action check for death/escape. If monster dies, award
 *       exp and possibly trigger level-up for the player. If player dies,
 *       end the game/loop with a loss condition.
 * 3. Clean up and return (e.g., update player state in the Player struct).
 */
void combat(Player *player, Monster *monster) {
    if (player == NULL || monster == NULL) return;

    printf("\n--- Combat Start: %s (Lv %d) ---\n", monster->name[0] ? monster->name : "Monster", monster->level);

    int guarded = 0; /* if player guarded last turn */

    while (player->currentHP > 0 && monster->currentHP > 0) {
        /* Display quick status */
        printf("\nPlayer HP: %d/%d | %s HP: %d/%d\n", player->currentHP, player->maxHP,
               monster->name, monster->currentHP, monster->maxHP);

        /* Determine turn order: higher speed goes first; tie -> player */
        int player_first = (player->speed >= monster->speed);

        for (int turn = 0; turn < 2 && player->currentHP > 0 && monster->currentHP > 0; ++turn) {
            if ((turn == 0 && !player_first) || (turn == 1 && player_first)) {
                /* Monster's action */
                int dmg = attackCalc(monster->attack, player->defense + (guarded ?  (player->defense/2) : 0));
                if (guarded) {
                    /* guard applies only for one monster attack */
                    printf("You guarded and reduced incoming damage.\n");
                    guarded = 0;
                }
                player->currentHP -= dmg;
                if (player->currentHP < 0) player->currentHP = 0;
                printf("%s attacks and deals %d damage to you.\n", monster->name, dmg);
                if (player->currentHP <= 0) break;
            } else {
                /* Player's action - prompt */
                printf("Choose action:\n");
                printf("1) Attack  2) Magic  3) Item  4) Guard  5) Run\n");
                int act = 0;
                printf("Enter choice: ");
                act = read_int_input();
                if (act == -1) act = 0;

                if (act == 1) {
                    int dmg = attackCalc(player->attack, monster->defense);
                    monster->currentHP -= dmg;
                    if (monster->currentHP < 0) monster->currentHP = 0;
                    printf("You attack and deal %d damage to %s.\n", dmg, monster->name);
                } else if (act == 2) {
                    /* Simple magic menu: 1 Heal, 2 Fire */
                    printf("Magic:\n1) Heal (cost: none)  2) Fire (damage = player magic stat)\n");
                    int mact = 0;
                    printf("Enter magic choice: ");
                    mact = read_int_input();
                    if (mact == -1) mact = 0;
                    if (mact == 1) {
                        int healed = healing(player, 20 + player->magic);
                        printf("You cast Heal and recover %d HP.\n", healed);
                    } else if (mact == 2) {
                        int dmg = player->magic + (rand() % (player->magic + 1));
                        monster->currentHP -= dmg;
                        if (monster->currentHP < 0) monster->currentHP = 0;
                        printf("You cast Fire and deal %d magic damage to %s.\n", dmg, monster->name);
                    } else {
                        printf("Invalid magic choice, you lose your turn.\n");
                    }
                } else if (act == 3) {
                    /* Item use: list inventory and use one */
                    if (player->inv_count == 0) {
                        printf("Your inventory is empty.\n");
                    } else {
                        print_inventory(player);
                        int slot = 0;
                        printf("Enter slot number to use (0 cancel): ");
                        slot = read_int_input();
                        if (slot == -1) slot = 0;
                        if (slot > 0 && slot <= player->inv_count) {
                            Item it = player->inventory[slot-1];
                            /* apply effect based on type */
                            if (it.type == ITEM_POTION) {
                                int healed = healing(player, it.power);
                                printf("You use %s and heal %d HP.\n", it.name, healed);
                                /* remove item by shifting */
                                for (int k = slot-1; k < player->inv_count-1; ++k) player->inventory[k] = player->inventory[k+1];
                                player->inv_count--;
                            } else if (it.type == ITEM_WEAPON) {
                                player->attack += it.power;
                                printf("You equip %s. Attack increased by %d (now %d).\n", it.name, it.power, player->attack);
                                for (int k = slot-1; k < player->inv_count-1; ++k) player->inventory[k] = player->inventory[k+1];
                                player->inv_count--;
                            } else if (it.type == ITEM_ARMOR) {
                                player->defense += it.power;
                                printf("You equip %s. Defense increased by %d (now %d).\n", it.name, it.power, player->defense);
                                for (int k = slot-1; k < player->inv_count-1; ++k) player->inventory[k] = player->inventory[k+1];
                                player->inv_count--;
                            } else {
                                printf("That item can't be used now.\n");
                            }
                        } else {
                            printf("Canceled item use.\n");
                        }
                    }
                } else if (act == 4) {
                    guarded = 1;
                    printf("You brace yourself and take a defensive stance.\n");
                } else if (act == 5) {
                    if (try_run(player, monster)) {
                        printf("You successfully escaped!\n");
                        return; /* exit combat */
                    } else {
                        printf("Escape failed! The battle continues.\n");
                    }
                } else {
                    printf("Invalid action. You lose your turn.\n");
                }
            }
        }
    }

    if (monster->currentHP <= 0) {
        printf("\nYou defeated %s and gain %d EXP!\n", monster->name, monster->expReward);
        player->exp += monster->expReward;
        /* Increase chance to find stairs: +5% per monster defeated, capped */
        player->stairs_bonus += 5;
        if (player->stairs_bonus > 50) player->stairs_bonus = 50;
        printf("Your chance to find stairs has increased by 5%% (bonus now %d%%).\n", player->stairs_bonus);

        /* Level up occurs at 100 EXP (Paper Mario style). Reset counter on level up. */
        if (player->exp >= 50) {
            player->level++;
            player->maxHP += 10;
            player->attack += 2;
            player->defense += 1;
            player->currentHP = player->maxHP;
            player->exp = 0; /* reset counter after level up */
            printf("You leveled up! Now level %d. HP fully restored.\n", player->level);
        }
    }
    if (player->currentHP <= 0) {
        printf("\nYou have been slain by %s...\n", monster->name);
    }
}


/* try_run
 * Algorithm (high-level):
 * 1. Compute a base escape chance using speed comparison:
 *       chance = base + (player->speed - monster->speed) * factor
 * 2. Clamp chance between reasonable bounds (e.g., 10%..90%).
 * 3. Roll a random value to determine success. If successful, return true
 *    and update any necessary state (e.g., mark battle as ended).
 * 4. If unsuccessful, return false and let the monster take a turn (or
 *    apply a penalty such as giving the monster a free attack).
 */
bool try_run(Player *player, Monster *monster) {
    if (player == NULL || monster == NULL) return false;
    /* base 30% chance + difference in speed * 5% */
    int base = 30;
    int diff = player->speed - monster->speed;
    int chance = base + diff * 5;
    if (chance < 10) chance = 10;
    if (chance > 90) chance = 90;
    int roll = rand() % 100;
    return (roll < chance);
}

/* ---------------- Item / Inventory helpers ---------------- */
Item create_potion(const char *name, int heal_amount) {
    Item it;
    it.type = ITEM_POTION;
    strncpy(it.name, name, NAME_LEN-1);
    it.name[NAME_LEN-1] = '\0';
    it.power = heal_amount;
    return it;
}

Item create_weapon(const char *name, int attack_bonus) {
    Item it;
    it.type = ITEM_WEAPON;
    strncpy(it.name, name, NAME_LEN-1);
    it.name[NAME_LEN-1] = '\0';
    it.power = attack_bonus;
    return it;
}

Item create_armor(const char *name, int defense_bonus) {
    Item it;
    it.type = ITEM_ARMOR;
    strncpy(it.name, name, NAME_LEN-1);
    it.name[NAME_LEN-1] = '\0';
    it.power = defense_bonus;
    return it;
}

bool add_item(Player *p, Item it) {
    if (p == NULL) return false;
    if (p->inv_count >= INVENTORY_CAP) return false;
    p->inventory[p->inv_count++] = it;
    return true;
}

bool replace_item(Player *p, int index, Item it) {
    if (p == NULL) return false;
    if (index < 0 || index >= p->inv_count) return false;
    p->inventory[index] = it;
    return true;
}

void print_inventory(Player *p) {
    if (p == NULL) return;
    printf("\n--- Inventory (%d/%d) ---\n", p->inv_count, INVENTORY_CAP);
    if (p->inv_count == 0) {
        printf("(empty)\n");
    } else {
        for (int i = 0; i < p->inv_count; ++i) {
            Item *it = &p->inventory[i];
            const char *typ = "?";
            switch (it->type) {
                case ITEM_POTION: typ = "Potion"; break;
                case ITEM_WEAPON: typ = "Weapon"; break;
                case ITEM_ARMOR:  typ = "Armor";  break;
                default: break;
            }
            printf("%d: %s (%s) power=%d\n", i+1, it->name, typ, it->power);
        }
    }
    printf("------------------------\n\n");
}

EventType random_event(void) {
    int r = rand() % 100; /* 0..99 */
    if (r < 60) return EV_NONE;
    if (r < 60+25) return EV_MONSTER;
    if (r < 60+25+10) return EV_CHEST;
    return EV_STAIRS;
}

EventType random_event_for_player(const Player *p) {
    int base_monster = 25;
    int base_chest = 20;
    int base_stairs = 5;

    int bonus = 0;
    if (p != NULL) bonus = p->stairs_bonus;
    if (bonus < 0) bonus = 0;
    int max_stairs = 100 - (base_monster + base_chest); /* prevent negative none */
    if (base_stairs + bonus > max_stairs) bonus = max_stairs - base_stairs;
    int stairs_prob = base_stairs + bonus;
    int none_prob = 100 - (base_monster + base_chest + stairs_prob);
    if (none_prob < 0) none_prob = 0;

    int r = rand() % 100;
    if (r < none_prob) return EV_NONE;
    if (r < none_prob + base_monster) return EV_MONSTER;
    if (r < none_prob + base_monster + base_chest) return EV_CHEST;
    return EV_STAIRS;
}

/* Read an integer from stdin safely. Returns -1 on invalid/EOF. */
int read_int_input(void) {
    char buf[64];
    if (!fgets(buf, sizeof(buf), stdin)) return -1;
    /* skip leading spaces */
    char *p = buf;
    while (*p && isspace((unsigned char)*p)) p++;
    if (*p == '\0' || *p == '\n') return -1;
    char *end;
    long v = strtol(p, &end, 10);
    if (p == end) return -1;
    if (v < INT_MIN || v > INT_MAX) return -1;
    return (int)v;
}

/* Use or equip an inventory item outside of combat. */
bool use_inventory_item(Player *p, int index) {
    if (p == NULL) return false;
    if (index < 0 || index >= p->inv_count) return false;
    Item it = p->inventory[index];
    if (it.type == ITEM_POTION) {
        int healed = healing(p, it.power);
        printf("You use %s and heal %d HP.\n", it.name, healed);
        /* remove item from inventory */
        for (int k = index; k < p->inv_count-1; ++k) p->inventory[k] = p->inventory[k+1];
        p->inv_count--;
        return true;
    } else if (it.type == ITEM_WEAPON) {
        /* Equip weapon: adjust stats, swap equipped if present */
        if (p->equipped_weapon.type != ITEM_NONE) {
            /* unequip previous */
            printf("Unequipping %s.\n", p->equipped_weapon.name);
            p->attack -= p->equipped_weapon.power;
            /* try to add previous to inventory */
            if (p->inv_count < INVENTORY_CAP) {
                p->inventory[p->inv_count++] = p->equipped_weapon;
            } else {
                printf("Inventory full, dropping %s.\n", p->equipped_weapon.name);
            }
        }
        /* equip new weapon */
        p->equipped_weapon = it;
        p->attack += it.power;
        /* remove item from inventory */
        for (int k = index; k < p->inv_count-1; ++k) p->inventory[k] = p->inventory[k+1];
        p->inv_count--;
        printf("Equipped %s. Attack is now %d.\n", it.name, p->attack);
        return true;
    } else if (it.type == ITEM_ARMOR) {
        if (p->equipped_armor.type != ITEM_NONE) {
            printf("Unequipping %s.\n", p->equipped_armor.name);
            p->defense -= p->equipped_armor.power;
            if (p->inv_count < INVENTORY_CAP) {
                p->inventory[p->inv_count++] = p->equipped_armor;
            } else {
                printf("Inventory full, dropping %s.\n", p->equipped_armor.name);
            }
        }
        p->equipped_armor = it;
        p->defense += it.power;
        for (int k = index; k < p->inv_count-1; ++k) p->inventory[k] = p->inventory[k+1];
        p->inv_count--;
        printf("Equipped %s. Defense is now %d.\n", it.name, p->defense);
        return true;
    }
    return false;
}
