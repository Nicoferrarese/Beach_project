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
int getdate();
int EasyRequest(umbrella *,int,char*);
int i;
int wp=0;
int rp=0;
char buffer[K];
int go=1;

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
    int date;
    //date= getdate();

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
	
    
    struct threadinput_t thin;
	
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {   
        thin.beach=beach;
        thin.sockfd=client_sock;
        puts("Connection accepted");
        
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*)&thin) < 0)
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
void *connection_handler(void *thin)
{   
    //Get the socket descriptor
    threadinput input = *(threadinput*)thin;
    struct umbrella_t * beach=input.beach;
    int sock=input.sockfd;
    int read_size;
    pthread_t self=pthread_self();
    char messagecpy[DIM] , client_message[DIM];
    int close=0;
    //Receive a message from client
    while( (read_size = recv(sock , client_message , DIM , 0)) > 0 )
    {  
        //---------------------------------------------------------------------
        client_message[read_size] = '\0';
        printf("client message: -%s-\n",client_message);
        
        //---------------------------------------------------------------------
        //end of string marker
        if(strcmp(client_message,"quit")==0){
            printf("Exiting...\n");
            sighand(2);
            }
        //strcpy(messagecpy,client_message);
		if(!EasyRequest(beach,sock,client_message)){
		//Send the message back to client
        write(sock , client_message , strlen(client_message));
        //clear the message buffer
		memset(client_message, 0, DIM);
        }
    }
    if(go==0){
        //write(sock , NULL , 0);
        shutdown(sock,SHUT_RDWR);
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
                beach[num].number=(atoi(p));  
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
    go=0;     
    if (sig == SIGINT){
        printf( "SIGINT received ... GAME OVER ! \n");
    }
    if (sig == SIGTERM){
        printf( "SIGTERM received ... GAME OVER ! \n");
    }
    if (sig == SIGQUIT){
        printf( "SIGQUIT received ... GAME OVER ! \n");
    }
    if (sig == SIGHUP){
        printf( "SIGHUP received ... GAME OVER ! \n");
    }
    exit(0);
} 
int getdate(){
    int date,i,dd,mm,yyyy;
    char input[20];
    printf("Insert today date: (dd mm yyyy)\n");
    fgets(input,20,stdin);
    if(strchr(input, '\n')==NULL){//check exist newline
        printf("\nOverflow, closing\n");  
        exit(0);
        }
    i=0;
    char *c=strtok(input," ");
    do {
        switch(i){
            case(0):
                dd=atoi(c);
                if(dd<1 || dd>31){  
                    i=-1;
                }               
            break;
            case(1):
                mm=atoi(c);
                if(mm<1 || mm>12){
                    i=-1;
                }
            break;
            case(2):
                yyyy=atoi(c);
                if(yyyy <1900 || yyyy>2100){
                    i=-1;
                }
            break;
        }
        i++;
        if(i==0){
            printf("Invalid, %d %d %d insert again: (dd mm yyyy)\n",dd,mm,yyyy);
                fgets(input,20,stdin);
                c=strtok(input," ");
        }
        if(i>0) c = strtok (NULL, " ");
    }while (c!=NULL);
    date=dd+(mm*100)+(yyyy*10000);
    printf("date: %d\n",date);
    return date;

} 
int EasyRequest(umbrella *beach,int sock,char* message){
    char *p,*c;
    char output[DIM];
    char sup[13];
    int z,i=0;
    int num;
    printf("Easy start\n");
    p = strtok (message," ");
    if( strcmp(p,"avabile")==0 || 
        strcmp(p,"Avabile")==0 ||
        strcmp(p,"AVABILE")==0 ){
        c=p;
        c=strtok(NULL," ");        
        if(c!=NULL){
            num=atoi(c);
            if((num>0) && (num<=NROW)){
            z=NCOL*(num-1);
            //printf("z:%d num:%d c:%s\n",z,num,c);
            for(z;z<num*NCOL;z++){
                if(beach[z].status==0){
                    sprintf(sup,"%d ",beach[z].number);
                    strcat(output,sup);
                }
                else{
                    strcat(output,"   ");
                }
            }
            i=1;
            }        
        }
        else if(c==NULL){
            for(z=0;z<N_um;z++){
                if(z%(NCOL)==0)strcat(output,"\n");
                if(beach[z].status==0){
                    sprintf(sup,"%d ",beach[z].number);
                    strcat(output,sup);                    
                }
                else{
                    strcat(output,"   ");
                }
            }
            i=1;
            //stampa Avabile+ n. ombrelloni disponibili
            // oppure n avabile
            //close_connection
        }
    }
     if( strcmp(p,"cancel")==0 || 
         strcmp(p,"Cancel")==0 ||
         strcmp(p,"CANCEL")==0 ){
            p=strtok(NULL," ");
            int z=atoi(p);
            printf("z: %d\n status: %d\n",z, beach[z].status);
            if(z>0 && z<N_um && beach[z-1].status!=0){
                beach[z-1].status=0;
                printf("cancel ok\n status: %d\n",beach[z-1].status);
                memset(output,0,sizeof(output));
                strcpy(output,"Cancel ok");
                i=1;
            }


        }


    if(i==1){
        write(sock,output,strlen(output));
        memset(output, 0, DIM);
        return 1;
    }
    else{
        return 0;
    }
}
