#include "StuckInTheDark.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> // For the visibility flag
#include <ctype.h> // For case-insensitive comparison


// Arrays to hold item names and their descriptions
const char *item_names[] = {
    "Water Flask", 
    "Torch", 
    "Worn Long Sword", 
    "Golden Coin", 
    "Stick"  
};


const char *item_descriptions[] = {
    "Restores 20 HP (one-time use)", 
    "Instantly look in the room (infinite use)", 
    "Deals 30 damage to creatures (one-time use)", 
    "Kills a creature instantly (one-time use)",
    "Well that's better than my fists(usable only 3 times)"
};


// Prototype for the function to fetch item description
const char* get_item_description(const char* item_name);

int main() {
    // Initialize player
    Player player;
    initialize_player(&player);

    // Create rooms
    Room center, up, down, left, right, up_left, up_right, down_left, down_right;
    initialize_room(&center, "You are in the starting room, dimly lit and mysterious.");
    initialize_room(&up, "You have entered a room with a strange atmosphere.");
    initialize_room(&down, "A dark chamber with a strange humming sound.");
    initialize_room(&left, "A damp cave with water dripping from the walls.");
    initialize_room(&right, "A narrow corridor with flickering torches.");
    initialize_room(&up_left, "A room filled with ancient carvings on the walls.");
    initialize_room(&up_right, "A bright room with a strange pedestal in the center.");
    initialize_room(&down_left, "A shadowy room with a chilling breeze.");
    initialize_room(&down_right, "A warm chamber with golden lights.");

    // Connect rooms
    center.up = &up;
    center.down = &down;
    center.left = &left;
    center.right = &right;
    up.left = &up_left;
    up.right = &up_right;
    down.left = &down_left;
    down.right = &down_right;

    up.down = &center;
    down.up = &center;
    left.right = &center;
    right.left = &center;

    up_left.right = &up;
    up_right.left = &up;
    down_left.right = &down;
    down_right.left = &down;

    // Add creatures to rooms with item drops (only item names)
    add_creature(&down, "Humming Shadow", 20, 5, NULL); // No drops
    add_creature(&left, "Water Wraith", 40, 5, "Water Flask"); // Drop is now just the item name
    add_creature(&right, "Torch Keeper", 30, 5, "Torch"); // Drop is now just the item name
    add_creature(&up_left, "Ancient Sentinel", 45, 10, "Worn Long Sword"); // Drop is now just the item name
    add_creature(&down_left, "Shadow Beast", 45, 10, NULL); // No drops
    add_creature(&down_right, "Golden Golem", 40, 15, "Golden Coin"); // Drop is now just the item name

    // Start the game loop
    game_loop(&player, &center);

    // Cleanup
    cleanup_player(&player);
    cleanup_room(&center);
    cleanup_room(&up);
    cleanup_room(&down);
    cleanup_room(&left);
    cleanup_room(&right);
    cleanup_room(&up_left);
    cleanup_room(&up_right);
    cleanup_room(&down_left);
    cleanup_room(&down_right);

    return 0;
}

// Function Implementations

void initialize_player(Player *player) {
    player->name = "Hero";
    player->health = 100;
    player->strength = 10;
    player->inventory_size = 0;
    player->has_torch = false;
    player->torch_active = false;
    player->stick_uses_left = 3;  // Stick starts with 3 uses
}


void cleanup_player(Player *player) {
    for (int i = 0; i < player->inventory_size; i++) {
        free(player->inventory[i]);
    }
}

void initialize_room(Room *room, const char *description) {
    room->description = strdup(description);
    room->items = NULL;
    room->item_count = 0;
    room->up = room->down = room->left = room->right = NULL;
    room->creature = NULL;
    room->has_trap_triggered = false; // Initialize the trap flag to false

    // Add special items to the first room (Center Room)
    if (strcmp(description, "You are in the starting room, dimly lit and mysterious.") == 0) {
        room->items = malloc(sizeof(char *) * 1);
        room->items[0] = strdup("Stick");
        room->item_count = 1;
    }
}



void add_creature(Room *room, const char *name, int health, int strength, const char *drop) {
    room->creature = malloc(sizeof(Creature));
    room->creature->name = strdup(name);
    room->creature->health = health;
    room->creature->strength = strength;
    room->creature->visible = false; // Creature starts as hidden
    room->creature->drop = drop ? strdup(drop) : NULL; // Set drop (now just the item name)
    room->creature->is_defeated = false; // Creature starts not defeated
}

