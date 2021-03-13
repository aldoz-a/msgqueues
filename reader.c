// reader.c - main processo figlio
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <mqueue.h>
#include "data.h"

// funzione main()
int main(int argc, char *argv[])
{
    // apro la message queue in modo lettura
    printf("processo %d partito\n", getpid());
    mqd_t mq;
    if ((mq = mq_open(MQUEUE_PATH, O_RDONLY)) == -1) {
        // errore di apertura
        printf("%s: non posso aprire la message queue (%s)\n", argv[0], strerror(errno));
        exit(EXIT_FAILURE);
    }

    // set clock e time per calcolare il tempo di CPU e il tempo di sistema
	clock_t t_start = clock();
    struct timeval tv_start;
    gettimeofday(&tv_start, NULL);

    // loop di lettura messaggi dal writer
    Data my_data;
    while (mq_receive(mq, (char *)(&my_data), sizeof(Data), NULL) != -1) {
        // test index per forzare l'uscita
        if (my_data.index == N_MESSAGES) {
            // get clock e time per calcolare il tempo di CPU e il tempo di sistema
            clock_t t_end = clock();
            double t_passed = ((double)(t_end - t_start)) / CLOCKS_PER_SEC;
            struct timeval tv_end, tv_elapsed;
            gettimeofday(&tv_end, NULL);
            timersub(&tv_end, &tv_start, &tv_elapsed);

            // il processo chiude la message queue ed esce per indice raggiunto
            printf("reader: ultimo messaggio ricevuto: %s\n", my_data.text);
            printf("processo %d terminato (index=%ld CPU time elapsed: %.3f - total time elapsed:%ld.%ld)\n",
                   getpid(), my_data.index, t_passed, tv_elapsed.tv_sec, tv_elapsed.tv_usec / 1000);
            mq_close(mq);
            exit(EXIT_SUCCESS);
        }
    }

    // il processo chiude la message queue ed esce per altro motivo (errore)
    printf("processo %d terminato con errore (%s)\n", getpid(), strerror(errno));
    mq_close(mq);
    exit(EXIT_FAILURE);
}
