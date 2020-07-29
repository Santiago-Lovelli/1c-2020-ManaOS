#include "Team.h"

int main (int argc, char *argv[]){
	inicializar(argc, argv);
	return EXIT_SUCCESS;
}

void crear_hilo_planificacion(){
	pthread_create(&hiloPlanificacion, NULL, (void*)planificarSegun, TEAM_CONFIG.ALGORITMO_PLANIFICACION);
	pthread_detach(hiloPlanificacion);
}

void inicializarSemaforos(){
//	sem_init(&semaforoSocket,0,1);
	sem_init(&semaforoGameboy,0,1);
	sem_init(&semaforoCambioEstado,0,1);
	sem_init(&semaforoConexionABroker,0,1);
	sem_init(&semaforoMisiones,0,1);
	sem_init(&semaforoDiccionario,0,1);
	sem_init(&semaforoAppeared,0,1);
	sem_init(&semaforoMovimiento,0,1);
	sem_init(&semaforoPokemon,0,1);
	sem_init(&semaforoGet,0,1);
}

void inicializar(int argc, char *argv[]){
	TEAM_LOG = iniciar_log("Team");
	inicializarSemaforos();
	iniciarConfig(argc, argv);
	crearEstados();
	crearEntrenadores();
	iniciarVariablesDePlanificacion();
	SEGUIR_ATENDIENDO = true;
	ID_QUE_NECESITO = list_create();
	IDs_GET = list_create();
	MISIONES_PENDIENTES = list_create();
	DEADLOCKS_RESUELTOS = 0;
	CAMBIOS_DE_CONTEXTO_REALIZADOS = 0;
	for (int i = 0; list_get(ENTRENADORES_TOTALES, i) != NULL; i++){
		entrenador * entrenador = list_get(ENTRENADORES_TOTALES, i);
		printf("Entrenador en posicion: x = %d, y = %d \n", entrenador->posicion.x, entrenador->posicion.y);
		if(entrenador->pokemones != NULL){
			for (int j = 0; entrenador->pokemones[j]!=NULL; j++){
				printf("Pokemon n° %d es un: %s \n", j, entrenador->pokemones[j]);
			}
		}
		for (int j = 0; entrenador->pokemonesObjetivo[j]!=NULL; j++){
			printf("Pokemon objetivo n° %d es un: %s \n", j, entrenador->pokemonesObjetivo[j]);
		}
	}
	setObjetivoGlobal();
	log_info (TEAM_LOG, "TEAM OK");
	//iniciarHilos();

	crear_hilo_planificacion();

	servidor = malloc(sizeof(pthread_t));
	iniciarServidorDeGameBoy(servidor);

	suscriptoAppearedPokemon = malloc(sizeof(pthread_t));
	conectarmeColaDe(suscriptoAppearedPokemon, d_APPEARED_POKEMON);

	suscriptoLocalizedPokemon = malloc(sizeof(pthread_t));
	conectarmeColaDe(suscriptoLocalizedPokemon, d_LOCALIZED_POKEMON);

	suscriptoCaughtPokemon = malloc(sizeof(pthread_t));
	conectarmeColaDe(suscriptoCaughtPokemon, d_CAUGHT_POKEMON);

	enviarGetXCadaPokemonObjetivo();

	pthread_join(*servidor, NULL);
	pthread_join(*suscriptoAppearedPokemon, NULL);
	pthread_join(*suscriptoLocalizedPokemon, NULL);
	pthread_join(*suscriptoCaughtPokemon, NULL);
}

