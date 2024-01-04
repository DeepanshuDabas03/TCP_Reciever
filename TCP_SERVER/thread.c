#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>

#define _port 8080
// common port for all web servers
#define _num_of_concurrent_client 4000
// number of concurrent clients that can be handled by server should be greater than 3000 as we need to handle 3000 concurrent clients atleast
// ip address of host machine in this case my host machine ip address is 10.0.2.4
#define BUF_SIZE 1024
typedef unsigned long long int ull;

void *handle_client(void *arg);

int main()
{
    int fd_listen, fd_client;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz = sizeof(clnt_adr);
    pthread_t tid;

    // Create socket
    if ((fd_listen = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket() ");
        exit(1);
    }
    // reset server address
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

    while (1)
    {
        // Accept new connection from client
        if ((fd_client = accept(fd_listen, (struct sockaddr *)&clnt_adr, &clnt_adr_sz)) == -1)
        {
            perror("accept() ");
            continue;
        }

        int *fd_client_ptr = (int *)malloc(sizeof(int));
        if (fd_client_ptr == NULL)
        {
            perror("malloc() ");
            close(fd_client);
            continue;
        }
        *fd_client_ptr = fd_client;
        // copy client socket to a memory location that can be accessed by thread and is not destroyed after thread is created
        // create thread to handle client
        if (pthread_create(&tid, NULL, handle_client, (void *)fd_client_ptr) != 0)
        {
            if (fd_client_ptr == NULL)
            {
                perror("malloc() ");
                close(fd_client);
                continue;
            }
            free(fd_client_ptr);
            close(fd_client);
            perror("pthread_create() ");
            continue;
        }
    }

    close(fd_listen);
    return 0;
}

void *handle_client(void *arg)
{
    // If error occurs, exit thread
    if (arg == NULL)
    {
        perror("malloc() ");
        return NULL;
    }

    int fd_client = *((int *)arg);
    // get client socket from argument of thread
    if (arg == NULL)
    {
        perror("malloc() ");
        return NULL;
    }
    // if error occurs, exit thread
    free(arg);

    char buf[BUF_SIZE];
    int ret;

    while (1)
    {
        ret = recv(fd_client, buf, BUF_SIZE, 0);
        if (ret == 0)
        {
            break;
            // Client finished sending data
        }
        else if (ret == -1)
        {
            perror("recv() ");
            break;
            // if error occurs, exit thread
        }
        else
        {
            ull n = atoi(buf);
            // convert string to integer
            ull fact = 1;
            n = n > 20 ? 20 : n;
            for (ull i = 2; i <= n; i++)
            {
                fact *= i;
            }

            // convert factorial to string
            snprintf(buf, BUF_SIZE, "%llu", fact);
            // send factorial to client
            ret = send(fd_client, buf, strlen(buf), 0);
            if (ret == -1)
            {
                perror("send() ");
                break; // Exit thread if error occurs
            }
        }
    }
    close(fd_client);
    // close client socket
    return NULL;
}
