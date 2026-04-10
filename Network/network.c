#define _POSIX_C_SOURCE 200112L
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <poll.h>
#include "network.h"

static const int MinPort = 1024;
static const int MaxPort = 65535;



int server_loop(int* fd0, void* suppliedptr, int timeout,
                packet_t* (*client_message_callback)(void*, msg_t*), 
                packet_t* (connect_callback)(void*, int),
                packet_t* (disconnect_callback)(void*, int));


int client_loop(int* clientfd, int* inputfd, int timeout, void* suppliedptr, 
                void (*receive_packet_callback)(void*, msg_t*), 
                void (*disconnect_callback)(void*));


/******** init_socket ***********/
/*
 *  - returns fd on success
 *
 *  - returns 0 on failure
 */
int init_socket(int* socket_fd){

  *socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(*socket_fd < 0){
    fprintf(stderr, "ERROR: Failed to create socket.\n");
    return 0;
  }
  return 1;
}


/**** get_addrinfo ***/
/*
 *  Takes : string or IP for desired connection
 *          addr_in initialized to NULL
 *
 *  Functionality: Sets addr_in
 *
 *  Returns: 1 on success, else 0
 *
 * */
int get_addrinfo(char* address, struct in_addr* addr_in){

  //get address info
  struct addrinfo *result, *rp;
  int s = getaddrinfo(address, NULL, NULL, &result);
  if(s != 0){
    fprintf(stderr, "ERROR: Invalid address supplied");
    exit(EXIT_FAILURE);
  }

  
  for (rp = result; rp != NULL; rp = rp->ai_next){
    if(rp->ai_family == AF_INET){
       struct sockaddr_in* ipv4 = (struct sockaddr_in*)rp->ai_addr;
       *addr_in = ipv4->sin_addr;
        //fprintf(stdout, "got IP address: %s\n", inet_ntoa(*addr_in));
        freeaddrinfo(result);
        return 1;
    }
  }
  freeaddrinfo(result);
  return 0;
}


/***************** init_addr_fd *****************
  *
  * initializes address and socket file descriptor using supplied char* address field
  * returns: 1 on success, otherwise 0
  *
  */
int init_network(char* address, int* socket_fd, struct in_addr* addr_in){
  //initialize socket
  if(!init_socket(socket_fd)){
    fprintf(stderr, "Failed to initialize socket");
    return 0;
  }


  //initialize address from input
  if(!get_addrinfo(address, addr_in)){
    fprintf(stdout, "ERROR: Invalid address supplied");
    return 0;
  }

  return 1;
}


typedef struct client_list{
  struct sockaddr_in address[150];
  int numconnected;
}clientlist_t;

int find_addr_index(struct sockaddr_in *addr, clientlist_t *clientlist){
  for(int i = 0; i < clientlist->numconnected; i++){
    struct sockaddr_in *currclient = &clientlist->address[i];
    if(currclient->sin_family == addr->sin_family &&
       currclient->sin_port == addr->sin_port &&
       currclient->sin_addr.s_addr == addr->sin_addr.s_addr){
       return i;
    }
  }
  return -1;
}



/*************** init_server *****************
  *
  * input: address - string
  *        socket_fd - pointer to file descriptor
  *        addr - sockaddr_in* to be populated
  *        port - optional, returns assigned port
  *
  * output - 1 on success, otherwise 0
  */
int init_server(char* address)
{
  int socket_fd;
  if(address == NULL){
    fprintf(stderr, "ERROR: init_server - invalid arguments received");
  }

  struct in_addr addr_in;
  //initialize socket and address
  init_network(address, &socket_fd, &addr_in);

  //initialize sockaddr with initialized addr_in
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(0);
  addr.sin_addr = addr_in;


  //bind socket
  socklen_t addrlen = (socklen_t)sizeof(addr);
  int instance = bind(socket_fd,(struct sockaddr *) &addr, addrlen);
  if(instance != 0){
    perror("ERROR: failed to bind socket");
    return -1;
  }
  //fetch port
  int portassignment = getsockname(socket_fd, (struct sockaddr * restrict)&addr,(socklen_t * restrict)&addrlen);
  if(portassignment != 0){
    perror("ERROR: failed to bind port");
    return -1;
  }
  int port = ntohs(addr.sin_port);
  if(port < MinPort || port > MaxPort){
    perror("ERROR: Invalid port");
    return -1;
  }

  int list = listen(socket_fd, SOMAXCONN);
  if(list != 0){
    perror("ERROR: failed to connect");
    return -1;
  }
  fprintf(stdout, "Server established at %s:%d\n", inet_ntoa(addr_in), port);
  return socket_fd;
}



