#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFF_SIZE 30

void error_handling(char* message);
void read_routine(int sock,char* buff);
void write_routine(int sock,char* buff);

int main(int argc,char*argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char message[BUFF_SIZE];
    int idx = 0,read_len = 0;
    int str_len = 0;

    if(argc != 3)
    {
        printf("Usage: %s <IP> <prot>\n",argv[0]);
        exit(1);
    }

    sock = socket(PF_INET,SOCK_STREAM,0);
    if(-1 == sock)
    {
        error_handling("socket() error");
    }

    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if(-1 == connect(sock,(struct sockaddr*)& serv_addr,sizeof(serv_addr)))
    {
        error_handling("connect() error");
    }

    pid_t pid = fork();
    if(0 == pid)
        write_routine(sock,message);
    else
        read_routine(sock,message);

    close(sock);

    return 0;
}

void error_handling(char* message)
{
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}

void read_routine(int sock,char* buff)
{
    while(1)
    {
        int str_len = read(sock,buff,BUFF_SIZE);
        if(0 == str_len)
            return;
        buff[str_len] = 0;
        printf("Message from server: %s\n",buff);
    }
}

void write_routine(int sock,char* buff)
{
    while(1)
    {
        fgets(buff,BUFF_SIZE,stdin);
        if(!strcmp(buff,"q\n")||!strcmp(buff,"Q\n"))
        {
            shutdown(sock,SHUT_WR);
            return;
        }
        write(sock,buff,strlen(buff));
    }
}