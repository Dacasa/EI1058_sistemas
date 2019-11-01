#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
int main(){
    int t1[2], t2[2], t3[2], t4[2], t5[2], t6[2], t7[2];
    char car;
    pipe(t1);
    if(fork()!=0){//proceso 1, mas viejo, pantalla
        close(t1[1]);  
        while(read(t1[0], &car, sizeof(char))>0){
            write(1, &car, sizeof(char));
        }
        close(t1[0]);
        exit(0);
    }else{
        pipe(t2);
        if(fork()!=0){ //proceso 2, fichero
            close(t1[0]); close(t1[1]);
            close(t2[1]);
            int fd = open("fich", O_WRONLY|O_CREAT|O_TRUNC, 0666);
            while(read(t2[0], &car, sizeof(char))>0){
                write(fd, &car, sizeof(char));
            }
            close(fd); close(t2[0]);
            exit(0);

        }else{
            pipe(t3);
            if(fork()!=0){ //proceso 3, coge el short y lo reparte
                close(t1[0]); close(t2[0]); close(t3[1]);
                while(read(t3[0],&car, sizeof(char))>0){
                    write(t1[1], &car, sizeof(char));
                    write(t2[1], &car, sizeof(char));
                }
                close(t1[1]); close(t2[1]); close(t3[0]);
                exit(0);
            }else{
                close(t1[0]); close(t1[1]); close(t2[0]); close(t2[1]);//ya no son necesarios
                pipe(t4);
                if(fork()!=0){//proceso 4, short
                    wait(NULL);
                    close(t3[0]); close(t4[1]);
                    close(0); dup(t4[0]); close(t4[0]);
                    close(1); dup(t3[1]); close(t3[1]);
                    execlp("short", "short", NULL); 
                }else{
                    close(t3[0]); close(t3[1]);
                    pipe(t5);
                    if(fork()!=0){ //proceso 5 grep -v d
                        wait(NULL); //esperamos que el otro grep termine
                        close(t4[0]); close(t5[1]);
                        close(0); dup(t5[0]); close(t5[0]);
                        close(1); dup(t4[1]); close(t4[1]);
                        execlp("grep", "grep", "-v", "^d", NULL);
                    }else{
                        pipe(t6);
                        if(fork()!=0){//proceso 6 grep d
                            close(t5[0]); close(t5[1]);close(t6[1]); close(t4[0]);
                            close(0); dup(t6[0]); close(t6[0]);
                            close(1); dup(t4[1]); close(t4[1]);
                            execlp("grep", "grep", "^d", NULL);
                        }else{
                            close(t4[0]); close(t4[1]);
                            pipe(t7);
                            if(fork()!=0){ //proceso 7 reparto ls
                                close(t7[1]); close(t6[0]); close(t5[0]);
                                while(read(t7[0], &car, sizeof(char))>0){
                                    write(t5[1], &car, sizeof(char));
                                    write(t6[1], &car, sizeof(char));
                                } 
                                exit(0);
                            }else{ // proceso 8 ls
                                close(t5[0]); close(t5[1]); close(t6[0]); close(t6[1]);
                                close(t7[0]);
                                close(1); dup(t7[1]); close(t7[1]);
                                execlp("ls", "ls", "-al", NULL);
                            }
                        }
                    }
                
                }
            }
        }
    }
}