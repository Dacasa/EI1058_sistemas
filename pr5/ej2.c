#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

//headers
void handler_c();
void handler_z();
void handler_alrm();

int suma_total = 0;
int suma_parcial = 0;
int primera_vez = 0;

void main() {
    //config
    int num;

    //solo señales SIGINT y SIGALRM, resto bloqueadas
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGINT);
    sigdelset(&mask, SIGALRM);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    //handlers
    //SIGINT
    struct sigaction act;
    act.sa_handler = handler_c;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT, &act, NULL);

    //SIGALRM
    act.sa_handler = handler_alrm;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGALRM, &act, NULL);

    //ignorar señales SIGSTP y SIGUSR1, hasta primera señal SIGINT
    //de esta forma evitamos su tratamiento al desbloquearlas
    act.sa_handler = SIG_IGN;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGTSTP, &act, NULL);
    sigaction(SIGUSR1, &act, NULL);

    // ignorar señales de sigusr2
    sigaction(SIGUSR2, &act, NULL);
    
    // main
    for (;;){
        alarm(10);
        printf("Introduce un numero: ");
        if(scanf("%d", &num)) {        //valor leido correcto
            //printf("\n");
            suma_parcial += num;
            suma_total += num;
        } else {
            printf("Valor introducido incorrecto\n");
        }
        num = 0;
        
        //desbloquear SIGSTP y SIGUSR1 si se ha recibido una señal SIGINT
        if (primera_vez == 1) {     
            sigset_t mask;
            sigemptyset(&mask);
            sigaddset(&mask, SIGTSTP);
            sigaddset(&mask, SIGUSR1);
            sigprocmask(SIG_UNBLOCK, &mask, NULL);

            struct sigaction act;
            act.sa_handler = handler_z;
            act.sa_flags = 0;
            sigemptyset(&act.sa_mask);
            sigaction(SIGTSTP, &act, NULL);
            sigaction(SIGUSR1, &act, NULL);
            primera_vez=99;
        }
    }
    


}


void handler_c() {          //SIGINT
    if (!primera_vez) {
        primera_vez++;       
    }
    printf("\nLa suma parcial es de: %d\n",suma_parcial);
    suma_parcial = 0;
}

void handler_z() {          //SIGTSTP
    printf("\nLa suma total es de: %d\n",suma_total);
    exit(0);
}

void handler_alrm() {       //SIGALRM
    printf("\nNo has llegado a tiempo...\n");
    printf("La suma total es de: %d\n",suma_total);
    exit(-1);
}
