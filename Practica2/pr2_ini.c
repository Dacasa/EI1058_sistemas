// David Castellano Sanchez

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>


#define PROMPT		    "\n=> "
#define ESPACIO		    ' '

#define MAX_BUF		    1024
#define	MAX_ARGS	    32
#define MAX_COMANDOS    10

struct comando{
    char *argv[MAX_ARGS];
    int nargs;
};



//Functions
int arrange(char *buffer) ;
int makeargs(char *buffer, char *args[]) ;
int desglosar_tub (char *buffer, struct comando lista_comandos[]) ;

int main( int argc, char *argv[] ) {

    int i, num_comandos, tub[2], pid;

    char buffer[MAX_BUF], *args[MAX_ARGS];
    struct comando lista_comandos[MAX_COMANDOS];

    puts("\nMinishell\n");
    while (1) {
        printf(PROMPT);
        if (fgets(buffer,  MAX_BUF, stdin) == NULL) 
            continue;

        
        num_comandos = desglosar_tub(buffer, lista_comandos);
        if (fork()==0){
            for (i=0 ; i<num_comandos-1 ; i++) {
                pipe(tub);
                if (fork()!=0){     //padre
                    close(0); dup(tub[0]); close(tub[0]); close(tub[1]);    //leer de tuberia
                    int c = num_comandos -1 -i;
                    execvp(lista_comandos[c].argv[0], lista_comandos[c].argv);
                } else {            //hijo
                    close(1); dup(tub[1]); close(tub[0]); close(tub[1]);    //escribir en tub
                    if(i==num_comandos-2){
                        execvp(lista_comandos[0].argv[0], lista_comandos[0].argv);
                    }
                }
            }
        } else {  //el proceso principal no ejecuta ningun exec
            wait(NULL);
        }
    }// while(1) 
}//main

int arrange(char *buffer) {

    int i = 0, j = 0;
    while (buffer[i]) {
        while (buffer[i] == ESPACIO) {
            i++;
        }
        while ((buffer[i] != ESPACIO) && buffer[i]) {
            buffer[j++] = buffer[i++];
        }
        if (! buffer[i]) {
            if(buffer[i-2] == ESPACIO) {
                buffer[j-2] = '\0';
            } else {
                buffer[j-1] = '\0';
            }
        } else {
            buffer[j++] = ESPACIO;
        }
    }
 return(j);
}

int makeargs(char *buffer, char *args[]) {

    int i = 0;
    char *puntero;

    puntero = buffer;
    args[i++] = puntero;

    while(puntero = strchr(puntero, ESPACIO)) {
        *puntero++ = '\0';
        args[i++] = puntero;
    }
    args[i] = NULL;
    return(i);
}

int desglosar_tub (char *buffer, struct comando lista_comandos[]) {

    char *p1, *p2;
    int i,j,n;
    int ncomandos=0;

    p1=buffer;
    i=0;

    n=arrange(p1);
    printf("Entrada: #%s#\n", buffer);//borrar

    if (n==1) 
        return(0); /* No hay comandos, sino blancos o <CR> */

    while (p2=strchr(p1,'|')) {
        if (*(p2-1)==ESPACIO) {
            *(p2-1)='\0'; 
        } else {
            *p2='\0';
        }
       
        p2++;
        lista_comandos[ncomandos].nargs = makeargs(p1,lista_comandos[ncomandos].argv);
        ncomandos++;
        if (p2!=NULL) { 
            if (*p2!=ESPACIO) 
                p1=p2; 
            else 
                p1=p2+1;
        }
    } /* fin while */

    lista_comandos[ncomandos].nargs = makeargs(p1,lista_comandos[ncomandos].argv);
    ncomandos++; 

    
    for (i=0; i<ncomandos; i++) {
        printf("Comando %d (%d argumentos):\n",i,lista_comandos[i].nargs);
        j=0;
        while(lista_comandos[i].argv[j]) {
            printf("    comando[%d].argv[%d] (dir. %08lu) = #%s#\n", i, j,(unsigned long) (lista_comandos[i].argv[j]), lista_comandos[i].argv[j]);
            j++;
        }
    }
    
    return(ncomandos);  /* Numero de componentes del vector lista_comandos 
                          (empezando a contar desde 1) */
}
