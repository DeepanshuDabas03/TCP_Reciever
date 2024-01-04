#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define _port 8080
#define _num_of_concurrent_client 4000
#define BUF_SIZE 1024

int main()
{
    int fd_listen, fd_client, maximum_select;
    int client[_num_of_concurrent_client];
    struct sockaddr_in serv_adr, clnt_adr;
    fd_set fd_client;

    // Create socket
    if ((fd_listen = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket() ");
        exit(1);
    }

    // Reset server address
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons(_port);

    // Bind socket
    if (bind(fd_listen, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
    {
        perror("bind() ");
        exit(1);
    }

    // Listen for incoming connections
    if (listen(fd_listen, _num_of_concurrent_client) == -1)
    {
        perror("listen() ");
        exit(1);
    }

    int clnt_adr_sz = sizeof(clnt_adr);

    for (int i = 0; i < _num_of_concurrent_client; i++)
    {
        client[i] = 0;
    }

    while (1)
    {
        FD_ZERO(&fd_client);
        FD_SET(fd_listen, &fd_client);
        maximum_select = fd_listen;

        for (int i = 0; i < _num_of_concurrent_client; i++)
        {
            int clnt_sckt = client[i];
            if (clnt_sckt > 0)
            {
                FD_SET(clnt_sckt, &fd_client);
            }
            if (clnt_sckt > maximum_select)
            {
                maximum_select = clnt_sckt;
            }
        }
        if (select(maximum_select + 1, &fd_client, NULL, NULL, NULL) < 0)
        {
            perror("Select error");
            exit(1);
        }

        // if new connection is requested, accept it
        if (FD_ISSET(fd_listen, &fd_client))
        {
            if ((fd_client = accept(fd_listen, (struct sockaddr *)&clnt_adr, (socklen_t *)&clnt_adr_sz)) == -1)
            {
                perror("accept() ");
                continue;
            }

            int i;
            for (i = 0; i < _num_of_concurrent_client; i++)
            {
                if (client[i] == 0)
                {
                    client[i] = fd_client;
                    break;
                }
            }

            if (i == _num_of_concurrent_client)
            {
                perror("Too many clients");
                // if we have reached maximum_selectmum number of clients, close socket
                close(fd_client);
            }
        }

        // handle data from client
        for (int i = 0; i < _num_of_concurrent_client; i++)
        {
            int clnt_sckt = client[i];
            // if client socket is set, handle request from client
            if (FD_ISSET(clnt_sckt, &fd_client))
            {
                char buf[BUF_SIZE];
                int ret = recv(clnt_sckt, buf, BUF_SIZE, 0);
                // if client disconnected, close socket and remove from client
                if (ret <= 0)
                {
                    if (ret != 0)
                    {
                        perror("recv() ");
                    }
                    close(clnt_sckt);
                    // remove from client, and close socket
                    client[i] = 0;
                }
                else
                {
                    // calculate factorial of n
                    ull n = atoi(buf);
                    ull fact = 1;
                    n = n > 20 ? 20 : n;
                    for (ull j = 2; j <= n; j++)
                    {
                        fact *= j;
                    }

                    snprintf(buf, BUF_SIZE, "%llu", fact);
                    ret = send(clnt_sckt, buf, strlen(buf), 0);
                    if (ret == -1)
                    {
                        perror("send() ");
                    }
                    // handle error of send()
                }
            }
        }
    }

    return 0;
}