int iniciarConexionABroker(){
	int conexion;
	while (1) {
		sem_wait(&semaforoConexionABroker);
		conexion = conectarse_a_un_servidor(TEAM_CONFIG.IP_BROKER, TEAM_CONFIG.PUERTO_BROKER, TEAM_LOG);
		sem_post(&semaforoConexionABroker);
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
	sem_wait(&semaforoPokemon);
	int index = (int)damePosicionFinalDoblePuntero(trainer->pokemones);
	if(index == 0 && trainer->pokemones[0] == NULL){
		trainer->pokemones = realloc(trainer->pokemones, (sizeof(char**)+( (sizeof(char*)*(index+2)) )) );
		trainer->pokemones[index] = pokemon;
		trainer->pokemones[index+1] = NULL;
		sem_post(&semaforoPokemon);
		return;
	}
	trainer->pokemones = realloc(trainer->pokemones, (sizeof(char**)+( (sizeof(char*)*(index+2)) )) );
	trainer->pokemones[index+1] = pokemon;
	trainer->pokemones[index+2] = NULL;
	sem_post(&semaforoPokemon);
}

void intercambiarPokemon(entrenador* trainer, int tidTrainerObjetivo, char* pokemon){
	entrenador *trainerObjetivo = list_get(ENTRENADORES_TOTALES, tidTrainerObjetivo);
	char* pokemon1;
	int posicionPokemonEnObjetivo = (int)damePosicionDeObjetoEnDoblePuntero(trainerObjetivo->pokemones, pokemon);
	char** sobrantes = quePokemonTengoDeMas(trainer);
	char** faltantes = quePokemonMeFalta(trainerObjetivo);
	char* pokemonParaObjetivo = (char*)primerElementoEnComun(faltantes,sobrantes);
	if(pokemonParaObjetivo != NULL){
		int posicionDeIntercambio = damePosicionDeObjetoEnDoblePuntero(trainer->pokemones, pokemonParaObjetivo);
		sem_wait(&semaforoPokemon);
		pokemon1 = trainer->pokemones[posicionDeIntercambio];
		trainer->pokemones[posicionDeIntercambio] = trainerObjetivo->pokemones[posicionPokemonEnObjetivo];
		trainerObjetivo->pokemones[posicionPokemonEnObjetivo] = pokemon1;
		sem_post(&semaforoPokemon);
		log_info(TEAM_LOG, "El entrenador %i intercambio a %s por %s con el entrenador %i", trainer->tid, pokemon1, trainer->pokemones[posicionDeIntercambio], trainerObjetivo->tid);
	}
	else if(sobrantes[0] != NULL){
		int posicionDeIntercambio = damePosicionDeObjetoEnDoblePuntero(trainer->pokemones, sobrantes[0]);
		sem_wait(&semaforoPokemon);
		pokemon1 = trainer->pokemones[posicionDeIntercambio];
		trainer->pokemones[posicionDeIntercambio] = trainerObjetivo->pokemones[posicionPokemonEnObjetivo];
		trainerObjetivo->pokemones[posicionPokemonEnObjetivo] = pokemon1;
		sem_post(&semaforoPokemon);
		log_info(TEAM_LOG, "El entrenador %i intercambio a %s por %s con el entrenador %i", trainer->tid, pokemon1, trainer->pokemones[posicionDeIntercambio], trainerObjetivo->tid);
	}
	free(sobrantes);
	free(faltantes);
	bloquearEntrenador(trainer->tid, t_DESOCUPADO);
	bloquearEntrenador(trainerObjetivo->tid, t_DESOCUPADO);
	sacarMision(trainer->tid);
	analizarDeadlockEspecifico(trainer);
	analizarDeadlockEspecifico(trainerObjetivo);
}


void enviarCatchPokemonYRecibirResponse(char *pokemon, int posX, int posY, int idEntrenadorQueMandaCatch){
	int conexion = conectarse_a_un_servidor(TEAM_CONFIG.IP_BROKER, TEAM_CONFIG.PUERTO_BROKER, TEAM_LOG);
	if(conexion == -1){
		entrenador *trainer = list_get(ENTRENADORES_TOTALES, idEntrenadorQueMandaCatch);
		sumarPokemon(trainer,pokemon);
		descontarDeObjetivoGlobal(pokemon);
		bloquearEntrenador(trainer->tid,t_DESOCUPADO);
		sacarMision(idEntrenadorQueMandaCatch);
		verPendientes(pokemon);
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
		log_info(TEAM_LOG, "El mensaje CATCH:%s se va a quedar esperando al IDCorrelativo: %i", pokemon, objetoID->idMensaje);
		list_add(ID_QUE_NECESITO, objetoID);
	}
}

void enviarGetPokemonYRecibirResponse(char *pokemon, void* value){
	int conexion = conectarse_a_un_servidor(TEAM_CONFIG.IP_BROKER, TEAM_CONFIG.PUERTO_BROKER, TEAM_LOG);
	if(conexion == -1){
		log_error(TEAM_LOG,"No se pudo conectar con el Broker para enviar GET %s. Se procede con comportamiento DEFAULT",pokemon);
		return; //comportamiento Default es asumir que el pokemon no esta
	}
	Serialize_PackAndSend_GET_POKEMON_NoID(conexion,pokemon);
	HeaderDelibird headerACK = Serialize_RecieveHeader(conexion);
	int* idResponse = malloc(sizeof(int));
	*idResponse = recibirResponse(conexion, headerACK);
	log_info(TEAM_LOG, "El GET:%s se va a quedar esperando al IDCorrelativo: %i", pokemon, *idResponse);
	sem_wait(&semaforoGet);
	list_add(IDs_GET, idResponse);
	sem_post(&semaforoGet);
}

void enviarGetXCadaPokemonObjetivo(){
	sem_wait(&semaforoDiccionario);
	dictionary_iterator(OBJETIVO_GLOBAL, (void*)enviarGetPokemonYRecibirResponse);
	sem_post(&semaforoDiccionario);
}

void iniciarServidorDeGameBoy(pthread_t* servidor) {
	if (pthread_create(servidor, NULL, (void*) atenderGameBoy, NULL) == 0) {
		log_info(TEAM_LOG, "::::Se creo hilo de GameBoy::::");
	} else {
		log_error(TEAM_LOG, "::::No se pudo crear el hilo de GameBoy::::");
	}
}


void* atenderGameBoy() {
	sem_t semaforoSocket;
	sem_init(&semaforoSocket,0,1);
	t_log* gameBoyLog = iniciar_log("GameBoy");
	int conexion = iniciar_servidor(TEAM_CONFIG.IP_TEAM, TEAM_CONFIG.PUERTO_TEAM, gameBoyLog);
	while (SEGUIR_ATENDIENDO) {
		int cliente = esperar_cliente_con_accept(conexion, gameBoyLog);
		sem_wait(&semaforoGameboy);
		log_info(gameBoyLog, "se conecto cliente: %i", cliente);
		pthread_t* dondeSeAtiende = malloc(sizeof(pthread_t));
		sem_wait(&semaforoSocket);
		p_elementoDeHilo *elemento = malloc(sizeof(p_elementoDeHilo));
		elemento->log = gameBoyLog;
		elemento->cliente = cliente;
		sem_post(&semaforoSocket);

		if (pthread_create(dondeSeAtiende, NULL,
				(void*) recibirYAtenderUnCliente, elemento) == 0) {
			log_info(gameBoyLog, ":::: Se creo hilo para cliente ::::");
		} else {
			log_error(gameBoyLog,
					":::: No se pudo crear el hilo para cliente ::::");

		}
		pthread_detach(*dondeSeAtiende);
		sem_post(&semaforoGameboy);
	}
	return NULL;
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
	while(1){
		int conexion = iniciarConexionABroker();
		Serialize_PackAndSend_SubscribeQueue(conexion, colaDeSuscripcion);
		p_elementoDeHilo* elemento = malloc(sizeof(p_elementoDeHilo));
		elemento->cliente = conexion;
		elemento->log = TEAM_LOG;
		recibirYAtenderUnCliente(elemento);
	}
	return 0;
}

void* recibirYAtenderUnCliente(p_elementoDeHilo* elemento) {
	sem_t semaforoSocket;
	sem_init(&semaforoSocket,0,1);
	while (SEGUIR_ATENDIENDO) {
		sem_wait(&semaforoSocket);
		HeaderDelibird headerRecibido = Serialize_RecieveHeader(elemento->cliente);
		if (headerRecibido.tipoMensaje == -1) {
			log_error(elemento->log, "Se desconecto el cliente\n");
			sem_post(&semaforoSocket);
			break;
		}
		atender(headerRecibido, elemento->cliente, elemento->log);
		sem_post(&semaforoSocket);
	}
	sem_destroy(&semaforoSocket);
	free(elemento);
	return 0;
}

bool idEstaEnLista(uint32_t id, t_list *lista){
	int *aux;
	sem_wait(&semaforoGet);
	for(int i =0; i<list_size(lista); i++){
		aux = list_get(lista,i);
		if(id == *aux){
			sem_post(&semaforoGet);
			return true;
		}
	}
	sem_post(&semaforoGet);
	return false;
}


void atender(HeaderDelibird header, int cliente, t_log* logger) {
	switch (header.tipoMensaje) {
	case d_APPEARED_POKEMON:;
		log_info(logger, "Llego un APPEARED POKEMON");
		void* packAppearedPokemon = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
//		sem_post(&semaforoSocket);
		uint32_t posicionAppearedX, posicionAppearedY, idMensajeAppeared, idCorrelativoAppeared;
		char *AppearedNombrePokemon;
		Serialize_Unpack_AppearedPokemon_IDCorrelativo(packAppearedPokemon, &idMensajeAppeared, &idCorrelativoAppeared, &AppearedNombrePokemon, &posicionAppearedX, &posicionAppearedY);
		log_info(logger, "Contenidos del mensaje: Pkm: %s, x: %i, y: %i\n", AppearedNombrePokemon, posicionAppearedX, posicionAppearedY);
		Serialize_PackAndSend_ACK(cliente, idMensajeAppeared);
		if(necesitoEstePokemon(AppearedNombrePokemon)){
			printf("Necesito este pokemon!!! \n ");
			hacerAppeared(AppearedNombrePokemon,posicionAppearedX,posicionAppearedY,logger);
		}
		else{ printf("No necesito este pokemon!!! \n "); }
		free(packAppearedPokemon);
		break;
	case d_LOCALIZED_POKEMON:;
		log_info(logger, "Llego un LOCALIZED POKEMON");
		void* packLocalizedPokemon = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
//		sem_post(&semaforoSocket);
		t_list *posCant = list_create();
		uint32_t idMensajeLocalized, idMensajeCorrelativo;
		char *localizedNombrePokemon;
		Serialize_Unpack_LocalizedPokemon_IDCorrelativo(packLocalizedPokemon,&idMensajeLocalized, &idMensajeCorrelativo,&localizedNombrePokemon,&posCant);
		log_info(logger,"Contenidos del mensaje: id: %i, id correlativo: %i, Pkm: %s",idMensajeLocalized, idMensajeCorrelativo,localizedNombrePokemon);
		for(int i = 0; i<posCant->elements_count; i++){
			d_PosCant* asd = list_get(posCant,i);
			log_info(logger,"x: %i, y:%i",asd->posX,asd->posY);
		}
		Serialize_PackAndSend_ACK(cliente, idMensajeLocalized);
		if(!idEstaEnLista(idMensajeCorrelativo,IDs_GET)){
			log_error(logger, "NO NECESITO ESTE ID DE LOCALIZED");
			break;
		}
		if(necesitoEstePokemon(localizedNombrePokemon)){
			printf("Necesito este pokemon!!! \n ");
			for(int i=0; i<list_size(posCant);i++){
				d_PosCant *posCantAUX = list_get(posCant,i);
				hacerAppeared(localizedNombrePokemon,posCantAUX->posX,posCantAUX->posY,logger);
			}
		}
		else{ printf("No necesito este pokemon!!! \n "); }
		free(packLocalizedPokemon);
		break;

	case d_CAUGHT_POKEMON:;
		log_info(logger, "Llego un CAUGHT POKEMON");
		//recibimos el resto del paquete
		void* packCaughtPokemon = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
//		sem_post(&semaforoSocket);
		//Con estas dos variables desempaquetamos el paquete
		uint32_t idMensajeCaught, resultadoCaught, idCorrelativoCaught;
		Serialize_Unpack_CaughtPokemon_IDCorrelativo(packCaughtPokemon, &idMensajeCaught, &idCorrelativoCaught, &resultadoCaught);
		log_info(logger, "Contenidos del mensaje: Id: %i, Id Correlativo: %i, Result: %i\n", idMensajeCaught, idCorrelativoCaught, resultadoCaught);
		Serialize_PackAndSend_ACK(cliente, idMensajeCaught);
		if(necesitoEsteID(idCorrelativoCaught)){
			hacerCaught(idCorrelativoCaught,resultadoCaught);
		}
		else{
			log_error(logger, "Este IDCorrelativo no me corresponde");
		}

		free(packCaughtPokemon);
		break;

	default:
		log_error(logger, "Mensaje no entendido: %i\n", header);
		void* packBasura = Serialize_ReceiveAndUnpack(cliente,
				header.tamanioMensaje);
		Serialize_PackAndSend_ACK(cliente, 0);
		free(packBasura);
//		sem_post(&semaforoSocket);
		break;
	}
}

void hacerAppeared(char* pokemon, int posicionAppearedX, int posicionAppearedY, t_log* logger){
	punto posicionPoke;
	posicionPoke.x = posicionAppearedX;
	posicionPoke.y = posicionAppearedY;
	sem_wait(&semaforoAppeared);
	if(!contandoMisionesActualesNecesitoEstePokemon(pokemon)){
		t_mision* misionPendiente = crearMision(pokemon,posicionPoke,false,(-1));
		list_add(MISIONES_PENDIENTES, misionPendiente);
		printf("\n Este pokemon se agregara a pendientes \n");
		sem_post(&semaforoAppeared);
		return;
	}
//	while(!contandoMisionesActualesNecesitoEstePokemon(pokemon)){
//		printf(" ::: VAMO A CALMARNO :::");
//		sleep(TEAM_CONFIG.RETARDO_CICLO_CPU);
//		if(!necesitoEstePokemon(pokemon))
//			return;
//	}
	int idEntrenador = entrenadorMasCercanoDisponible(posicionPoke);
	while(idEntrenador == -1){
		log_error(logger, "No hay entrenadores disponibles, se esperara a que los haya");
		sleep(TEAM_CONFIG.RETARDO_CICLO_CPU);
		idEntrenador = entrenadorMasCercanoDisponible(posicionPoke);
		if(!necesitoEstePokemon(pokemon)){
			sem_post(&semaforoAppeared);
			return;
		}
	}
	darMision(idEntrenador,pokemon,posicionPoke,false,(-1));
	pasarEntrenadorAEstado(idEntrenador, t_READY);
	sem_post(&semaforoAppeared);

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
		verPendientes(objetoID->pokemon);
		free(objetoID);
	}
	else{
		log_info(TEAM_LOG, "No pudimos atrapar a este pokemon");
		free(objetoID);
		objetoID = list_get(ID_QUE_NECESITO, index);
		list_remove(ID_QUE_NECESITO, index);
		bloquearEntrenador(objetoID->idEntrenador, t_DESOCUPADO);
		sacarMision(objetoID->idEntrenador);
		asignarMisionPendienteDePoke(objetoID->pokemon);
		free(objetoID);
	}
}

