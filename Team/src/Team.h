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
#include <semaphore.h>
#include "ManejoDePunterosDobles/ManejoDePunterosDobles.h"

/////////ESTRUCTURA/////////

typedef enum t_estado {
	t_NEW,
	t_READY,
	t_EXEC,
	t_BLOCKED,
	t_EXIT
} t_estado;


typedef enum t_razonBloqueo {
	t_NULL,
	t_DESOCUPADO,
	t_ESPERANDO_RESPUESTA,
	t_DEADLOCK,
	t_ESPERANDO_INTERCAMBIO
} t_razonBloqueo;

typedef struct config{
	char** POSICIONES_ENTRENADORES;
	char** POKEMON_ENTRENADORES;
	char** OBJETIVOS_ENTRENADORES;
	int TIEMPO_RECONEXION;
	int RETARDO_CICLO_CPU;
	char* ALGORITMO_PLANIFICACION;
	int QUANTUM;
	double ALPHA;
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

typedef struct t_mision {
	char* pokemon;
	punto point;
	bool esIntercambio;
	int tidObjetivo;
}t_mision;

typedef struct entrenador{
	int tid; //trainer ID jeje soy re gracioso
	punto posicion;
	char ** pokemones;
	char ** pokemonesObjetivo;
	t_estado estado;
	t_razonBloqueo razonBloqueo;
	t_mision *mision;
	int ciclosCPUEjecutados;
	int ciclosCPUEjecutadosEnTotal;
	int ciclosCPUEjeutadosAnteriormente;
	int ciclosCPUEstimados;
	sem_t semaforoDeEntrenador;
}entrenador;


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
void iniciarConfig(int argc, char *argv[]);
void inicializar(int argc, char *argv[]);
void inicializarSemaforos();
void crearEntrenadores();
void escucharMensajes();
punto crearPunto(char * posiciones); // x|y
entrenador * crearEntrenador(punto punto, char ** pokemones, char **pokemonesObjetivo);
void crearEstados();
void setObjetivoGlobal();
void cargarEstadoNew();
void iniciarVariablesDePlanificacion();

/*
 * ESTA FUNCION DADO UN ENTRENADOR, DESCUENTA LOS POKEMONS QEU ESTE
 * TENGA QUE SEAN NECESARIOS PARA NUESTRO OBJETIVO GLOBAL DE NUESTRO
 * OBJETIVO GLOBAL
 */

void descontarPokemonsActualesDeOBJGlobal(entrenador* trainer);

//////FUNCIONES FINALES/////////

/*
 * ESTA FUNCION ES LLAMADA CUANDO TEAM FINALIZA Y
 * SE OCUPA DE LLAMAR A TODAS LAS FUNCIONES NECESARIAS
 * PARA DICHO FIN
 */

void finalFeliz();

/*
 * ESTA FUNCION LOGEA TODO LO PEDIDO POR ENUNCIADO A LA
 * HORA DE FINALIZAR EL PROCESO TEAM
 */

void logearFin();

/*
 * ESTA FUNCION SE OCUPA DE LIBERAR TODA LA MEMORIA
 * QUE HAYA OCUPADO EL TEAM DURANTE SU EJECUCION
 */

void destruirTodo();

/*
 * ESTA FUNCION FINALIZA LA EJECUCION DE LOS HILOS
 */

void matarHilos();

void destruirObjetivoGlobal();

void destruirEstados();

void destruirEntrenadores();

void destruirSemaforos();

void destruirElementoGlobal();

void entrenadorDestroy(entrenador *self);

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
 * ESTA FUNCION DADO UN ENTRENADOR Y UN POKEMON SE LO SUMA
 * A SU LISTA DE POKEMONS CAPTURADOS
 */

void sumarPokemon(entrenador* trainer, char* pokemon);

/*
 * ESTA FUNCION DADO UN ENTRENADOR, EL ID DE UN ENTRENADOR OBJETIVO Y UN POKEMON,
 * REALIZA EL INTERCAMBIO MAS EFICIENTE ENTRE LOS DOS (YENDO EL POKEMON PASADO POR
 * PARAMETROS AL ENTRENADOR Y UN POKEMON SOBRANTE DEL ENTRENADOR AL ENTRENADOR OBJETIVO)
 */

void intercambiarPokemon(entrenador* trainer, int tidTrainerObjetivo, char* pokemon);

/*
 * ESTA FUNCION DADO UN POKEMON NOS DEVUELVE
 * SI ESTE SE ENCUENTRA EN NUESTRO OBJETIVO GLOBAL
 */

bool necesitoEstePokemon(char *pokemon);

/*
 * ESTA FUNCION DADO UN ENTRENADOR Y UN POKEMON
 * NOS DICE CUANTOS DE ESE POKEMON YA ESTAN EN POSESION
 * DE ESE ENTRENADOR
 */

int cuantosDeEstePokemonTengo(entrenador* trainer, char* pokemon);

/*
 * ESTA FUNCION DADO UN ENTRENADOR Y UN POKEMON
 * NOS DICE CUANTOS DE ESE POKEMON NECESITA ESE ENTRENADOR
 */

int cuantosDeEstePokemonNecesito(entrenador* trainer, char*pokemon);

/*
 * ESTA FUNCION DADO UN ENTRENADOR NOS DEVUELVE
 * TODOS LOS POKEMON POKEMON EN SU LISTA DE OBJETIVOS
 * QUE NO ESTEN EN SU LISTA DE POKEMONS ACTUAL
 */

char** quePokemonMeFalta(entrenador* trainer);

/*
 * ESTA FUNCION DADO UN ENTRENADOR NOS DEVUELVE
 * TODOS LOS POKEMON EN SU LISTA DE POKEMONES ACTUALES
 * QUE NO ESTEN EN SU LISTA DE OBJETIVOS
 */

char** quePokemonTengoDeMas(entrenador *trainer);

/*
 * ESTA FUNCION DADO UN POKEMON NOS DEVUELVE SI ESTE
 * SE ENCUENTRA EN NUESTRO OBJETIVO GLOBAL Y PERO TAMBIEN
 * CONSIDERANDO SI TENEMOS ENTRENADORES YENDO A BUSCARLO
 */

bool contandoMisionesActualesNecesitoEstePokemon(char *pokemon);

/*
 * ESTA FUNCION DADO UN POKEMON RESTA UNO DEL
 * MISMO DE LA CANTIDAD QUE NECESITAMOS EN UN
 * NUESTRO OBJETIVO GLOBAL
 */

void descontarDeObjetivoGlobal(char *pokemon);

/*
 * ESTA FUNCION DADO UN ENTRENADOR Y UNA LISTA SACA
 * AL MISMO DE LA LISTA
 */

void eliminarDeListaEntrenador(entrenador  *trainer, t_list* lista);

/*
 * ESTA FUNCION DADO EL ID DE UN ENTRENADOR, LE
 * SACA SU MISION ACTUAL DEJANDOLA EN NULL
 */

void sacarMision(int idEntrenador);

/*
 * ESTA FUNCION DADO UN ID ENTRENADOR, UN POKEMON
 * UNA POSICION Y SI ES UN INTERCAMBIO
 * LE ASIGNA ESA MISION A ESE ENTRENADOR
 */

void darMision(int idEntrenador, char* pokemon, punto point, bool esIntercambio, int tidObjetivo);

/*
 * ESTA FUNCION DADO EL ENTRENADOR PASADO POR PARAMETROS
 * CUMPLE SU MISION Y SE QUEDA A LA ESPERA DE OTRA
 */

void cumplirMision(entrenador* trainer);

/*
 * ESTA FUNCION DADO UN POKEMON, UN PUNTO Y SI ES O NO UN INTERCAMBIO
 * CREA UNA MISION Y LA RETORNA
 */

t_mision* crearMision(char *pokemon, punto point, bool esIntercambio, int tidObjetivo);

/*
 * ESTA FUNCION RETORNA SI EXISTE ALGUNA MISION ACTIVA
 */

bool hayAlgunaMision();

/*
 * ESTA FUNCION DADO EL ID DE UN  ENTRENADOR
 * Y UNA MISION YA CREADA, SE LA ASIGNA
 */

void asignarMision(int idEntrenador, t_mision* misionYaCreada);

/*
 * ESTA FUNCION DADO UN ENTRENADOR Y UNA RAZON
 * PASA EL ESTADO DE ESTE A BLOQUEADO PARA BLOQUEAR
 * A UN ENTRENADOR ES IMPORTANTE QUE SE USE ESTA
 * FUNCION
 */


void bloquearEntrenador(int idEntrenador, t_razonBloqueo razon);

/*
 * ESTA FUNCION DADO EL ID DE UN ENTRENADOR
 * (RESPECTO A LA LISTA TOTAL DE ENTRENADORES)
 * Y UN ESTADO, PASA EL ENTRENADOR A DICHO ESTADO
 * MOVIENDOLO TAMBIEN ENTRE LAS LISTAS DE ESTADOS
 */

void pasarEntrenadorAEstado(int index, t_estado estado);


/*
 * ESTA FUNCION DADO EL ENTRENADOR PASADO POR PARAMETROS
 * LO TOMA DE LA LISTA Y LO PONE AL FINAL
 */

void ponerAlFinalDeLista(entrenador *trainer, t_list *lista);

/*
 * ESTA FUNCION DADO EL ENTRENADOR PASADO POR PARAMETROS
 * INFORMA A LOS SEMAFOROS CORRESPONDIENTES QUE EL ENTRENADOR
 * HA CUMPLIDO SU MISION
 */

void avisarQueTermine(entrenador *trainer);

/*
 * ESTA FUNCION DADO UN POKEMON NOS DEVUELVE
 * LA CANTIDAD DE ENTRENADORES QUE LO TIENEN COMO
 * MISION ACTUALMENTE
 */

int cantidadDeMisiones(char *pokemon);

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

/*
 * ESTA FUNCION DADA UNA MISION LA DESTRUYE
 */

void destruirMision(t_mision *mision);

/*
 * ESTA FUNCION DADO UN POKEMON, BORRA TODAS LAS MISIONES
 * PENDIENTES DEL MISMO
 */

void borrarEstePokemonDePendientes(char *pokemon);

/*
 * ESTA FUNCION DADO UN POKEMON SE FIJA:
 * 1) SI LO NECESITAMOS Y HAY MISIONES PENDIENTES
 * MANDARA A HACERSE UNA MISION
 * 2) SI NO LO NECESITAMOS BORRA TODAS LAS
 * MISIONES PENDIENTES QUE HAYA
 */

void verPendientes(char *pokemon);

/*
 *	ESTA FUNCION DADO UNA MISION Y UN POKEMON RETORNA TRUE
 *	SI EL POKEMON OBJETIVO DE ESA MISION ES EL POKEMON PASADO
 *	POR PARAMETROS
 */

bool mismoPokemonDeMision(t_mision* pokemon1, char* pokemon2);

/*
 * ESTA FUNCION DADO UN POKEMON BUSCA ALGUNA MISION PENDIENTE
 * DEL MISMO Y SE LA ASIGNA AL ENTRENADOR DISPONIBLE MAS CERCANO
 */

void asignarMisionPendienteDePoke(char* pokemon);

////////FUNCIONES PLANIFICACION////////////


/*
 * ESTA FUNCION CREA EL HILO QUE ESTARA
 * EJECUTANDO EL ALGORITMO
 */

void crear_hilo_planificacion();

/*
 * ESTA FUNCION DADO UN TIPO DE PLANIFICACION LLAMA
 * A LA FUNCION CORRESPONDIENTE
 */

void planificarSegun(char* tipoPlanificacion);

/* ESTA FUNCION DADO UN ENTRENADOR SUMA LA CANTIDAD
 * ciclos EN SUS RAFAGAS EJECUTADAS
 */

void sumarXCiclos(entrenador *trainer, int ciclos);

/*
 * ESTA FUNCION ESPERA A QUE HAYA AL MENOS
 * UN ENTRENADOR EN ESTADO READY
 */

void esperarAlgunoEnReady(bool isDeadlock);


/*
 * ESTA FUNCION PLANIFICA LOS ENTRENADORES
 * EN ESTADO READY EN FORMA FIFO
 */

void FIFO();
void RR();
void SJFCD();
void SJFSD();
unsigned long int getClockTime();
void agregarTiempo(int cantidad);

/*
 * ESTA FUNCION DADO UN ENTRENADOR ANALIZA SU DEADLOCK
 * SI SIGUE EN DEADLOCK LO VUELVE A PONER EN BLOCKED POR RAZON
 * DEADLOCK, Y SI NO LE DA LA MISION DE TERMINAR
 */

void analizarDeadlockEspecifico(entrenador *trainer);

/*
 * ESTA FUNCION PONE EN READY CON UNA MISION
 * A UN ENTRENADOR PARA PLANIFICARLO HACIA OTRO
 */

void planificarDeadlocks();

/*
 * ESTA FUNCION DADA UNA LISTA
 * PASADA POR PARAMETROS LA ORDENA
 * DE MENOR A MAYOR EN CANTIDAD DE RAFAGAS
 * DE CPU ESTIMADAS USANDO LA FORMULA
 * DE LA MEDIA EXPONENCIAL
 */

void ordenarListaSJF(t_list *lista);

/*
 * ESTA FUNCION DADO UN ENTRENADOR PASADO POR PARAMETROS
 * LE ESTABLECE SU NUEVA ESTIMACION
 */

void establecerNuevaEstimacion(entrenador* trainer);

/*
 * ESTA FUNCION DADO UN ENTRENADOR
 * RETORNA EL VALOR DE LA NUEVA
 * ESTIMACION
 */

int nuevaEstimacion(entrenador *trainer);

/*
 * ESTA FUNCION DADOS DOS ENTRENADORES, RETORNA TRUE SI LA ESTIMACION
 * DEL ENTRENADOR 1 ES MENOR  O IGUAL QUE LA DEL ENTRENADOR 2
 */

bool entrenador1MenorEstimacionQueEntrenador2(entrenador* trainer1, entrenador* trainer2);

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
 * ESTA FUNCION RECIBE UNA CONEXION DEL GAMEBOY Y LA ATIENDE
 */

void* recibirYAtenderUnClienteGameboy(p_elementoDeHilo* elemento);

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
int CICLOS_TOTALES;
sem_t semaforoPlanifiquenme;
sem_t semaforoTermine;
//sem_t semaforoSocket;
sem_t semaforoGameboy;
sem_t semaforoCambioEstado;
sem_t semaforoConexionABroker;
sem_t semaforoMisiones;
sem_t semaforoDiccionario;
sem_t semaforoAppeared;
sem_t semaforoMovimiento;
sem_t semaforoPokemon;
sem_t semaforoGet;
sem_t semaforoSocketGameboy;
sem_t semaforoListaIDS;
sem_t semaforoConectarseServidor;

/////////VARIABLES/////////
t_config* creacionConfig;
config TEAM_CONFIG;
t_log * TEAM_LOG;
t_dictionary * OBJETIVO_GLOBAL;
t_list * ENTRENADORES_TOTALES;
t_list * POKEMONES_ATRAPADOS;
t_list * ID_QUE_NECESITO;
t_list * MISIONES_PENDIENTES;
t_list * IDs_GET;
int AUX_ID_TRAINER;
bool SEGUIR_ATENDIENDO;

///////Estados//////////
t_list* EstadoNew;
t_list* EstadoReady;
t_list* EstadoBlock;
entrenador* EstadoExec;
t_list* EstadoExit;

/////////HILOS///////////
pthread_t hiloPlanificacion;
pthread_t* servidor;
pthread_t* suscriptoAppearedPokemon;
pthread_t* suscriptoLocalizedPokemon;
pthread_t* suscriptoCaughtPokemon;
#endif /* TEAM_H_ */
