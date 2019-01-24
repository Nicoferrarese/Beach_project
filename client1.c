#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#define __USE_MISC
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <netdb.h>

#define DIM 250
int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int read_size;
    char buffer[DIM];
    char test[DIM];
//----------------------------
    char *p,*c;
    char del[]=" \n";
    int i=0;
    int ex=0;
    
//-----------------------------
    if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);
    /* Create a socket point */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        perror("ERROR opening socket");
        exit(1);
    }
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(portno);

    /* Now connect to the server */
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)  //-------------------!!!!!! CAST SOCK ADDR !!!!
    {
         perror("ERROR connecting");
         exit(1);
    }	
    
    /* Send message to the server */
    while(1){
         //-------------------------------------------READ FROM BUFFER
        memset(buffer,0,256);
        
        printf("INPUT:\n");
        fgets(buffer,256,stdin);                                                       //Send the message back to server
        n=write(sockfd , buffer , strlen(buffer));
        i=0;
        if (n<0) {
            perror("ERROR writing to socket");
            exit(1);
        }
        if((read_size = recv(sockfd , buffer , 256 , 0)) > 0){
            printf("server says:\n\t%s\n",buffer); //-------------------------------------------READ FROM BUFFER
            memset(buffer,0,256);
            }
        else if(read_size==0){
            printf("Connection closed from server\n");
            exit(0);
        }
    }
        	       
            
    return 0;
}
