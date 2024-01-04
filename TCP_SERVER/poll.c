#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/poll.h>

#define _port 8080
#define _num_of_concurrent_client 4000
#define BUF_SIZE 1024

int main()
{
    int fd_listen, fd_client;
    int maximum_poll, client[_num_of_concurrent_client];
    struct sockaddr_in serv_adr, clnt_adr;
    struct pollfd fd_poll[_num_of_concurrent_client];

    if ((fd_listen = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket() ");
        exit(1);
    }

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons(_port);
    // bind the socket to the port
    if (bind(fd_listen, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
    {
        perror("bind() ");
        exit(1);
    }
    // listen for incoming connections
    if (listen(fd_listen, _num_of_concurrent_client) == -1)
    {
        perror("listen() ");
        exit(1);
    }

    int clnt_adr_sz = sizeof(clnt_adr);
    // initialize the connection array and poll fd array
    for (int i = 0; i < _num_of_concurrent_client; i++)
    {
        client[i] = 0;
        fd_poll[i].fd = -1;
        fd_poll[i].events = POLLIN;
    }

    fd_poll[0].fd = fd_listen;

    while (1)
    {
        if (poll(fd_poll, _num_of_concurrent_client, -1) < 0)
        {
            perror("poll() ");
            exit(1);
        }

        if (fd_poll[0].revents & POLLIN)
        {
            // accept the connection
            if ((fd_client = accept(fd_listen, (struct sockaddr *)&clnt_adr, (socklen_t *)&clnt_adr_sz)) == -1)
            {
                perror("accept() ");
                continue;
            }
            // add the client to the connection array if there is space
            int i;
            for (i = 0; i < _num_of_concurrent_client; i++)
            {
                if (client[i] == 0)
                {
                    client[i] = fd_client;
                    fd_poll[i].fd = fd_client;
                    break;
                }
            }

            if (i == _num_of_concurrent_client)
            {
                perror("Reached limit of number of concurrent connections");
                close(fd_client);
            }

            fd_poll[0].revents = 0;
        }
        // check for data from clients
        for (int i = 1; i < _num_of_concurrent_client; i++)
        {
            // if client is active and there is data to read
            if (client[i] > 0 && (fd_poll[i].revents & POLLIN))
            {
                char buf[BUF_SIZE];
                // read the data
                int ret = recv(client[i], buf, BUF_SIZE, 0);
                if (ret <= 0)
                {
                    if (ret != 0)
                    {
                        perror("recv() ");
                    }
                    close(client[i]);
                    client[i] = 0;
                    fd_poll[i].fd = -1;
                    // reset the poll events
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
                // reset the poll events
                fd_poll[i].revents = 0;
            }
        }
    }
    return 0;
}
