#ifndef UART_H_
#define UART_H_

#include <unistd.h>  //Used for UART
#include <fcntl.h>   //Used for UART
#include <termios.h> //Used for UART

#define MATRICULA 0x03, 0x03, 0x03, 0x03

#define CLIENT_CODE 0x00
#define SERVER_CODE 0x01
#define CMD_CODE 0x23
#define SEND_COMMAND_CODE 0x16

#define GET_TEMPERATURE 0xC1
#define GET_POTENCIOMETRO 0xC2
#define GET_COMMAND 0xC3

#define SEND_CONTROL_SIGNAL 0xD1
#define SEND_REF_SIGNAL 0xD2
#define SEND_SYSTEM_STATUS 0xD3
#define SEND_REF_MODE 0xD4

#define CMD_TURN_ON 1
#define CMD_TURN_OFF 2
#define CMD_REF_POTENCIOMETRO 3
#define CMD_REF_CURVE 4

#define WAIT_TIME 500000

// SETUP UART
void init_uart(int *);

// OPEN SERIAL PORT
void setup_start(int *);

// CONFIGURE THE UART
// The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
void configure_uart(int *);

// CLOSE UART
void close_uart(int *);

// TX BYTES
int write_uart(int *uart_filestream, unsigned char *message, int size);

// CHECK FOR ANY RX BYTES
int read_data(int *uart_filestream, unsigned char *code, void *data, int size);
int read_int(int *uart_filestream);
float read_float(int *uart_filestream);
char read_char(int *uart_filestream);
int read_string(int *uart_filestream, char *message);

#endif
