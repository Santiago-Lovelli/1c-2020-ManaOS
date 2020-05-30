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
	pthread_t* servidor = malloc(sizeof(pthread_t));
	iniciarServidorDeGameBoy(servidor);

	pthread_t* suscriptoAppearedPokemon = malloc(sizeof(pthread_t));
	conectarmeColaDe(suscriptoAppearedPokemon, d_APPEARED_POKEMON);

	pthread_t* suscriptoLocalizedPokemon = malloc(sizeof(pthread_t));
	conectarmeColaDe(suscriptoLocalizedPokemon, d_LOCALIZED_POKEMON);

	pthread_t* suscriptoCaughtPokemon = malloc(sizeof(pthread_t));
	conectarmeColaDe(suscriptoCaughtPokemon, d_CAUGHT_POKEMON);

	pthread_join(*servidor, NULL);
	pthread_join(*suscriptoAppearedPokemon, NULL);
	pthread_join(*suscriptoLocalizedPokemon, NULL);
	pthread_join(*suscriptoCaughtPokemon, NULL);
}

void iniciarServidorDeGameBoy(pthread_t* servidor) {
	if (pthread_create(servidor, NULL, (void*) atenderGameBoy, NULL) == 0) {
		log_info(TEAM_LOG, "::::Se creo hilo de GameBoy::::");
	} else {
		log_error(TEAM_LOG, "::::No se pudo crear el hilo de GameBoy::::");
	}
}


void* atenderGameBoy() {
	t_log* gameBoyLog = iniciar_log("GameBoy");

	int conexion = iniciar_servidor(TEAM_CONFIG.IP_TEAM, TEAM_CONFIG.PUERTO_TEAM, gameBoyLog);
	while (1) {
		int cliente = esperar_cliente_con_accept(conexion, gameBoyLog);
		log_info(gameBoyLog, "se conecto cliente: %i", cliente);
		pthread_t* dondeSeAtiende = malloc(sizeof(pthread_t));

		p_elementoDeHilo elemento;
		elemento.cliente = cliente;
		elemento.log = gameBoyLog;


		if (pthread_create(dondeSeAtiende, NULL,
				(void*) recibirYAtenderUnCliente, &elemento) == 0) {
			log_info(gameBoyLog, ":::: Se creo hilo para cliente ::::");
		} else {
			log_error(gameBoyLog,
					":::: No se pudo crear el hilo para cliente ::::");
		}
		pthread_detach(*dondeSeAtiende);
	}
}

void conectarmeColaDe(pthread_t* hilo, d_message colaDeSuscripcion) {

	if (pthread_create(hilo, NULL, (void*) suscribirme,
			(void*) colaDeSuscripcion) == 0) {
		log_info(TEAM_LOG, "::::Se creo hilo de suscripcion para: %i::::",
				colaDeSuscripcion);
	} else {
		log_error(TEAM_LOG,
				"::::No se pudo crear el hilo de suscripcion para: %i::::",
				colaDeSuscripcion);
	}

}


void* suscribirme(d_message colaDeSuscripcion) {
	int conexion;

	while (1) {
		conexion = conectarse_a_un_servidor(TEAM_CONFIG.IP_BROKER, TEAM_CONFIG.PUERTO_BROKER, TEAM_LOG);
		if (conexion == -1) {
			log_error(TEAM_LOG,
					"No se pudo conectar con el Broken a la cola de: %i\n",
					colaDeSuscripcion);
			sleep(TEAM_CONFIG.TIEMPO_RECONEXION);
		} else {
			break;
		}
	}
	Serialize_PackAndSend_SubscribeQueue(conexion, colaDeSuscripcion);

	p_elementoDeHilo* elemento;
	elemento->cliente = conexion;
	elemento->log = TEAM_LOG;

	recibirYAtenderUnCliente(elemento);
	return 0;
}

void* recibirYAtenderUnCliente(p_elementoDeHilo* elemento) {
	while (1) {
		HeaderDelibird headerRecibido = Serialize_RecieveHeader(elemento->cliente);
		if (headerRecibido.tipoMensaje == -1) {
			log_error(elemento->log, "Se desconecto el GameBoy\n");
			break;
		}
		atender(headerRecibido, elemento->cliente, elemento->log);
	}
	return 0;
}


