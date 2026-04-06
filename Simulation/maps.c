#include "types.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "labyrinth.h"
#include "maps.h"
/*
    This file is responsible for initializing the maps and updating them

    It contains a map update function which will receive a gamestate object

    It will update all maps held in the gamestate structure, including the base maps and the visible maps
*/



/*
    Expects:
    params->map_file
    params->map_name
    params->bounds->z

    to be populated
*/
map_t* map_generate(state_t* params)
{

    //Populate X and Y bounds
    map_bounds(params->map_file, &params->bounds);
    //map_t* map = malloc
    map_t* map = malloc(sizeof(map_t));
    if (map == NULL){
        fprintf(stderr, "ERROR: failed to allocate map");
        return NULL;
    }
    map->bounds = &params->bounds;
    //fprintf(stdout,"\n%d, %d, %d\n", map->bounds->x, map->bounds->y, map->bounds->z);
    
        map->grid = malloc(map->bounds->x * sizeof(char**));

    if (map->grid == NULL || map->bounds == NULL){
        map_delete(map);
        return NULL; 
    }

    

    //Allocate memory for each element in the grid
    for (int i = 0; i < map->bounds->x; i++) {//allocate xy
        map->grid[i] = malloc(map->bounds->y * sizeof(char*));

        if (map->grid[i] == NULL) {
            map_delete(map);
            return NULL; 
        }

        for (int j = 0; j < map->bounds->y; j++){//allocate z

            map->grid[i][j] = malloc(map->bounds->z * sizeof(char));

            if (map->grid[i][j] == NULL) {
                map_delete(map);
                return NULL; 
            }
        }
    }

    map_populate(params->map_file, map);

    generate_labyrinth(map, 1);

    return map;
}

//get map bounds from file
int map_bounds(FILE* map_file, bounds_t* bounds)
{

    if(map_file == NULL){
        fprintf(stderr, "ERROR: Invalid Map File.");
        return 1;
    }

    int xmax = 0;
    int ymax = 0;
    int xtemp = 0;
    rewind(map_file);

    char ch;

    while((ch = fgetc(map_file)) != EOF){
        if (ch == '\n'){
        ymax++;
        xtemp = 0;
        continue;
        }
        xtemp++;
        if(xtemp > xmax){
            xmax = xtemp;
        }
    }
        bounds->x = xmax;
        bounds->y = ymax;
        rewind(map_file);
        return 0;
}

//Populate map with chars from file
//Generates map with empty dungeon levels
int map_populate(FILE* map_file, map_t* map)
{
    rewind(map_file);
    int x = 0;
    int y = 0;
    char ch = 0;
    while ((ch = fgetc(map_file)) != EOF) {
        if (ch == '\n') {
        y++;
        x = 0;
        continue;
        }
        else {
        map->grid[x][y][0] = ch;

        x++;
        } 
    }


    for (int i = 0; i < map->bounds->x; i++){
        for (int j = 0 ; j < map->bounds->y; j++){
            for(int k = 1; k < map->bounds->z; k++){
                map->grid[i][j][k] = ' ';
            }
        }
    }
        //
    for (int i = 0; i < map->bounds->x; i++){
        for( int j = 0; j < map->bounds->y; j++){
            if(map->grid[i][j][0] == 'u'){
                map->grid[i][j][1] = 'n';
            }
        }
    }
}

//change a char on map
int map_place(map_t* map, coords_t c, char ch){
    if (map ==NULL) {return 1;}
    map->grid[c.x][c.y][c.z] = ch;
    return 0;
}

//Map to string
char* mapToString(map_t* map, int level)
{
  if (map == NULL) {
    fprintf(stderr, "Null Map Recieved In mapToString\n");
    return NULL;
  }


  int sizex = map->bounds->x;
  int sizey = map->bounds->y;
  // Allocate memory for string
  char* string = malloc(sizex*sizey*sizeof(char) + sizey*sizeof(char) + sizeof(char));
  int k = 0;
  for(int j = 0; j < map->bounds->y; j++){
    for(int i = 0; i < map->bounds->x; i++){
      char ch = map->grid[i][j][level];
      if(ch == '\0'){
        string[k] = ' ';
        continue;
      }
      string[k] = map->grid[i][j][level];
      k++;
    }
    string[k] = '\n';
    k++;
  }
  string[k] = '\0';
  
  return string;
}

//Delete a map
void map_delete(map_t* map)
{
    if (map == NULL) return;
    
    if (map->grid != NULL) {
        for (int i = 0; i < map->bounds->x; i++) {
            if (map->grid[i] != NULL) {
                for (int j = 0; j < map->bounds->y; j++) {
                    free(map->grid[i][j]);
                }
                free(map->grid[i]);
            }
        }
        free(map->grid);
    }
    
    free(map);
}