packet_t* append_packets(packet_t* a, packet_t* b){
    if(a == NULL) return b;
    packet_t* cur = a;
    while(cur->nextmessage != NULL) cur = cur->nextmessage;
    cur->nextmessage = b;
    return a;
}


int server_loop(int* fd0, void* suppliedptr, int timeout,
                packet_t* (*client_message_callback)(void*, msg_t*), 
                packet_t* (*connect_callback)(void*, int),
                packet_t* (*disconnect_callback)(void*, int))
{
  int maxClients = 1000;
  struct pollfd fds[maxClients];
  memset(&fds, -1, sizeof(fds));
  fds[0].fd = *fd0;
  fds[0].events = POLLIN;
  while(true){
    int ret = poll(fds, maxClients, timeout);
    if (ret == 0) {return 0;}
    if (ret < 0) {return 1;}

    packet_t* packets = NULL;

    //---------------------poll new connections---------------------
    if(fds[0].revents & POLLIN){    //check for new connections
      for(int i = 1; i < maxClients; i++){
        if(fds[i].fd == -1){ // upon connection
          struct sockaddr_in clientaddr;
          socklen_t addr_len = sizeof(clientaddr);
          fds[i].fd = accept(fds[0].fd, (struct sockaddr *)&clientaddr, &addr_len);
          
          packets = append_packets(packets, connect_callback(suppliedptr, fds[i].fd));
          break;
        }
      }
    }


    for(int i = 1; i < maxClients; i++){    //poll all clients

      int err = 0;
      if(fds[i].fd != -1 && (fds[i].revents & POLLIN)){
        

        //---------------------read message length---------------------
        uint32_t msglen;
        int slen = 0;
        int n = 0;
        while(slen != sizeof(uint32_t)){
          n = recv(fds[i].fd, ((char*)&msglen) + slen, sizeof(uint32_t) - slen, 0);
          if(n <= 0){
            err = 1;
            break;
          }
          slen += n;
        }
        
        //---------------------read typecode---------------------
        uint8_t typecode;
        slen = 0;
        n = 0;
        while(slen != sizeof(uint8_t)){ 
          n = recv(fds[i].fd, ((char*)&typecode) + slen, sizeof(uint8_t) - slen, 0);
          if(n <= 0){
            err = 1;
            break;
          }
          slen += n;
        }

        //---------------------read message---------------------
        char* message = malloc(sizeof(char) * msglen);
        slen = 0;
        n = 0;

        while(slen != (int)(sizeof(char)*msglen)){ // read message based on size specified by the uint32 received
          n = recv(fds[i].fd, message + slen, msglen - slen, 0);
          if(n <= 0){
            err = 1;
            break;
          }
          slen += n;
        }

        //---------Collect Packets-----------
        

        msg_t* pkt = NULL;
        if (err == 1){
            free(message);
            close(fds[i].fd); //error and/or disconnect
            packets = append_packets(packets, disconnect_callback(suppliedptr, fds[i].fd));
            fds[i].fd = -1;
        }
        else{
          pkt = malloc(sizeof(msg_t)); //allocate a msg (should just replace with generic packet)
          pkt->fd = fds[i].fd;
          pkt->typecode = typecode;
          pkt->length = msglen;
          pkt->message = message;
          packets = append_packets(packets, client_message_callback(suppliedptr, pkt));
          free(pkt);
          free(message);
        }

      }
    }
    
        // debug before send loop
    packet_t* debug = packets;
    while(debug != NULL){
        fprintf(stdout, "queued packet: fd=%d typecode=%d length=%d\n", 
                debug->fd, debug->typecode, debug->length);
                fflush(stdout);
        debug = debug->nextmessage;
    }



    //---------Send Packets---------
    while(packets != NULL){
      int currfd = packets->fd;
      uint32_t currlen = packets->length;
      uint8_t typecode = packets->typecode;
      unsigned char* message = packets->message;
      send(currfd, &currlen, sizeof(currlen), 0);
      send(currfd, &typecode, sizeof(typecode), 0);
      send(currfd, message, currlen, 0);
      packet_t* next = packets->nextmessage;
      free(packets->message);
      free(packets);
      packets = next;
    }
  }

}






