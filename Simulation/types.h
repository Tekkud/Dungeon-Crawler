#pragma once
//#include "../support/message.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
//Instantiation struct - everything game needs to initialize


/*
    Intent types:
        0 - move up
        1 - move down
        2 - move left
        3 - move right
        4 - skip up
        5 - skip down
        6 - skip left
        7 - skip rights
*/

typedef struct intents {
    uint8_t player_ID; // maybe I'll add unique names in the future but for now a number id is fine
    char command;
} intent_t;

//stores map boundaries
typedef struct bounds {
    int x;
    int y;
    int z;
} bounds_t;



typedef struct map{
    char*** grid;
    bounds_t* bounds;
} map_t;


typedef struct args{
    char* path;
    bool plain;
    int totalgold;
    int minpiles;
    int maxpiles;
    int seed;
    int blackholespawnrate;
} args_t;


typedef struct entity{
    int ID; //ID denotes type - player, monster, spectator etc
    //addr_t addr;
    map_t* map;
    int x;
    int y;
    int z;
} entity_t;



typedef struct state {
    FILE* map_file;
    bounds_t bounds;
    map_t* map;
    entity_t** entities;
} state_t;






typedef struct coords {
  int x;
  int y;
  int z;
} coords_t;

typedef struct room{
  coords_t* coords;
  coords_t* dimensions;
} room_t;

typedef struct rooms{
  room_t* room;
  int size;
} rooms_t;




//internal family system
//internal external voices