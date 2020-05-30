#ifndef TEAM_H_
#define TEAM_H_

#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/config.h>
#include <Conexiones/Conexiones.h>
#include "Serializacion/Serializacion.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "Logger/Logger.h"

//Ver como crear lista de listas
/////////ESTRUCTURA/////////
typedef struct config{
	char** POSICIONES_ENTRENADORES;
	char** POKEMON_ENTRENADORES;
	char** OBJETIVOS_ENTRENADORES;
	int TIEMPO_RECONEXION;
	int RETARDO_CICLO_CPU;
	char* ALGORITMO_PLANIFICACION;
	int QUANTUM;
	int ESTIMACION_INICIAL;
	char* IP_BROKER;
	char* PUERTO_BROKER;
	char* LOG_FILE;
	char* IP_TEAM;
	char* PUERTO_TEAM;
}config;

typedef struct punto{
	int x;
	int y;
}punto;

typedef struct entrenador{
	punto posicion;
	char ** pokemones;
	char ** pokemonesObjetivo;
}entrenador;

typedef struct proceso{
	entrenador* entrenador;
	int ciclosUsados;
}proceso;

typedef struct {
	int cliente;
	t_log* log;
} p_elementoDeHilo;

//////FUNCIONES INICIALES/////////
void iniciarConfig();
void inicializar();
void crearEntrenadores();
void escucharMensajes();
void conectarseConBroker();
punto crearPunto(char * posiciones); // x|y
entrenador * crearEntrenador(punto punto, char ** pokemones, char **pokemonesObjetivo);
void crearEstados();
void setObjetivoGlobal();
void cargarEstadoNew();
void iniciarVariablesDePlanificacion();

//////FUNCIONES FINALES/////////
void finalFeliz();
static void entrenadorDestroy(entrenador *self);

/////////FUNCIONES PROPIAS TEAMS/////////////
int objetivoTerminado();

////////FUNCIONES PLANIFICACION////////////
proceso* planificarSegun(char* tipoPlanificacion, t_list* procesos);
proceso* FIFO(t_list* procesos);
proceso* RR(t_list* procesos);
proceso* SJFCD(t_list* procesos);
proceso* SJFSD(t_list* procesos);
unsigned long int getClockTime();
void agregarTiempo(int cantidad);

////////FUNCIONES HILOS////////////
void iniciarServidorDeGameBoy(pthread_t* servidor);
void* atenderGameBoy();
void conectarmeColaDe(pthread_t* hilo, d_message colaDeSuscripcion);
void* suscribirme(d_message colaDeSuscripcion);
void* recibirYAtenderUnCliente(p_elementoDeHilo* elemento);
void atender(HeaderDelibird header, int cliente, t_log* logger);


////////VARIABLES PLANIFICACION////////////
unsigned long int CLOCK;
int CAMBIOS_DE_CONTEXTO_REALIZADOS;
int DEADLOCKS_PRODUCIDOS;
int DEADLOCKS_RESUELTOS;

/////////VARIABLES/////////
config TEAM_CONFIG;
t_log * TEAM_LOG;
t_dictionary * OBJETIVO_GLOBAL;
t_list * ENTRENADORES_TOTALES;
t_list * POKEMONES_ATRAPADOS;

///////Estados//////////
t_list* EstadoNew;
t_list* EstadoReady;
t_list* EstadoBlock;
entrenador* EstadoExec;
t_list* EstadoExit;

/////////HILOS///////////
pthread_t hiloEscucha;
pthread_t hiloConexionBroker;
#endif /* TEAM_H_ */