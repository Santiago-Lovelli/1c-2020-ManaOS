#include "Team.h"

int main (void){
	inicializar();
	list_add_all(EstadoNew, ENTRENADORES_TOTALES);
	list_add_all(EstadoReady, ENTRENADORES_TOTALES);
	while(objetivoTerminado() == 0){
		proceso* procesoAEjecutar = planificarSegun(TEAM_CONFIG.ALGORITMO_PLANIFICACION, EstadoReady);
	}
	finalFeliz();
	return EXIT_SUCCESS;
}

void inicializar(){
	TEAM_LOG = iniciar_log("Team");
	iniciarConfig();
	crearEntrenadores();
	crearEstados();
	iniciarVariablesDePlanificacion();
	for (int i = 0; list_get(ENTRENADORES_TOTALES, i) != NULL; i++){
		entrenador * entrenador = list_get(ENTRENADORES_TOTALES, i);
		printf("Entrenador en posicion: x = %d, y = %d \n", entrenador->posicion.x, entrenador->posicion.y);
		for (int j = 0; entrenador->pokemones[j]!=NULL; j++){
			printf("Pokemon n° %d es un: %s \n", j, entrenador->pokemones[j]);
		}
		for (int j = 0; entrenador->pokemonesObjetivo[j]!=NULL; j++){
			printf("Pokemon objetivo n° %d es un: %s \n", j, entrenador->pokemonesObjetivo[j]);
		}
	}
	//cargarEstadoNew(); //
	setObjetivoGlobal(); //
	log_info (TEAM_LOG, "TEAM OK");
	//iniciarHilos();
}

void finalFeliz(){
	//destruirObjetivoGlobal();
	//destruirEstados();
	//destruirEntrenadores();//list_destroy_and_destroy_elements(ENTRENADORES_TOTALES, entrenadorDestroy);
}

void iniciarConfig(){
	t_config* creacionConfig = config_create("/home/utnso/tp-2020-1c-ManaOS-/Team/Team.config");
	TEAM_CONFIG.POSICIONES_ENTRENADORES = config_get_array_value(creacionConfig, "POSICIONES_ENTRENADORES");
	TEAM_CONFIG.POKEMON_ENTRENADORES = config_get_array_value(creacionConfig, "POKEMON_ENTRENADORES");
	TEAM_CONFIG.OBJETIVOS_ENTRENADORES = config_get_array_value(creacionConfig, "OBJETIVOS_ENTRENADORES");
	TEAM_CONFIG.TIEMPO_RECONEXION = config_get_int_value (creacionConfig, "TIEMPO_RECONEXION");
	TEAM_CONFIG.RETARDO_CICLO_CPU = config_get_int_value (creacionConfig, "RETARDO_CICLO_CPU");
	TEAM_CONFIG.ALGORITMO_PLANIFICACION = config_get_string_value (creacionConfig, "ALGORITMO_PLANIFICACION");
	TEAM_CONFIG.QUANTUM = config_get_int_value (creacionConfig, "QUANTUM");
	TEAM_CONFIG.ESTIMACION_INICIAL = config_get_int_value (creacionConfig, "ESTIMACION_INICIAL");
	TEAM_CONFIG.IP_BROKER = config_get_string_value (creacionConfig, "IP_BROKER");
	TEAM_CONFIG.PUERTO_BROKER = config_get_int_value (creacionConfig, "PUERTO_BROKER");
	TEAM_CONFIG.LOG_FILE = config_get_string_value (creacionConfig, "LOG_FILE");
	free(creacionConfig);
}

void crearEntrenadores(){
	ENTRENADORES_TOTALES = list_create();
	for (int i = 0; TEAM_CONFIG.POSICIONES_ENTRENADORES[i] != NULL; i++){
		punto punto = crearPunto(TEAM_CONFIG.POSICIONES_ENTRENADORES[i]);
		char ** pokemones = string_split(TEAM_CONFIG.POKEMON_ENTRENADORES[i], "|");
		char ** pokemonesObjetivo = string_split(TEAM_CONFIG.OBJETIVOS_ENTRENADORES[i], "|");
		entrenador * entrenador = crearEntrenador(punto, pokemones, pokemonesObjetivo);
		list_add(ENTRENADORES_TOTALES, entrenador);
	}
}

punto crearPunto(char * posiciones){
	char ** xey = string_split(posiciones, "|");
	punto p = {
		atoi(xey[0]),
		atoi(xey[1])
	};
	return p;
}

