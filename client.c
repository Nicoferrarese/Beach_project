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
    while((read_size = recv(sockfd , buffer , 256 , 0)) > 0 ){
        printf("server says:\n\t%s\n",buffer); //-------------------------------------------READ FROM BUFFER
        memset(buffer,0,256);
        int c=0;
        while (c==0){
            printf("Please enter the message: "); //--------------------Input     
        fgets(buffer,256,stdin);
        strcpy(test,buffer);
        p = strtok (test, del);
        printf("%s",p);	
        do{ if(i>0)
                i++;
            if(i==0){
                if( strcmp(p,"BOOK")==0 ||
                    strcmp(p,"Book")==0 ||
                    strcmp(p,"book")==0 ){  
                    i++;
                } 
                if( strcmp(p,"CANCEL")==0 || 
                    strcmp(p,"Cancel")==0 ||
                    strcmp(p,"cancel")==0 ){
                    i++;
                }
                if( strcmp(p,"AVABILE")==0 || 
                    strcmp(p,"Avabile")==0 ||
                    strcmp(p,"avabile")==0 ){
                    i++;
                }
            }                  
            p = strtok(NULL, del);
        }while (p!=NULL && i>0);
            c=1;
        if(i==0 || i>4){
            printf("Invalid Input: %d\n",i);
            c=0;
            
        }
        }
        
                                                               //Send the message back to server
            n=write(sockfd , buffer , strlen(buffer));
            i=0;
            if (n<0) {
                perror("ERROR writing to socket");
                exit(1);
            }
        	       
    }
    printf("\n%s",buffer);        
    return 0;
}
