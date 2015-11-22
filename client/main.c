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
#include <conio.h>

#if WINDOWS
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#endif

union data {
  char data[256];
  struct {
    char instruction[4];
    char data[248];
    char tail[4];
  } message;
};

int main(int argc, char * argv[]) {

  int server_port = 25565, client_port = 25565;
  int n, error_check;
  char buffer[256];
  struct sockaddr_in server_addr, client_addr;
  struct fd_set nfds;
  struct timeval tv;

  //Initialize sockets. Little extra needed with windows
#if WINDOWS
  WSADATA wsa;
  SOCKET server_sock_fd = INVALID_SOCKET,
          client_sock_fd = INVALID_SOCKET,
          client_connected_sock_fd = INVALID_SOCKET;

  error_check = WSAStartup(MAKEWORD(2, 2), &wsa);
  if(error_check != 0) {
    fprintf(stderr, "ERROR winsock failed to initialize\n");
    exit(1);
  }
#else
  int server_sock_fd = 0, client_sock_fd = 0, client_connected_sock_fd = 0;
#endif

  server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(server_sock_fd == INVALID_SOCKET) {
    fprintf(stderr, "ERROR opening server socket, exiting\n");
    exit(1);
  }

  client_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(client_sock_fd == INVALID_SOCKET) {
    fprintf(stderr, "ERROR opening client socket, exiting\n");
    exit(1);
  }

  if(argc >= 2)
    server_port = atoi(argv[1]);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(server_port);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if(argc >= 3)
    client_port = atoi(argv[2]);
  client_addr.sin_family = AF_INET;
  client_addr.sin_port = htons(client_port);
  client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  error_check = bind(server_sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
  if(error_check == SOCKET_ERROR) {
    fprintf(stderr, "ERROR, error binding port, exiting\n");
    exit(1);
  }

  fprintf(stdout, "Server bound to port %d\n", server_port);

  listen(server_sock_fd, 2);

  fprintf(stdout, "P2P server setup complete\n");

  //Infinite loop, if there is a connection incoming
  while(1) {
    //If there is a connection incoming, connect to it and read from it
    // error_check = accpet();

    memset(buffer, 0, 256);

    FD_ZERO(&nfds);
    FD_SET(server_sock_fd, &nfds);

    tv.tv_sec = 0;
    tv.tv_usec = 1000;

    error_check = select(server_sock_fd+1, &nfds, NULL, NULL, &tv);
    if(error_check == -1) {
      fprintf(stderr, "Error on select\n");
      continue;
    } else if(error_check == 0) {
      // fprintf(stdout, "Nothing ready to be read from external clients\n");
    } else {
      int sock_length = sizeof(client_addr);
      client_connected_sock_fd = accept(server_sock_fd, (struct sockaddr *) &client_addr, &sock_length);
      if(client_connected_sock_fd == INVALID_SOCKET) {
        error_check = recv(client_connected_sock_fd, buffer, 256, 0);
        if(error_check == INVALID_SOCKET) {
          fprintf(stderr, "ERROR, error on read from client\n");
          break;
        }
        fprintf(stdout, "Received: %s\n", buffer);
      }

#if WINDOWS
      shutdown(client_sock_fd, SD_BOTH);
#else
      shutdown(client_sock_fd, SHUT_RDWR);
#endif
    }

    memset(buffer, 0, 256);

    // Input stdin to the messaage buffer
#if WINDOWS
    if(_kbhit()) {
#else
    FD_ZERO(&nfds);
    FD_SET(0, &nfds);

    tv.tv_sec = 0;
    tv.tv_usec = 1000;

    error_check = select(1, &nfds, NULL, NULL, &tv);
    if(error_check == -1) {
      frpintf(stderr, "ERROR, error on select of stdin \n");
      break;
    } else if (error_check == 0) {
      fprintf(stdout, "Nothing to read from stdin\n");
    } else {
#endif
      n = read(0, buffer, 256);
      if(n == -1) {
        fprintf(stderr, "ERROR, error reading stdin\n");
        exit(1);
      }
    }

    if(strlen(buffer) > 0) {
      fprintf(stdout, "Output started\n");

      //If there is something that needs to be sent somewhere, connect to remote server (Other P2P Client)
      fprintf(stdout, "Attempting connection \n");
      error_check = connect(client_sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
      fprintf(stdout, "Connection attempted\n");
      if(error_check < 0) {
        //If it cannot connect, print the error and try again.
        fprintf(stderr, "ERROR, cannot connect to server\n");
        continue;
      }

      fprintf(stdout, "Connected to remote server, now closing\n");

#if WINDOWS
      shutdown(client_sock_fd, SD_BOTH);
#else
      shutdown(client_sock_fd, SHUT_RDWR);
#endif
    }
  }

#if WINDOWS
  closesocket(server_sock_fd);
  WSACleanup();
#else
  close(server_sock_fd);
#endif

  return(0);
}
