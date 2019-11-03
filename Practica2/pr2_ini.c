// David Castellano Sanchez
//Practica 2


#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define ESPACIO		    ' '

#define MAX_BUF		    1024
#define	MAX_ARGS	    32
#define MAX_COMANDOS    10
#define MAX_PATH        100
#define MAX_USER        20

struct comando{
    char *argv[MAX_ARGS];
    int nargs;
};



//Functions
int arrange(char *buffer) ;
int makeargs(char *buffer, char *args[]) ;
int desglosar_tub (char *buffer, struct comando lista_comandos[]) ;
void ejecuta_simple(struct comando *comando);
void ejecuta_tub(int n_c, struct comando lista_comandos[]);
void redireccion_entrada(struct comando *comando);
void redireccion_salida_error (struct comando *comando);
void prompt(char *path, char *user);
int contains_char(char *st, char c);
void split_equals(char *buffer, char *s[]);


int main( int argc, char *argv[] ) {

    int i, num_comandos;

    char buffer[MAX_BUF], *args[MAX_ARGS];
    struct comando lista_comandos[MAX_COMANDOS];
    char path[MAX_PATH];
    getcwd(path, sizeof(path));
    char user[MAX_USER];
    getlogin_r(user, sizeof(user));


    puts("\nMinishell");
    while (1) {
        prompt(path, user);
        if (fgets(buffer,  MAX_BUF, stdin) == NULL) 
            continue;

        num_comandos = desglosar_tub(buffer, lista_comandos);

        printf("\n");
        if (num_comandos==1){
            if ( !strcmp(lista_comandos[0].argv[0], "cd") ) {    //si se trata de un cd
                chdir(lista_comandos[0].argv[1]);
                getcwd(path, sizeof(path));
            } else if (contains_char(lista_comandos[0].argv[0], '=')){      //añadir variable
                char *var[2];
                split_equals(lista_comandos[0].argv[0],var);
                setenv(var[0], var[1], 1);
            } else if (!strcmp(lista_comandos[0].argv[0], "exit")){      //cerrar minishell
                printf("DONE\n");
                exit(0);
            } else {
                ejecuta_simple(&lista_comandos[0]);     //comando simple
            }
        } else {
            ejecuta_tub(num_comandos, lista_comandos);  //comando con tub
        }

    }// while(1) 
}//main


int contains_char(char *st, char c){
    int i=0;
    while(st[i]){
        if(st[i]==c){ 
            return 1;
        }
        i++;
    }
    return 0;
}

void prompt(char *path, char *user){
    printf("\n%s@%s=>", user, path);
}

void ejecuta_simple(struct comando *comando) {
    if(fork()==0){

        redireccion_entrada(comando);
        redireccion_salida_error (comando);

        execvp(comando->argv[0], comando->argv);
        perror("Error en exec");
        exit(-1);
    } else{
        int estado;
        char aux[32];
        wait(&estado);
        estado &= 0x0000ff00;
        estado = estado >> 8;
        sprintf(aux,"%d",estado);
        setenv("?", aux, 1);
    }
}

void ejecuta_tub(int n_c, struct comando lista_comandos[]) {
    int i, tub[2];
    if (fork()==0){
        for (i=0 ; i<n_c-1 ; i++) {
            pipe(tub);
            if (fork()!=0){     //padre
                close(0); dup(tub[0]); close(tub[0]); close(tub[1]);    //leer de tuberia
                int c = n_c -1 -i;
                if(c == n_c-1){ //ultimo comando
                    redireccion_salida_error (&lista_comandos[c]);
                }
                execvp(lista_comandos[c].argv[0], lista_comandos[c].argv);
                perror("Error en exec");
                exit(-1);
            } else {            //hijo
                close(1); dup(tub[1]); close(tub[0]); close(tub[1]);    //escribir en tub
                if( i == n_c-2 ) {
                    redireccion_entrada(&lista_comandos[0]);
                    execvp(lista_comandos[0].argv[0], lista_comandos[0].argv);
                    perror("Error en exec");
                    exit(-1);
                }
            }
        }
    } else {  //el proceso principal no ejecuta ningun exec
        int estado;
        char aux[32];
        wait(&estado);
        estado &= 0x0000ff00;
        estado = estado >> 8;
        sprintf(aux,"%d",estado);
        setenv("?", aux, 1);
    }
}

void redireccion_entrada(struct comando *comando) {
    int i;
    
    for (i=1 ; i< comando->nargs ; i++){
        if ( !strcmp(comando->argv[i], "<") ) {
            int fd = open(comando->argv[i+1], O_RDONLY);
            close(0); dup(fd); close (fd);
            comando->argv[i] = NULL;
            comando->argv[i+1] = NULL;
            break;
        }
    }
}

void redireccion_salida_error (struct comando *comando) {
    int i, fd;
    for (i=1 ; i< comando->nargs ; i++){
        if (comando->argv[i] == NULL) continue;

        if (!strcmp(comando->argv[i],">" ) ) {
            fd = open(comando->argv[i+1],O_CREAT|O_TRUNC|O_WRONLY, 0644);
            close(1); dup(fd); close(fd);
            comando->argv[i] = NULL;
            comando->argv[i+1] = NULL;
            i++;
            continue;
        }
        if (!strcmp(comando->argv[i],">>" ) ) {
            fd = open(comando->argv[i+1],O_CREAT|O_APPEND|O_WRONLY, 0644);
            close(1); dup(fd); close(fd);
            comando->argv[i] = NULL;
            comando->argv[i+1] = NULL;
            i++;
            continue;
        }
        if (!strcmp(comando->argv[i],"2>" ) ) {
            fd = open(comando->argv[i+1],O_CREAT|O_TRUNC|O_WRONLY, 0644);
            close(2); dup(fd); close(fd);
            comando->argv[i] = NULL;
            comando->argv[i+1] = NULL;
            i++;
            continue;
        }
        if (!strcmp(comando->argv[i],"2>>" ) ) {
            fd = open(comando->argv[i+1],O_CREAT|O_APPEND|O_WRONLY, 0644);
            close(2); dup(fd); close(fd);
            comando->argv[i] = NULL;
            comando->argv[i+1] = NULL;
            i++;
            continue;
        }
    }
}

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

void split_equals(char *buffer, char *s[]){
    int i = 0;
    char *puntero;

    puntero = buffer;
    s[i++] = puntero;

    while(puntero = strchr(puntero, '=')) {
        *puntero++ = '\0';
        s[i++] = puntero;
    }
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
    //printf("Entrada: #%s#\n", buffer);//borrar

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

    /*
    for (i=0; i<ncomandos; i++) {
        printf("Comando %d (%d argumentos):\n",i,lista_comandos[i].nargs);
        j=0;
        while(lista_comandos[i].argv[j]) {
            printf("    comando[%d].argv[%d] (dir. %08lu) = #%s#\n", i, j,(unsigned long) (lista_comandos[i].argv[j]), lista_comandos[i].argv[j]);

            //modificado para que compruebe el uso de variables de entorno y las sustituye
            if (contains_char(lista_comandos[i].argv[j], '$')) {   
                char *var = getenv(lista_comandos[i].argv[j]+1); 
                lista_comandos[i].argv[j] = var;
            }
            j++;
        }
    }*/
    return(ncomandos);  /* Numero de componentes del vector lista_comandos 
                          (empezando a contar desde 1) */
}