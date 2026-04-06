// init.c
#include <stdio.h>
#include <stdlib.h>
#include "labyrinth.h"
#include "types.h"
#include "maps.h"
int main()
{
    state_t params = {0};

    params.map_file = fopen("../Maps/main.txt", "r");
    if (params.map_file == NULL) {
        fprintf(stderr, "ERROR: could not open map file\n");
        return 1;
    }

    // populate bounds->z before calling map_generate
    params.bounds.z = 2; // adjust as needed

    map_t* map = map_generate(&params);
    if (map == NULL) {
        fprintf(stderr, "ERROR: map_generate failed\n");
        fclose(params.map_file);
        return 1;
    }

    coords_t test = {0, 0, 0};

    map_place(map, test, 'E');

    // print each level
    for (int z = 0; z < params.bounds.z; z++) {
        printf("--- Level %d ---\n", z);
        char* str = mapToString(map, z);
        if (str != NULL) {
            printf("%s\n", str);
            free(str);
        }
    }

    map_delete(map);
    fclose(params.map_file);
    return 0;
}