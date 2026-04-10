#include "network.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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
  int client_ids[1000];
  bool ready[1000];
  
  int id_tab;
  int num_clients;
  int num_ready;
    bool started;
} clientlist_t;

packet_t* client_message_callback(void* ptr, msg_t* message){
  clientlist_t* state = (clientlist_t*)ptr;
  char* contents = malloc(message->length + 1);
  memcpy(contents, message->message, message->length);
  contents[message->length] = '\0';
  fprintf(stdout, "message received from client: %s\n", contents);
  



  //broadcast received message
  packet_t* head = NULL;
  packet_t* tail = NULL;



  //---check Ready_Up--------

        bool ready_up = false;
        int client_id = -1;
        for(int i = 0; i < state->num_clients; i++){
            if(state->client_fds[i] == message->fd){
                
                if(strcmp(contents, "r") == 0 && state->ready[i] == 0 && state->started == false){
                    state->ready[i] = 1;
                    ready_up = true;
                }
                client_id = state->client_ids[i];
            }
        }
        if (ready_up == true){
        state->num_ready++;}

    if(state->num_ready >= state->num_clients ){
        if (state->started == false){
            for(int i = 0; i < state->num_clients; i++){
                packet_t* p = malloc(sizeof(packet_t));
                p->fd = state->client_fds[i];
                p->message = malloc(sizeof(int));
                p->length = sizeof(int);
                p->typecode = 3;
                p->nextmessage = NULL;
                if(head == NULL) head = p;
                if(tail != NULL) tail->nextmessage = p;
                tail = p;

            }
        }
        state->started = true;
        free(contents);
        return head;
    }


  

  //package messages as intents and send
  intent_t* intent = malloc(sizeof(intent_t));
  intent->player_ID = client_id;
  intent->command = contents[0];

  for(int i = 0; i < state->num_clients; i++){
      packet_t* p = malloc(sizeof(packet_t));
      p->fd = state->client_fds[i];
      p->message = malloc(sizeof(intent_t));
      memcpy(p->message, intent, sizeof(intent_t));
      p->length = sizeof(intent_t);
      p->typecode = 0;
      p->nextmessage = NULL;
      if(head == NULL) head = p;
      if(tail != NULL) tail->nextmessage = p;
      tail = p;
  }
  free(contents);
  free(intent);
  return head;
}

packet_t* disconnect_callback(void* ptr, int fd){
    clientlist_t* state = (clientlist_t*)ptr;
    int client_id = -1;
    for(int i = 0; i < state->num_clients; i++){
        if(state->client_fds[i] == fd){
            client_id = state->client_ids[i];
            state->client_fds[i] = state->client_fds[--state->num_clients];
            state->client_ids[i] = state->client_ids[state->num_clients];
            if(state->ready[i] == 1){
                state->num_ready--;
                state->ready[i] = state->ready[state->num_clients];
            }
            
            break;
        }
    }
    fprintf(stdout, "client disconnected\n");

    if (client_id == -1){
      fprintf(stderr, "Disconnection of unknown client");
      return NULL;
    }

  //broadcast disconnect
  packet_t* head = NULL;
  packet_t* tail = NULL;
  for(int i = 0; i < state->num_clients; i++){
      packet_t* p = malloc(sizeof(packet_t));
      p->fd = state->client_fds[i];
      p->message = malloc(sizeof(int));
      memcpy(p->message, &client_id, sizeof(int));
      p->length = sizeof(int);
      p->typecode = 2;
      p->nextmessage = NULL;
      if(head == NULL) head = p;
      if(tail != NULL) tail->nextmessage = p;
      tail = p;
  }
  return head;

}

packet_t* connect_callback(void* ptr, int num){
  clientlist_t* state = (clientlist_t*)ptr;
  int client_id = state->id_tab;
  state->client_fds[state->num_clients] = num;
  state->client_ids[state->num_clients++] = state->id_tab++;


  packet_t* head = NULL;
  packet_t* tail = NULL;

  // send new client the IDs of all already-connected clients
  for(int j = 0; j < state->num_clients - 1; j++){
    packet_t* p = malloc(sizeof(packet_t));
    p->fd = num;
    p->message = malloc(sizeof(int));
    memcpy(p->message, &state->client_ids[j], sizeof(int));
    p->length = sizeof(int);
    p->typecode = 1;
    p->nextmessage = NULL;
    if(head == NULL) head = p;
    if(tail != NULL) tail->nextmessage = p;
    tail = p;
  }
  fprintf(stdout, "Client Connected");
  // broadcast new client's ID to everyone excluding itself
  for(int i = 0; i < state->num_clients - 1; i++){
    packet_t* p = malloc(sizeof(packet_t));
    p->fd = state->client_fds[i];
    p->message = malloc(sizeof(int));
    memcpy(p->message, &client_id, sizeof(int));
    p->length = sizeof(int);
    p->typecode = 1;
    p->nextmessage = NULL;
    if(head == NULL) head = p;
    if(tail != NULL) tail->nextmessage = p;
    tail = p;
  }
  return head;
}

int main(int argc, char* argv[]){
  clientlist_t broadcast = {0};
  broadcast.id_tab = 0;
  int socket_fd = init_server("localhost");
while(true){
    server_loop(&socket_fd, &broadcast, 100000, &client_message_callback, &connect_callback, &disconnect_callback);
}
}