void verPendientes(char *pokemon){
	if(contandoMisionesActualesNecesitoEstePokemon(pokemon))
		asignarMisionPendienteDePoke(pokemon);
	if(!necesitoEstePokemon(pokemon))
		borrarEstePokemonDePendientes(pokemon);
}

void destruirMision(t_mision *mision){
	free(mision);
}

void borrarEstePokemonDePendientes(char *pokemon){
	t_mision *mision;
	for(int i=0; i<list_size(MISIONES_PENDIENTES); i++){
		mision = list_get(MISIONES_PENDIENTES,i);
		if(mismoPokemonDeMision(mision,pokemon))
			list_remove_and_destroy_element(MISIONES_PENDIENTES,i,(void*)destruirMision);
	}
}

bool mismoPokemonDeMision(t_mision* pokemon1, char* pokemon2){
	return ( (strcmp(pokemon1->pokemon,pokemon2)) == 0);
}

int cuantosDeEstePokemonTengo(entrenador* trainer, char* pokemon){
	int index = damePosicionFinalDoblePuntero(trainer->pokemones);
	int response = 0;
	for(int i=0; i<=index; i++){
		if(strcmp(trainer->pokemones[i],pokemon) == 0)
			response = response +1;
	}
	return response;
}

int cuantosDeEstePokemonNecesito(entrenador* trainer, char*pokemon){
	int index = damePosicionFinalDoblePuntero(trainer->pokemonesObjetivo);
	int response = 0;
	for(int i=0; i<=index; i++){
		if(strcmp(trainer->pokemonesObjetivo[i],pokemon) == 0)
			response = response +1;
	}
	return response;
}

char** quePokemonMeFalta(entrenador* trainer){
	sem_wait(&semaforoPokemon);
	int indexPoke = damePosicionFinalDoblePuntero(trainer->pokemonesObjetivo);
	int cuantosTengo = 0;
	int cuantosNecesito = 0;
	int indexRespuesta = 0;
	char** respuesta = malloc(sizeof(char**) + 4);
	respuesta[0] = NULL;
	for(int i=0; i<=indexPoke; i++){
		cuantosTengo = cuantosDeEstePokemonTengo(trainer, trainer->pokemonesObjetivo[i]);
		cuantosNecesito = cuantosDeEstePokemonNecesito(trainer, trainer->pokemonesObjetivo[i]);
		if((cuantosNecesito - cuantosTengo) > 0){
			respuesta = realloc(respuesta, sizeof(char**) + sizeof(char*)*(indexRespuesta+2));
			respuesta[indexRespuesta] = trainer->pokemonesObjetivo[i];
			respuesta[indexRespuesta+1] = NULL;
			indexRespuesta = indexRespuesta +1;
		}
	}
	sem_post(&semaforoPokemon);
	return respuesta;
}

char** quePokemonTengoDeMas(entrenador *trainer){
	sem_wait(&semaforoPokemon);
	int indexPoke = damePosicionFinalDoblePuntero(trainer->pokemones);
	int cuantosTengo = 0;
	int cuantosNecesito = 0;
	int indexRespuesta = 0;
	char** respuesta = malloc(sizeof(char**) + 4);
	respuesta[0] = NULL;
	for(int i=0; i<=indexPoke; i++){
		cuantosTengo = cuantosDeEstePokemonTengo(trainer, trainer->pokemones[i]);
		cuantosNecesito = cuantosDeEstePokemonNecesito(trainer, trainer->pokemones[i]);
		if((cuantosNecesito - cuantosTengo) < 0){
			respuesta = realloc(respuesta, sizeof(char**) + sizeof(char*)*(indexRespuesta+2));
			respuesta[indexRespuesta] = trainer->pokemones[i];
			respuesta[indexRespuesta+1] = NULL;
			indexRespuesta = indexRespuesta +1;
		}
	}
	sem_post(&semaforoPokemon);
	return respuesta;
}

