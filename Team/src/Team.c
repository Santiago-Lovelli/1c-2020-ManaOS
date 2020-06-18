#include "Team.h"

//TODO Un entrenador arranca con pokemons ya atrapados, corregir.

int main (void){
	inicializar();
	list_add_all(EstadoNew, ENTRENADORES_TOTALES);
	list_add_all(EstadoReady, ENTRENADORES_TOTALES);
//	while(objetivoTerminado() == 0){
//		proceso* procesoAEjecutar = planificarSegun(TEAM_CONFIG.ALGORITMO_PLANIFICACION, EstadoReady);
//	}
	finalFeliz();
	return EXIT_SUCCESS;
}

void crear_hilo_planificacion(){
	pthread_create(&hiloPlanificacion, NULL, planificarSegun, TEAM_CONFIG.ALGORITMO_PLANIFICACION);
	pthread_detach(hiloPlanificacion);
}

void inicializar(){
	TEAM_LOG = iniciar_log("Team");
	iniciarConfig();
	crearEstados();
	crearEntrenadores();
	iniciarVariablesDePlanificacion();
	ID_QUE_NECESITO = list_create();
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
	setObjetivoGlobal();
	log_info (TEAM_LOG, "TEAM OK");
	//iniciarHilos();

	crear_hilo_planificacion();

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

int iniciarConexionABroker(){
	int conexion;
	while (1) {
		conexion = conectarse_a_un_servidor(TEAM_CONFIG.IP_BROKER, TEAM_CONFIG.PUERTO_BROKER, TEAM_LOG);
		if (conexion == -1) {
			log_error(TEAM_LOG,
					"No se pudo conectar con el Broker");
			sleep(TEAM_CONFIG.TIEMPO_RECONEXION);
		} else {
			break;
		}
	}
	return conexion;
}

uint32_t recibirResponse(int conexion, HeaderDelibird headerACK){
	if(headerACK.tipoMensaje == d_ACK)
	{
		void* packACK = Serialize_ReceiveAndUnpack(conexion, headerACK.tamanioMensaje);
		uint32_t id = Serialize_Unpack_ACK(packACK);
		free(packACK);
		//cerrar conexion;
		close(conexion);
		return id;
	}
	else
		return -1; //codigo de error
}

void sumarPokemon(entrenador* trainer, char* pokemon){
	int index = damePosicionFinalDoblePuntero(trainer->pokemones);
	trainer->pokemones[index+1] = pokemon;
}

void enviarCatchPokemonYRecibirResponse(char *pokemon, int posX, int posY, int idEntrenadorQueMandaCatch){
	int conexion = conectarse_a_un_servidor(TEAM_CONFIG.IP_BROKER, TEAM_CONFIG.PUERTO_BROKER, TEAM_LOG);
	if(conexion == -1){
		log_error(TEAM_LOG,"No se pudo conectar con el Broker");
		entrenador *trainer = list_get(ENTRENADORES_TOTALES, idEntrenadorQueMandaCatch);
		sumarPokemon(trainer,pokemon);
		descontarDeObjetivoGlobal(pokemon);
		trainer->razonBloqueo = t_DESOCUPADO;
		sacarMision(idEntrenadorQueMandaCatch);
		//comportamiento Default es asumir que el pokemon pudo atraparse
	}
	else{
		Serialize_PackAndSend_CATCH_POKEMON_NoID(conexion, pokemon, posX, posY);
		HeaderDelibird headerACK = Serialize_RecieveHeader(conexion);
		int idEsperado = recibirResponse(conexion, headerACK);
		objetoID_QUE_NECESITO *objetoID = malloc(2*sizeof(int) + strlen(pokemon)+1);
		objetoID->idMensaje = idEsperado;
		objetoID->idEntrenador = idEntrenadorQueMandaCatch;
		objetoID->pokemon = pokemon;
		bloquearEntrenador(objetoID->idEntrenador, t_ESPERANDO_RESPUESTA);
		list_add(ID_QUE_NECESITO, objetoID);
	}
}

void enviarGetPokemonYRecibirResponse(char *pokemon, void* value){
	int conexion = conectarse_a_un_servidor(TEAM_CONFIG.IP_BROKER, TEAM_CONFIG.PUERTO_BROKER, TEAM_LOG);
	if(conexion == -1){
		log_error(TEAM_LOG,"No se pudo conectar con el Broker");
		return; //comportamiento Default es asumir que el pokemon no esta
	}
	Serialize_PackAndSend_GET_POKEMON_NoID(conexion,pokemon);
	HeaderDelibird headerACK = Serialize_RecieveHeader(conexion);
	recibirResponse(conexion, headerACK); //NO ESTOY USANDO ESTE DATO

}

void enviarGetXCadaPokemonObjetivo(){
	dictionary_iterator(OBJETIVO_GLOBAL, (void*)enviarGetPokemonYRecibirResponse);
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
	int conexion = iniciarConexionABroker();
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
		void* packAppearedPokemon = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
		uint32_t posicionAppearedX, posicionAppearedY;
		char *AppearedNombrePokemon;
		Serialize_Unpack_AppearedPokemon_NoID(packAppearedPokemon, &AppearedNombrePokemon, &posicionAppearedX, &posicionAppearedY);
		log_info(logger, "Me llego mensaje de %i. Pkm: %s, x: %i, y: %i\n", header.tipoMensaje, AppearedNombrePokemon, posicionAppearedX, posicionAppearedY);

		if(necesitoEstePokemon(AppearedNombrePokemon)){
			printf("Necesito este pokemon!!! \n ");
			hacerAppeared(AppearedNombrePokemon,posicionAppearedX,posicionAppearedY,logger);
		}
		else{ printf("No necesito este pokemon!!! \n "); }
		free(packAppearedPokemon);
		break;
	case d_LOCALIZED_POKEMON:
		;
		//SERIALIZACION PENDIENTE

		break;

	case d_CAUGHT_POKEMON:
		;
		log_info(logger, "Llego un Caught Pokemon");
		//recibimos el resto del paquete
		void* packCaughtPokemon = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
		//Con estas dos variables desempaquetamos el paquete
		uint32_t idMensajeCaught, resultadoCaught;
		Serialize_Unpack_CaughtPokemon(packCaughtPokemon, &idMensajeCaught, &resultadoCaught);
		log_info(logger, "Me llego mensaje de %i. Id: %i, Result: %i\n", header.tipoMensaje, idMensajeCaught, resultadoCaught);

		if(necesitoEsteID(idMensajeCaught)){
			hacerCaught(idMensajeCaught,resultadoCaught);
		}

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

void hacerAppeared(char* pokemon, int posicionAppearedX, int posicionAppearedY, t_log* logger){
	punto posicionPoke;
	posicionPoke.x = posicionAppearedX;
	posicionPoke.y = posicionAppearedY;
	int idEntrenador = entrenadorMasCercanoDisponible(posicionPoke);
	if(idEntrenador == -1){
		log_error(logger, "No hay entrenadores disponibles");
		return;
	}
	darMision(idEntrenador,pokemon,posicionPoke,false);
	calcularRafagasCPUAEjecutar(idEntrenador);
	pasarEntrenadorAEstado(idEntrenador, t_READY);
}

void hacerCaught(int idMensajeCaught, int resultadoCaught){
	objetoID_QUE_NECESITO *objetoID = malloc(sizeof(objetoID_QUE_NECESITO));
	objetoID->idMensaje = idMensajeCaught;
	int index = list_get_index(ID_QUE_NECESITO, objetoID, (void*)comparadorIDs);
	if(resultadoCaught){
		free(objetoID);
		objetoID = list_get(ID_QUE_NECESITO, index);
		entrenador *trainer = list_get(ENTRENADORES_TOTALES, objetoID->idEntrenador);
		list_remove(ID_QUE_NECESITO, index);
		sumarPokemon(trainer,objetoID->pokemon);
		descontarDeObjetivoGlobal(objetoID->pokemon);
		trainer->razonBloqueo = t_DESOCUPADO;
		sacarMision(objetoID->idEntrenador);
		free(objetoID);
	}
	else{
		free(objetoID);
		objetoID = list_get(ID_QUE_NECESITO, index);
		list_remove(ID_QUE_NECESITO, index);
		bloquearEntrenador(objetoID->idEntrenador, t_DESOCUPADO);
		sacarMision(objetoID->idEntrenador);
		free(objetoID);
	}
}

bool entrenadorEstaDisponible(entrenador* entrenadorAUX){
	return (entrenadorAUX->estado != t_EXIT) &&
		   (entrenadorAUX->estado != t_READY) &&
		   (list_size(entrenadorAUX->pokemones) != list_size(entrenadorAUX->pokemonesObjetivo)) &&
		   (entrenadorAUX->razonBloqueo != t_ESPERANDO_RESPUESTA) &&
		   (entrenadorAUX->mision == NULL);

}

int entrenadorMasCercanoDisponible(punto point){
	int index = -1;
	int distanciaMinima = 0;
	int distanciaAUX = 0;
	entrenador *entrenadorAUX;

	entrenadorAUX = (entrenador*)list_get(ENTRENADORES_TOTALES,0);
	distanciaMinima = diferenciaEntrePuntos( entrenadorAUX->posicion , point);
	if( entrenadorEstaDisponible(entrenadorAUX) )
		index = 0;

	for(int j = 1; j < list_size(ENTRENADORES_TOTALES); j=j+1){
		entrenadorAUX = (entrenador*)list_get(ENTRENADORES_TOTALES,j);
		distanciaAUX = diferenciaEntrePuntos( entrenadorAUX->posicion , point);
		if( (distanciaAUX < distanciaMinima) && entrenadorEstaDisponible(entrenadorAUX)){
			distanciaMinima = distanciaAUX;
			index = j;
		}
	}

	return index;
}

bool mismaPosicion(entrenador* e1, entrenador* e2){
	return ((e1->posicion.x == e2->posicion.x) && (e1->posicion.y == e2->posicion.y));
}

bool mismaPosicion2(entrenador* e1, punto e2){
	return ((e1->posicion.x == e2.x) && (e1->posicion.y == e2.y));
}

bool acercar(int *punto1, int punto2){
	if(*punto1<punto2){
		*punto1 = *punto1 + 1;
		return true;
	}
	if(*punto1>punto2){
		*punto1 = *punto1 - 1;
		return true;
	}
	return false;
}

bool moveHacia(entrenador* e1, punto destino){
	if(mismaPosicion2(e1,destino))
		return false;
	if(!acercar(&e1->posicion.x, destino.x))
		acercar(&e1->posicion.y, destino.y);
	return true;
}


void eliminarDeListaIndex(int index, t_list* lista){
	if(index != -1)
		list_remove(lista,index);
	else
		printf("Indice Invalido");
}

void bloquearEntrenador(int idEntrenador, t_razonBloqueo razon){
	entrenador *trainer = list_get(ENTRENADORES_TOTALES, idEntrenador);
	trainer->razonBloqueo = razon;
	pasarEntrenadorAEstado(idEntrenador, t_BLOCKED);
}

void sacarEntrenadorDeEstadoActual(entrenador* trainer){
	int index=0;
	trainer->razonBloqueo = t_NULL;
	switch(trainer->estado){
	case t_NEW:;
		eliminarDeListaIndex(trainer->tid,EstadoNew);
		break;
	case t_READY:;
		eliminarDeListaIndex(trainer->tid,EstadoReady);
		break;
	case t_BLOCKED:;
		eliminarDeListaIndex(trainer->tid,EstadoBlock);
		break;
	case t_EXEC:;
		if(mismaPosicion(EstadoExec, trainer))
			EstadoExec = NULL;
		else
			printf("Este entrenador NO esta en exec");
		break;
	case t_EXIT:;
		eliminarDeListaIndex(trainer->tid,EstadoExit);
		break;
	default:;
		printf("Estado invalido");
		break;
	}
}

void pasarEntrenadorAEstado(int index, t_estado estado){
	entrenador *entrenadorAUX = (entrenador*)list_get(ENTRENADORES_TOTALES,index);
	sacarEntrenadorDeEstadoActual(entrenadorAUX);
	switch(estado){
	case t_NEW:;
		entrenadorAUX->estado = t_NEW;
		list_add(EstadoNew, entrenadorAUX);
		break;
	case t_READY:;
		entrenadorAUX->estado = t_READY;
		list_add(EstadoReady, entrenadorAUX);
		break;
	case t_BLOCKED:;
		entrenadorAUX->estado = t_BLOCKED;
		list_add(EstadoBlock, entrenadorAUX);
		break;
	case t_EXEC:;
		entrenadorAUX->estado = t_EXEC;
		EstadoExec = entrenadorAUX;
		break;
	case t_EXIT:;
		entrenadorAUX->estado = t_EXIT;
		list_add(EstadoExit, entrenadorAUX);
		break;
	default:;
		printf("Estado invalido");
		break;
	}

}

void sacarMision(int idEntrenador){
	entrenador *trainer = list_get(ENTRENADORES_TOTALES,idEntrenador);
	free(trainer->mision);
	trainer->mision = NULL;
}

void darMision(int idEntrenador, char* pokemon, punto point, bool esIntercambio){
	entrenador *trainer = list_get(ENTRENADORES_TOTALES,idEntrenador);
	t_mision *mision = malloc(sizeof(punto) + strlen(pokemon) +1 + sizeof(bool));
	mision->point = point;
	mision->pokemon = pokemon;
	mision->esIntercambio = esIntercambio;
	trainer->mision = mision;

}

void sumarXCiclos(entrenador *trainer, int ciclos){
	trainer->ciclosCPUEjecutados = trainer->ciclosCPUEjecutados+ciclos;
	CICLOS_TOTALES = CICLOS_TOTALES + trainer->ciclosCPUEjecutados;
}

void cumplirMision(entrenador* trainer){
	printf("Se creo el hilo del entrenador %i \n", trainer->tid);
	while( !entrenadorCumplioObjetivo(trainer) ){
		if(trainer->mision != NULL){
			sem_wait(&(trainer->semaforoDeEntrenador));
			printf("Hola soy el entrenador %i \n", trainer->tid);
			printf("Mi posicion actual es: \n x = %i \n y = %i \n", trainer->posicion.x,trainer->posicion.y);
			while( moveHacia(trainer, trainer->mision->point) ){
				printf("Mi posicion actual es: \n x = %i \n y = %i \n", trainer->posicion.x,trainer->posicion.y);
				sumarXCiclos(trainer,1);
				sleep(TEAM_CONFIG.RETARDO_CICLO_CPU);
			}
			if(trainer->mision->esIntercambio){
				//TODO
				sumarXCiclos(trainer,5);
				sleep(5*TEAM_CONFIG.RETARDO_CICLO_CPU);
			}
			else{
				printf("Le voy a tirar una pokebola a %s \n", trainer->mision->pokemon);
				sumarXCiclos(trainer,1);
				enviarCatchPokemonYRecibirResponse( trainer->mision->pokemon, trainer->mision->point.x, trainer->mision->point.x, trainer->tid);
				sleep(TEAM_CONFIG.RETARDO_CICLO_CPU);
			}
			sem_post(&semaforoTermine);
		}
	}
}

int diferenciaEntrePuntos(punto origen, punto destino){
	return abs( abs(destino.x - origen.x) + abs(destino.y - origen.y) );
}

bool necesitoEstePokemon(char *pokemon){
	int valor = (int)dictionary_get(OBJETIVO_GLOBAL, pokemon);
	return (valor>0);
}

bool comparadorIDs(objetoID_QUE_NECESITO *objetoID1, objetoID_QUE_NECESITO *objetoID2){
	return objetoID1->idMensaje == objetoID2->idMensaje;
}

bool necesitoEsteID(int id){
	objetoID_QUE_NECESITO* objetoIDAux = malloc(sizeof(objetoID_QUE_NECESITO));
	objetoIDAux->idMensaje = id;
	int resultado = list_get_index(ID_QUE_NECESITO, objetoIDAux, comparadorIDs);
	free(objetoIDAux);
	if(resultado == -1)
		return false;
	return true;
}

void descontarDeObjetivoGlobal(char *pokemon){
	int valor = (int)dictionary_get(OBJETIVO_GLOBAL, pokemon);
	valor = valor - 1 ;
	if(valor>0)
		dictionary_put(OBJETIVO_GLOBAL, pokemon, valor);
	else
		dictionary_remove(OBJETIVO_GLOBAL, pokemon);
	printf("Hemos atrapado al pokemon: %s \n",pokemon);
}

void finalFeliz(){
	//destruirObjetivoGlobal();
	//destruirEstados();
	//destruirEntrenadores();//list_destroy_and_destroy_elements(ENTRENADORES_TOTALES, entrenadorDestroy);
}

void iniciarConfig(){
	t_config* creacionConfig = config_create("/home/utnso/workspace/tp-2020-1c-ManaOS-/Team/Team.config");
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
	AUX_ID_TRAINER = 0;
	int cantTrainers = damePosicionFinalDoblePuntero(TEAM_CONFIG.POSICIONES_ENTRENADORES);
	pthread_t hiloEntrenadores[cantTrainers+1];
	for (int i = 0; TEAM_CONFIG.POSICIONES_ENTRENADORES[i] != NULL; i++){
		punto punto = crearPunto(TEAM_CONFIG.POSICIONES_ENTRENADORES[i]);
		char ** pokemones = string_split(TEAM_CONFIG.POKEMON_ENTRENADORES[i], "|");
		char ** pokemonesObjetivo = string_split(TEAM_CONFIG.OBJETIVOS_ENTRENADORES[i], "|");
		entrenador * entrenador = crearEntrenador(punto, pokemones, pokemonesObjetivo);
		list_add(ENTRENADORES_TOTALES, entrenador);
		list_add(EstadoNew, entrenador);
		pthread_create(&hiloEntrenadores[i], NULL, (void*)cumplirMision, entrenador);
		pthread_detach(hiloEntrenadores[i]);
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
	newTrainer->tid = AUX_ID_TRAINER;
	newTrainer->posicion = punto;
	newTrainer->pokemones = pokemones;
	newTrainer->pokemonesObjetivo = pokemonesObjetivo;
	newTrainer->estado = t_NEW;
	newTrainer->razonBloqueo = t_NULL;
	newTrainer->mision = NULL;
	newTrainer->ciclosCPUAEjecutar = 0;
	newTrainer->ciclosCPUEjecutados = 0;
	sem_init(&newTrainer->semaforoDeEntrenador,0,0);
	AUX_ID_TRAINER = AUX_ID_TRAINER + 1 ;
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
	sem_init(&semaforoTermine,0,0);
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


////////////Funciones planificacion/////////////
void planificarSegun(char* tipoPlanificacion){
	if(string_equals_ignore_case(tipoPlanificacion, "FIFO"))
		FIFO();
	if(string_equals_ignore_case(tipoPlanificacion, "RR"))
		RR();
	if(string_equals_ignore_case(tipoPlanificacion, "SJFCD"))
		SJFCD();
	if(string_equals_ignore_case(tipoPlanificacion, "SJFSD"))
		SJFSD();
}

void FIFO(){
	CICLOS_TOTALES = 0;
	while(!objetivoGlobalCumplido()){
		while(list_is_empty(EstadoReady)){
			sleep(TEAM_CONFIG.RETARDO_CICLO_CPU);
		}
		entrenador *trainer = list_get(EstadoReady,0);
		sem_post(&(trainer->semaforoDeEntrenador));
		sem_wait(&semaforoTermine);
	}
}

void RR(){
	printf("Holis, me llamaron? Soy RR");
}
void SJFCD(){
	printf("Holis, me llamaron? Soy SJFCD");
}
void SJFSD(){
	printf("Holis, me llamaron? Soy SJFSD");
}
unsigned long int getClockTime(){
	return CLOCK;
}

void agregarTiempo(int cantidad){
	CLOCK += cantidad;
}

void calcularRafagasCPUAEjecutar(int idEntrenador){
	entrenador* trainer = list_get(ENTRENADORES_TOTALES, idEntrenador);
	if(trainer->mision == NULL)
		return;
	int distancia = diferenciaEntrePuntos(trainer->posicion, trainer->mision->point);
	if(trainer->mision->esIntercambio)
		distancia = distancia +5;
	else
		distancia = distancia +1;
	trainer->ciclosCPUAEjecutar = distancia;
}

void setObjetivoGlobal(){
	OBJETIVO_GLOBAL = dictionary_create();
	int j=0;
	int unPokemon = 0;
	entrenador * trainer = list_get(ENTRENADORES_TOTALES, j);
	while(trainer != NULL){
		for (int i = 0; trainer->pokemonesObjetivo[i] != NULL; i++){
			unPokemon = (int)dictionary_get(OBJETIVO_GLOBAL, trainer->pokemonesObjetivo[i]);
			dictionary_put(OBJETIVO_GLOBAL, trainer->pokemonesObjetivo[i], unPokemon + 1);
		}
		j++;
		trainer = list_get(ENTRENADORES_TOTALES, j);
	}
	printf("Objetivo global setteado \n");
}

bool teamCumplioSuObjetivo(){
	return (( objetivoGlobalCumplido() ) && ( todosLosEntrenadoresCumplieronObjetivo() ));
}

bool objetivoGlobalCumplido(){
	return dictionary_is_empty(OBJETIVO_GLOBAL); //asumiendo que vamos a usar dictionary_remove cuando capturemos
}

bool todosLosEntrenadoresCumplieronObjetivo(){
	return list_all_satisfy(ENTRENADORES_TOTALES, (void*)entrenadorCumplioObjetivo);
}

bool entrenadorCumplioObjetivo(entrenador* trainer){
	return sonLosMismosPokemon(trainer->pokemones,trainer->pokemonesObjetivo);
}

bool sonLosMismosPokemon(char **pokemons1, char **pokemons2){
	//TODO
	return false;
}
