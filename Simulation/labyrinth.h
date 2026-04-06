#pragma once
#include "types.h"

int generate_labyrinth(map_t* map, int level);

int rooms_are_touching(room_t room1, room_t room2);

int place_hallways(map_t* map, int level, int x, int y, int iteration);

int place_rooms(map_t* map, int level);