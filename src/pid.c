#include "pid.h"
#include <stdio.h>

struct PID
{
    double saida_medida;
    double sinal_de_controle;
    double referencia;
    double Kp; // Ganho Proporcional
    double Ki; // Ganho Integral
    double Kd; // Ganho Derivativo
    double T;  // Período de Amostragem (ms)
    unsigned long last_time;
    double erro_total;
    double erro_anterior;
    double sinal_de_controle_MAX;
    double sinal_de_controle_MIN;
} pid = {0, 0, 0, 0, 0, 0, 1.0, 0, 0, 0, 100.0, 100.0};

typedef struct PID PID;

void pid_configura_constantes(double Kp_, double Ki_, double Kd_)
{
    pid.Kp = Kp_;
    pid.Ki = Ki_;
    pid.Kd = Kd_;
}

void pid_atualiza_referencia(float referencia_)
{
    pid.referencia = (double)referencia_;
}

double pid_controle(double saida_medida)
{
    double erro = pid.referencia - saida_medida;

    printf("Saida: \t Ref: \t Erro \t Erro Acum \t Erro (CAP) \t Erro Ant. \t Delta Erro \t Sinal Controle\n");

    pid.erro_total += erro;

    printf("%.2f\t %.2f\t %.2f\t %.2f\t", saida_medida, pid.referencia, erro, pid.erro_total);

    if (pid.erro_total >= pid.sinal_de_controle_MAX)
    {
        pid.erro_total = pid.sinal_de_controle_MAX;
    }
    else if (pid.erro_total <= pid.sinal_de_controle_MIN)
    {
        pid.erro_total = pid.sinal_de_controle_MIN;
    }
    printf("\t%.2f\t", pid.erro_total);

    double delta_error = erro - pid.erro_anterior; // Diferença entre os erros (Termo Derivativo)
    pid.erro_anterior = erro;

    printf("\t%.2f\t \t%.2f\t", pid.erro_anterior, delta_error);

    pid.sinal_de_controle = pid.Kp * erro + (pid.Ki * pid.T) * pid.erro_total + (pid.Kd / pid.T) * delta_error; // PID calcula sinal de controle

    if (pid.sinal_de_controle >= pid.sinal_de_controle_MAX)
    {
        pid.sinal_de_controle = pid.sinal_de_controle_MAX;
    }
    else if (pid.sinal_de_controle <= pid.sinal_de_controle_MIN)
    {
        pid.sinal_de_controle = pid.sinal_de_controle_MIN;
    }

    printf("\t%.2f\n", pid.sinal_de_controle);

    return pid.sinal_de_controle;
}
