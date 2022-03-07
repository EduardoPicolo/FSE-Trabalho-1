#ifndef LOGGER_H
#define LOGGER_H

#include <unistd.h>

void openLogFile();
void logger(float ti, float te, float tr, float cs);

#endif