#include<stdio.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include<errno.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include<iostream>

#include "SSHClient.h"

using namespace std;

#define PORT 12700
#define MAX_LENGTH 10000

int startServer(SSHClient *client)
{
    int sockfd, connfd, len;
    unsigned char buffer[MAX_LENGTH];
    struct sockaddr_in servaddr, clientaddr;

    /* Create TCP socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Socket creation failed: %s\n", strerror(errno));
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    /* Bind socket */
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        printf("Socket bind failed: %s\n", strerror(errno));
        return -1;
    }

    /* Listen on socket */
    if (listen(sockfd, 5) != 0) {
        printf("Listen failed: %s\n", strerror(errno));
        return -1;
    }
    printf("Waiting for connection\n");

    len = sizeof(clientaddr);
    connfd = accept(sockfd, (struct sockaddr *) &clientaddr, (socklen_t *) &len);
    if (connfd < 0) {
        printf("Server accept failed: %s\n", strerror(errno));
        return -1;
    }
    printf("Accepted connection\n");

    while (1) {
        memset(buffer, 0, MAX_LENGTH);
        int nbytes = read(connfd, buffer, MAX_LENGTH);
        if (nbytes > 0) {
            string command;
            len = 0;

            command.assign((const char *)buffer);

            cout << "Received command: " << command << endl;

            memset(buffer, 0, MAX_LENGTH);
            client->runCommand(command, buffer, &len);
            write(1, buffer, len);
            write(connfd, buffer, len);
        }

        usleep(100);
    }

    close(connfd);
    close(sockfd);

    printf("Returning from tcpServer\n");
    return 0;
}