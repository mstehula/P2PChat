#ifndef WINDOWS
#define WINDOWS 0
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>

#if WINDOWS
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#endif

int main(int argc, char * argv[]) {

  int port_no = 25565, n, client_addr_len, error_check;
  char input_buffer[256], output_buffer[256], inst[4], data[252];
  struct sockaddr_in server_addr, client_addr;

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
#if WINDOWS
  if(sock_fd == INVALID_SOCKET) {
#else
  if(sock_fd == 0) {
#endif
    fprintf(stderr, "ERROR opening socket, exiting\n");
    exit(1);
  }

  if(argc >= 2)
    port_no = atoi(argv[1]);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port_no);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  error_check = bind(sock_fd, (struct sockaddr * ) &server_addr, sizeof(server_addr));
#if WINDOWS
  if(error_check == SOCKET_ERROR) {
#else
  if(error_check < 0) {
#endif
    fprintf(stderr, "ERROR binding address, address already in use?\n");
    exit(1);
  }

  listen(sock_fd, 5);

  fprintf(stdout, "Server Started, waiting for connections\n");

  client_addr_len = sizeof(client_addr);
  memset(input_buffer, 0, 256);
  memset(output_buffer, 0, 256);
  while(1) {
    fd_set rfds;
    struct timeval tv;

    new_sock_fd = accept(sock_fd, (struct sockaddr *) &client_addr, &client_addr_len);
    if(new_sock_fd < 0) {
      fprintf(stderr, "ERROR, failed to accept a new client\n");
      exit(1);
    }

    fprintf(stdout, "Connected to client, now reading\n");

    //Set the file descriptor set to zero, then add the
    //client file descriptor
    FD_ZERO(&rfds);
    FD_SET(new_sock_fd + 1, &rfds)

    //Select timeout is 5 seconds each select cycle
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    error_check = select(1, &rtfs, NULL, NULL, &tv);

    fprintf(stdout, "Client connection closed\n");
  }

  return 0;
}