int init_client(char* address, int port, int* input_socket_fd)
{

  if(input_socket_fd == NULL){
    fprintf(stdout, "Please provide a socket for the input");
  }


    int client_fd; //client socket
    struct in_addr addr_in;

  //initialize socket and address
  init_network(address,&client_fd, &addr_in);



  //initialize sockaddr with initialized addr_in
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr = addr_in;


  //connect socket to server
  socklen_t addrlen = (socklen_t)sizeof(addr);
  int conn = connect(client_fd, (const struct sockaddr *)&addr, addrlen);
  if(conn != 0){
    perror("ERROR: failed to connect to server");
    return -1;
  }
  else{
    fprintf(stdout, "Established client connection to: %s:%d", address, port);
    fflush(stdout);
  }
  return client_fd;
}



int client_loop(int* clientfd, int* inputfd, int timeout,  void* suppliedptr, 
                void (*server_message_callback)(void*, msg_t*), 
                void (*disconnect_callback)(void*))
{
  struct pollfd fds[2];//establish file descriptors for polling
  fds[0].fd = *clientfd;
  fds[0].events = POLLIN;
  fds[1].fd = *inputfd;
  fds[1].events = POLLIN;
  while(true){
    int ret = poll(fds, 2, timeout); //poll fds
    if (ret == 0) {return 0;}
    if (ret < 0) {return 1;}
    if(fds[0].revents & POLLIN){ //if received data
      uint32_t msglen; 
      int slen = 0;
      int n = 0;
      while(slen != sizeof(uint32_t)){ //read one uint32_t
        n = recv(fds[0].fd, ((char*)&msglen) + slen, sizeof(uint32_t) - slen, 0);
        if(n <= 0){
          fprintf(stderr, "Packet error");
          disconnect_callback(suppliedptr);
          return 1;
        }
        slen += n;
      }
      uint8_t typecode;
      slen = 0;
      n = 0;
      while(slen != sizeof(uint8_t)){ // read one uint8
        n = recv(fds[0].fd, ((char*)&typecode) + slen, sizeof(uint8_t) - slen, 0);
        if(n <= 0){
          fprintf(stderr, "Packet error");
          disconnect_callback(suppliedptr);
          return 1;
        }
        slen += n;
      }
      slen = 0;
      n = 0;
      char* message = malloc(sizeof(char) * msglen);
      while(slen != (int)(sizeof(char)*msglen)){ // read message based on size specified by the uint32 received
        n = recv(fds[0].fd, message + slen, msglen - slen, 0);
        if(n <= 0){
          fprintf(stderr, "Packet error");
          free(message);
          disconnect_callback(suppliedptr);
          return 1;
        }
        slen += n;
      }
      msg_t* pkt = malloc(sizeof(msg_t));; //allocate a msg
      pkt->fd = *clientfd;
      pkt->typecode = typecode;
      pkt->length = msglen;
      pkt->message = message;
      server_message_callback(suppliedptr, pkt);
      free(pkt);
      free(message);
    }

    if (fds[1].revents & POLLIN) {
      char msg;
      int len = read(fds[1].fd, &msg, 1);
      if (len > 0) {
          send(fds[0].fd, &msg, len, 0);
      }
    }
  }
}


//
//
// int main(int argc, char* argv[]){
//
//
//   char address[30] = "127.0.0.1"; // address <- this will be passed in
//   int socket_fd_server;//socket fd <- this will be passed in
//   //
//   int port;
//   //initialize server
//   init_server(address, &socket_fd_server, &port);
//   sleep(1);
//
//   //initialize client
//   int socket_fd_client;
//   init_client(address, &socket_fd_client, port);
//   sleep(1);
//
//
//   //server accept client
//   struct sockaddr_in client_addr;
//   socklen_t addr_len = sizeof(client_addr);
//   int client_fd = accept(socket_fd_server, (struct sockaddr*)&client_addr, &addr_len);
//
//
//   //client sends ping
//   send(socket_fd_client, "ping", 4, 0);
//   fprintf(stdout, "\nClient sent ping");
//   fflush(stdout);
//   sleep(1);
//
//   //server receives ping
//   char buffer_server[1024];
//   int lens = recv(client_fd, buffer_server, sizeof(buffer_server), 0);
//
//   buffer_server[lens] = '\0';
//   fprintf(stdout, "\nServer received: %s", buffer_server);
//   fflush(stdout);
//
//   // Echo response
//   sendto(socket_fd_server, buffer_server, lens, 0,
//   (struct sockaddr*)&client_addr, addr_len);
//   sleep(1);
//
//   // Client receives message
//   char buffer_client[1024];
//   int lenc = recv(socket_fd_client, buffer_client, sizeof(buffer_client), 0);
//   buffer_client[lenc] = '\0';
//   fprintf(stdout, "\nClient received: %s", buffer_client);
//   fflush(stdout);
// }

