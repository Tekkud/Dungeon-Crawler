#include "types.h"
#include "labyrinth.h"


#include <stdbool.h>


int generate_labyrinth(map_t* map, int level){

  if(level == map->bounds->z){
    return 0;
  }


  for(int i = 0; i < map->bounds->x; i++){
    for(int j = 0; j < map->bounds->y; j++){
      if(map->grid[i][j][level] == '\0'){
        map->grid[i][j][level] = ' ';
      }
    }
  }


  if(level == map->bounds->z - 1){
    for(int i = 1; i < map->bounds->x - 1; i++){
      for(int j = 1; j < map->bounds->y - 1; j++){
        if(map->grid[i][j][level] != 'n'){
          map->grid[i][j][level] = ',';
        }
      }
    }
  }
    place_rooms(map, level);

  if(level == map->bounds->z - 1){
    for(int i = 1; i < map->bounds->x - 1; i++){
      for(int j = 1; j < map->bounds->y - 1; j++){
        if(map->grid[i][j][level] != 'n'){
          map->grid[i][j][level] = ',';
        }
      }
    }
  }
    int mapx = map->bounds->x;
    int mapy = map->bounds->y;
    /**** fill in border with stone ****/ 
    for(int i = 1; i < mapx - 1; i++){
      for(int j = 1; j < mapy - 1; j++){

        if(map->grid[i][j][level] == ' '){
          int ydirs[8] = {1, -1, 1, -1, -1,  1, 0,  0};
          int xdirs[8] = {0,  0, 1, -1,  1, -1, 1, -1};
          int numconnected2 = 0;
          for(int k = 0; k < 8; k++){
            if(j + ydirs[k] > 0 && j + ydirs[k] < mapy - 1 && i + xdirs[k] > 0 && i + xdirs[k] < mapx - 1){
              char ch = map->grid[i + xdirs[k]][j + ydirs[k]][level];
              if(ch == ' '){
                numconnected2++;
              }
            }
          }
      
          if(numconnected2 < 8){
            map->grid[i][j][level] = ';';
          }
        }
      }
    }





  generate_labyrinth(map, level + 1);

  return 0;
}





void shuffle(int* ints, int num){
  for(int i = 0; i < num; i++){
    int swap = rand()%num;
    int j = ints[swap];
    ints[swap] = ints[i];
    ints[i] = j;
  }
}

void stepback(int* dist, int stepback){
  if((*dist) > stepback){
    (*dist) -= stepback;
  }
  else{
    (*dist) = 0;
  }
}


