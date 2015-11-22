#ifndef WINDOWS
#define WINDOWS 0
#endif

#if !(WINDOWS)
#define INVALID_SOCKET -1
#define SOCKET_ERROR 0
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if WINDOWS
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#endif

int main(int argc, char * argv[]) {

  int port_no = 25565, n, client_addr_len, error_check;
  char input_buffer[256], output_buffer[256], inst[4], data[252];
  struct sockaddr_in server_addr;

#if WINDOWS
  WSADATA wsa;
  SOCKET sock_fd = INVALID_SOCKET, new_sock_fd = INVALID_SOCKET;

  error_check = WSAStartup(MAKEWORD(2, 2), &wsa);
  if(error_check != 0) {
    fprintf(stderr, "ERROR winsock failed to initialize\n");
    exit(1);
  }
#else
  int sock_fd = 0, new_sock_fd = 0;
#endif

  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(sock_fd == INVALID_SOCKET) {
    fprintf(stderr, "ERROR opening socket, exiting\n");
    exit(1);
  }

  if(argc >= 2)
    port_no = atoi(argv[1]);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port_no);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  fprintf(stdout, "Attempting connection \n");

  error_check = connect(sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
  if(error_check < 0) {
    fprintf(stderr, "ERROR, cannot connect to server\n");
    exit(1);
  }

  fprintf(stdout, "Connected to remote server\n");

  while(1) {

  }

#if WINDOWS
  closesocket(sock_fd);
  WSACleanup();
#else
  close(sock_fd);
#endif

  return(0);
}