void asignarMisionPendienteDePoke(char* pokemon){
	t_mision *mision;
	for(int i=0; i<list_size(MISIONES_PENDIENTES); i++){
		mision = list_get(MISIONES_PENDIENTES,i);
		if(mismoPokemonDeMision(mision,pokemon)){
			hacerAppeared(mision->pokemon,mision->point.x,mision->point.y, TEAM_LOG);
			return;
		}
	}
}

bool entrenadorEstaDisponible(entrenador* entrenadorAUX){
	sem_wait(&semaforoCambioEstado);
	sem_wait(&semaforoMisiones);
	bool response = (entrenadorAUX->estado != t_EXIT) &&
			   	   (entrenadorAUX->estado != t_READY) &&
				   (entrenadorAUX->razonBloqueo != t_ESPERANDO_RESPUESTA) &&
				   (entrenadorAUX->mision == NULL);
	if(entrenadorAUX->pokemones[0] != NULL){
		response = response && (damePosicionFinalDoblePuntero(entrenadorAUX->pokemones) < damePosicionFinalDoblePuntero(entrenadorAUX->pokemonesObjetivo));
	}
	sem_post(&semaforoMisiones);
	sem_post(&semaforoCambioEstado);
	return response;

}

int entrenadorMasCercanoDisponible(punto point){
	int index = -1;
	int distanciaMinima = 9999;
	int distanciaAUX = 0;
	entrenador *entrenadorAUX;
	sem_wait(&semaforoMovimiento);
	entrenadorAUX = (entrenador*)list_get(ENTRENADORES_TOTALES,0);
	if( entrenadorEstaDisponible(entrenadorAUX) ){
		index = 0;
		distanciaMinima = diferenciaEntrePuntos( entrenadorAUX->posicion , point);
	}

	for(int j = 1; j < list_size(ENTRENADORES_TOTALES); j=j+1){
		entrenadorAUX = (entrenador*)list_get(ENTRENADORES_TOTALES,j);
		distanciaAUX = diferenciaEntrePuntos( entrenadorAUX->posicion , point);
		if( (distanciaAUX <= distanciaMinima) && entrenadorEstaDisponible(entrenadorAUX)){
			distanciaMinima = distanciaAUX;
			index = j;
		}
	}
	sem_post(&semaforoMovimiento);
	return index;
}

bool mismaPosicion(entrenador* e1, entrenador* e2){
	return ((e1->posicion.x == e2->posicion.x) && (e1->posicion.y == e2->posicion.y));
}

bool mismaPosicion2(entrenador* e1, punto e2){
	return ((e1->posicion.x == e2.x) && (e1->posicion.y == e2.y));
}

bool acercar(int *punto1, int punto2){
	sem_wait(&semaforoMovimiento);
	if(*punto1<punto2){
		*punto1 = *punto1 + 1;
		sem_post(&semaforoMovimiento);
		return true;
	}
	if(*punto1>punto2){
		*punto1 = *punto1 - 1;
		sem_post(&semaforoMovimiento);
		return true;
	}
	sem_post(&semaforoMovimiento);
	return false;
}

bool moveHacia(entrenador* e1, punto destino){
	if(mismaPosicion2(e1,destino))
		return false;
	if(!acercar(&e1->posicion.x, destino.x))
		acercar(&e1->posicion.y, destino.y);
	return true;
}


void eliminarDeListaEntrenador(entrenador  *trainer, t_list* lista){
	if(trainer->tid != -1){
		int index = list_get_index(lista, trainer, (void*)mismaPosicion);
		if(index == -1){
			log_error(TEAM_LOG, "No se pudo sacar al entrenador %i de la lista", trainer->tid);
		}
		list_remove(lista,index);
	}
	else
		printf("Indice Invalido");
}

void bloquearEntrenador(int idEntrenador, t_razonBloqueo razon){
	entrenador *trainer = list_get(ENTRENADORES_TOTALES, idEntrenador);
	pasarEntrenadorAEstado(idEntrenador, t_BLOCKED);
	sem_wait(&semaforoCambioEstado);
	trainer->razonBloqueo = razon;
	sem_post(&semaforoCambioEstado);
}

void sacarEntrenadorDeEstadoActual(entrenador* trainer){
	trainer->razonBloqueo = t_NULL;
	switch(trainer->estado){
	case t_NEW:;
		eliminarDeListaEntrenador(trainer,EstadoNew);
		break;
	case t_READY:;
		eliminarDeListaEntrenador(trainer,EstadoReady);
		break;
	case t_BLOCKED:;
		eliminarDeListaEntrenador(trainer,EstadoBlock);
		break;
	case t_EXEC:;
		if(mismaPosicion(EstadoExec, trainer))
			EstadoExec = NULL;
		else
			printf("Este entrenador NO esta en exec");
		break;
	case t_EXIT:;
		eliminarDeListaEntrenador(trainer,EstadoExit);
		break;
	default:;
		printf("Estado invalido");
		break;
	}
}

void pasarEntrenadorAEstado(int index, t_estado estado){
	sem_wait(&semaforoCambioEstado);
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
	sem_post(&semaforoCambioEstado);

}

void sacarMision(int idEntrenador){
	entrenador *trainer = list_get(ENTRENADORES_TOTALES,idEntrenador);
	if(trainer->mision == NULL)
		return;
	sem_wait(&semaforoMisiones);
	destruirMision(trainer->mision);
	trainer->mision = NULL;
	sem_post(&semaforoMisiones);
}

t_mision* crearMision(char *pokemon, punto point, bool esIntercambio, int tidObjetivo){
	t_mision *mision = malloc(sizeof(punto) + strlen(pokemon) + 1 + sizeof(bool) + sizeof(tidObjetivo) + sizeof(int));
	mision->point = point;
	mision->pokemon = pokemon;
	mision->esIntercambio = esIntercambio;
	mision->tidObjetivo = tidObjetivo;
	return mision;
}

bool hayAlgunaMision(){
	entrenador* trainer;
	for(int i=0; i<list_size(ENTRENADORES_TOTALES); i++){
		trainer = list_get(ENTRENADORES_TOTALES,i);
		if(trainer->mision != NULL){
			return true;
		}
	}
	return false;
}

void darMision(int idEntrenador, char* pokemon, punto point, bool esIntercambio, int tidObjetivo){
	entrenador *trainer = list_get(ENTRENADORES_TOTALES,idEntrenador);
	sem_wait(&semaforoMisiones);
	trainer->mision = crearMision(pokemon,point,esIntercambio,tidObjetivo);
	sem_post(&semaforoMisiones);

}

void asignarMision(int idEntrenador, t_mision* misionYaCreada){
	entrenador *trainer = list_get(ENTRENADORES_TOTALES,idEntrenador);
	trainer->mision = misionYaCreada;
}

void sumarXCiclos(entrenador *trainer, int ciclos){
	trainer->ciclosCPUEjecutados = trainer->ciclosCPUEjecutados+ciclos;
	trainer->ciclosCPUEjecutadosEnTotal = trainer->ciclosCPUEjecutadosEnTotal+ciclos;
	CICLOS_TOTALES = CICLOS_TOTALES + ciclos;
	sleep(TEAM_CONFIG.RETARDO_CICLO_CPU);
	if(string_equals_ignore_case(TEAM_CONFIG.ALGORITMO_PLANIFICACION, "RR") || string_equals_ignore_case(TEAM_CONFIG.ALGORITMO_PLANIFICACION, "SJFCD")){
		sem_post(&semaforoPlanifiquenme);
		sem_wait(&(trainer->semaforoDeEntrenador));
	}
}

void avisarQueTermine(entrenador *trainer){
	trainer->ciclosCPUEjeutadosAnteriormente = trainer->ciclosCPUEjecutados;
	trainer->ciclosCPUEjecutados = 0;
	if(string_equals_ignore_case(TEAM_CONFIG.ALGORITMO_PLANIFICACION, "RR") || string_equals_ignore_case(TEAM_CONFIG.ALGORITMO_PLANIFICACION, "SJFCD")){
		sem_post(&semaforoPlanifiquenme);
	}
	else{
		sem_post(&semaforoTermine);
	}
}