void cleanup_room(Room *room) {
    free(room->description);
    for (int i = 0; i < room->item_count; i++) {
        free(room->items[i]);
    }
    free(room->items);
    if (room->creature) {
        free(room->creature->name);
        if (room->creature->drop) free(room->creature->drop);
        free(room->creature);
    }
}

void list_inventory(Player *player) {
    printf("Inventory:\n");
    if (player->inventory_size == 0) {
        printf("Your inventory is empty.\n");
    } else {
        for (int i = 0; i < player->inventory_size; i++) {
            printf("- %s\n", player->inventory[i]);
        }
    }
}

void pickup_item(Player *player, Room *current_room, const char *item_name) {
    // Special pickup logic for the Stick
    if (strcmp(item_name, "Stick") == 0) {
        if (player->inventory_size < 5) { // Check inventory limit
            player->inventory[player->inventory_size++] = strdup(item_name);
            printf("You picked up the Stick.\n");
            free(current_room->items[0]);  // Remove the Stick from the room after pickup
            current_room->items[0] = NULL;
            current_room->item_count--;  // Decrease item count in the room
            return;
        } else {
            printf("Your inventory is full. You cannot pick up the Stick.\n");
            return;
        }
    }

    // Generic pickup logic for other items
    for (int i = 0; i < current_room->item_count; i++) {
        if (strcmp(current_room->items[i], item_name) == 0) {
            if (player->inventory_size < 5) { // Inventory limit is 5
                player->inventory[player->inventory_size++] = strdup(item_name);
                printf("You picked up %s.\n", item_name);
                free(current_room->items[i]);
                for (int j = i; j < current_room->item_count - 1; j++) {
                    current_room->items[j] = current_room->items[j + 1];
                }
                current_room->item_count--;
                return;
            } else {
                printf("Your inventory is full.\n");
                return;
            }
        }
    }

    printf("Item not found: %s\n", item_name);  // If the item is not found in the room
}


void attack_creature(Player *player, Room *current_room) {
    if (current_room->creature && current_room->creature->visible) {
        printf("You attack %s!\n", current_room->creature->name);
        current_room->creature->health -= player->strength;
        printf("You deal %d damage to %s. (Creature HP: %d)\n", player->strength, current_room->creature->name, current_room->creature->health);

        // Creature attacks back
        if (current_room->creature->health > 0) {
            player->health -= current_room->creature->strength;
            printf("%s deals %d damage to you. (Player HP: %d)\n", current_room->creature->name, current_room->creature->strength, player->health);
        }

        // Check if the creature is defeated
        if (current_room->creature->health <= 0) {
            current_room->creature->health = 0; // Ensure creature health does not go below 0
            current_room->creature->is_defeated = true; // Mark the creature as defeated
            printf("You defeated %s!\n", current_room->creature->name);

            // Handle item drop when the creature is defeated
            if (current_room->creature->drop) {
                printf("%s dropped: %s\n", current_room->creature->name, current_room->creature->drop);
                current_room->items = realloc(current_room->items, sizeof(char *) * (current_room->item_count + 1));
                current_room->items[current_room->item_count++] = strdup(current_room->creature->drop);
                print_items_in_room(current_room);  // Print items in the room after drop
            }

            // Cleanup creature after it's defeated
            free(current_room->creature->name);
            if (current_room->creature->drop) free(current_room->creature->drop);
            free(current_room->creature);
            current_room->creature = NULL;
        }

        // If the player dies, game over
        if (player->health <= 0) {
            printf("You have been defeated by %s. Game over.\n", current_room->creature->name);
            exit(0);
        }
    } else {
        printf("There is no creature here to attack.\n");
    }
}



void print_items_in_room(Room *current_room) {
    if (current_room->item_count > 0) {
        printf("Items in the room:\n");
        for (int i = 0; i < current_room->item_count; i++) {
            printf("- %s: %s\n", current_room->items[i], get_item_description(current_room->items[i]));
        }
    } else {
        printf("There are no items in this room.\n");
    }
}


