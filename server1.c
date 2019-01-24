#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> 
 #include <signal.h>
#include"umbrella.h"


void *connection_handler(void *);
int i;
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
    
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_family= AF_INET;
     server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);

    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
	pthread_t thread_id;
	
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
         
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
        puts("Handler assigned");
        i++;
        printf("\tClient connected: %d\n",i);
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;
}
void *connection_handler(void *socket_desc)
{   int f=4;
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    pthread_t self=pthread_self();
    char *message , client_message[256];
    int close=0;
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 256 , 0)) > 0 )
    {  
        //---------------------------------------------------------------------
        client_message[read_size-1] = '\0';
         printf("client message: -%s-\n",client_message);

        //---------------------------------------------------------------------
        //end of string marker
        if(strcmp(client_message,"quit")==0){
            printf("Exiting...\n");
            write(sock , NULL , 0);
            pthread_kill(self,9);
            }

		client_message[sizeof(client_message)] = '\0';
		
		//Send the message back to client
        write(sock , client_message , strlen(client_message));
        //clear the message buffer
		memset(client_message, 0, 256);
		
    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        i--;
        printf("\tClient connected: %d\n",i);
        
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    return 0;
} 