void cumplirMision(entrenador* trainer){
	printf("Se creo el hilo del entrenador %i \n", trainer->tid);
	while( !entrenadorCumplioObjetivo(trainer) ){
		//if(trainer->mision != NULL){
			sem_wait(&(trainer->semaforoDeEntrenador));

			if(entrenadorCumplioObjetivo(trainer)){
				printf("El entrenador: %i cumplio su objetivo! Yupiiii!!! \n", trainer->tid);
				pasarEntrenadorAEstado(trainer->tid, t_EXIT);
				avisarQueTermine(trainer);
				return;
			}

			trainer->ciclosCPUEjecutados = 0;
			printf("Hola soy el entrenador %i \n", trainer->tid);
			log_info(TEAM_LOG,"\n Mi posicion inicial es: \n x = %i \n y = %i \n", trainer->posicion.x,trainer->posicion.y);
			while( moveHacia(trainer, trainer->mision->point) ){
				log_info(TEAM_LOG,"\n Mi posicion actual es: \n x = %i \n y = %i \n", trainer->posicion.x,trainer->posicion.y);
				sumarXCiclos(trainer,1);
			}
			if(trainer->mision->esIntercambio){
				log_info(TEAM_LOG,"Hola soy el entrenador %i \n Se va a intentar intercambiar por: %s \n En la posicion: x:%i y:%i \n Con el entrenador: %i",trainer->tid, trainer->mision->pokemon, trainer->posicion.x, trainer->posicion.y, trainer->mision->tidObjetivo);
				for(int i=0; i<5; i++){
					sumarXCiclos(trainer,1);
				}
				intercambiarPokemon(trainer, trainer->mision->tidObjetivo, trainer->mision->pokemon);
			}
			else{
				log_info(TEAM_LOG,"Hola soy el entrenador %i \n Se va a intentar atrapar a: %s \n En la posicion: x:%i y:%i", trainer->tid, trainer->mision->pokemon, trainer->posicion.x, trainer->posicion.y);
				sumarXCiclos(trainer,1);
				enviarCatchPokemonYRecibirResponse( trainer->mision->pokemon, trainer->mision->point.x, trainer->mision->point.y, trainer->tid);
			}
			avisarQueTermine(trainer);
		//}
	}
	printf("El entrenador: %i cumplio su objetivo! Yupiiii!!! \n", trainer->tid);
	pasarEntrenadorAEstado(trainer->tid, t_EXIT);
}

int diferenciaEntrePuntos(punto origen, punto destino){
	return abs( abs(destino.x - origen.x) + abs(destino.y - origen.y) );
}

int cantidadDeMisiones(char *pokemon){
	int cantidad = 0;
	entrenador *trainer;
	for(int i=0; i<list_size(ENTRENADORES_TOTALES); i++){
		trainer = list_get(ENTRENADORES_TOTALES,i);
		sem_wait(&semaforoMisiones);
		if( (trainer->mision != NULL) && (strcmp(trainer->mision->pokemon , pokemon)) == 0 ){
			cantidad = cantidad +1;
		}
		sem_post(&semaforoMisiones);
	}
	return cantidad;
}

bool necesitoEstePokemon(char *pokemon){
	if(pokemon == NULL)
		return false;
	sem_wait(&semaforoDiccionario);
	int valor = (int)dictionary_get(OBJETIVO_GLOBAL, pokemon);
	sem_post(&semaforoDiccionario);
	return (valor>0);
}

bool contandoMisionesActualesNecesitoEstePokemon(char *pokemon){
	sem_wait(&semaforoDiccionario);
	int valor = (int)dictionary_get(OBJETIVO_GLOBAL, pokemon);
	sem_post(&semaforoDiccionario);
	int cantidad = cantidadDeMisiones(pokemon);
	return ((valor-cantidad)>0);
}

bool comparadorIDs(objetoID_QUE_NECESITO *objetoID1, objetoID_QUE_NECESITO *objetoID2){
	return objetoID1->idMensaje == objetoID2->idMensaje;
}

bool necesitoEsteID(int id){
	objetoID_QUE_NECESITO* objetoIDAux = malloc(sizeof(objetoID_QUE_NECESITO));
	objetoIDAux->idMensaje = id;
	int resultado = list_get_index(ID_QUE_NECESITO, objetoIDAux, (void*)comparadorIDs);
	free(objetoIDAux);
	if(resultado == -1)
		return false;
	return true;
}

void descontarDeObjetivoGlobal(char *pokemon){
	sem_wait(&semaforoDiccionario);
	int valor = (int)dictionary_get(OBJETIVO_GLOBAL, pokemon);
	valor = valor - 1 ;
	if(valor>0)
		dictionary_put(OBJETIVO_GLOBAL, pokemon, valor);
	else
		dictionary_remove(OBJETIVO_GLOBAL, pokemon);
	printf("Hemos atrapado al pokemon: %s \n",pokemon);
	sem_post(&semaforoDiccionario);
}

void logearFin(){
	printf("\n \n \n LLEGAMOS \n AL \n FINAL \n FELIZ \n \n\n ");
	log_info(TEAM_LOG, "::: TEAM HA FINALIZADO EXITOSAMENTE :::");
	log_info(TEAM_LOG, "::: ESTADISTICAS :::");
	log_info(TEAM_LOG, "::: CICLOS TOTALES: %i :::",CICLOS_TOTALES);
	log_info(TEAM_LOG, "::: CAMBIOS DE CONTEXTO REALIZADOS: %i", CAMBIOS_DE_CONTEXTO_REALIZADOS);
	log_info(TEAM_LOG, "::: CANTIDAD DE DEADLOCKS PRODUCIDOS: %i", DEADLOCKS_PRODUCIDOS);
	log_info(TEAM_LOG, "::: CANTIDAD DE DEADLOCKS RESUELTOS: %i", DEADLOCKS_RESUELTOS);
	for(int i=0;i<list_size(ENTRENADORES_TOTALES); i++){
		entrenador * trainer = list_get(ENTRENADORES_TOTALES,i);
		log_info(TEAM_LOG, "::: CANTIDAD DE CICLOS EJECUTADOS POR EL ENTRENADOR %i: %i", trainer->tid,trainer->ciclosCPUEjecutadosEnTotal);
	}
}

void destruirElementoGlobal(void* element, void*key){
	free(element);
}

void destruirObjetivoGlobal(){
	sem_wait(&semaforoDiccionario);
	dictionary_destroy_and_destroy_elements(OBJETIVO_GLOBAL, (void*)destruirElementoGlobal);
//	dictionary_destroy(OBJETIVO_GLOBAL);
	sem_post(&semaforoDiccionario);
}

void destruirEstados(){
	list_destroy(EstadoBlock);
	list_destroy(EstadoExit);
	list_destroy(EstadoNew);
	list_destroy(EstadoReady);
}

void destruirEntrenadores(){
	list_destroy_and_destroy_elements(ENTRENADORES_TOTALES, (void*)entrenadorDestroy);
}

void destruirSemaforos(){
	sem_destroy(&semaforoCambioEstado);
	sem_destroy(&semaforoConexionABroker);
	sem_destroy(&semaforoDiccionario);
	sem_destroy(&semaforoGameboy);
	sem_destroy(&semaforoMisiones);
	sem_destroy(&semaforoPlanifiquenme);
//	sem_destroy(&semaforoSocket);
	sem_destroy(&semaforoTermine);
	sem_destroy(&semaforoAppeared);
}

