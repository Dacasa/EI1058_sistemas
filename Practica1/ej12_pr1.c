/* David Castellano Sanchez */

/*
    He utilizado una jerarquia abuelo-padre-hijo en el cual el abuelo es el primero en
    escribir por que de esta forma los hijos redirigen la tuberia por la entrada 
    estandar y no se pierde el prompt.

    Es mala idea utilizar una jerarquia padre con varios hijos en la cual el primero
    en ecribir es el padre y despues el ultimo hijo hasta el primero por que tenemos
    que redirigir la salida estandar y perdemos el prompt. Esto mismo sucederia en el
    caso de usar una jerarquia abuelo-padre-hijo en el que el primero en escribir es el
    hijo.

    Tambien seria valida una jerarqui padre con varios hijo en el cual el primero en 
    escribir es el primer hijo y el ultimo el padre.

*/


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    int tub[2], n, i, dato, e;
    dato = 1;
    if(argc!=2){
        perror("numero de argumentos incorrectos");
        exit(-1);
    }
    n = atoi(argv[1]);
    for(i=1 ; i<n ; i++){
        pipe(tub);
        
        if(fork()!=0){ //padre
            
            if(i!=1){
                read(0, &dato, sizeof(dato));
            }
            printf("Soy el proceso con pid: %d\n", getpid());
            printf("tabla del %d : \n",i);
            //sleep(3);
            for(e=1 ; e<11 ; e++){
                printf("    %d * %d = %d\n", i,e, i*e);
            }

            write(tub[1], &dato, sizeof(dato));
            close(tub[0]); close(tub[1]);

            wait(NULL);
            exit(0);

        }else{       //hijo
            close(0); dup(tub[0]); 
            close(tub[0]);close(tub[1]);
        }
    } //for
    if(n>1) read(0, &dato, sizeof(dato));
    printf("Soy el proceso con pid: %d\n", getpid());
    printf("tabla del %d :\n",i);
    for(e=1 ; e<11 ; e++){
        printf("    %d * %d = %d\n", i,e, i*e);
    }
    close(tub[0]); close(tub[1]);
    exit(0);

}