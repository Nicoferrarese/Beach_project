#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> 
#include<signal.h>
#include <sys/stat.h>
#include <syslog.h>
#include<semaphore.h>
#include"umbrella.h"

#define K 101

sem_t SlotLibero,SlotOccupato,mutex;
void* connection_handler(void *);
void* connection_master(void *);
void sighand(int);

int i;
int wp=0;
int rp=0;
char buffer[K];

int main(int argc , char *argv[])
{   pid_t myself;
    struct umbrella_t beach[N_um];
    sem_init(&SlotLibero, 1, K);
    sem_init(&SlotOccupato, 1, 0);
    sem_init(&mutex, 1, 0);
    pthread_t thread_id,masterThr;
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
    struct sigaction sa;
    //sigemptyset(&sa);
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sighand;

    myself=fork();
    
    if(myself!=0){
        exit(0);        
    }      
    if (setsid() < 0)
        exit(EXIT_FAILURE);        

     sigaction(SIGINT,  &sa, 0);
     sigaction(SIGTERM, &sa, 0);
     sigaction(SIGHUP,  &sa, 0);
     sigaction(SIGQUIT, &sa, 0);

    /* Fork off for the second time*/
    myself = fork();

    /* An error occurred */
    if (myself< 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (myself > 0)        
        exit(EXIT_SUCCESS);

    /* Set new file permissions
    https://stackoverflow.com/questions/17954432/creating-a-daemon-in-linux/17955149#17955149
    !!change directory and permessi 
    */
    printf("\nson SERVER PID: %d\n Father PID: %d\n",(int)getpid(),(int)getppid());   
     
    
    if(pthread_create(&masterThr,NULL,connection_master,(void*)&beach)!=0)
    {
        perror("Thread master creation failed. Error");
        return 0;
    }
    puts("--->Thread Master created");
    sem_wait(&SlotOccupato);
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
	
    

	
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
         
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }

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
void *connection_master(void *beach1){
    sem_wait(&SlotLibero);
    puts("\tReading Beach Status..");
    readBeachStatus(beach1);
    puts("\tRead success");
    //PrintStatus(beach1);
    sem_post(&SlotOccupato);
    }
void *connection_handler(void *socket_desc)
{   
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
            sighand(2);
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

void readBeachStatus(void* beach1){
    struct umbrella_t * beach=(struct umbrella_t*)beach1;
    char del[]=" ";
    FILE * fd;
    int num=0;
    char c[30];  
    int i=0; 
    char *p;
       
    if((fd = fopen("spiaggia.txt", "r")) == NULL) {
        perror("Errore apertura");
        exit(-1);
    } 
    puts("\tINIT open correctly!");   
    while(fgets(c, 30, fd)!=NULL) {               
        p = strtok (c, del);
        while(p){
            if(i==0){
                beach[num].number=(atoi(p)-1);  
            }
            else if(i==1){
                beach[num].status=atoi(p);           
            }
            else if(i==2){
                beach[num].start=atoi(p);  
            }
            else if(i==3){
                beach[num].expire=atoi(p);            
            }
            p = strtok (NULL, del);
            i++;
            if(i==4){i=0;num++;}
        }
    }
    if (fclose(fd) != 0) {
        perror("Errore chiusura file\n");
        exit(-2);
    } 
    }
    void PrintStatus(void* beach1){
        struct umbrella_t * beach=(struct umbrella_t*)beach1;
        int n_um=0,r,c;

        printf(">>Stato Spiaggia:\n");
        for(r=0;r<NROW;r++){
            if(r==0)
                printf("\n");
            for(c=0;c<NCOL;c++){
                if(c==0)
                    printf("\t");
                if(n_um<N_um){
                if(beach[n_um].status){
                    printf(ANSI_COLOR_CYAN "%d \t" ANSI_COLOR_RESET,beach[n_um].number);
                }
                else
                    printf("%d \t",beach[n_um].number);
                }
                n_um++;   
            }
            printf("\n");
        }
        
    }
void sighand(int sig){ 
    printf("\n");
     
    if (sig == SIGINT){
        printf( "CTRL-C received ... GAME OVER ! \n");
        exit(0);
    }
    if (sig == SIGTERM){
        printf( "SIGTERM received ... GAME OVER ! \n");
        exit(0);
    }
    if (sig == SIGQUIT){
        printf( "SIGQUIT received ... GAME OVER ! \n");
        exit(0);
    }
    if (sig == SIGHUP){
        printf( "SIGHUP received ... GAME OVER ! \n");
        exit(0);
    }
}