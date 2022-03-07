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
} pid = {0, 0, 0, 0, 0, 0, 0.5, 0, 0, 0, 100, -100};

typedef struct PID PID;

void pid_set_reference(double reference)
{
    pid.referencia = reference;
}

double pid_get_reference()
{
    return pid.referencia;
}

void pid_configura_constantes(double Kp, double Ki, double Kd)
{
    pid.Kp = Kp;
    pid.Ki = Ki;
    pid.Kd = Kd;
}

void pid_atualiza_referencia(double referencia)
{
    pid.referencia = referencia;
}

double pid_controle(double saida_medida)
{

    double erro = pid.referencia - saida_medida;

    pid.erro_total += erro; // Acumula o erro (Termo Integral)

    if (pid.erro_total >= pid.sinal_de_controle_MAX)
    {
        pid.erro_total = pid.sinal_de_controle_MAX;
    }
    else if (pid.erro_total <= pid.sinal_de_controle_MIN)
    {
        pid.erro_total = pid.sinal_de_controle_MIN;
    }

    double delta_error = erro - pid.erro_anterior; // Diferença entre os erros (Termo Derivativo)

    pid.sinal_de_controle = pid.Kp * erro + (pid.Ki * pid.T) * pid.erro_total + (pid.Kd / pid.T) * delta_error; // PID calcula sinal de controle

    if (pid.sinal_de_controle >= pid.sinal_de_controle_MAX)
    {
        pid.sinal_de_controle = pid.sinal_de_controle_MAX;
    }
    else if (pid.sinal_de_controle <= pid.sinal_de_controle_MIN)
    {
        pid.sinal_de_controle = pid.sinal_de_controle_MIN;
    }

    pid.erro_anterior = erro;

    return pid.sinal_de_controle;
}
