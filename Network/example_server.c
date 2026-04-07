#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// int init_server(char* address, void* suppliedptr, 
//                 packet_t* (*client_message_callback)(void*, clientmsg_t*),
//                 void (*disconnect_callback)(void*, int));
//
// int init_client(char* address, int port, int* input_socket_fd, void* suppliedptr, 
//                 void (*server_message_callback)(void*, servermsg_t*), 
//                 void (*disconnect_callback)(void*));
//



typedef struct clientlist {
  int client_fds[1000];
  int num_clients;
} clientlist_t;

packet_t* client_message_callback(void* ptr, clientmsg_t* message){
  char ch = message->msg;
  fprintf(stdout, "message received from client: %c\n", ch);
  clientlist_t* state = (clientlist_t*)ptr;
  
  //broadcast received message
  packet_t* head = NULL;
  packet_t* tail = NULL;
  for(int i = 0; i < state->num_clients; i++){
      packet_t* p = malloc(sizeof(packet_t));
      p->fd = state->client_fds[i];
      p->message = malloc(sizeof(clientmsg_t));
      memcpy(p->message, message, sizeof(clientmsg_t));
      p->length = sizeof(clientmsg_t);
      p->typecode = 0;
      p->nextmessage = NULL;
      if(head == NULL) head = p;
      if(tail != NULL) tail->nextmessage = p;
      tail = p;
  }
  return head;
}

void disconnect_callback(void* ptr, int fd){
    clientlist_t* state = (clientlist_t*)ptr;
    for(int i = 0; i < state->num_clients; i++){
        if(state->client_fds[i] == fd){
            state->client_fds[i] = state->client_fds[--state->num_clients];
            break;
        }
    }
    fprintf(stdout, "client disconnected\n");
}

void connect_callback(void* ptr, int num){
  clientlist_t* state = (clientlist_t*)ptr;
  state->client_fds[state->num_clients++] = num;
  return;
}

int main(int argc, char* argv[]){
  clientlist_t broadcast = {0};
  init_server("localhost", &broadcast, &client_message_callback, &connect_callback, &disconnect_callback);
}
