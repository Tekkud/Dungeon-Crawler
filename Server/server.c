#include "server.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include "../support/message.h"
#include "../support/log.h"

/**************** main ****************/
/* Entry point of the program. Validate correct usage, then call parseArgs and run game.
 *
 * Caller provides: 
 *  argc  number of command-line arguments
 *  argv  string array of the command-line arguments
 *
 * We return:
 *  0 on success, 1 on failure
 *
 * Usage:
 *  ./server map.txt [seed]
 *    map.txt - pathname for a map file
 *    seed - seed for random-number generator
 */

 int main(int argc, char *argv[]){

    //------------------------
    // 1. Parse Arguments
    //------------------------
    args_t args;
    
    parseArgs(argc, argv, &args);

    //------------------------
    // 2. Load Map File
    //------------------------
    if(args.path == NULL){

        fprintf(stderr, "\nERROR. Invalid pathname for map file.\n\n");
        print(USAGE_MSG);
        return 1;
    }

    FILE* mapFile = fopen(args.path, "r");or to the result of re-simulating from the old gamestate_server using only the newly acknowledged intents?

    if(mapFile == NULL){

        fprintf(stderr, "\nError: Map file cannot be opened.\n\n");
        print(USAGE_MSG);
        return 1;
    }

    //------------------------
    // 3. Initialize Game
    //------------------------
    maps_t* maps = create_maps(mapFile);

    fclose(mapFile);

    //------------------------
    // 4. Initialize Server
    //------------------------
    int port = message_init(stderr);

    if(port == 0){
        fprintf(stderr, "\nError: Failed to initialize network.\n");
        return 1;
    }

    fprintf(stdout, "Network intialized at Port: %d\n", port);
    
    //------------------------
    // 5. Enter Game Loop
    //------------------------
    
    //------------------------
    // 6. End Game
    //------------------------
    
    //------------------------
    // 7. Clean Up
    //------------------------

    return 0
}

/**************** parseargs ****************/
/*

    Takes arguments and populates passed args struct

*/
args_t parseArgs(int argc, char *argv[], args_t* args){

    static struct option long_options[] = {
        {"plain", no_argument, 0, 'p'},
        {"gold", required_argument, 0, 'g'},
        {"minpiles", required_argument, 0, 'l'},
        {"maxpiles", required_argument, 0, 'h'},
        {"seed", required_argument, 0, 's'},
        {"blackholespawnrate", required_argument, 0, 'b'},
        {0, 0, 0, 0}
    };

    int opt;
    while((opt = getopt_long(argc, argv, "g:l:h:s:b:p", long_options, NULL)) != -1){
        switch(opt){
        case 'g':
        
            args->totalgold = atoi(optarg);
            break;
        case 'l':
            
            args->minpiles = atoi(optarg);
            break;
        case 'h':
            
            args->maxpiles = atoi(optarg);
            break;
        case 's':
            
            args->seed = atoi(optarg);
            break;
        case 'b':
            
            args->blackholespawnrate = atoi(optarg);
            break;
        case 'p':
            
            args->plain = false;
            break;
        case 0:
            
            break;
        default:
            printUsage();
            exit(1);  
        }
    }

 }


/**************** handleMessage ****************/
/* Process message from client and respond accordingly.
 *
 * Caller provides: 
 *  arg   uncasted game_t struct
 *  from  client that sent message
 *  buf   buffer containing message
 *
 * We return:
 *  true if message was fulfilled successfully,
 *  false otherwise
 */
static bool handleMessage(void* arg, const addr_t from, const char* buf){