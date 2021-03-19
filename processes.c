// processes.c - main processo padre
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <mqueue.h>
#include <sys/wait.h>
#include "data.h"

// funzione main()
int main(int argc, char* argv[])
{
    // set attributi della message queue
    struct mq_attr ma;
    ma.mq_maxmsg  = 10;             // numero massimo di messaggi permessi nella coda
    ma.mq_msgsize = sizeof(Data);   // massimo size di un messaggio (in byte)

    // creo la message queue
    if (mq_open(MQUEUE_PATH, O_RDWR | O_CREAT, 0700, &ma) == -1) {
        // errore di creazione
        printf("%s: non posso creare la message queue (%s)\n", argv[0], strerror(errno));
        exit(EXIT_FAILURE);
    }

    // crea i processi figli
    pid_t pid1, pid2;
    (pid1 = fork()) && (pid2 = fork());

    // test pid processi
    if (pid1 == 0) {
        // sono il figlio 1
        printf("sono il figlio 1 (%d): eseguo il nuovo processo\n", getpid());
        char *pathname = "reader";
        char *newargv[] = { pathname, NULL };
        execv(pathname, newargv);
        exit(EXIT_FAILURE);   // exec non ritorna mai
    }
    else if (pid2 == 0) {
        // sono il figlio 2
        printf("sono il figlio 2 (%d): eseguo il nuovo processo\n", getpid());
        char *pathname = "writer";
        char *newargv[] = { pathname, NULL };
        execv(pathname, newargv);
        exit(EXIT_FAILURE);   // exec non ritorna mai
    }
    else if (pid1 > 0 && pid2 > 0) {
        // sono il padre
        printf("sono il padre (%d): attendo la terminazione dei figli\n", getpid());
        int status;
        pid_t wpid;
        while ((wpid = wait(&status)) > 0)
            printf("sono il padre (%d): figlio %d terminato (%d)\n", getpid(), (int)wpid, status);

        // rimuovo la message queue ed esco
        printf("%s: processi terminati\n", argv[0]);
        mq_unlink(MQUEUE_PATH);
        exit(EXIT_SUCCESS);
    }
    else {
        // errore nella fork(): rimuovo la message queue ed esco
        printf("%s: fork error (%s)\n", argv[0], strerror(errno));
        mq_unlink(MQUEUE_PATH);
        exit(EXIT_FAILURE);
    }
}
