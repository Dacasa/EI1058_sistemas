/* David Castellano Sanchez */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    if(argc<=3){
        perror("Numero de argumentos incorrectos\n");
        exit(-1);
    }
    int pid, estado;
    if(fork()==0){ //proceso hijo
        int fd = open(argv[1], O_CREAT|O_APPEND|O_WRONLY, 0644);
        close(1); dup(fd); close(fd);

        fd = open(argv[2], O_CREAT|O_TRUNC|O_WRONLY, 0644);
        close(2); dup(fd); close(fd);
        
        execvp(argv[3], &argv[3]);
        perror("Error en la ejecucion del execvp\n");
        exit(-1);
    }
    //proceso padre
    //mirar transparencias para arreglar el error
    pid = wait(&estado);
    estado &= 0x0000ff00;
    estado = estado >> 8;
    exit(0);
}