void destruirTodo(){
	destruirObjetivoGlobal();
	destruirEntrenadores();
	destruirEstados();
	destruirSemaforos();
	matarHilos();
}

void matarHilos(){
	pthread_cancel(*servidor);
	pthread_cancel(*suscriptoAppearedPokemon);
	pthread_cancel(*suscriptoLocalizedPokemon);
	pthread_cancel(*suscriptoCaughtPokemon);

}

void finalFeliz(){
	SEGUIR_ATENDIENDO = false;
	planificarDeadlocks();
	sleep(TEAM_CONFIG.RETARDO_CICLO_CPU);
	logearFin();
	destruirTodo(); //Hakai
}

void iniciarConfig(int argc, char *argv[]){
	t_config* creacionConfig;
	if(argc == 2){
		creacionConfig = config_create(argv[1]);
	}else{
		creacionConfig = config_create("../Team.config");
//		creacionConfig = config_create("/home/utnso/workspace/tp-2020-1c-ManaOS-/Team/Team.config");
	}
//	t_config* creacionConfig = config_create("/home/utnso/workspace/tp-2020-1c-ManaOS-/Team/Team2.config");
	TEAM_CONFIG.POSICIONES_ENTRENADORES = config_get_array_value(creacionConfig, "POSICIONES_ENTRENADORES");
	TEAM_CONFIG.POKEMON_ENTRENADORES = config_get_array_value(creacionConfig, "POKEMON_ENTRENADORES");
	TEAM_CONFIG.OBJETIVOS_ENTRENADORES = config_get_array_value(creacionConfig, "OBJETIVOS_ENTRENADORES");
	TEAM_CONFIG.TIEMPO_RECONEXION = config_get_int_value (creacionConfig, "TIEMPO_RECONEXION");
	TEAM_CONFIG.RETARDO_CICLO_CPU = config_get_int_value (creacionConfig, "RETARDO_CICLO_CPU");
	TEAM_CONFIG.ALGORITMO_PLANIFICACION = config_get_string_value (creacionConfig, "ALGORITMO_PLANIFICACION");
	TEAM_CONFIG.QUANTUM = config_get_int_value (creacionConfig, "QUANTUM");
	TEAM_CONFIG.ALPHA = config_get_double_value(creacionConfig, "ALPHA");
	TEAM_CONFIG.ESTIMACION_INICIAL = config_get_int_value (creacionConfig, "ESTIMACION_INICIAL");
	TEAM_CONFIG.IP_BROKER = config_get_string_value (creacionConfig, "IP_BROKER");
	TEAM_CONFIG.PUERTO_BROKER = config_get_string_value (creacionConfig, "PUERTO_BROKER");
	TEAM_CONFIG.LOG_FILE = config_get_string_value (creacionConfig, "LOG_FILE");
	TEAM_CONFIG.IP_TEAM = config_get_string_value(creacionConfig, "IP_TEAM");
	TEAM_CONFIG.PUERTO_TEAM = config_get_string_value(creacionConfig, "PUERTO_TEAM");
	free(creacionConfig);
}

void descontarPokemonsActualesDeOBJGlobal(entrenador* trainer){
	if(trainer->pokemones == NULL)
		return;
	int posFinal = damePosicionFinalDoblePuntero(trainer->pokemones);
	for(int i=0; i<=posFinal; i++){
		if( necesitoEstePokemon(trainer->pokemones[i]) ){
			descontarDeObjetivoGlobal(trainer->pokemones[i]);
		}
	}
}

void crearEntrenadores(){
	ENTRENADORES_TOTALES = list_create();
	AUX_ID_TRAINER = 0;
	int cantTrainers = damePosicionFinalDoblePuntero(TEAM_CONFIG.POSICIONES_ENTRENADORES);
	char ** pokemones = NULL;
	char ** pokemonesObjetivo = NULL;
	pthread_t hiloEntrenadores[cantTrainers+1];
	for (int i = 0; TEAM_CONFIG.POSICIONES_ENTRENADORES[i] != NULL; i++){
		pokemones = NULL;
		pokemonesObjetivo = NULL;
		punto punto = crearPunto(TEAM_CONFIG.POSICIONES_ENTRENADORES[i]);
		if(i <= damePosicionFinalDoblePuntero(TEAM_CONFIG.POKEMON_ENTRENADORES) && TEAM_CONFIG.POKEMON_ENTRENADORES[0] != NULL)
			pokemones = string_split(TEAM_CONFIG.POKEMON_ENTRENADORES[i], "|");
		else{
			pokemones = malloc(sizeof(char**)+ 4);
			pokemones[0] = NULL;
		}

		if(i <= damePosicionFinalDoblePuntero(TEAM_CONFIG.OBJETIVOS_ENTRENADORES) && TEAM_CONFIG.OBJETIVOS_ENTRENADORES[0] != NULL)
			pokemonesObjetivo = string_split(TEAM_CONFIG.OBJETIVOS_ENTRENADORES[i], "|");
		else{
			pokemonesObjetivo = malloc(sizeof(char**) + 4);
			pokemonesObjetivo[0] = NULL;
		}
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
	newTrainer->ciclosCPUEjeutadosAnteriormente = 0;
	newTrainer->ciclosCPUEjecutados = 0;
	newTrainer->ciclosCPUEstimados = 0;
	newTrainer->ciclosCPUEjecutadosEnTotal = 0;
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
}

void entrenadorDestroy(entrenador *self) {
	liberarDoblePuntero(self->pokemones);
	liberarDoblePuntero(self->pokemonesObjetivo);
	sem_destroy(&(self->semaforoDeEntrenador));
	free(self);
}

void ponerAlFinalDeLista(entrenador *trainer, t_list *lista){
	int index = list_get_index(lista,trainer, (void*)mismaPosicion);
	if(index != -1){
		list_remove(lista,index);
		list_add(lista,trainer);
	}
	trainer->ciclosCPUEjecutados = 0;
}

////////////Funciones planificacion/////////////

void esperarAlgunoEnReady(bool isDeadlock){
	if(isDeadlock){
		sem_wait(&semaforoCambioEstado);
		while(list_is_empty(EstadoReady)){
			sem_post(&semaforoCambioEstado);
			sleep(TEAM_CONFIG.RETARDO_CICLO_CPU);
			planificarDeadlocks();
			sem_wait(&semaforoCambioEstado);
		}
		sem_post(&semaforoCambioEstado);
	}
	else{
		sem_wait(&semaforoCambioEstado);
		while(list_is_empty(EstadoReady) && !objetivoGlobalCumplido()){
			sem_post(&semaforoCambioEstado);
			sleep(TEAM_CONFIG.RETARDO_CICLO_CPU);
			sem_wait(&semaforoCambioEstado);
		}
		sem_post(&semaforoCambioEstado);
	}
}

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
	sem_init(&semaforoTermine,0,0);
	while(!objetivoGlobalCumplido()){
		esperarAlgunoEnReady(false);
		if(objetivoGlobalCumplido())
			break;
		CAMBIOS_DE_CONTEXTO_REALIZADOS = CAMBIOS_DE_CONTEXTO_REALIZADOS + 2;
		sem_wait(&semaforoCambioEstado);
		entrenador *trainer = list_get(EstadoReady,0);
		sem_post(&semaforoCambioEstado);
		log_info(TEAM_LOG, "Se planificara al entrenador nro: %i",trainer->tid);
		sem_post(&(trainer->semaforoDeEntrenador));
		sem_wait(&semaforoTermine);
		log_info(TEAM_LOG, "Se va a intercambiar al entrenador nro: %i porque termino sus rafagas", trainer->tid);
	}
	//Manejo de Deadlock
	DEADLOCKS_PRODUCIDOS = list_size(EstadoBlock);
	log_info(TEAM_LOG, "La cantidad de entrenadores actuales en deadlock es: %i\nSe procedera a manejar estos deadlock a continuacion \n", list_size(EstadoBlock));
	while(!teamCumplioSuObjetivo()){
		planificarDeadlocks();
		esperarAlgunoEnReady(true);
		CAMBIOS_DE_CONTEXTO_REALIZADOS = CAMBIOS_DE_CONTEXTO_REALIZADOS + 2;
		sem_wait(&semaforoCambioEstado);
		entrenador *trainer = list_get(EstadoReady,0);
		sem_post(&semaforoCambioEstado);
		log_info(TEAM_LOG, "Se planificara al entrenador nro: %i",trainer->tid);
		sem_post(&(trainer->semaforoDeEntrenador));
		sem_wait(&semaforoTermine);
		log_info(TEAM_LOG, "Se va a intercambiar al entrenador nro: %i porque termino sus rafagas", trainer->tid);
	}
	log_info(TEAM_LOG, "TEAM ha cumplido su objetivo!");
	finalFeliz();


}

