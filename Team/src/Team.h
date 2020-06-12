#ifndef TEAM_H_
#define TEAM_H_

#include <sys/socket.h>
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

typedef enum t_estado {
	t_NEW,
	t_READY,
	t_EXEC,
	t_BLOCKED,
	t_EXIT
} t_estado;

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
	t_estado estado;
}entrenador;

typedef struct proceso{
	entrenador* entrenador;
	int ciclosUsados;
}proceso;

typedef struct {
	int cliente;
	t_log* log;
} p_elementoDeHilo;

typedef struct objetoID_QUE_NECESITO{
	int idMensaje;
	int idEntrenador;
	char *pokemon;
}objetoID_QUE_NECESITO;


//////FUNCIONES INICIALES/////////
void iniciarConfig();
void inicializar();
void crearEntrenadores();
void escucharMensajes();
punto crearPunto(char * posiciones); // x|y
entrenador * crearEntrenador(punto punto, char ** pokemones, char **pokemonesObjetivo);
void crearEstados();
void setObjetivoGlobal();
void cargarEstadoNew();
void iniciarVariablesDePlanificacion();

//////FUNCIONES FINALES/////////
void finalFeliz();
static void entrenadorDestroy(entrenador *self);

/*
 * FUNCION DE CONTROL PARA IDENTIFICAR SI
 * EL MODULO HA TERMINADO SU FUNCION, DARA TRUE
 * SI CADA ENTRENADOR CUMPLIO SU OBJETIVO Y ADEMAS
 * SE CUMPLIO EL OBJETIVO GLOBAL
 */

bool teamCumplioSuObjetivo();

/*
 * FUNCION DE CONTROL PARA IDENTIFICAR SI
 * EL MODULO HA TERMINADO UNA DE SUS FUNCIONES, DARA TRUE
 * SI SE CONSIGUIERON TODOS LOS POKEMONS DEL OBJETIVO GLOBAL
 */

bool objetivoGlobalCumplido();

/*
 * FUNCION DE CONTROL PARA IDENTIFICAR SI
 * EL MODULO HA TERMINADO UNA DE SUS FUNCIONES, DARA TRUE
 * SI TODOS LOS ENTRENADORES CUMPLIERON SUS OBJETIVOS
 */

bool todosLosEntrenadoresCumplieronObjetivo();

/*
 * FUNCION DE CONTROL PARA IDENTIFICAR SI
 * EL MODULO HA TERMINADO UNA DE SUS FUNCIONES, DARA TRUE
 * SI El ENTRENADOR CUMPLIO SU OBJETIVO
 */

bool entrenadorCumplioObjetivo(entrenador* trainer);

/*
 * FUNCION QUE DADOS DOS POKEMON RETORNA TRUE SI SON IGUALES
 */

bool sonLosMismosPokemon(char **pokemons1, char **pokemons2);

/////////FUNCIONES PROPIAS TEAMS/////////////

/*
 * ESTA FUNCION DADO UN ENTRENADOR RETORNA TRUE
 * SI ESTE ESTA DISPONIBLE, ES DECIR, NO ESTA EN EXIT
 * Y AUN PUEDE ATRAPAR POKEMONS
 */

bool entrenadorEstaDisponible(entrenador* entrenadorAUX);

/*
 * ESTA FUNCION DADO UN PUNTO DEVUELVE EL
 * ID (CORRESPONDIENTE A LA LISTA DE ENTRENADORES)
 * DEL ENTRENADOR MAS CERCANO
 * RETORNA -1 SI NO HAY NINGUNO DISPONIBLE
 */

int entrenadorMasCercanoDisponible(punto point);

/*
 * ESTA FUNCION DADA DOS PUNTOS NOS DEVUELVE
 * LA DIFERENCIA ENTRE ELLOS
 */

int diferenciaEntrePuntos(punto origen, punto destino);

/*
 * ESTA FUNCION DADO UN POKEMON NOS DEVUELVE
 * SI ESTE SE ENCUENTRA EN NUESTRO OBJETIVO GLOBAL
 */

bool necesitoEstePokemon(char *pokemon);

/*
 * ESTA FUNCION DADO UN POKEMON RESTA UNO DEL
 * MISMO DE LA CANTIDAD QUE NECESITAMOS EN UN
 * NUESTRO OBJETIVO GLOBAL
 */

void descontarDeObjetivoGlobal(char *pokemon);

/*
 * ESTA FUNCION DADO EL ID DE UN ENTRENADOR
 * (RESPECTO A LA LISTA TOTAL DE ENTRENADORES)
 * Y UN ESTADO, PASA EL ENTRENADOR A DICHO ESTADO
 * MOVIENDOLO TAMBIEN ENTRE LAS LISTAS DE ESTADOS
 */

void pasarEntrenadorAEstado(int index, t_estado estado);

/*
 * ESTA FUNCION DADOS DOS ENTRENADORES DEVUELVE
 * TRUE SI SE ENCUENTRAN EN LA MISMA POSICION
 */

bool mismaPosicion(entrenador* e1, entrenador* e2);

/*
 * ESTA FUNCION DADOS UN ENTRENADOR Y UN PUNTO
 * DEVUELVE TRUE SI SE ENCUENTRAN EN LA MISMA
 * POSICION
 */

