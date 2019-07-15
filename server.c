#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

void error_handling(char* message);
void read_childproc(int sig);

int main(int argc,char* argv[])
{
    
    int serv_sock;
    int clnt_sock;

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;

    //struct sigaction act;
    char message[BUF_SIZE]; 
    int str_len = 0,state = 0;
    pid_t pid;

    if(argc != 2)
    {
        printf("Usage:%s <port>\n",argv[0]);
        exit(1);
    }

    

    serv_sock = socket(PF_INET,SOCK_STREAM,0);
    if(-1 == serv_sock)
    {
        error_handling("socket() error");
    }

    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));


    if (-1 == bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr)))
    {
        error_handling("bind() error");
    }

    if(-1 == listen(serv_sock,5))
    {
        error_handling("listen() error");
    }
    
    
    while(1)
    {


        clnt_addr_size = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_size);
        if (-1 == clnt_sock)
        {
            //error_handling("accept() errpr");
            continue;
        }
        else
        {
            //printf("IP: %s\n",inet_ntoa(clnt_addr.sin_addr));
            printf("New  client Connected... IP: %s\n",inet_ntoa(clnt_addr.sin_addr));
            
        }
        pid = fork();
        if(-1 == pid)
        {
            close(clnt_sock);
            continue;
        }
        else if(0 == pid)
        {
            close(serv_sock);
            while (str_len = read(clnt_sock,message,BUF_SIZE)!=0)
            {
                
                // str_len = read(clnt_sock,message,BUF_SIZE);
                // if (str_len == 0)
                // {
                //     error_handling("reda()=0");
                //     break;
                // }
                //printf("client message: %s\n",message);
                write(clnt_sock,message,str_len);
            }
                close(clnt_sock);
                puts("client disconnected......");
                return 0;
        }
        else
            close(clnt_sock);
    }
    close(serv_sock);
    return 0;
}

void error_handling(char* message)
{
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}

void read_childproc(int sig)
{
    pid_t pid;
    int status;
    pid = waitpid(-1,&status,WNOHANG);
    printf("removed proc id: %d\n",pid);
}