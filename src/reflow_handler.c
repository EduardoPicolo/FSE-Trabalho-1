#include <reflow_handler.h>
#include <stdio.h>
#include <stdlib.h>

FILE *file = NULL;
float curva[10][2];

void loadReflowData()
{
    openFile();
    readFile();
}

void openFile()
{
    if ((file = fopen("src/curva_reflow.csv", "r")) == NULL)
    {
        printf("Error opening file");
        exit(1);
    }
}

void readFile()
{
    char buffer[20];
    float time, temp;
    int i = 0;
    while (fscanf(file, "%f,%f", &time, &temp) != EOF)
    {
        // printf("%f, %f\n", time, temp);
        curva[i][0] = time;
        curva[i][1] = temp;
        i++;
    }
    fclose(file);
}

float get_curve_value(int *time)
{
    return (float)curva[(*time)++ / 60][1];
}