#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define BUFFER_SIZE 5

int main( int argc,char* argv[] ){
    if( argc<=2 ){
        printf("usage: %s ip port recv_buf_size\n",basename(argv[0]));
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi( argv[2] );
 
    struct sockaddr_in address;
    bzero( &address,sizeof(address) );
    address.sin_family = AF_INET;
    inet_pton( AF_INET,ip,&address.sin_addr );
    address.sin_port = htons(port);

    int sock = socket( PF_INET,SOCK_STREAM,0 );
    assert( sock>=0 );

    int ret = bind( sock,(struct sockaddr*)&address,sizeof(address) );
    assert( ret!=-1 );

    ret = listen( sock,5 );
    assert( ret!=-1 );

    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof( client );
    int connfd = accept( sock,(struct sockaddr*)&client,&client_addrlength );
    if(connfd<0){
        printf("errno is:%d\n",errno);
    }else{
        char buffer[BUFFER_SIZE];
        memset( buffer,'\0',BUFFER_SIZE );
        ret=read(connfd,buffer,BUFFER_SIZE-1);
        write( connfd,buffer,ret);
        buffer[ret]='\0';
        printf("%s",buffer);
        printf("\n");
        close(connfd);
    }
    close(sock);
    return 0;
}