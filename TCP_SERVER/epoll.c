#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#define _port 8080
#define _num_of_concurrent_client 4000
#define BUF_SIZE 1024
typedef unsigned long long int ull;

int main()
{
    int fd_listen, fd_client;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz = sizeof(clnt_adr);

    if ((fd_listen = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket() ");
        exit(1);
    }
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons(_port);
    if (bind(fd_listen, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
    {
        perror("bind() ");
        exit(1);
    }
    if (listen(fd_listen, _num_of_concurrent_client) == -1)
    {
        perror("listen() ");
        exit(1);
    }
    int fd_epoll = epoll_create1(0);
    if (fd_epoll == -1)
    {
        perror("epoll_create1() ");
        exit(1);
    }
    struct epoll_event clnt_sckt, client[_num_of_concurrent_client];
    clnt_sckt.events = EPOLLIN;
    clnt_sckt.data.fd = fd_listen;
    if (epoll_ctl(fd_epoll, EPOLL_CTL_ADD, fd_listen, &clnt_sckt) == -1)
    {
        perror("epoll_ctl()");
        exit(1);
    }

    while (1)
    {
        int maximum_epoll=epoll_wait(fd_epoll, client, _num_of_concurrent_client, -1);
        if (bum_ready < 0)
        {
            perror("API epoll_wait() ");
            exit(1);
        }

        for (int i = 0; i < maximum_epoll; i++)
        {
            //  new client connection
            if (client[i].data.fd == fd_listen)
            {
                if ((fd_client = accept(fd_listen, (struct sockaddr *)&clnt_adr, &clnt_adr_sz)) == -1)
                {
                    perror("accept() ");
                }
                else
                {
                    clnt_sckt.events = EPOLLIN;
                    clnt_sckt.data.fd = fd_client;
                    if (epoll_ctl(fd_epoll, EPOLL_CTL_ADD, fd_client, &clnt_sckt) == -1)
                    {
                        perror("API epoll_ctl() ");
                    }
                }
            }
            else
                // client request
            {
                int fd_client = client[i].data.fd;
                char buf[BUF_SIZE];
                int ret = read(fd_client, buf, sizeof(buf));

                if (ret <= 0)
                {
                    if (ret == 0)
                    {
                        close(fd_client);
                    }
                    else
                    {
                        perror("read() ");
                    }
                }
                else
                {
                    // calculate factorial
                    ull n = atoi(buf);
                    ull fact = 1;
                    n = n > 20 ? 20 : n;
                    for (ull j = 2; j <= n; j++)
                    {
                        fact *= j;
                    }
                    // write the result to the client
                    snprintf(buf, BUF_SIZE, "%llu", fact);
                    ret = send(client[i], buf, strlen(buf), 0);
                    if (ret == -1)
                    {
                        perror("send() ");
                    }
                }
            }
        }
    }

    return 0;
}
