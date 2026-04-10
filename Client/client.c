#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <GLFW/glfw3.h>
#include "types.h"
// int init_server(char* address, int* input_fd, void* suppliedptr, 
//                 packet_t* (*client_message_callback)(void*, clientmsg_t*),
//                 void (*disconnect_callback)(void*, int));
//
// int init_client(char* address, int port, int* input_socket_fd, void* suppliedptr, 
//                 void (*server_message_callback)(void*, servermsg_t*), 
//                 void (*disconnect_callback)(void*));
//


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


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(action == GLFW_PRESS || action == GLFW_REPEAT){
        int writefd = *(int*)glfwGetWindowUserPointer(window);
        char ch;
        switch(key){
            case GLFW_KEY_W: ch = 'w'; break;
            case GLFW_KEY_A: ch = 'a'; break;
            case GLFW_KEY_S: ch = 's'; break;
            case GLFW_KEY_D: ch = 'd'; break;
            case GLFW_KEY_R: ch = 'r'; break;
            case GLFW_KEY_Q:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                return;
            default: return;
        }

        uint32_t length = 1;
        uint8_t typecode = 0;
        write(writefd, &length, sizeof(uint32_t));
        write(writefd, &typecode, sizeof(uint8_t));
        write(writefd, &ch, sizeof(char));
    }
}


int main(int argc, char* argv[]){
    if(argv[1] == NULL){
        fprintf(stderr, "please provide port for client test\n");
        return 1;
    }

    //---------------Initialize Network-------------------
    int input_socket_fd[2];
    pipe(input_socket_fd);

    int port = atoi(argv[1]);
    int client_fd = init_client("localhost", port, input_socket_fd);

    int fds[3] = {client_fd, input_socket_fd[0], input_socket_fd[1]};
    pthread_t network;
    pthread_create(&network, NULL, network_thread, fds);

    //---------------Initialize Window-------------------
    if(!glfwInit()){
            fprintf(stderr, "failed to init glfw\n");
            return 1;
    }

    GLFWwindow* window = glfwCreateWindow(640, 480, "client", NULL, NULL);
    if(!window){
        fprintf(stderr, "failed to create window\n");
        glfwTerminate();
        return 1;
    }

    glfwSetWindowUserPointer(window, &input_socket_fd[1]);
    glfwSetKeyCallback(window, key_callback);

    //---------------Initialize Maps-------------------



    //---------------Controller Loop-------------------
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
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