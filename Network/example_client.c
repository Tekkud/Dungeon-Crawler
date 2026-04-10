#include "network.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

// int init_server(char* address, int* input_fd, void* suppliedptr, 
//                 packet_t* (*client_message_callback)(void*, clientmsg_t*),
//                 void (*disconnect_callback)(void*, int));
//
// int init_client(char* address, int port, int* input_socket_fd, void* suppliedptr, 
//                 void (*server_message_callback)(void*, servermsg_t*), 
//                 void (*disconnect_callback)(void*));
//

void server_message_callback(void* nothing, msg_t* msg){

  if(msg->typecode == 0){
  char* contents = malloc(msg->length + 1);
  memcpy(contents, msg->message, msg->length);
  contents[msg->length] = '\0';
  fprintf(stdout, "\nMESSAGE: %s\n", contents, msg->fd);
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


}


void* input_thread(void* arg){
    int writefd = *(int*)arg;
    char ch;
    fprintf(stdout, "Input: ");
    while((ch = fgetc(stdin)) != EOF){
        // buffer a full line
        static char buf[1024];
        static int i = 0;
        buf[i++] = ch;
        if(ch == '\n' || i >= 1024){
            // frame and write to pipe
            uint32_t length = i;
            uint8_t typecode = 1;
            write(writefd, &length, sizeof(uint32_t));
            write(writefd, &typecode, sizeof(uint8_t));
            write(writefd, buf, length);
            sleep(1);
            fprintf(stdout, "Input: ");
            i = 0;
        }
    }
    return NULL;
}


int main(int argc, char* argv[]){
    if(argv[1] == NULL){
        fprintf(stderr, "please provide port for client test\n");
        return 1;
    }
    int input_socket_fd[2];
    pipe(input_socket_fd);

    // write framed message to pipe
    char* msg = "12345678";
    uint32_t length = strlen(msg);
    uint8_t typecode = 1;




    int port = atoi(argv[1]);
    int client_fd = init_client("localhost", port, input_socket_fd);

    pthread_t input;
    pthread_create(&input, NULL, input_thread, &input_socket_fd[1]);
    fprintf(stdout,"\n");
    while(true){
        client_loop(&client_fd, input_socket_fd, -1, NULL, server_message_callback, NULL);
    }
}
