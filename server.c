/*
    C socket server example, handles multiple clients using threads
    Compile
    gcc server.c -lpthread -o server
*/
 
#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> 
#include"umbrella.h"

 
//the thread function
void *connection_handler(void *);
void BookCheck(umbrella * toCheck);
umbrella* BookInput(char *book,umbrella *readed);
char *  handleBook(umbrella* readed);
int Book_FirstStep(umbrella *toBook);
struct umbrella_t beach[N_um];
//beach=malloc(sizeof(umbrella*N_um));
int i=0;
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
   
    beach[1].status=0;
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
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
     
    
     
     
    //Accept and incoming connection
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
 
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[256];
    int close=0;
    //Send some messages to the client
    message = "Greetings! I am your connection handler\n";
    write(sock , message , strlen(message));
     
    message = "\nInsert booking option:\n";
    write(sock , message , strlen(message));
     
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 256 , 0)) > 0 )
    {  
        //---------------------------------------------------------------------
        umbrella *readed;
        client_message[read_size-1] = '\0';
         printf("client message: -%s-\n",client_message);
        readed=(umbrella*)malloc(sizeof(umbrella));
        readed=BookInput(client_message,readed);
       
        memset(client_message, 0, 256);
        strcpy(client_message,handleBook(readed));
        if(readed->request==101){
            shutdown(sock,0);
        }
        //---------------------------------------------------------------------
        //end of string marker
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

int Book_FirstStep(umbrella *toBook){
    if(beach[toBook->number].status==0){
        if(beach[toBook->number].status!=0){
            //put Navabile in buffer e chiudi connesione
            //client_message="NAVABILE\n";
            return 0;

        }
        else{
            beach[toBook->number]->status=2;
            return 1;
            
        }   
    }
}
umbrella* BookInput(char *book,umbrella *readed){
    int dateSt=0;
    int dateEn=0;
    int num=0;
    char *p,*c;
    char del[]=" ";
    int i=0;
    int ex=0;
    p = strtok (book, del);
    if( strcmp(p,"avabile")==0 || 
        strcmp(p,"Avabile")==0 ||
        strcmp(p,"AVABILE")==0 ){
        c=p;
        c=strtok(NULL,del);
        if((c!=NULL) && (num=atoi(c)>0) && (num<=NROW)){
            //stampa riga richiesta su buffer (num)
            //close:connection          
        }
        else if(c==NULL){
            //stampa Avabile+ n. ombrelloni disponibili
            // oppure n avabile
            //close_connection
        }
    }
    do {
        switch(i){
            case(0):
                i++;
                if( strcmp(p,"BOOK")!=0 &&
                    strcmp(p,"Book")!=0 &&
                    strcmp(p,"book")!=0 ){  
                    i=-1;
                } 
                if( strcmp(p,"CANCEL")==0 || 
                    strcmp(p,"Cancel")==0 ||
                    strcmp(p,"cancel")==0 ){
                    i=-2;
                    p = strtok(NULL, del);
                    if(p!=NULL){
                        if((num=atoi(p))>0 && num<=NROW*NCOL){

                            i=-3;  ///-------------------------eventualmente aggiungere il controllo con id(?)
                        }
                    }    
                }               
            break;
            case(1):
                num=atoi(p);
                i++;
            break;
            case(2):
                dateSt=atoi(p);
                i++;
            break;
            case(3):
                dateEn=atoi(p);
                i++;   
            break;
            default:
                i++;
            break;
        }
        p = strtok (NULL, del);
    }while (p!=NULL && i>0);
    
    if(i==3) dateEn=dateSt;     //  Per gestire il caso in cui  (book ombrellone "giorno_x" "giorno_x")
    if(dateSt==dateEn && dateSt!=0) i=3;     //      comportamento uguale a  (book ombrellone "giorno_x") 
     
    if(i>4 || i==-1){
        dateSt=0;
        dateEn=0;
        num=0;
        //i=0;
        printf("Invalid Input, i: %d\n",i);
    }
            
    readed->number=num;
    readed->start=dateSt;
    readed->expire=dateEn;
    readed->request=i;

    return readed;
}
 char message[20];
char * handleBook(umbrella* readed){
    int request=readed->request;
    int esito;
   
    switch(request){
        case(-3):          //-3 CANCEL OMBRELLONE 
          //  Cancel_UM(readed);
        break;
        case(-2):          //-2 CANCEL
            //Cancel(readed);
        break;
        case(1):            //1 BOOK (ok nok)
            //BookCheck(readed);
        break;
        case(2):            //2 book Ombrelllone
            esito=Book_FirstStep(readed);
            if(!esito){
                readed->request=-101;
                strcpy(message,"NAVABILE\n");
                return message;
            }
            else{
                //wait(messaggio disponibile)
                //read from buffer    //threadid control!
                strcpy(message,"AVABILE\n");
                return message;
            BookInput();
            handleBook();
            }
        break;
        case(3):            //3 book ombrellone end
            //Book_Dayly(readed);
        break;
        case(4):            //4 book ombrellone start end
            //Book_Sub(readed);
        break;
    }
    
}