int gen_room(map_t* map, int level, int roomx, int roomy, int b_x, int b_y, int regen){
  int stepback_val = 2;
  int hit = 0;
  int hitx = 0;
  int hity = 0;
  int hitxflag = 0;
  int hityflag = 0;
  int distx = 1;
  int disty = 1;
  int mapx = map->bounds->x;
  int mapy = map->bounds->y;
  int testinc = 0;



/****** Determining Bounds for Room ******/
if(!regen){//original room gen
    while(true){

      if(roomx - distx < 1 || roomx + distx > mapx - 2){// if out of bounds
        if(hitx == 0){
          stepback(&distx, stepback_val);
        }
        hitx = 1;
      }



      if(roomy - disty < 1 || roomy + disty > mapy - 2){//if out of bounds
        if(hity == 0){
          stepback(&disty, stepback_val);
        }
        hity = 1;
        //printf("hitdisty");
      }
      

      if(hity == 0){
        for(int x = roomx - distx; x <= roomx + distx; x++){//if hit object
          if(map->grid[x][roomy - disty][level] != ' ' || map->grid[x][roomy + disty][level] != ' '){
            stepback(&disty, stepback_val);
            hity = 1;
          }
        }
      }


      if(hitx == 0){
        for(int y = roomy - disty; y <= roomy + disty; y++){//if hit object
          if(map->grid[roomx - distx][y][level] != ' ' || map->grid[roomx + distx][y][level] != ' '){
            //printf("hitely");
            stepback(&distx, stepback_val);
            hitx = 1;
          }
        }
      }

      if((float)b_x <= (2.0f * (float)distx)){// check max room size
        stepback(&distx, stepback_val);
        hitx = 1;
      }
      if((float)b_y <= (2.0f * (float)disty)){
        stepback(&disty, stepback_val);
        hity = 1;
      }

      if(hity == 1 && hitx == 1){
        break;
      }



      if(hity == 0){//increment if not exiting
        disty++;
      }
      if(hitx == 0){
        distx++;
      }


    }
  }
  else{/****** Determining Bounds for Regen ******/


    int x = roomx;
    int y = roomy;
    disty = 1;
    distx = 1;
    while(true){
      if(map->grid[x][y + disty][level] != ' '){
        break;
      }
      else{ disty++;}
    }
    disty -= 2;
    while(true){
        if(map->grid[x + distx][y][level] != ' '){
          break;
        }
        else{ distx++;}
    }
    distx -= 2;
  }
  if(distx <= 0 || disty <= 0){
    return 0;
  }
  else{

    int a = 0;
    int b = 0;
    for(int x = roomx - distx; x <= roomx + distx; x++){

      for(int y = roomy - disty; y <= roomy + disty; y++){
        char* c = &map->grid[x][y][level];
        if(!regen){
          if(*c != 'n' && *c != 's') {*c = 'x';}

          if(x == roomx && y == roomy && distx > 1 && disty > 1){
            *c = *c != 'n' ? 's' : 'n';
          }
        }
        else{
          if(*c == ' '){*c = '.';}
          if(x == roomx && y == roomy && distx > 1 && disty > 1){
            *c = *c != 'n' ? 's' : 'n';
          }

          if(x == roomx-distx || x==roomx+distx ){
            *c = *c != 'n' ? '|' : 'n';

            a = 1;
          }
          if( y==roomy-disty || y==roomy+disty){
            *c = *c != 'n' ? '-' : 'n';

            b = 1;
          }
          if(a == 1 && b == 1){
            *c = *c != 'n' ? '+' : 'n';
          }
          a = 0;
          b = 0;

        }
      }
    } 

    if(regen){
    int inc = 0;
    int p = rand() % 4;
    int indx[4] = {0,0,1,-1};
    int indy[4] = {1,-1,0,0};
    int doorx;
    int doory;
    while(true){
      int xind = roomx + distx * indx[p] + inc * indx[p];
      int yind = roomy + disty * indy[p] + inc * indy[p];
      if(inc == 0){
        map->grid[xind][yind][level] = 'd';
        doorx = xind;
        doory = yind;
      }
      else{
        map->grid[xind][yind][level] = '#';
      } 
      if(map->grid[xind + indx[p]][yind + indy[p]][level] != ' '){
          break;
      }
      inc++;
    }

    
    if(map->grid[roomx][roomy][level] == 's'){

        int placewhat = rand() % 3;
        map->grid[roomx][roomy][level] = '.';
        int amax = 0;
        int maxindx = 0;
        int maxindy = 0;
        if(distx == 1 && disty == 1){
          maxindx = roomx;
          maxindy = roomy;
        }
        else{
          for(int x = roomx - distx + 1; x <= roomx + distx - 1; x++){
            for(int y = roomy - disty + 1; y <= roomy + disty - 1; y++){
              if(map->grid[x][y][level] == '.'){
                int dist = ((doorx-x) * (doorx - x)) + ((doory - y) * (roomy - y));
                if(dist > amax){
                  amax = dist;
                  maxindx = x;
                  maxindy = y;
                }
              }
            }
          }
        }
       // fprintf(stdout, "roomx: %d, roomy: %d, distx: %d, disty: %d, max_x: %d, max_y: %d\n", roomx, roomy, distx, disty, maxindx, maxindy);

        if(placewhat == 0){

          map->grid[maxindx][maxindy][level] = 'u';
          map->grid[maxindx][maxindy][level + 1] = 'n';
        }
        else if(placewhat == 1){

          map->grid[maxindx][maxindy][level] = 'u';
          map->grid[maxindx][maxindy][level + 1] = 'n';
        }
        else if(placewhat == 2){

          map->grid[maxindx][maxindy][level] = 'b';
        }
    }
    }



    }

    return 1;
}


