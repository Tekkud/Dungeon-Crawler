#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "./Graphics_Libraries/glad/include/glad/glad.h" 
#include "./Graphics_Libraries/glfw-3.4/include/GLFW/glfw3.h"
#include "types.h"
#include "maps.h"
#include "labyrinth.h"
#include "graphics.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



//---------------------------------------------------
//                     CONTROLLER LOOP
//---------------------------------------------------


//hmmm so which sort of information will clients be sending
//i guess right now only movement, spawn, disconnect
//ill use typecode 1 for spawn, 2 for disconnect and 0 for movement/controls
//but I also want to synchronize random seeds between clients, so ill use 3 for the
//server seed message -- but this will be just a typecode, not an intention
//others ill add as needed
//another I will need to add is a ready up system -- this will allow everyone to start
//their simulations at the same time so I dont need to send the whole simulation to each
//client that joins
//another system I need to set up the the rolling average tick speed adjustment for the
//clients local simulation so that it doesnt have consistent average divergence due to
//latency




void process_keys(char key, int action, char* string, char* truth){
    if(action == GLFW_PRESS){
      for(int i = MAX_KEYS - 2; i >= 0; i--){
        string[i + 1] = string[i];
        truth[i + 1] = truth[i];
      }
      string[0] = key;
      truth[0] = '1';
    }
    if(action == GLFW_RELEASE){
      for(int i = 0; i < MAX_KEYS; i++){
          if(string[i] == key){
            truth[i] = '0';
          }
      }
    }
}





char get_key(GLFWwindow* window){
    keys_t* keys = (keys_t*)glfwGetWindowUserPointer(window);
    if(keys == NULL){ return 0; }
    for(int i = 0; i < MAX_KEYS; i++){
        if(keys->truth[i] == '1'){
            return keys->string[i];
        }
    }
    return 0;
}


//---------------------------------------------------
//                     NETWORK
//---------------------------------------------------
void server_message_callback(void* nothing, msg_t* msg){
    
    //client processes
    // 0 - intentions
    // 1 - connections
    // 2 - disconnections
    // 3 - simulation start command
    if(msg->typecode == 0){
        intent_t intent;
        memcpy(&intent, msg->message, sizeof(intent_t));
        fprintf(stdout, "\nMESSAGE: player=%d command=%c\n", intent.player_ID, intent.command);
    }
    else if (msg->typecode == 1){
        int client_id;
        memcpy(&client_id, msg->message, sizeof(int));
        fprintf(stdout, "\nconnected client id: %d\n", client_id);
    }
    else if (msg->typecode == 2){
        int client_id;
        memcpy(&client_id, msg->message, sizeof(int));
        fprintf(stdout, "\ndisconnected client id: %d\n", client_id);
    }
    else if (msg->typecode == 3){
        fprintf(stdout, "all clients ready");
        fflush(stdout);
    }

}


void* network_thread(void* arg){
    int* fds = (int*)arg;
    int client_fd = fds[0];
    int pipe_read = fds[1];
    int pipe_write = fds[2];
    int input_socket_fd[2] = {pipe_read, pipe_write};
    while(true){
        client_loop(&client_fd, input_socket_fd, -1, NULL, server_message_callback, NULL);
    }
    return NULL;
}







