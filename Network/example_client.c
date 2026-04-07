#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// int init_server(char* address, int* input_fd, void* suppliedptr, 
//                 packet_t* (*client_message_callback)(void*, clientmsg_t*),
//                 void (*disconnect_callback)(void*, int));
//
// int init_client(char* address, int port, int* input_socket_fd, void* suppliedptr, 
//                 void (*server_message_callback)(void*, servermsg_t*), 
//                 void (*disconnect_callback)(void*));
//

void server_message_callback(void* nothing, servermsg_t* msg){
  clientmsg_t* client_msg = (clientmsg_t*)msg->message;

  fprintf(stdout, "\nMESSAGE: %c, %d END\n", client_msg->msg, client_msg->fd);

}

int main(int argc, char* argv[]){

  if(argv[1] == NULL){
    fprintf(stderr, "please provide port for client test\n");
    return 1;
  }
  int input_socket_fd[2];
  pipe(input_socket_fd);
  write(input_socket_fd[1], "12345678", 8);
  int port = atoi(argv[1]);
  init_client("localhost", port, input_socket_fd, NULL, &server_message_callback, NULL);
}
