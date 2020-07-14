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
#include <semaphore.h>

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

typedef struct punto{
	uint32_t posX;
	uint32_t posY;
}punto;

///////////ESTRUCTURAS PARA GUARDADO/LEVANTADO EN MEMORIA PRINCIPAL/////////////
typedef struct newEnMemoria{
	uint32_t largoDeNombre;
	char * nombrePokemon;
	uint32_t posX;
	uint32_t posY;
	uint32_t cantidad;
}newEnMemoria;

typedef struct catchEnMemoria{
	uint32_t largoDeNombre;
	char * nombrePokemon;
	uint32_t posX;
	uint32_t posY;
}catchEnMemoria;

typedef struct getEnMemoria{
	uint32_t largoDeNombre;
	char * nombrePokemon;
}getEnMemoria;

typedef struct appearedEnMemoria{
	uint32_t largoDeNombre;
	char * nombrePokemon;
	uint32_t posX;
	uint32_t posY;
}appearedEnMemoria;

typedef struct caughtEnMemoria{
	uint32_t atrapado;
}caughtEnMemoria;

typedef struct localizedEnMemoria{
	uint32_t largoDeNombre;
	char * nombrePokemon;
	uint32_t cantidadDePuntos;
	t_list* puntos;
}localizedEnMemoria;

///////FUNCIONES INICIALIZACION/////////
void Init();
void ConfigInit();
void ListsInit();
void MemoriaPrincipalInit();
void SemaphoresInit();

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
void * levantarMensaje(d_message tipoMensaje, void * lugarDeComienzo);
int contarTamanio();

//////FUNCIONES ESTRUCTURA ADMINISTRATIVA//////////
estructuraAdministrativa * guardarMensaje(d_message tipoMensaje, void * mensajeAGuardar);
void * buscarParticionLibrePara(int mensajeAGuardar);
estructuraAdministrativa* buscarEstructuraAdministrativaConID(int id);
int obtenerID();
int tamanioDeMensaje(d_message tipoMensaje, void * unMensaje);
void * levantarMensaje(d_message tipoMensaje, void * lugarDeComienzo);
void reposicionarParticionesOcupadas(t_list * listaAuxiliar);

//////////FUNCION BUDDY Y PARTICION DINAMICA//////////////
void composicion();
estructuraAdministrativa * particionAMedida(d_message tipoMensaje, void*mensaje, estructuraAdministrativa * particion);
bool hayParticion(d_message tipoMensaje, void *mensaje);
estructuraAdministrativa* buscarParticionLibreBS(d_message tipoMensaje, void* mensaje);
int primeraParticion();
int particionMenosReferenciada();
//void reemplazar (d_message tipoMensaje, void* mensaje);
//void limpiarParticion (estructuraAdministrativa * particion);

////////////FUNCIONES DESTROYER//////////////////
static void estructuraAdministrativaDestroyer(estructuraAdministrativa *self);
static void suscriptorDestroyer(int *self);

////////VARIABLES GLOBALES//////////
config BROKER_CONFIG;
t_log * LOGGER_GENERAL;
t_log * LOGGER_OBLIGATORIO;
enum queueName COLAS;
t_list* CONEXIONES;
void * MEMORIA_PRINCIPAL;
t_list* ADMINISTRADOR_MEMORIA;
int CONTADOR = 0;
int TIEMPO = 0;
int FLAG_COMPOSICION = 0;
int FLAG_COMPACTACION = 0;
int FLAG_REEMPLAZAR = 1;

////////SEMAFOROS///////////
sem_t MUTEX_CLIENTE;
sem_t MUTEX_CONTADOR;
sem_t MUTEX_MEMORIA;
sem_t MUTEX_TIEMPO;

////////LISTA DE SUSCRIPTORES//////
t_list* SUSCRIPTORES_NEW;
t_list* SUSCRIPTORES_APPEARED;
t_list* SUSCRIPTORES_GET;
t_list* SUSCRIPTORES_CATCH;
t_list* SUSCRIPTORES_CAUGHT;
t_list* SUSCRIPTORES_LOCALIZED;


#endif /* BROKER_H_ */







