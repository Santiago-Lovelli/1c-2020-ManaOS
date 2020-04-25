#ifndef BROKER_H_
#define BROKER_H_

///////INCLUDES/////////////
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <Logger/Logger.h>
#include <Conexiones/Conexiones.h>
#include <Serializacion/Serializacion.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>

///////ESTRUCTURAS//////////
typedef struct config
{
    int TAMANO_MEMORIA;
    int TAMANO_MINIMO_PARTICION;
	char* ALGORITMO_MEMORIA;
	char* ALGORITMO_REEMPLAZO;
	char* ALGORITMO_PARTICION_LIBRE;
	char* IP_BROKER;
	char* PUERTO_BROKER;
	int FRECUENCIA_COMPACTACION;
    char* LOG_FILE;
}config;

enum queueName {
	GET,
	LOCALIZED,
	CATCH,
	CAUGHT,
	NEW,
	APPEARED
};

///////FUNCIONES INICIALIZACION/////////
void Init();
void ConfigInit();
void ListsInit();

//////FUNCIONES DE CONEXION//////////
void EsperarClientes();
void* AtenderCliente();
void ActuarAnteMensaje();


////////VARIABLES GLOBALES//////////
config BROKER_CONFIG;
t_log * LOGGER_GENERAL;
t_log * LOGGER_OBLIGATORIO;
enum queueName COLAS;
t_list* CONEXIONES;

#endif /* BROKER_H_ */