void atender(HeaderDelibird header, int cliente, t_log* logger) {
	//es el atender del gamecard, ahora hay que tunearlo para que atienda el team

	switch (header.tipoMensaje) {
	case d_APPEARED_POKEMON:
		;
		log_info(logger, "Llego un appeared pokemon");
		void* packAppearedPokemon = Serialize_ReceiveAndUnpack(cliente,
				header.tamanioMensaje);
		uint32_t idMensajeAppeared, posicionAppearedX, posicionAppearedY;
		char *AppearedNombrePokemon;
		Serialize_Unpack_AppearedPokemon(packAppearedPokemon, &idMensajeAppeared,
				&AppearedNombrePokemon, &posicionAppearedX, &posicionAppearedY);
		log_info(logger,
				"Me llego mensaje de %i. Id: %i, Pkm: %s, x: %i, y: %i, cant: %i\n",
				header.tipoMensaje, idMensajeAppeared, AppearedNombrePokemon,
				posicionAppearedX, posicionAppearedY);

		//HACER APPEARED

		free(packAppearedPokemon);
		break;
	case d_LOCALIZED_POKEMON:
		;
		//SERIALIZACION PENDIENTE

			/*log_info(logger, "Llego un catch pokemon");

			void* packCatchPokemon = Serialize_ReceiveAndUnpack(cliente,
					header.tamanioMensaje);
			uint32_t idMensajeCatch, posicionCatchX, posicionCatchY;
			char *catchNombrePokemon;
			Serialize_Unpack_CatchPokemon(packCatchPokemon, &idMensajeCatch,
					&catchNombrePokemon, &posicionCatchX, &posicionCatchY);
			log_info(logger,
					"Me llego mensaje de %i. Id: %i, Pkm: %s, x: %i, y: %i\n",
					header.tipoMensaje, idMensajeCatch, catchNombrePokemon,
					posicionCatchX, posicionCatchY);
			// Se hace lo necesario
			free(packCatchPokemon);*/
		break;
	case d_CAUGHT_POKEMON:
		;
		log_info(logger, "Llego un caught pokemon");

		void* packCaughtPokemon = Serialize_ReceiveAndUnpack(cliente,
				header.tamanioMensaje);
		uint32_t idMensajeCaught, resultadoCaught;
		Serialize_Unpack_CaughtPokemon(packCaughtPokemon, &idMensajeCaught,
				&resultadoCaught);
		log_info(logger, "Me llego mensaje de %i. Id: %i, Pkm: %s\n",
				header.tipoMensaje, idMensajeCaught, resultadoCaught);

		// HACER CAUGHT

		free(packCaughtPokemon);
		break;
	default:
		log_error(logger, "Mensaje no entendido: %i\n", header);
		void* packBasura = Serialize_ReceiveAndUnpack(cliente,
				header.tamanioMensaje);
		free(packBasura);
		break;
	}
}

void finalFeliz(){
	//destruirObjetivoGlobal();
	//destruirEstados();
	//destruirEntrenadores();//list_destroy_and_destroy_elements(ENTRENADORES_TOTALES, entrenadorDestroy);
}

void iniciarConfig(){
	t_config* creacionConfig = config_create("../Team.config");
	TEAM_CONFIG.POSICIONES_ENTRENADORES = config_get_array_value(creacionConfig, "POSICIONES_ENTRENADORES");
	TEAM_CONFIG.POKEMON_ENTRENADORES = config_get_array_value(creacionConfig, "POKEMON_ENTRENADORES");
	TEAM_CONFIG.OBJETIVOS_ENTRENADORES = config_get_array_value(creacionConfig, "OBJETIVOS_ENTRENADORES");
	TEAM_CONFIG.TIEMPO_RECONEXION = config_get_int_value (creacionConfig, "TIEMPO_RECONEXION");
	TEAM_CONFIG.RETARDO_CICLO_CPU = config_get_int_value (creacionConfig, "RETARDO_CICLO_CPU");
	TEAM_CONFIG.ALGORITMO_PLANIFICACION = config_get_string_value (creacionConfig, "ALGORITMO_PLANIFICACION");
	TEAM_CONFIG.QUANTUM = config_get_int_value (creacionConfig, "QUANTUM");
	TEAM_CONFIG.ESTIMACION_INICIAL = config_get_int_value (creacionConfig, "ESTIMACION_INICIAL");
	TEAM_CONFIG.IP_BROKER = config_get_string_value (creacionConfig, "IP_BROKER");
	TEAM_CONFIG.PUERTO_BROKER = config_get_string_value (creacionConfig, "PUERTO_BROKER");
	TEAM_CONFIG.LOG_FILE = config_get_string_value (creacionConfig, "LOG_FILE");
	TEAM_CONFIG.IP_TEAM = config_get_string_value(creacionConfig, "IP_TEAM");
	TEAM_CONFIG.PUERTO_TEAM = config_get_string_value(creacionConfig, "PUERTO_TEAM");
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