int iswall(char elem){
  if(elem == '+' || elem == '|' || elem == '-' || elem == 'd' || elem == 'x'){
    return 1;
  }
  return 0;
}

int yes_stairs(char elem){//returns 1 if nonblocking and traversable
  if(elem == ',' || elem == '.'){
    return 1;
  }
  return 0;
}

int place_rooms(map_t* map, int level){
    int mapx = map->bounds->x;
    int mapy = map->bounds->y;
    int b_x, b_y;
  if(level < map->bounds->z - 1){
    /**** For each u found on the map *****/

    /**** Call gen_room function for that u *****/
    /** for gen_room function:
      *    pass: map, coordinate, placestairs bool
      *    with pass: place floor, walls, corners, stairs, doors
      **/

    for(int i = 0; i < map->bounds->x; i++){//generate staircase rooms first
      for(int j = 0; j < map->bounds->y; j++){
        if(map->grid[i][j][level] == 'n'){
          b_x = rand() % 4 + 9;
          b_y = rand() % 4 + 9;
          if(gen_room(map, level, i, j, b_x, b_y, 0) == 0){
            if(level > 0){
              map->grid[i][j][level] = ' ';
              map->grid[i][j][level - 1] = '.';
            }
          }
        }
      }
    }
    /**** For each ' ' found on the map ****/
    int xind[mapx];
    int yind[mapy];
    for(int i = 0; i < mapx; i++){
      xind[i] = i;
    }
    for(int j = 0; j < mapy; j++){
      yind[j] = j;
    }
    shuffle(xind, mapx);
    shuffle(yind, mapy);
    /**** Call gen_room function for ' ' ****/
    int numplaced = 1;
    while(numplaced > 0){//generate random room blocks
      numplaced = 0;
      for(int i = 0; i < mapx; i++){
        for(int j = 0; j < mapy; j++){
          if(map->grid[xind[i]][yind[j]][level] == ' '){
            b_x = rand() % 10 + 4;
            b_y = rand() % 10 + 4;
            numplaced += gen_room(map, level, xind[i], yind[j], b_x, b_y, 0);
          }
        }
      }
    }
    /**** For each ' ' on map ****/
    /**   if number of traversable neighbors == 2 ? hall : corridor(floor) **/ 
    for(int i = 1; i < mapx - 1; i++){
      for(int j = 1; j < mapy - 1; j++){

        if(map->grid[i][j][level] == ' '){
          int ydirs[8] = {1, -1, 1, -1, -1,  1, 0,  0};
          int xdirs[8] = {0,  0, 1, -1,  1, -1, 1, -1};
          int numconnected = 0;
          for(int k = 0; k < 8; k++){
            if(j + ydirs[k] > 0 && j + ydirs[k] < mapy - 1 && i + xdirs[k] > 0 && i + xdirs[k] < mapx - 1){
              char ch = map->grid[i + xdirs[k]][j + ydirs[k]][level];
              if(!iswall(ch)){
                numconnected++;
              }
            }
          }
      
          if(numconnected >= 5){
            map->grid[i][j][level] = '.';
          }
          else if(numconnected <= 4){
            map->grid[i][j][level] = '#';
          }
        }
      }
    }

    for(int i = 1; i < mapx - 1; i++){//get rid of room placeholders
      for(int j = 1; j < mapy - 1; j++){
        if(map->grid[i][j][level] == 'x'){
          map->grid[i][j][level] = ' ';
        }
      }
    }

    for(int i = 1; i < mapx - 1; i++){//regenerate detailed rooms populate with items/stairs
      for(int j = 1; j < mapy - 1; j++){
        char* ch = &map->grid[i][j][level];
        if(*ch == 's' || *ch == 'n'){
          if(gen_room(map, level, i, j, b_x, b_y, 1) == 0){
            if(level > 0 && (*ch == 'n')){
              map->grid[i][j][level] = ' ';
              if(level - 1 == 0){map->grid[i][j][level - 1] = '.';}
              else{map->grid[i][j][level - 1] = 'b';}//just to avoid placing items on top level
            }
          }
        }
      }
    }
  }

    




    //final check to ensure each level has at least one staircase. If not, place "emergency stairs" on random viable option

    bool is_stairs = false;

    int xind2[mapx];
    int yind2[mapy];
    for(int i = 0; i < mapx; i++){
      xind2[i] = i;
    }
    for(int j = 0; j < mapy; j++){
      yind2[j] = j;
    }
    shuffle(xind2, mapx);
    shuffle(yind2, mapy);
  if(level > 0){
    for(int i = 1; i < mapx - 1; i++){//check if there is at least one staircase
      for(int j = 1; j < mapy - 1; j++){
             if(map->grid[i][j][level] == 'n'){
             is_stairs = true;
             }
      }
    }
    int exit = 0;
    if(!is_stairs){
      for(int i = 1; i < mapx - 1; i++){//if no stairs from above, place one
        for(int j = 1; j < mapy - 1; j++){
          char* chcurr = &map->grid[xind2[i]][yind2[j]][level];
          char* chup = &map->grid[xind2[i]][yind2[j]][level - 1];
          if(yes_stairs(*chcurr) && yes_stairs(*chup)){
            *chcurr = 'n';
            *chup = 'u';
            exit = 1;
            break;
          }
        }
        if(exit){break;}
      }
    }
  }

  return 0;
}




