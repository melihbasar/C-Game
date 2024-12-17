# C-Game
# Adventure Game

## Gameplay

In this game, you explore various rooms filled with creatures, items, and traps. You can move between rooms, look around, pick up items, use items, and attack creatures. The game ends when your health reaches zero.

### Main Commands
- `look`: Look around the current room and see any creatures or items.
- `pickup <item>`: Pick up an item in the current room.
- `use <item>`: Use an item from your inventory.
- `attack`: Attack a creature in the room.
- `move <direction>`: Move in the specified direction (`up`, `down`, `left`, `right`).
- `inventory`: Show your current inventory.
- `exit`: Exit the game.

## Code Structure

- **StuckInTheDark.h**: Contains declarations for all the game structures (Player, Room, Creature) and function prototypes.
- **StuckInTheDark.c**: Implements the game logic, including room setup, player actions, and gameplay loop.
- **StuckInTheDark.c**: Contains the entry point of the program that initializes the game and starts the loop.

## Game Logic

- The game world consists of interconnected rooms. Each room may contain creatures, items, and traps.
- When you enter a room, you can choose to interact with its contents using commands like `look`, `attack`, or `pickup`.
- Certain rooms have traps that deal damage to the player upon entry.
- Creatures in the room can attack the player when looked at or attacked.

## Compilation

Use the following `Makefile` to compile the game:

```make
CC = gcc
CFLAGS = -Wall -std=c99

all: game

game: game.c main.c game.h
	$(CC) $(CFLAGS) -o game game.c main.c

clean:
	rm -f game
