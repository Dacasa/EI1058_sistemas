#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    if(argc<=2){
        perror("numero de argumentos incorrectos\n");
        exit(-1);
    }
    printf("ejecutando\n");
    int fd = open(argv[1], O_CREAT|O_TRUNC|O_WRONLY, 0666);
    close(1);
    dup(fd);
    close(fd);
    execvp(argv[2], &argv[2]);
    perror("Error en la ejecucion del execvp\n");
    exit(-1);
}