int place_hallways(map_t* map, int level, int x, int y, int iteration){

  if (iteration > rand()%1500000 || x < 1 || x >= map->bounds->x - 2 || y < 1 || y >= map->bounds->y - 2 || level < 0 || level >= map->bounds->z) {
    map->grid[x][y][level] = 'u';
    map->grid[x][y][level + 1] = 'u';
    return 0;
  }


  int valid = 0;
  int numconnected = 0;
  int u = y + 1;
  int d = y - 1;
  int l = x - 1;
  int r = x + 1;

  char up = map->grid[x][u][level];
  char down = map->grid[x][d][level];
  char left = map->grid[l][y][level];
  char right = map->grid[r][y][level];

  bool ut = false;
  bool dt = false;
  bool lt = false;
  bool rt = false;

  if(up == '#'){
    numconnected++;
    ut = true;
  }
  if(down == '#'){
    numconnected++;
    dt = true;
  }
  if(left == '#'){
    numconnected++;
    lt = true;
  }
  if (right == '#'){
    numconnected++;
    rt = true;
  }

  int arand;  
  if(numconnected == 0){
    dt = true;
    lt = true;
    rt = true;
    ut = true;
  }
  int ind[4] = {1,2,3,4};
  shuffle(ind, 4);
  int ind1, ind2;
  for(int i = 0; i < 1000; i++){
    int branch = rand() % 100 > 80 ? 1:0;
    switch(ind[i]){
      case 1:
        if((ut || branch) && map->grid[x][d][level] == ' '){
          map->grid[x][d][level] = '#';
          ind1 = x;
          ind2 = d;
          valid = 1;
        }
        break;

      case 2:
        if((dt || branch) && map->grid[x][u][level] == ' '){
          map->grid[x][u][level] = '#';
          ind1 = x;
          ind2 = u;
          valid = 1;
        }
        break;

      case 3:
        if((lt || branch) && map->grid[r][y][level] == ' '){
          map->grid[r][y][level] = '#';
          ind1 = r;
          ind2 = y;
          valid = 1;
        }
        break;

      case 4:
        if((rt || branch) && map->grid[l][y][level] == ' '){
          map->grid[l][y][level] = '#';
          ind1 = x;
          ind2 = d;
          valid = 1;
        }
        break;

    }
    if(valid){
      break;
    }

  }
  if(valid == 1){
    place_hallways(map, level, ind1, ind2, iteration + 1);
  }
  return 0;
}