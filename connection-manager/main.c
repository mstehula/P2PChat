#ifndef WINDOWS
#define WINDOWS 0
#endif

#if !(WINDOWS)
#define SOCKET_ERROR -1
#define INVALID_SOCKET 0
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if WINDOWS
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#endif

int main(int argc, char * argv[]) {

  int port_no = 25565, n, client_addr_len, error_check;
  char input_buffer[256], output_buffer[256], inst[4], data[252];
  struct sockaddr_in server_addr, client_addr;
  fd_set rfds;
  fd_set wfds;
  struct timeval tv;


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
  server_addr.sin_addr.s_addr = INADDR_ANY;

  error_check = bind(sock_fd, (struct sockaddr * ) &server_addr, sizeof(server_addr));
  if(error_check == SOCKET_ERROR) {
    fprintf(stderr, "ERROR binding address, address already in use?\n");
    exit(1);
  }

  client_addr_len = sizeof(client_addr);
  memset(input_buffer, 0, 256);
  memset(output_buffer, 0, 256);

  listen(sock_fd, 5);

  fprintf(stdout, "Server Started, waiting for connections\n");

  while(1) {
    new_sock_fd = accept(sock_fd, (struct sockaddr *) &client_addr, &client_addr_len);
    if(new_sock_fd < 0) {
      fprintf(stderr, "ERROR, failed to accept a new client\n");
      exit(1);
    }

    fprintf(stdout, "Connected to client, now reading\n");

    //Set the file descriptor set to zero, then add the
    //client file descriptor
    FD_ZERO(&rfds);
    FD_SET(new_sock_fd + 1, &rfds);
    FD_SET(0, &rfds);

    //Select timeout is 5 seconds each select cycle
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    do {

      error_check = select(new_sock_fd + 1, &rfds, NULL, NULL, &tv);
      if(error_check == SOCKET_ERROR) {
        fprintf(stderr, "ERROR, error with select\n");
        exit(1);
      } else if(error_check == 0) {
        fprintf(stdout, "No data on select, closing this connection\n");
      } else {
        if()
        error_check = recv(new_sock_fd, input_buffer, sizeof(input_buffer), 0);
        if(error_check < 0) {
          fprintf(stderr, "ERROR, error with recv\n");
          exit(1);
        }

        fprintf(stdout, input_buffer);


      }
    //Do until select timeout
    } while(error_check != 0);
    fprintf(stdout, "Client connection closed\n");

    close(new_sock_fd);
  }

  return 0;
}