const char* get_item_description(const char* item_name) {
    for (int i = 0; i < sizeof(item_names) / sizeof(item_names[0]); i++) {
        if (strcasecmp(item_name, item_names[i]) == 0) {
            return item_descriptions[i];
        }
    }
    return "No description available for this item.";
}


void visualize_room(Room *current_room) {
    printf("\n _________________________\n");
    printf("|                         |\n");
    if (current_room->creature && current_room->creature->visible) {
        printf("|       (%d HP)           |\n", current_room->creature->health);
        printf("|        ^  ^             |\n");
        printf("|       ( o  o )          |\n");
        printf("|        \\_==_/           |\n");
    } else {
        printf("|                         |\n");
        printf("|                         |\n");
        printf("|                         |\n");
    }
    printf("|                         |\n");
    printf("|_________________________|\n");
}


void game_loop(Player *player, Room *starting_room) {
    char command[256];
    Room *current_room = starting_room;

    printf("Welcome, %s! Your adventure begins now.\n", player->name);

    while (1) {
        printf("\nCurrent Room: %s\n", current_room->description);
        visualize_room(current_room);
        printf("Enter command: ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0; // Remove newline

        if (strcmp(command, "exit") == 0) {
            printf("Exiting the game. Goodbye!\n");
            break;
        } else if (strcmp(command, "look") == 0) {
            printf("You look around: %s\n", current_room->description);

            if (current_room->creature) {
                current_room->creature->visible = true;
                printf("You see a %s here with %d health.\n", current_room->creature->name, current_room->creature->health);

                // Player takes damage from the creature when looking
                printf("The %s attacks you!\n", current_room->creature->name);
                player->health -= current_room->creature->strength;
                printf("You take %d damage from the %s. (Player HP: %d)\n", current_room->creature->strength, current_room->creature->name, player->health);

                // If the player's health drops to 0 or below, the player loses the game
                if (player->health <= 0) {
                    printf("You have been defeated by %s. Game over.\n", current_room->creature->name);
                    exit(0);
                }
            } else {
                printf("There is no creature in this room.\n");
            }

            if (current_room->item_count > 0) {
                print_items_in_room(current_room);  // Print items in room when looking
            } else {
                printf("There are no items in this room.\n");
            }
        } else if (strcmp(command, "inventory") == 0) {
            list_inventory(player);
        } else if (strncmp(command, "pickup ", 7) == 0) {
            pickup_item(player, current_room, command + 7);
        } else if (strcmp(command, "attack") == 0) {
            attack_creature(player, current_room);
        } else if (strncmp(command, "use ", 4) == 0) {
            // Handle item usage
            char item_name[256];
            strcpy(item_name, command + 4);
            use_item(player, current_room, item_name);
        } else if (strcmp(command, "move up") == 0) {
            if (current_room->up) {
                current_room = current_room->up;
                printf("You move up.\n");
                trigger_trap(player, current_room); // Check if the trap triggers in the "up" room
            } else {
                printf("You can't go that way.\n");
            }
        } else if (strcmp(command, "move down") == 0) {
            if (current_room->down) {
                current_room = current_room->down;
                printf("You move down.\n");
            } else {
                printf("You can't go that way.\n");
            }
        } else if (strcmp(command, "move left") == 0) {
            if (current_room->left) {
                current_room = current_room->left;
                printf("You move left.\n");
            } else {
                printf("You can't go that way.\n");
            }
        } else if (strcmp(command, "move right") == 0) {
            if (current_room->right) {
                current_room = current_room->right;
                printf("You move right.\n");
            } else {
                printf("You can't go that way.\n");
            }
        } else {
            printf("Unknown command: %s\n", command);
        }
    }
}

// Trigger the trap in the "up" room if the player enters it for the first time
void trigger_trap(Player *player, Room *current_room) {
    if (current_room->has_trap_triggered == false) {
        printf("As you enter the room, a poisonous gas fills the air!\n");
        player->health -= 10; // Deal 10 damage from the poisonous gas
        printf("You take 10 damage from the poisonous gas. (Player HP: %d)\n", player->health);
        current_room->has_trap_triggered = true; // Set the flag to true so the trap doesn't trigger again
    }
}



void use_item(Player *player, Room *current_room, const char *item_name) {
    if (strcasecmp(item_name, "Stick") == 0) {
        // Check if the Stick is in the player's inventory
        bool has_stick = false;
        for (int i = 0; i < player->inventory_size; i++) {
            if (strcmp(player->inventory[i], "Stick") == 0) {
                has_stick = true;
                break;
            }
        }

        if (!has_stick) {
            printf("You don't have the Stick in your inventory.\n");
            return;  // If the Stick is not in inventory, exit the function
        }

        // Check if there is a creature in the current room
        if (current_room->creature == NULL || !current_room->creature->visible) {
            printf("There is no creature in this room to use the Stick on.\n");
            return;  // If no creature is present, exit the function
        }

        if (player->stick_uses_left > 0) {
            // Stick usage logic
            printf("You use the Stick to attack the creature!\n");
            current_room->creature->health -= 15;
            printf("You deal 15 damage to %s. (Creature HP: %d)\n", current_room->creature->name, current_room->creature->health);

            // The creature attacks back
            if (current_room->creature->health > 0) {
                player->health -= current_room->creature->strength;
                printf("%s deals %d damage to you. (Player HP: %d)\n", current_room->creature->name, current_room->creature->strength, player->health);
            }

            // Check if the creature is defeated (health <= 0)
            if (current_room->creature->health <= 0) {
                current_room->creature->health = 0; // Ensure creature health doesn't go below 0
                current_room->creature->is_defeated = true; // Mark creature as defeated
                printf("You defeated %s!\n", current_room->creature->name);
                if (current_room->creature->drop) {
                    printf("%s dropped: %s\n", current_room->creature->name, current_room->creature->drop);
                    current_room->items = realloc(current_room->items, sizeof(char *) * (current_room->item_count + 1));
                    current_room->items[current_room->item_count++] = strdup(current_room->creature->drop);
                    print_items_in_room(current_room);  // Print items in the room after drop
                }
                free(current_room->creature->name);
                if (current_room->creature->drop) free(current_room->creature->drop);
                free(current_room->creature);
                current_room->creature = NULL;
            }

            // Decrease the number of uses of the Stick
            player->stick_uses_left--;
            printf("You used the Stick. It has %d use(s) left.\n", player->stick_uses_left);

            // If no uses are left, the Stick breaks
            if (player->stick_uses_left == 0) {
                printf("The Stick breaks after its last use. You can't use it anymore.\n");

                // Remove the Stick from the player's inventory
                for (int i = 0; i < player->inventory_size; i++) {
                    if (strcmp(player->inventory[i], "Stick") == 0) {
                        free(player->inventory[i]);
                        for (int j = i; j < player->inventory_size - 1; j++) {
                            player->inventory[j] = player->inventory[j + 1];
                        }
                        player->inventory_size--;  // Remove the Stick from the inventory
                        break;
                    }
                }
            }
        } else {
            printf("The Stick is broken and cannot be used anymore.\n");
        }
    }
    // Handle other items similarly (Water Flask, Torch, etc.)
    else if (strcasecmp(item_name, "Water Flask") == 0) {
        // Water Flask: Restore 20 HP
        if (player->health < 100) {
            player->health += 20;
            if (player->health > 100) {
                player->health = 100; // Ensure the health does not exceed 100
            }
            printf("You used the Water Flask. Your health is now %d.\n", player->health);
            // Remove Water Flask from inventory
            for (int i = 0; i < player->inventory_size; i++) {
                if (strcmp(player->inventory[i], "Water Flask") == 0) {
                    free(player->inventory[i]);
                    for (int j = i; j < player->inventory_size - 1; j++) {
                        player->inventory[j] = player->inventory[j + 1];
                    }
                    player->inventory_size--;
                    break;
                }
            }
        } else {
            printf("Your health is already full, you cannot use the Water Flask.\n");
        }
    } else if (strcasecmp(item_name, "Torch") == 0) {
        // Torch: Instantly reveal the room
        printf("You use the Torch. Instantly looking around...\n");
        printf("You look around: %s\n", current_room->description);

        if (current_room->creature) {
            current_room->creature->visible = true;
            printf("You see a %s here with %d health.\n", current_room->creature->name, current_room->creature->health);
        } else {
            printf("There is no creature in this room.\n");
        }

        if (current_room->item_count > 0) {
            print_items_in_room(current_room);
        } else {
            printf("There are no items in this room.\n");
        }
    } else if (strcasecmp(item_name, "Worn Long Sword") == 0) {
        // Check if the Worn Long Sword is in the player's inventory
        bool has_sword = false;
        for (int i = 0; i < player->inventory_size; i++) {
            if (strcmp(player->inventory[i], "Worn Long Sword") == 0) {
                has_sword = true;
                break;
            }
        }

        if (!has_sword) {
            printf("You don't have the Worn Long Sword in your inventory.\n");
            return;  // If Worn Long Sword is not in inventory, exit the function
        }

        // Check if there's a creature in the room
        if (current_room->creature && current_room->creature->visible) {
            printf("You use the Worn Long Sword to attack the creature!\n");
            current_room->creature->health -= 30;
            printf("You deal 30 damage to %s. (Creature HP: %d)\n", current_room->creature->name, current_room->creature->health);

            // The creature attacks back
            if (current_room->creature->health > 0) {
                player->health -= current_room->creature->strength;
                printf("%s deals %d damage to you. (Player HP: %d)\n", current_room->creature->name, current_room->creature->strength, player->health);
            }

            // Check if the creature is defeated (health <= 0)
            if (current_room->creature->health <= 0) {
                current_room->creature->health = 0;  // Ensure creature health is not negative
                current_room->creature->is_defeated = true;  // Mark creature as defeated
                printf("You defeated %s!\n", current_room->creature->name);

                // If the creature drops an item, print it and add it to the room
                if (current_room->creature->drop) {
                    printf("%s dropped: %s\n", current_room->creature->name, current_room->creature->drop);
                    current_room->items = realloc(current_room->items, sizeof(char *) * (current_room->item_count + 1));
                    current_room->items[current_room->item_count++] = strdup(current_room->creature->drop);
                    print_items_in_room(current_room);  // Print items in the room after drop
                }

                // Cleanup the creature from the room
                free(current_room->creature->name);
                if (current_room->creature->drop) free(current_room->creature->drop);
                free(current_room->creature);
                current_room->creature = NULL;  // Remove the creature from the room
            }

            // Remove Worn Long Sword from inventory
            for (int i = 0; i < player->inventory_size; i++) {
                if (strcmp(player->inventory[i], "Worn Long Sword") == 0) {
                    free(player->inventory[i]);
                    for (int j = i; j < player->inventory_size - 1; j++) {
                        player->inventory[j] = player->inventory[j + 1];
                    }
                    player->inventory_size--;  // Remove the Worn Long Sword from the inventory
                    break;
                }
            }
        } else {
            printf("There is no creature to attack.\n");
        }
        } else if (strcasecmp(item_name, "Golden Coin") == 0) {
        // Golden Coin: Instantly kill the creature in the room
        if (player->inventory_size > 0) {
            printf("You use the Golden Coin. The creature in the room is instantly killed.\n");
            if (current_room->creature) {
                // Mark the creature as defeated
                current_room->creature->health = 0;  // Set health to 0
                current_room->creature->is_defeated = true; // Mark as defeated
                printf("The %s has been killed instantly!\n", current_room->creature->name);
                
                // Drop items from the creature (if any)
                if (current_room->creature->drop) {
                    printf("%s dropped: %s\n", current_room->creature->name, current_room->creature->drop);
                    current_room->items = realloc(current_room->items, sizeof(char *) * (current_room->item_count + 1));
                    current_room->items[current_room->item_count++] = strdup(current_room->creature->drop);
                    print_items_in_room(current_room);  // Print items in the room after drop
                }

                // Cleanup the creature from the room
                free(current_room->creature->name);
                if (current_room->creature->drop) free(current_room->creature->drop);
                free(current_room->creature);
                current_room->creature = NULL;  // Remove the creature from the room
            }
            
            // Remove Golden Coin from inventory
            for (int i = 0; i < player->inventory_size; i++) {
                if (strcmp(player->inventory[i], "Golden Coin") == 0) {
                    free(player->inventory[i]);
                    for (int j = i; j < player->inventory_size - 1; j++) {
                        player->inventory[j] = player->inventory[j + 1];
                    }
                    player->inventory_size--; // Remove the Golden Coin from the inventory
                    break;
                }
            }
        } else {
            printf("You don't have the Golden Coin.\n");
        }
    } else {
        printf("Item '%s' not found in your inventory.\n", item_name);
    }
}