bool mismaPosicion2(entrenador* e1, punto e2);

/*
 * ESTA FUNCION DADOS DOS PUNTOS, ACERCA EL PUNTO1
 * HACIA EL PUNTO 2 MOVIENDOSE DE A UN ESPACIO A LA
 * VEZ PRIMERO SOBRE EL EJE X Y DESPUES SOBRE EL EJE Y
 */

bool acercar(int *punto1, int punto2);

/*
 * ESTA FUNCION DADOS UN ENTRENADOR Y UN PUNTO
 * ACERCA AL ENTRENADOR  HACIA EL PUNTO 2 MOVIENDOSE
 *  DE A UN ESPACIO A LA VEZ PRIMERO SOBRE EL EJE X
 *   Y DESPUES SOBRE EL EJE Y
 */

bool moveHacia(entrenador* e1, punto destino);

/*
 * ESTA FUNCION DADO UN ENTRENADOR LO SACA DE
 * LA LISTA CORRESPONDIENTE A SU ESTADO ACTUAL
 */

void sacarEntrenadorDeEstadoActual(entrenador* trainer);

/*
 * ESTA LISTA DADAS DOS TUPLAS DE ID ENTRENADOR, LAS COMPARA
 */

bool comparadorIDs(objetoID_QUE_NECESITO *tupla1, objetoID_QUE_NECESITO *tupla2);

/*
 * ESTA FUNCION DADO UN ID NOS DICE SI ES UN ID QUE ESTAMOS
 * ESPERANDO RESPUESTA
 */

bool necesitoEsteID(int id);

/*
 * Esta funcion dado una posicion de X|Y y un pokemon, planifica
 * al entrenador mas cercano a capturarlo
 */


void hacerAppeared(char* pokemon, int posicionAppearedX, int posicionAppearedY, t_log* logger);

/*
 * ESTA FUNCION DADO UN ID DE MENSAJE CORRELATIVO Y UN RESULTADO
 * SE ENCARGA DE AVISARLE AL ENTRENADOR EL RESULTADO DE SU CAPTURA
 */

void hacerCaught(int idMensajeCaught, int resultadoCaught);

////////FUNCIONES PLANIFICACION////////////
proceso* planificarSegun(char* tipoPlanificacion, t_list* procesos);
proceso* FIFO(t_list* procesos);
proceso* RR(t_list* procesos);
proceso* SJFCD(t_list* procesos);
proceso* SJFSD(t_list* procesos);
unsigned long int getClockTime();
void agregarTiempo(int cantidad);

////////FUNCIONES HILOS////////////

/*
 * FUNCION PARA CONECTARSE A BROKER
 * EN CASO DE QUE LA CONEXION FALLA REINTENTA
 * CADA UNA CANTIDAD X DE SEGUNDOS DEFINIDA POR
 * ARCHIVO DE CONFIGURACION
 */

int iniciarConexionABroker();

/*
 * FUNCION PARA ATENDER EL GAMEBOY
 */

void iniciarServidorDeGameBoy(pthread_t* servidor);

/*
 * FUNCION PARA ATENDER EL GAMEBOY
 */

void* atenderGameBoy();

/*
 * ESTA FUNCION SE CONECTA A UNA COLA DEL BROKER
 */

void conectarmeColaDe(pthread_t* hilo, d_message colaDeSuscripcion);

/*
 * ESTA FUNCION DADA UNA COLA DE SUSCRIPCION, MANDA
 * EL CORRESPONDIENTE MENSAJE AL BROKER
 */

void* suscribirme(d_message colaDeSuscripcion);

/*
 * ESTA FUNCION RECIBE UNA CONEXION Y LA ATIENDE
 */

void* recibirYAtenderUnCliente(p_elementoDeHilo* elemento);

/*
 * FUNCION PRINCIPAL DE TEAM, CUANDO NOS LLEGA UN MENSAJE POR SOCKET
 * ESTA FUNCION LO DIRIJE HACIA LA FUNCION ADECUADA
 */

void atender(HeaderDelibird header, int cliente, t_log* logger);

/*
 * ESTA FUNCION DADO UN HEADER DE MENSAJE ACK, RECIBE EL ID
 * ENVIADO POR EL BROKER Y LO DEVUELVE
 */

uint32_t recibirResponse(int conexion, HeaderDelibird headerACK);

/*
 * ESTA FUNCION LE ENVIA UN CATCH DEL POKEMON CORRESPONDIENTE AL BROKER,
 * RECIBE EL ID CORRESPONDIENTE Y LO AGREGA A LA LISTA DE IDS DESEADOS
 */

void enviarCatchPokemonYRecibirResponse(char *pokemon, int posX, int posY, int idEntrenadorQueMandaCatch);

/*
 * ESTA FUNCION LE ENVIA UN GET POKEMON AL BROKER Y ESPERA SU RESPUESTAS
 */

void enviarGetPokemonYRecibirResponse(char *pokemon, void* value);

/*
 * ESTA FUNCION LE ENVIA UN GET POKEMON AL BROKER
 * POR CADA POKEMON PRESENTE NE NUESTRO OBJETIVO
 * GLOBAL
 */

void enviarGetXCadaPokemonObjetivo();


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
t_list * ID_QUE_NECESITO;

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
