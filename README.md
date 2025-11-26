# CIS204-ai-research

## Program Purpose  
- This program will allow users to play a fully interactive, text based only game. Complete with exploration through a dungeon, random encounters with different creatures, and simple but fleshed out rpg style battles.
- While this program doesn't solve any practical problems, this might help solve the problem of one's boredom for even just a small while. It can act as a mini stress reliever or something just to pass the time.  
- This program will have users go through a procedurally generated tower dungeon that changes on each run of the program. While they won't be able to physically see it, as everthing will be text based, the program will let them know exactly what is around them. The goal is to make it to the top of the tower which will always only be 5 levels. The program will spawn chests for users to obtain items (such as a potion to heal themself or a new weapon/piece of gear to increase their stats with modifiers), create 5 maze-like levels for the user to go through where the only way to get to the next level is to find the staircase, and where said chests will spawn in random locations, and allow users to take on randomly spawned in monsters with different names and stats that get progressively more diffcult as the user goes through each level of the tower dungeon (starting at the bottom). Users will also be able to level up after a certain amount of battles (which the program will let them know when the do level up). Leveling up will increase the user's base stats and fully heal them upon said level up, allowing them to make it through the tower easier. One they've made it to the top of the tower dungeon, they'll get to fight the final boss, but if they lose all of their health points (HP) the program ends, telling them they've lost and to try again. It will also make sure to give them a brief explanation of all of this when the program starts.

## Input, Output, and Memory Management 
- Users will need to provide the direction they're going in for traveling the dungeon, their actions in combat (Attack, Magic [which will need its own special categories like fire, thunder, healing, etc.], Items, Guard, and Run [so they might have a chance to flee form battle]), and input to go upstairs, open chests, take items they find in chests [this will also need a small inventory to store said items], and replace items if their inventory is full.
- The program will output what's near the user, notify them if a monster appears and if they level up, show them their inventory if an item is used or needs to be replaced, ask what direction they want to go in, tell them what they find in a chest, and most likely will need to output many more things. Hopefully not to much to handle.
- Some of possibly many variables that will need to be definde would include: a struct to hold player stats, a struct for the monster's stats, maybe some temporary variables for storing variables, and possibly much more.
- How much memory will your program require to operate?
I imagine quite a bit of memory will be required for this program. If less can be allocated while still keeping everything optimized, the program will be sure to use less. Somethign with the scope of this might take and amount of memory that can't necessarily be predicted.

## Functions Needed 
- void combat();
- int attackCalc();
- int healing();
- Monster generate_monster(int floor);

## Data Structures 
```C
typedef struct {
    char name[25];

    int level;
    int maxHP;
    int currentHP;

    int attack;
    int defense;
    int magic;
    int speed;

    int expReward;
} Monster;

// (Might get changed)
```
## File Responsibilities - The following files are required in your project submission. Describe the purpose and contents of each file. 
- main.c (contains `main()`, controling the flow of the program)
- helpers.h 
- Helpers.c 

## Research Plan - Write a 250 words describing your strategy for using AI to help you to write the code for this project. 
- There are many things I could ask this AI as it helps me develop this code. The scope of this could be immense, and possibly could go outside my normal skills, but I’m willing to push those skills to complete this project. I could be bloating that up a bit further than I’m thinking, but to be fair, I’ve always been an overthinker.

- I’ll most likely be asking the AI to help set up the process and handle more technically inclined matters such as attack calculations, if the player or monsters have elemental magic weaknesses, or calculations for critical hits. I don’t know what I’d exactly ask the AI as I, in all honesty, am not super familiar with using AI systems and my only true experience was in my Information Systems class. I tend to try to do things the hard way most times more as an authenticity thing , but I’ve always been pretty neutral on it, so I have no quarrels with it as long as I don’t become too reliant on it.

- This project will act as a learning experience for me. Allowing me to become a tad more comfortable asking AI for help, and while I definitely can’t say if I’ll use it in the future as I probably still won’t be used to it, I can say it will be quite the rollercoaster working alongside it, using it as a tool rather than having it do my work for me like I’ve seen so many do.
- Some prompts I may ask might include: "How would you handle the user or the monsters landing a critical hit?", "How might you create a dungeon with walls, strairs and whatnot without consuming too much memory?", or "How would you go about potentially including status effects.

(I definitely feel I took "use your imagination" and stretched its limits here)