//---------------------------------------------------
//                     CONTROLLER LOOP
//---------------------------------------------------
int game_loop(graphicEnv context, int write_fd){
    //---------------Controller Loop-------------------
    while(!glfwWindowShouldClose(context.window)){
        glfwPollEvents();

            unsigned int input_idle_counter = 0;
        input_idle_counter++;

        fprintf(stdout,"VAO: %u, VBO: %u, EBO: %u\n", context.VAO, context.VBO, context.EBO);
        fflush(stdout);
        printf("Number of indices: %d\n", context.num_indices);
        fflush(stdout);
        if(input_idle_counter >= 5){
            char key = get_key(context.window);
            if(key){

                input_idle_counter = 0;
                uint32_t length = 1;
                uint8_t typecode = 0;
                write(write_fd, &length, sizeof(uint32_t));
                write(write_fd, &typecode, sizeof(uint8_t));
                write(write_fd, &key, sizeof(char));
            }
        }
        glClearColor(0.2f,0.34f,0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(context.shaderProgram);
        glBindVertexArray(context.VAO);
        glDrawElements(GL_TRIANGLES, context.num_indices, GL_UNSIGNED_INT, 0);
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
        fprintf(stderr, "OpenGL error: %d\n", err);
        return 1;
        }
        glfwSwapBuffers(context.window);
        glfwPollEvents();


        usleep(16000);

    }
    glDeleteVertexArrays(1, context.VAO);
    glDeleteBuffers(1, context.VBO);
    glDeleteProgram(context.shaderProgram);

    glfwDestroyWindow(context.window);
    glfwTerminate();
    return 0;
}

/**************** mapToString ****************/
/* see map.h for documentation */
char* map_to_string(map_t* map, int level)
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

int main(int argc, char* argv[]){
    if(argv[1] == NULL){
        fprintf(stderr, "please provide port for client test\n");
        return 1;
    }


    //---------------Initialize Maps-------------------



    state_t params = {0};

    char resolved[4096];
    realpath("./Maps/main.txt", resolved);
    fprintf(stderr, "Looking for map file at: %s\n", resolved);
    params.map_file = fopen("./Maps/main.txt", "r");
    if(params.map_file == NULL){
        fprintf(stderr, "ERROR: could not open map file\n");
        return 1;
    }

    // populate bounds->z before calling map_generate
    params.bounds.z = 5; // adjust as needed

    map_t* map = map_generate(&params);
    if (map == NULL) {
        fprintf(stderr, "ERROR: map_generate failed\n");
        fclose(params.map_file);
        return 1;
    }

    // print each level
    for (int z = 0; z < params.bounds.z; z++) {
        printf("--- Level %d ---\n", z);
        char* str = mapToString(map, z);
        if (str != NULL) {
            printf("%s\n", str);
            free(str);
        }
    }


    //---------------Initialize Network-------------------
    int input_socket_fd[2];
    pipe(input_socket_fd);

    int port = atoi(argv[1]);
    int client_fd = init_client("localhost", port, input_socket_fd);

    int fds[3] = {client_fd, input_socket_fd[0], input_socket_fd[1]};
    pthread_t network;
    pthread_create(&network, NULL, network_thread, fds);

    //---------------Initialize GLFW-------------------
    graphicEnv context = {0};
    context.numrows = map->bounds->y;
    context.numcols = map->bounds->x;



    if(context_init(context.numrows, context.numcols, &context)!= 0){ 
        return 1;
    }

    //need to fix problem here -- something wrong
    //vao not initialized? Not sure. Something wrong..
    
      init_map_buffers(&context);


    char* map_contiguous = map_to_string(map, 0);
    update_map(map_contiguous, &context);

    int a = game_loop(context, input_socket_fd[1]);


    glfwDestroyWindow(context.window);
    glfwTerminate();
    map_delete(map);
    fclose(params.map_file);
    return 0;
}


//int main(int *argc, char *argv[]){

    //variables:

    // gamestate_client -- local simulation state, updated asynchronously

    // gamestate_server -- global simulation state, synchronized across clients


    // client loop:

    //poll client intents
    //send intents to server

    //update local intents object

    //update gamestate_client based on local intents

    //update display using gamestate_client

    //poll server

    //compare server intents object to client intents object

    //if most recent local (client) intents are included
        //if intents dont match
            //overwrite gamestate_client with copy of gamestate_server
        //if intents match
            //overwrite gamestate_server with copy of gamestate_client
    //if most recent intents are not included
        //server messages not up to date yet, do not change anything

//}