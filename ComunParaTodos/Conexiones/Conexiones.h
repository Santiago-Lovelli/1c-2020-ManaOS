#include <commons/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <readline/readline.h>
#include <unistd.h>
#include <commons/collections/list.h>
#include <string.h>

#ifndef CONECCIONES_H_
#define CONECCIONES_H_

/*
 * Recibe un char *ip, char* puerto y un t_log* log,
 * Retorna un numero de socket cliente,
 * si no logra conectarse retorna -1
 */
int conectarse_a_un_servidor(char *ip, char* puerto,t_log* log);

/*
 * Recibe un char* ip, char* puerto y un t_log* logger,
 * Retorna un numero de socket de servidor,
 * si no logra conectarse retorna -1
 */
int iniciar_servidor(char* ip, char* puerto,t_log* logger);

/*
 * Recibe un int socket_servidor y un t_log* logger,
 * Retorna un numero de socket de cliente luego de conectarse,
 * mientras no se conecte espera que se conecte uno
 */
int esperar_cliente_con_accept(int socket_servidor, t_log* logger);

#endif /* CONECCIONES_H_ */
