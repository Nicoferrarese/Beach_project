#include"umbrella.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>

void handleBook(umbrella* readed){
    int request=readed->request;
    switch(request){
        case(-3):          //-3 CANCEL OMBRELLONE 
            Cancel_UM(readed);
        break;
        case(-2):          //-2 CANCEL
            Cancel(readed);
        break;
        case(1):            //1 BOOK (ok nok)
            BookCheck(readed);
        break;
        case(2):            //2 book Ombrelllone
            Book_FirstStep(readed);
        break;
        case(3):            //3 book ombrellone end
            Book_Dayly(readed);
        break;
        case(4):            //4 book ombrellone start end
            Book_Sub(readed);
        break;
    }
    
}
void Book_FirstStep(umbrella *toBook){
    if(beach[toBook->number]->status==0){
        if(beach[toBook->number]->status!=0){
            //put Navabile in buffer e chiudi connesione
        }
        else{
            beach[toBook->number]->status=2;
            //put Avabile in buffer
            //wait(messaggio disponibile)
            //read from buffer
            BookInput();
            handleBook();
        }    
    }
}
void Book_Dayly(umbrella *tobook){
    int num=tobook->number;
    if(gettid(actual_thread)==id && beach[num]->status==2){
        //aggiorna stato ombrellone
        //scrivi log
        //messaggio sul buffer
        //chiudi connesione

    }
}
void Book_Sub(umbrella *tobook){
    int num=tobook->number;
    // controllo disponibilità
        //aggiorna stato ombrellone
        //scrivi log
        //messaggio sul buffer
        //chiudi connesione
    
}
void Cancel(umbrella* toCancel){
    int num=toCancel->number;
    if(gettid(actual_thread)==id && beach[num]->status==2){
        beach[num]->status=0;
        //conferma su buffer
        close_connection;
    }
}
void Cancel_UM(umbrella* toCancel){
    beach[toCancel->number]->status=0;
    //scrivi log
    //conferma su buffer
    close_connection;
}
void BookCheck(umbrella * toCheck){
    //-------------------------------------------------------------------
    /*
    If(n_client_connessi < limite_connessioni)
        OK;
    else
        NOK;
    Close_connection;    
    */
    //-------------------------------------------------------------------
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
        i=0;
        printf("Invalid Input\n");
    }
            
    readed->number=num;
    readed->start=dateSt;
    readed->expire=dateEn;
    readed->request=i;

    return readed;
}


//----------------------------------------------------------------------------Fine INPUT UMBRELLA   -----------------

void main(){
    char book[200];
    int i=0;
    umbrella* readed;
    readed=(umbrella*)malloc(sizeof(umbrella));
    printf("Inserire dati:\n");
    gets(book);
    
    readed=BookInput(book,readed);

    if(readed->request>0){
        printf("Ombrelllone: %d\n Start: %d\n Finish: %d\n Request: %d\n",readed->number,readed->start,readed->expire,readed->request);
    }

}
