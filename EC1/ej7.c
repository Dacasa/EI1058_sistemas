#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]){
    int tubo[2], status, i, dato, res, pid;
    pipe(tubo);
    pid=fork();
    if (pid != 0){  
        close(tubo[0]);
        dato=atoi(argv[1]);
        for (i=1;i<=dato;i++) 
            write(tubo[1],&i,sizeof(i));
        close(tubo[1]);
        wait(&status);
        printf ("Proceso B finaliza\n");
        
        exit(0);
    }else {
        close(tubo[1]);
        res = 1;
        while (read(tubo[0],&dato,sizeof(int))>0) 
            res = res * dato;
        close(tubo[0]);
        printf("Proceso A finaliza. Resultado: %d \n", res);
        exit(0);
        }
    }