entrenador * crearEntrenador(punto punto, char ** pokemones, char **pokemonesObjetivo){
	entrenador * newTrainer = malloc(sizeof(entrenador));
	newTrainer->posicion = punto;
	newTrainer->pokemones = pokemones;
	newTrainer->pokemonesObjetivo = pokemonesObjetivo;
	return newTrainer;
}

void crearEstados(){
	EstadoNew = list_create();
	EstadoReady = list_create();
	EstadoBlock = list_create();
	EstadoExit = list_create();
}

void iniciarVariablesDePlanificacion(){
	CLOCK = 0;
	CAMBIOS_DE_CONTEXTO_REALIZADOS = 0;
	DEADLOCKS_PRODUCIDOS = 0;
	DEADLOCKS_RESUELTOS = 0;
}


/*void iniciarHilos(){
	pthread_create(&hiloEscucha, NULL, (void*) escucharMensajes, NULL);
	log_info(TEAM_LOG, "Se creo el hilo Escucha");
	pthread_create(&hiloConexionBroker, NULL, (void*) conectarseConBroker, NULL);
	log_info(TEAM_LOG, "Se creo el hilo Conexion Broker");
}
*/

/*void escucharMensajes(){
	int servidor = iniciar_servidor(TEAM_CONFIG.IP_TEAM, TEAM_CONFIG.PUERTO_TEAM,TEAM_LOG);
	while(1){
	int cliente = esperar_cliente_con_accept(servidor, TEAM_LOG);
	log_info ("Se conecto el cliente: %i", cliente);
	}
}*/

void conectarseConBroker(){
	int cliente = conectarse_a_un_servidor(TEAM_CONFIG.IP_BROKER, TEAM_CONFIG.PUERTO_BROKER, TEAM_LOG);
	if (cliente == -1){
		//reintentarConexión(); ////////Ver como hago con TIEMPO_RECONEXION del config
	}
	else ;
		//log_info ("Conexion exitosa");
}

static void entrenadorDestroy(entrenador *self) {
	for (int i = 0; self->pokemones[i]!=NULL; i++){
		free(self->pokemones[i]);
	}
	for (int i = 0; self->pokemonesObjetivo[i]!=NULL; i++){
		free(self->pokemonesObjetivo[i]);
	}
    free(self->pokemones);
    free(self->pokemonesObjetivo);
}

int objetivoTerminado(){
	return 0;
}

////////////Funciones planificacion/////////////
proceso* planificarSegun(char* tipoPlanificacion, t_list* procesos){
	if(string_equals_ignore_case(tipoPlanificacion, "FIFO")){
		return FIFO(procesos);
	}
	if(string_equals_ignore_case(tipoPlanificacion, "RR")){
		return RR(procesos);
	}
	if(string_equals_ignore_case(tipoPlanificacion, "SJFCD")){
		return SJFCD(procesos);
	}
	if(string_equals_ignore_case(tipoPlanificacion, "SJFSD")){
		return SJFSD(procesos);
	}
	return NULL;
}
proceso* FIFO(t_list* procesos){
	printf("Holis, me llamaron? Soy FIFO");
	return list_remove(procesos, 0);
}
proceso* RR(t_list* procesos){
	printf("Holis, me llamaron? Soy RR");
}
proceso* SJFCD(t_list* procesos){
	printf("Holis, me llamaron? Soy SJFCD");
}
proceso* SJFSD(t_list* procesos){
	printf("Holis, me llamaron? Soy SJFSD");
}
unsigned long int getClockTime(){
	return CLOCK;
}

void agregarTiempo(int cantidad){
	CLOCK += cantidad;
}

void setObjetivoGlobal(){
	OBJETIVO_GLOBAL = dictionary_create();
	int j=0;
	entrenador * trainer = list_get(ENTRENADORES_TOTALES, j);
	while(trainer != NULL){
		for (int i = 0; trainer->pokemonesObjetivo[i] != NULL; i++){
			int unPokemon = (int)dictionary_get(OBJETIVO_GLOBAL, trainer->pokemonesObjetivo[i]);
			dictionary_put(OBJETIVO_GLOBAL, trainer->pokemonesObjetivo[i], unPokemon + 1);
		}
		j++;
		trainer = list_get(ENTRENADORES_TOTALES, j);
	}
	printf("Objetivo global setteado \n");
}
