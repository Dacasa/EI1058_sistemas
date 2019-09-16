#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc,char *argv[]){
    int tub[2], i, dato;
    int n = atoi(argv[1]);
    dato = 0;
    for(i=1 ; i<n ; i++){
        pipe(tub);
        if(fork()!=0){ //padre
            //wait(NULL);
            read(tub[0],&dato,sizeof(dato));
            close(tub[0]);close(tub[1]);
            if(i==1){
                printf("soy %d y el dato: %d\n", getpid(),dato);
            }else{
                write(1, &dato, sizeof(dato));
            }
            exit(0);
        }else{ //hijo
            close(tub[0]);
            close(1); dup(tub[1]); close(tub[1]);
            if(i==n-1){
                dato = 666;//getpid();
                write(1,&dato, sizeof(dato));
            }
        }
    }
    exit(0);
}