#pragma once
//#include "../support/message.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
//Instantiation struct - everything game needs to initialize



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