#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#define _port 8080
#define _num_of_concurrent_client 3000
#define BUF_SIZE 1024
typedef unsigned long long int ull;
// Define all functionality
ull factorial(ull n);
int create_new_socket(int *fd_listen);
int new_connection(int fd_listen, int *fd_client);
int create_process(int fd_listen, int fd_client);
int send_data(int fd_client, char *buf);
int recv_data(int fd_client, char *buf);
int main()
{
    int fd_listen, fd_client;
    // Create new socket
    if(create_new_socket(&fd_listen) == -1){
        perror("socket() system call error");
        exit(1);
    }
    while(1){
        // Create new connection to client
        new_connection(fd_listen, &fd_client);
        // Create new process to handle client
        create_process(fd_listen, fd_client);
    }
    // Close socket
    close(fd_listen);
    return 0;
}
int create_new_socket(int *fd_listen)
{
    struct sockaddr_in serv_adr;
    memset(&serv_adr, 0, sizeof(serv_adr));
    *fd_listen = socket(AF_INET, SOCK_STREAM, 0);
    if (*fd_listen == -1)
    {
        perror("socket() system call error");
        return -1;
    }
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);;
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons(_port);
    // Define socket properties,such as port, ip address, protocol
    if (bind(*fd_listen, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
    {
        perror("bind() system call error");
        return -1;
    }
    if (listen(*fd_listen, _num_of_concurrent_client) == -1)
    {
        perror("listen() system call  error");
        return -1;
    }
    // Bind socket to port and listen to port
    return 0;
}
int new_connection(int fd_listen, int *fd_client)
{
    struct sockaddr_in clnt_adr;
    int clnt_adr_sz = sizeof(clnt_adr);
    memset(&clnt_adr, 0, sizeof(clnt_adr));
    *fd_client = accept(fd_listen, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
    // Accept new connection from client
    if (*fd_client == -1)
    {
        perror("accept() system call error");
        return -1;
    }
    // if return -1, accept() system call has error, write error message to terminal
    return 0;
}
int create_process(int fd_listen, int fd_client)
{
    pid_t pid = fork();
    // create process using fork() system call
    if (pid == 0)
    // Its a child process, execute client request
    {
        close(fd_listen);
        char buf[BUF_SIZE];
        while (1)
        {
            // Receive data from client 
            int ret= recv_data(fd_client, buf);
            if(ret == 1){
                close(fd_client);
                exit(0);
                // if return 1, all data has been received, close socket and exit
            }
            if(ret == -1){
                close(fd_client);
                exit(1);
                // if return -1, recv() system call has error, close socket and exit
            }
            ull n = atoi(buf);
            // convert string to integer
            ull fact = factorial(n);
            // calculate factorial of n
            sprintf(buf, "%llu", fact);
            // convert integer to string
            if(send_data(fd_client, buf) == -1){
                exit(1);
            }
            // Send data to client and exit if error occurs
        }
    }
    else if (pid < 0)
    {
        perror("fork() system call error");
        // if return -1, fork() system call has error, write error message to terminal and exit
        exit(0);
    }
    else
    {
        // Its a parent process, close client socket and continue to accept new connection since fork spawn a new process and copy of all data from parent process will be there, and there is no need of client socket in parent process
        close(fd_client);
        return 0;
    }
}
int send_data(int fd_client, char *buf)
{
    int num_of_bytes=send(fd_client, buf, strlen(buf), 0);
    // Send data to client using send() system call
    if(num_of_bytes == -1){
        perror("send() system call error");
        close(fd_client);
        return -1;
    }
    // If return -1, send() system call has error, write error message to terminal and close socket
    return 0;
}
int recv_data(int fd_client, char *buf)
{
    memset(buf, 0, BUF_SIZE);
    int num_of_bytes_recieved = recv(fd_client, buf, BUF_SIZE, 0);
    // Receive data from client using recv() system call
    if(num_of_bytes_recieved == 0){
        close(fd_client);
        // if num_of_bytes_recieved is 0, client has finished sending data, close socket and exit
        exit(0);
    }
    if (num_of_bytes_recieved == -1)
    {
        perror("recv() system call error");
        // If not able to receive data, write error message to terminal and close socket
        close(fd_client);
        exit(0);
    }
    return 0;
}
// factorial function, calculate factorial of n if n is less than equal to 20 otherwise return 20!
ull factorial(ull n)
{
    n = 20 < n ? 20 : n;
    ull fact = 1;
    for (ull i = 2; i <= n; i++)
    {
        fact *= i;
    }
    return fact;
}
