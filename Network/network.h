#include <stdint.h>


typedef struct packet packet_t;
typedef struct packet{
  int fd;
  uint32_t length; //length of message
  uint8_t typecode; //specifies message type
  unsigned char* message; //flattened array carrying message
  packet_t* nextmessage;
} packet_t;


typedef struct msg{
  int fd;
  uint8_t typecode;
  uint32_t length;
  char* message;
} msg_t;


/*************** init_server *****************
  *
  *
  * The init server function is used to initialize the server at a specified
  * address. The address can be passed as either an IP or plaintext. The function
  * will initialize the server and print the assigned port to stdout.
  *
  *
  *
  * The disconnect callback function will receive the following struct cast as
  * void*:
  *
        typedef struct clientmsg{
        char msg; // the char from the client
        int fd;   // the fd of the client
        void* passed; // the void ptr supplied to init_server
        } clientmsg_t;
  *
  *
t * The recvmsg_callback function will receive a clientmsg_t cast as void* and
  * is expected to respond with the following struct:
  * 
      typedef struct servermsg{
        uint8_t typecode; //special descriptor for use by the client callback function
        uint32_t length;  //length of message in bytes
        char* message;    // server message to client
        void* suppliedptr //<-- should be set to NULL, this will be supplied by the client
      } servermsg_t;
  *
  *
  * In addition, the server expects a callback function to call in the event of 
  * a client disconnect or error. It will supply to this function with the  
  *
  *
  * input: address - string
  *        socket_fd - pointer to file descriptor
  *        addr - sockaddr_in* to be populated
  *        port - optional, returns assigned port
  *
  * output - 1 on success, otherwise 0
  */

int init_server(char* address);


/************** init_client **************
 *
 *
 *
 *
 *
 *
 *
 */

int init_client(char* address, int port, int* input_socket_fd);

 
