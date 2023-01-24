#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUF_SIZE 1024

int main(int argc,char* argv[]){
    if(argc<=2){
        printf("usage: %s ip port\n",basename(argv[0]) );
        return 1;
    }
    const char *ip=argv[1];
    int port = atoi(argv[2]);

    struct sockaddr_in server_address;
    bzero( &server_address,sizeof( server_address ) );
    server_address.sin_family = AF_INET;
    inet_pton( AF_INET,ip,&server_address.sin_addr );
    server_address.sin_port = htons(port);

    int sockfd = socket( PF_INET,SOCK_STREAM,0 );
    assert( sockfd>=0 );
    if( connect( sockfd,(struct sockaddr*)&server_address,sizeof(server_address) )<0 ){
        printf("connection failed\n");
    }else{
        const char *msg="qwertyuiop";
        send( sockfd,msg,strlen(msg),0 );

        char buffer[BUF_SIZE];
        memset(buffer,'\0',BUF_SIZE);
        printf("data is: \n");
        while( recv(sockfd,buffer,BUF_SIZE-1,0) ){
            printf("%s",buffer);
            memset( buffer,'\0',BUF_SIZE );
        }
        printf("\n");
        //printf("got %d bytes of normal data '%s'\n",ret,buffer);
    }
    close( sockfd );
    return 0;
}