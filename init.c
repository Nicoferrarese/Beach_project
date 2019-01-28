 #include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include"umbrella.h"
 int main(){
     FILE *fd;
     if((fd = fopen("spiaggia.txt", "w")) == NULL) {
        perror("Errore apertura");
        exit(-1);
    }
    int i;
    int z=0;
    for(i=1;i<=N_um;i++){
        if((i%10)==0)
            z=1;
        else z=0;
        fprintf(fd,"%d %d 0 0\n",i,z);
    }
    if (fclose(fd) != 0){
        perror("Errore chiusura file\n");
        exit(-2);
    } 
 }    