void RR(){
	CICLOS_TOTALES = 0;
	sem_init(&semaforoPlanifiquenme,0,0);
	while(!objetivoGlobalCumplido()){
		esperarAlgunoEnReady(false);
		if(objetivoGlobalCumplido())
			break;
		CAMBIOS_DE_CONTEXTO_REALIZADOS = CAMBIOS_DE_CONTEXTO_REALIZADOS + 2;
		sem_wait(&semaforoCambioEstado);
		entrenador *trainer = list_get(EstadoReady,0);
		sem_post(&semaforoCambioEstado);
		log_info(TEAM_LOG, "\n ::: Se planificara al entrenador nro: %i ::: \n",trainer->tid);
		sem_post(&(trainer->semaforoDeEntrenador));
		while(1){
			sem_wait(&semaforoPlanifiquenme);
			if(trainer->mision == NULL){
				log_info(TEAM_LOG, "Se va a intercambiar al entrenador nro: %i porque termino sus rafagas", trainer->tid);
				break;
			}
			else if(trainer->ciclosCPUEjecutados < TEAM_CONFIG.QUANTUM){
				CAMBIOS_DE_CONTEXTO_REALIZADOS = CAMBIOS_DE_CONTEXTO_REALIZADOS + 1;
				sem_post(&(trainer->semaforoDeEntrenador));
			}
			else{
				log_info(TEAM_LOG, "Se va a intercambiar al entrenador nro: %i por corte de quantum", trainer->tid);
				ponerAlFinalDeLista(trainer,EstadoReady);
				break;
			}
		}
	}
	//Manejo de Deadlock
	DEADLOCKS_PRODUCIDOS = list_size(EstadoBlock);
	log_info(TEAM_LOG, "La cantidad de entrenadores actuales en deadlock es: %i\nSe procedera a manejar estos deadlock a continuacion \n", list_size(EstadoBlock));
	while(!teamCumplioSuObjetivo()){
		planificarDeadlocks();
		esperarAlgunoEnReady(true);
		CAMBIOS_DE_CONTEXTO_REALIZADOS = CAMBIOS_DE_CONTEXTO_REALIZADOS + 2;
		sem_wait(&semaforoCambioEstado);
		entrenador *trainer = list_get(EstadoReady,0);
		sem_post(&semaforoCambioEstado);
		log_info(TEAM_LOG, "\n ::: Se planificara al entrenador nro: %i ::: \n",trainer->tid);
		sem_post(&(trainer->semaforoDeEntrenador));
		while(1){
			sem_wait(&semaforoPlanifiquenme);
			if(trainer->mision == NULL){
				log_info(TEAM_LOG, "Se va a intercambiar al entrenador nro: %i porque termino sus rafagas", trainer->tid);
				break;
			}
			else if(trainer->ciclosCPUEjecutados < TEAM_CONFIG.QUANTUM){
				CAMBIOS_DE_CONTEXTO_REALIZADOS = CAMBIOS_DE_CONTEXTO_REALIZADOS + 1;
				sem_post(&(trainer->semaforoDeEntrenador));
			}
			else{
				log_info(TEAM_LOG, "Se va a intercambiar al entrenador nro: %i por corte de quantum", trainer->tid);
				ponerAlFinalDeLista(trainer,EstadoReady);
				break;
			}
		}
	}
	log_info(TEAM_LOG, "TEAM ha cumplido su objetivo!");
	finalFeliz();
}
void SJFCD(){
	CICLOS_TOTALES = 0;
	sem_init(&semaforoPlanifiquenme,0,0);
	while(!objetivoGlobalCumplido()){
		esperarAlgunoEnReady(false);
		if(objetivoGlobalCumplido())
			break;
		ordenarListaSJF(EstadoReady);
		CAMBIOS_DE_CONTEXTO_REALIZADOS = CAMBIOS_DE_CONTEXTO_REALIZADOS + 2;
		sem_wait(&semaforoCambioEstado);
		entrenador *trainer = list_get(EstadoReady,0);
		sem_post(&semaforoCambioEstado);
		log_info(TEAM_LOG, "\n ::: Se planificara al entrenador nro: %i ::: \n",trainer->tid);
		sem_post(&(trainer->semaforoDeEntrenador));
		while(1){
			sem_wait(&semaforoPlanifiquenme);
			if(trainer->mision == NULL){
				log_info(TEAM_LOG, "Se va a intercambiar al entrenador nro: %i porque termino sus rafagas", trainer->tid);
				break;
			}
			else{
				ordenarListaSJF(EstadoReady);
				entrenador *trainer = list_get(EstadoReady,0);
				log_info(TEAM_LOG, "\n ::: Se planificara al entrenador nro: %i ::: \n",trainer->tid);
				CAMBIOS_DE_CONTEXTO_REALIZADOS = CAMBIOS_DE_CONTEXTO_REALIZADOS + 2;
				sem_post(&(trainer->semaforoDeEntrenador));
			}
		}
	}
	//Manejo de Deadlock
	DEADLOCKS_PRODUCIDOS = list_size(EstadoBlock);
	log_info(TEAM_LOG, "La cantidad de entrenadores actuales en deadlock es: %i\nSe procedera a manejar estos deadlock a continuacion \n", list_size(EstadoBlock));
	while(!teamCumplioSuObjetivo()){
		planificarDeadlocks();
		esperarAlgunoEnReady(true);
		ordenarListaSJF(EstadoReady);
		CAMBIOS_DE_CONTEXTO_REALIZADOS = CAMBIOS_DE_CONTEXTO_REALIZADOS + 2;
		sem_wait(&semaforoCambioEstado);
		entrenador *trainer = list_get(EstadoReady,0);
		sem_post(&semaforoCambioEstado);
		log_info(TEAM_LOG, "\n ::: Se planificara al entrenador nro: %i ::: \n",trainer->tid);
		sem_post(&(trainer->semaforoDeEntrenador));
		while(1){
			sem_wait(&semaforoPlanifiquenme);
			if(trainer->mision == NULL){
				log_info(TEAM_LOG, "Se va a intercambiar al entrenador nro: %i porque termino sus rafagas", trainer->tid);
				break;
			}
			else{
				ordenarListaSJF(EstadoReady);
				CAMBIOS_DE_CONTEXTO_REALIZADOS = CAMBIOS_DE_CONTEXTO_REALIZADOS + 1;
				entrenador *trainer = list_get(EstadoReady,0);
				log_info(TEAM_LOG, "\n ::: Se planificara al entrenador nro: %i ::: \n",trainer->tid);
				sem_post(&(trainer->semaforoDeEntrenador));
			}
		}
	}
	log_info(TEAM_LOG, "TEAM ha cumplido su objetivo!");
	finalFeliz();
}
void SJFSD(){
	CICLOS_TOTALES = 0;
	sem_init(&semaforoTermine,0,0);
	while(!objetivoGlobalCumplido()){
		esperarAlgunoEnReady(false);
		if(objetivoGlobalCumplido())
			break;
		ordenarListaSJF(EstadoReady);
		CAMBIOS_DE_CONTEXTO_REALIZADOS = CAMBIOS_DE_CONTEXTO_REALIZADOS + 2;
		sem_wait(&semaforoCambioEstado);
		entrenador *trainer = list_get(EstadoReady,0);
		sem_post(&semaforoCambioEstado);
		log_info(TEAM_LOG, "Se planificara al entrenador nro: %i",trainer->tid);
		sem_post(&(trainer->semaforoDeEntrenador));
		sem_wait(&semaforoTermine);
		log_info(TEAM_LOG, "Se va a intercambiar al entrenador nro: %i porque termino sus rafagas", trainer->tid);
	}
	//Manejo de Deadlock
	DEADLOCKS_PRODUCIDOS = list_size(EstadoBlock);
	log_info(TEAM_LOG, "La cantidad de entrenadores actuales en deadlock es: %i\nSe procedera a manejar estos deadlock a continuacion \n", list_size(EstadoBlock));
	while(!teamCumplioSuObjetivo()){
		planificarDeadlocks();
		esperarAlgunoEnReady(true);
		ordenarListaSJF(EstadoReady);
		CAMBIOS_DE_CONTEXTO_REALIZADOS = CAMBIOS_DE_CONTEXTO_REALIZADOS + 2;
		sem_wait(&semaforoCambioEstado);
		entrenador *trainer = list_get(EstadoReady,0);
		sem_post(&semaforoCambioEstado);
		log_info(TEAM_LOG, "Se planificara al entrenador nro: %i",trainer->tid);
		sem_post(&(trainer->semaforoDeEntrenador));
		sem_wait(&semaforoTermine);
		log_info(TEAM_LOG, "Se va a intercambiar al entrenador nro: %i porque termino sus rafagas", trainer->tid);
	}
	log_info(TEAM_LOG, "TEAM ha cumplido su objetivo!");
	finalFeliz();
}

