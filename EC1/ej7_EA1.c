#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <time.h>
#define MAX_DIM 100 
int main(int argc, char *argv[]) { 
    int n, i, j, fd[2], dato=1, fila;  
     
    int A [MAX_DIM][MAX_DIM];   
    time_t t;   
    FILE *fsalida;   
    n=atoi(argv[1]);
       
    srandom(time(&t));
       
    for (i=0; i<n; i++)      
        for (j=0; j<n; j++)         
            A[i][j]= rand() % 10;   
    printf("Proceso mas alto con pid %d:  \n", getpid());   
    printf("Matriz generada (%dx%d):\n", n, n);   
    for (i=0; i<n; i++)   {  
        for (j=0; j<n; j++) 
            printf("%d  ", A[i][j]);
        printf("\n");   
    }   
    fsalida=fopen(argv[2],"w");   
    for (i=0; i<n-1; i++)   { 
        pipe(fd);     
            //fsalida=fopen(argv[2],"w");
        if (fork()!=0) {
                        
                read (fd[0], &dato, sizeof(dato));
                close(fd[0]); close(fd[1]);  
                fila = n - i - 1;
                fprintf(fsalida, "Proceso con pid %d, fila %d:  ", getpid(), fila);            
                            
                for (j=0; j<n; j++) 
                    fprintf(fsalida, "%d  ", A[fila][j]);
                fprintf(fsalida, "\n");
                if (i!=0) write(1,&dato,sizeof(dato));            
                fclose(fsalida);
                exit(0);
        } else {      
                close(1);dup(fd[1]);close(fd[1]); close(fd[0]);
        }  /* if */   
    } /* for */   
    fprintf(fsalida, "Proceso con pid %d, fila 0:  ", getpid()); 
        
        for (j=0; j<n; j++) 
            fprintf(fsalida, "%d  ", A[0][j]);
        fprintf(fsalida, "\n");   
        fclose(fsalida);   
        if (n>1) write(1,&dato,sizeof(dato)); 
        //close(1);
        exit(0); 
}