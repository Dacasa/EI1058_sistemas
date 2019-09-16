#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
main(int argc, char*argv[]){
    int i;
    if (argc < 2) {
        printf("N ́umero de par ́ametos incorrecto\n"); 
        exit(-1);
    }
    printf("\nSoy el padre %d y espero a que acabe mi hijo\n",getpid());
    if (fork()!=0)
        wait(NULL);
    else{
        printf("\nSoy %d y ejecuto %s:\n",getpid(),argv[1]);
        execvp(argv[1],&argv[1]);
        perror("El error es");
        exit (-1);
    }
    exit(0);
}
