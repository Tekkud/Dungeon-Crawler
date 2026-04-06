#pragma once
#include "types.h"
#include <stdio.h>
#include "types.h"


map_t* map_generate(state_t* params);

void map_delete(map_t* map);

int map_bounds(FILE* map_file, bounds_t* bounds);

char* mapToString(map_t* map, int level);

int map_populate(FILE* map_file, map_t* map);

int map_place(map_t* map, coords_t c, char ch);