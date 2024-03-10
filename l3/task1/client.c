#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <signal.h>

#define SOCK_NAME "server.soc"

int sockfd;

int main()
{
    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);

    if (sockfd < 0)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr serv_addr;
    serv_addr.sa_family = AF_UNIX;
    strcpy(serv_addr.sa_data, SOCK_NAME);

    struct sockaddr clnt_addr;
    clnt_addr.sa_family = AF_UNIX;
    strcpy(clnt_addr.sa_data, "client.soc");
    /* sprintf(clnt_addr.sa_data, "%d.soc", getpid()); */

    int socklen = strlen(clnt_addr.sa_data)
        + sizeof(clnt_addr.sa_family);
    if (bind(sockfd, &clnt_addr, socklen) < 0)
    {
        perror("bind");
        exit(1);
    }

    int bytes;
    char buf[256];
    unsigned int addrlen;

    int pid = getpid();
    sprintf(buf, "%d", pid);
    printf("send %d to serv\n", pid);

    if (sendto(sockfd, buf, strlen(buf), 0, &serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("sendto");
        exit(1);
    }

    memset(buf, 0, 256);
    bytes = recvfrom(sockfd, buf, 256, 0, &serv_addr, &addrlen);

    if (bytes < 0) 
    {
        perror("recvfrom");
        exit(1);
    }

    printf("recv %s from %s\n", buf, serv_addr.sa_data);

    unlink("client.soc");
    close(sockfd);

    pause();

    return 0;
}