void analizarDeadlockEspecifico(entrenador *trainer){
	if(trainer == NULL)
		return;
	if(trainer->razonBloqueo == t_ESPERANDO_INTERCAMBIO)
		return;
	char **pokemonFaltante = quePokemonMeFalta(trainer);
	if(pokemonFaltante[0] == NULL){
		sacarMision(trainer->tid);
		pasarEntrenadorAEstado(trainer->tid, t_EXIT);
		printf("El entrenador: %i cumplio su objetivo! Yupiiii!!!! \n", trainer->tid);
		free(pokemonFaltante);
		DEADLOCKS_RESUELTOS = DEADLOCKS_RESUELTOS + 1;
		return;
	}
	free(pokemonFaltante);
	trainer->razonBloqueo = t_DEADLOCK;
}

void planificarDeadlocks(){
	int cantidadEntrenadoresEnDeadlock = list_size(EstadoBlock);

	if(cantidadEntrenadoresEnDeadlock == 0 || hayAlgunaMision())
		return;

	entrenador *trainer1;
	entrenador *trainer2;
	char **pokemonFaltante = NULL;
	char **pokemonSobrante = NULL;
	char* pokemonParaIntercambiar = NULL;
	for(int i=0; i<cantidadEntrenadoresEnDeadlock; i++){
		trainer1 = list_get(EstadoBlock,i);
		analizarDeadlockEspecifico(trainer1);
		pokemonFaltante = quePokemonMeFalta(trainer1);
//		if(pokemonFaltante[0] == NULL){
//			darMision(trainer1->tid, "TERMINATE" , trainer1->posicion, false, -1);
//			pasarEntrenadorAEstado(trainer1->tid, t_READY);
//			sem_post(&(trainer1->semaforoDeEntrenador));
//			return;
//		}
		if(trainer1->razonBloqueo == t_DEADLOCK && pokemonFaltante[0] != NULL){
			for(int j=0; j<cantidadEntrenadoresEnDeadlock; j++){
				trainer2 = list_get(EstadoBlock,j);
				analizarDeadlockEspecifico(trainer2);
				pokemonSobrante = NULL;
				pokemonSobrante = quePokemonTengoDeMas(trainer2);
				pokemonParaIntercambiar = (char*)primerElementoEnComun(pokemonFaltante,pokemonSobrante);
				if( pokemonParaIntercambiar != NULL && trainer2->razonBloqueo == t_DEADLOCK){
					darMision(trainer1->tid, pokemonParaIntercambiar, trainer2->posicion, true, trainer2->tid);
					pasarEntrenadorAEstado(trainer1->tid, t_READY);
					bloquearEntrenador(trainer2->tid,t_ESPERANDO_INTERCAMBIO);
					free(pokemonFaltante);
					free(pokemonSobrante);
					return;
				}
				free(pokemonSobrante);
			}
		}
		free(pokemonFaltante);
	}
	return;
}

void ordenarListaSJF(t_list *lista){
	log_info(TEAM_LOG, "Se reordena la lista de ready SJF");
	list_sort(lista, (void*)entrenador1MenorEstimacionQueEntrenador2);
	list_map(lista, (void*)establecerNuevaEstimacion);
}

void establecerNuevaEstimacion(entrenador* trainer){
	trainer->ciclosCPUEstimados = nuevaEstimacion(trainer);
}

int nuevaEstimacion(entrenador *trainer){
	return ( (TEAM_CONFIG.ALPHA * trainer->ciclosCPUEjeutadosAnteriormente) + ( (1-TEAM_CONFIG.ALPHA) * trainer->ciclosCPUEstimados) ) ;
}

bool entrenador1MenorEstimacionQueEntrenador2(entrenador* trainer1, entrenador* trainer2){
	int estimacionTrainer1 = nuevaEstimacion(trainer1);
	int estimacionTrainer2 = nuevaEstimacion(trainer2);
	if(estimacionTrainer1 <= estimacionTrainer2){
		return true;
	}
	else{
		return false;
	}
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
	int unPokemon = 0;
	entrenador * trainer = list_get(ENTRENADORES_TOTALES, j);
	while(trainer != NULL){
		for (int i = 0; trainer->pokemonesObjetivo[i] != NULL; i++){
			unPokemon = (int)dictionary_get(OBJETIVO_GLOBAL, trainer->pokemonesObjetivo[i]);
			unPokemon = unPokemon +1;
			sem_wait(&semaforoDiccionario);
			dictionary_put(OBJETIVO_GLOBAL, trainer->pokemonesObjetivo[i], unPokemon);
			sem_post(&semaforoDiccionario);
		}
		descontarPokemonsActualesDeOBJGlobal(trainer);
		j++;
		trainer = list_get(ENTRENADORES_TOTALES, j);
	}
	printf("Objetivo global setteado \n");
}

bool teamCumplioSuObjetivo(){
	return (( objetivoGlobalCumplido() ) && ( todosLosEntrenadoresCumplieronObjetivo() ));
}

bool objetivoGlobalCumplido(){
	sem_wait(&semaforoDiccionario);
	bool response = dictionary_is_empty(OBJETIVO_GLOBAL);
	sem_post(&semaforoDiccionario);
	return response;
}

bool todosLosEntrenadoresCumplieronObjetivo(){
	return list_all_satisfy(ENTRENADORES_TOTALES, (void*)entrenadorCumplioObjetivo);
}

bool entrenadorCumplioObjetivo(entrenador* trainer){
	return (sonIgualesSinInportarOrden(trainer->pokemones,trainer->pokemonesObjetivo) == 1);
}

