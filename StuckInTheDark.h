#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

// Creature Structure Definition
typedef struct Creature {
    char *name;
    int health;
    int strength;
    bool visible;
    char *drop;
    bool is_defeated;
} Creature;

// Room Structure Definition
typedef struct Room {
    char *description;
    char **items;
    int item_count;
    struct Room *up, *down, *left, *right;
    Creature *creature;
    bool has_trap_triggered;
} Room;

// Player Structure Definition
typedef struct Player {
    char *name;
    int health;
    int strength;
    char *inventory[5];
    int inventory_size;
    bool has_torch;
    bool torch_active;
    int stick_uses_left;
} Player;

// Function Prototypes
void initialize_player(Player *player);
void cleanup_player(Player *player);
void initialize_room(Room *room, const char *description);
void add_creature(Room *room, const char *name, int health, int strength, const char *drop);
void cleanup_room(Room *room);
void game_loop(Player *player, Room *starting_room);
void list_inventory(Player *player);
void pickup_item(Player *player, Room *current_room, const char *item_name);
void attack_creature(Player *player, Room *current_room);
void visualize_room(Room *current_room);
void print_items_in_room(Room *current_room);
void use_item(Player *player, Room *current_room, const char *item_name);
void print_item_description(const char *item_name);
void trim_spaces(char *str);
int strcasecmp_custom(const char *s1, const char *s2);
const char* get_item_description(const char* item_name);

#endif