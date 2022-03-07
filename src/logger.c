#include <logger.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

FILE *logfile = NULL;

void logger(const char *filepath, float ti, float te, float tr, float cs)
{
    if ((logfile = fopen(filepath, "a")) == NULL)
    {
        printf("Error opening log file");
        return;
    }

    struct timeval tv;
    time_t t;
    struct tm *info;
    char buffer[64];

    gettimeofday(&tv, NULL);
    t = tv.tv_sec;

    info = localtime(&t);

    strftime(buffer, sizeof buffer, "%d-%m-%Y %H:%M:%S", info); // datetime
    // printf("%s", buffer);

    fprintf(logfile, "%s, %f, %f, %f, %f\n", buffer, ti, te, tr, cs);

    fclose(logfile);
}

// Data e hora, temperatura interna, temperatura externa, temperatura definida pelo usuário, valor de acionamento dos atuadores (Resistor e Venoinha em valor percentual)