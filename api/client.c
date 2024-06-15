#include "client.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

int client(int argc, char* argv[])
{
    int sockfd = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr;

    if (argc != 2) {
        printf("\n Usage: %s <ip of server> \n", argv[0]);
        return 1;
    }

    memset(recvBuff, '0', sizeof(recvBuff));
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    struct addrinfo hints, *result = NULL;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(argv[1], "5000", &hints, &result)) {
        printf("\n Error : getaddrinfo \n");
        return 1;
    }

    if (connect(sockfd, result->ai_addr, (int)result->ai_addrlen) < 0) {
        printf("\n Error : Connect Failed \n");
        return 1;
    }

    freeaddrinfo(result);

    const char sendBuff[1024] = "GET / HTTP/1.1\r\n\r\n";

    if (send(sockfd, sendBuff, strlen(sendBuff), 0) < 0) {
        printf("\n Eror : Send Failed \n");
        return 1;
    }

    shutdown(sockfd, SHUT_WR);

    while ((n = read(sockfd, recvBuff, sizeof(recvBuff) - 1)) > 0) {
        recvBuff[n] = 0;
        if (fputs(recvBuff, stdout) == EOF) {
            printf("\n Error : Fputs error\n");
        }
    }

    if (n < 0) {
        printf("\n Read error \n");
    }

    return 0;
}