#ifndef TEAM_H_
#define TEAM_H_

#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/config.h>
#include <Conexiones/Conexiones.h>
#include <Serializacion/Serializacion.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <Logger/Logger.h>

//Ver como crear lista de listas
/////////ESTRUCTURA/////////
typedef struct config{
	t_list* POSICIONES_ENTRENADORES;
	t_list* POKEMON_ENTRENADORES;
	t_list* OBJETIVOS_ENTRENADORES;
	int TIEMPO_RECONEXION;
	int RETARDO_CICLO_CPU;
	char* ALGORITMO_PLANIFICACION;
	int QUANTUM;
	int ESTIMACION_INICIAL;
	char* IP_BROKER;
	int PUERTO_BROKER;
	char* LOG_FILE;
	char* IP_TEAM;
	int PUERTO_TEAM;
}config;

//////FUNCIONES/////////
void iniciarConfig();
void iniciar();
void escucharMensajes();
void conectarseConBroker();

/////////VARIABLES/////////
config TEAM_CONFIG;
t_log * TEAM_LOG;

#endif /* TEAM_H_ */
