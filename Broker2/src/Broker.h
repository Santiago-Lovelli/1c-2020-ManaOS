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
#include <commons/txt.h>
#include <signal.h>
#include <commons/temporal.h>

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
    char* DUMP_FILE;
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
	char * tiempo;
	char * ultimaReferencia;
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
void DumpFileInit();

//////FUNCIONES DE CONEXION//////////
void EsperarClientes();
void* AtenderCliente(int* a);
void ActuarAnteMensaje();
void * cargarMensajeAGuardar(d_message tipoMensaje, void *paquete, uint32_t * id);

//////FUNCIONES VARIAS//////////
void suscribir (uint32_t variable, int cliente);
mensajeConID agregarIDMensaje (void* paquete);
void actualizarEnviadosPorID(int id, int socketCliente);
void * levantarMensaje(d_message tipoMensaje, void * lugarDeComienzo);
int contarTamanio();
bool primerFechaEsAnterior(char* unaFecha, char* otraFecha);
int posicionALog(void* unaPosicion);
int tratarMensaje (d_message tipoMensaje, void *paquete);
void * leerInfoYActualizarUsoPorID(int id);
t_list * tomarLosMensajes (d_message tipoMensaje);
void enviarVariosMensajes(int * cliente, d_message tipoMensaje);
void actualizarRecibidosPorID(int id, int socketCliente);
void enviarUnMensaje (void* mensaje, d_message tipoMensaje, estructuraAdministrativa * resultado, t_list * lista, uint32_t id);
t_list* suscriptoresPara(d_message tipoDeMensaje);
bool sirveCompactar(int tamanioMensaje);
void enviarACK(int cliente, int ID);
estructuraAdministrativa * newParticion();
void limpiarParticion(estructuraAdministrativa * unaParticion);

//////FUNCIONES ESTRUCTURA ADMINISTRATIVA//////////
estructuraAdministrativa * guardarMensaje(d_message tipoMensaje, void * mensajeAGuardar);
estructuraAdministrativa* buscarEstructuraAdministrativaConID(int id);
int obtenerID();
int tamanioDeMensaje(d_message tipoMensaje, void * unMensaje);
void * levantarMensaje(d_message tipoMensaje, void * lugarDeComienzo);
void reposicionarParticionesOcupadas(t_list * listaAuxiliar);
void dump();
bool noPuedoReemplazarMas();
void guardarMensajeEnMemoria(d_message tipoMensaje, void * mensaje, void * lugarDeComienzo);

//////////FUNCION BUDDY Y PARTICION DINAMICA//////////////
void composicion();
estructuraAdministrativa * particionAMedida(d_message tipoMensaje, void*mensaje, estructuraAdministrativa * particion);
bool hayParticion(d_message tipoMensaje, void *mensaje);
estructuraAdministrativa* buscarParticionLibre(d_message tipoMensaje, void* mensaje);
int primeraParticion();
int particionMenosReferenciada();
void compactacion();
int reemplazar ();
//void limpiarParticion (estructuraAdministrativa * particion);

////////////FUNCIONES DESTROYER//////////////////
static void estructuraAdministrativaDestroyer(estructuraAdministrativa *self);
static void suscriptorDestroyer(int *self);
static void estructuraAdministrativaDestroyerSinDestruirListas(estructuraAdministrativa *self);
void destruirTodo();
void limpiarSemaforos();

////////VARIABLES GLOBALES//////////
config BROKER_CONFIG;
t_log * LOGGER_OBLIGATORIO;
void * MEMORIA_PRINCIPAL;
t_list* ADMINISTRADOR_MEMORIA;
int CONTADOR = 0;
int FLAG_COMPOSICION = 0;
int FLAG_REEMPLAZAR = 1;
int FLAG_COMPACTACION = 1;
char* nombresColas[] = {"NEW_POKEMON", "CATCH_POKEMON", "GET_POKEMON", "APPEARED_POKEMON", "CAUGHT_POKEMON", "LOCALIZED_POKEMON"};
int BUSQUEDAS_FALLIDAS = 0;

////////SEMAFOROS///////////
sem_t MUTEX_CLIENTE;
sem_t MUTEX_CONTADOR;
sem_t MUTEX_MEMORIA;
sem_t MUTEX_LISTA;
sem_t MUTEX_SOCKET;
sem_t MUTEX_REEMPLAZAR;
sem_t MUTEX_COMPACTACION;
sem_t MUTEX_COMPOSICION;
sem_t MUTEX_BUSQUEDA;
sem_t MUTEX_LEERBUSQUEDA;
sem_t MUTEX_LEERREEMPLAZAR;
sem_t MUTEX_LEERCOMPACTACION;
sem_t MUTEX_LEERCOMPOSICION;

////////LISTA DE SUSCRIPTORES//////
t_list* SUSCRIPTORES_NEW;
t_list* SUSCRIPTORES_APPEARED;
t_list* SUSCRIPTORES_GET;
t_list* SUSCRIPTORES_CATCH;
t_list* SUSCRIPTORES_CAUGHT;
t_list* SUSCRIPTORES_LOCALIZED;

void funcionParaVerMemoria();

#endif /* BROKER_H_ */
