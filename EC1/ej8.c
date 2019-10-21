#include <fcntl.h>
#include <unistd.h>
int main(){
    int t1[2], t2[2], t3[2];
    pipe(t1);
    if (fork()==0){
        close(1); 
        dup(t1[1]); 
        close(t1[1]); 
        close(t1[0]);
        execlp("ps","ps","-efl",NULL);
    }
    pipe(t2);
    if (fork()==0){   
        close(0); 
        dup(t1[0]); 
        close(t1[0]); 
        close(t1[1]);
        close(1); 
        dup(t2[1]); 
        close(t2[1]); 
        close(t2[0]);
        execlp("grep","grp","castano",NULL);
    }
    pipe(t3);
    if (fork()==0){   
        close(0); 
        dup(t2[0]); 
        close(t2[0]); 
        close(t2[1]);
        close(1); 
        dup(t3[1]); 
        close(t3[1]); 
        close(t3[0]);
        close(t1[0]); 
        close(t1[1]);
        execlp("head","head","-10",NULL); 
    }
    if (fork()==0){   
        close(0); 
        dup(t3[0]); 
        close(t3[0]); 
        close(t3[1]);
        close(t2[0]); 
        close(t2[1]);
        close(t1[0]); 
        close(t1[1]);
        execlp("nl","nl",NULL);
    }
    close(t1[0]); 
    close(t1[1]);
    close(t2[0]); 
    close(t2[1]);
    close(t3[0]); 
    close(t3[1]);
    wait(NULL);
}