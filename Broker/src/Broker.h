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
#include <commons/log.h>

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

typedef struct memoriaInterna {
	int idMensaje;
	d_message tipoMensaje;
	t_list* suscriptoresConMensaje;
	t_list* suscriptoresConACK;
}memoriaInterna;


///////FUNCIONES INICIALIZACION/////////
void Init();
void ConfigInit();
void ListsInit();

//////FUNCIONES DE CONEXION//////////
void EsperarClientes();
void* AtenderCliente();
void ActuarAnteMensaje();

//////FUNCIONES VARIAS//////////
void suscribir (uint32_t variable, int cliente);
void enviarMensajeNewASuscriptores (void* paquete, t_list* SUSCRIPTORES_NEW);
void enviarMensajeCatchASuscriptores (void* paquete, t_list* lista);
void enviarMensajeGetASuscriptores (void* paquete, t_list* lista);
void enviarMensajeAppearedASuscriptores (void* paquete, t_list* lista);
void enviarMensajeCaughtASuscriptores (void* paquete, t_list* lista);
void enviarMensajeLocalizedASuscriptores (void* paquete, t_list* lista);


////////VARIABLES GLOBALES//////////
config BROKER_CONFIG;
t_log * LOGGER_GENERAL;
t_log * LOGGER_OBLIGATORIO;
enum queueName COLAS;
t_list* CONEXIONES;


////////LISTA DE SUSCRIPTORES//////
t_list* SUSCRIPTORES_NEW;
t_list* SUSCRIPTORES_APPEARED;
t_list* SUSCRIPTORES_GET;
t_list* SUSCRIPTORES_CATCH;
t_list* SUSCRIPTORES_CAUGHT;
t_list* SUSCRIPTORES_LOCALIZED;

#endif /* BROKER_H_ */












