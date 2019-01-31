#include<stdlib.h>
#include<stdio.h>
#define N_um 100
typedef struct _abbonamento{
    int id;
    int num;
    int start;
    int expire;
}abbonamento;

typedef struct _nodo{
    abbonamento el;
    struct _nodo *next;
    struct _nodo *prev;
}nodo;

typedef struct _chain{
    nodo* cella[N_um];
}tabella;



void pretab(tabella* tabella){
    int i;
    for(i=0;i<N_um;i++){
        nodo* z=(nodo*)malloc(sizeof(nodo));
        z=NULL;
	    tabella->cella[i]=z;	
    }
}
void insert(tabella* tabella,int pos,nodo* abbonamento){
    
    if(tabella->cella[pos]!=NULL){
        tabella->cella[pos]->prev=abbonamento;
        abbonamento->next=tabella->cella[pos];        
    }
    tabella->cella[pos]=abbonamento;
    abbonamento->prev=NULL;     
}

void main(){
    int i=1;
    tabella* tab=(tabella*)malloc(sizeof(tabella));
    pretab(tab);
    
    while(i){
        printf("num id st ex:\n");
        nodo* z=(nodo*)malloc(sizeof(nodo));
        scanf("%d %d %d %d",&(z->el.num),&(z->el.id),&(z->el.start),&(z->el.expire));
        insert(tab,z->el.num,z);
        printf("keep?\n");
        scanf("%d",&i);
    }
    for(i=0;i<N_um;i++){
            nodo* t=(nodo*)malloc(sizeof(nodo));
            t=tab->cella[i];
            
            while(t!=NULL){
                printf("num: %d ID: %d -- ",t->el.num,t->el.id);
                if(t->next==NULL)printf("\n");
                t=t->next;
            }
    
    }
}
