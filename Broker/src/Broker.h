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

typedef struct estructuraAdministrativa {
	int idMensaje;
	int estaOcupado;
	int tamanioParticion;
	void* donde;
	int tiempo;
	int ultimaReferencia;
	d_message tipoMensaje;
	t_list* suscriptoresConMensajeEnviado;
	t_list* suscriptoresConACK;
}estructuraAdministrativa;

typedef struct mensajeConID{
	void *pack;
	int id;
}mensajeConID;

typedef struct cachearNew{
	uint32_t largoDeNombre;
	char * nombrePokemon;
	uint32_t posX;
	uint32_t posY;
	uint32_t cantidad;
}cachearNew;

///////FUNCIONES INICIALIZACION/////////
void Init();
void ConfigInit();
void ListsInit();
void MemoriaPrincipalInit();

//////FUNCIONES DE CONEXION//////////
void EsperarClientes();
void* AtenderCliente();
void ActuarAnteMensaje();

//////FUNCIONES VARIAS//////////
void suscribir (uint32_t variable, int cliente);
void tratarMensajeNewASuscriptores (void* paquete, t_list* SUSCRIPTORES_NEW);
void enviarMensajeCatchASuscriptores (void* paquete, t_list* lista);
void enviarMensajeGetASuscriptores (void* paquete, t_list* lista);
void enviarMensajeAppearedASuscriptores (void* paquete, t_list* lista);
void enviarMensajeCaughtASuscriptores (void* paquete, t_list* lista);
void enviarMensajeLocalizedASuscriptores (void* paquete, t_list* lista);
mensajeConID agregarIDMensaje (void* paquete);
void actualizarEnviadosPorID(int id, int socketCliente);

//////FUNCIONES CACHE//////////
void * guardarMensaje(d_message tipoMensaje, void * mensajeAGuardar);
void * buscarParticionLibrePara(int mensajeAGuardar);
estructuraAdministrativa* buscarEstructuraAdministrativaConID(int id);
int obtenerID();
int tamanioDeMensaje(d_message tipoMensaje, void * unMensaje);

//////////FUNCION BUDDY Y PARTICION DINAMICA//////////////
int composicion();
void particionAMedida(d_message tipoMensaje, void*mensaje, estructuraAdministrativa * particion);
bool hayParticion(d_message tipoMensaje, void *mensaje);
void* buscarParticionLibreBS(d_message tipoMensaje, void* mensaje);
int primeraParticion();
int particionMenosReferenciada();
void reemplazar (d_message tipoMensaje, void* mensaje);

////////VARIABLES GLOBALES//////////
config BROKER_CONFIG;
t_log * LOGGER_GENERAL;
t_log * LOGGER_OBLIGATORIO;
enum queueName COLAS;
t_list* CONEXIONES;
void * MEMORIA_PRINCIPAL;
t_list* ADMINISTRADOR_MEMORIA;
int CONTADOR = 0;

////////LISTA DE SUSCRIPTORES//////
t_list* SUSCRIPTORES_NEW;
t_list* SUSCRIPTORES_APPEARED;
t_list* SUSCRIPTORES_GET;
t_list* SUSCRIPTORES_CATCH;
t_list* SUSCRIPTORES_CAUGHT;
t_list* SUSCRIPTORES_LOCALIZED;


#endif /* BROKER_H_ */







