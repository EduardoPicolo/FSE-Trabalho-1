#ifndef PID_H_
#define PID_H_

void pid_set_reference(double reference);
double pid_get_reference();
void pid_configura_constantes(double Kp, double Ki, double Kd);
void pid_atualiza_referencia(double referencia);
double pid_controle(double saida_medida);

#endif /* PID_H_ */
