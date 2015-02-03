#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int initSocket(int port,int* serverSocket){
    *serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    int val=1;
    setsockopt(*serverSocket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    struct sockaddr_in serverAddr;

    serverAddr.sin_family      = AF_INET;
    serverAddr.sin_port        = htons(port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&(serverAddr.sin_zero), 0, 8);


    int rc = bind(*serverSocket, (struct sockaddr*) &serverAddr,
       sizeof(struct sockaddr));
    return rc;
}


void do_recv(u_char* buff,int len){
    printf("len=%d\n",len);
    for(int i=0;i<len;i++){
        printf("%02x ",buff[i]);
    }
    printf("\n\n");
}

int main(int argc, char const *argv[])
{

    while(1){
        int serverSocket;
        int rc=initSocket(8000,&serverSocket);
        if (rc == -1) {
         printf("Bad bind\n");
         exit(1);
        }
        rc = listen(serverSocket, 5);
        if (rc == -1) {
         printf("Bad listen\n");
         exit(1);
        }

        int sock;
        struct sockaddr_in clientAddr;
        int clientAddrSize = sizeof(struct sockaddr_in);
        sock = accept(serverSocket,
           (struct sockaddr*) &clientAddr,
           (socklen_t*) &clientAddrSize);
        close(serverSocket);
        int len;
        u_char buf[BUFSIZ];
        while((len=recv(sock,buf,BUFSIZ,0))>0){
            do_recv(buf,len);
        }
    }

    return 0;
}