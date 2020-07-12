#include "Broker.h"

int main(void) {
	Init();
	EsperarClientes();
	return EXIT_SUCCESS;
}

void EsperarClientes(){
	int server = iniciar_servidor(BROKER_CONFIG.IP_BROKER, BROKER_CONFIG.PUERTO_BROKER, LOGGER_GENERAL);
	while(1){
		int cliente = esperar_cliente_con_accept(server, LOGGER_GENERAL);
		log_info(LOGGER_GENERAL, "Se conecto un cliente: %i", cliente);
		log_info(LOGGER_OBLIGATORIO, "Se conecto un cliente: %i", cliente);
		pthread_t* hiloDeAtencion = malloc(sizeof(pthread_t));
		//Falta un sem acá
		pthread_create(hiloDeAtencion, NULL, AtenderCliente, cliente);
	}
}

void* AtenderCliente(void* cliente) {
	while(1){
		HeaderDelibird headerRecibido =  Serialize_RecieveHeader(cliente);
		if(headerRecibido.tipoMensaje == -1){
			log_error(LOGGER_GENERAL, "Se desconecto el cliente %i: \n", cliente);
			return (NULL);
		}
		ActuarAnteMensaje(headerRecibido, cliente);
	}
}

void ActuarAnteMensaje(HeaderDelibird header, int cliente){
	switch (header.tipoMensaje) {
		case d_NEW_POKEMON:
			log_info(LOGGER_GENERAL, "Llego un new pokemon");
			void* packNewPokemon = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			tratarMensajeNewASuscriptores (packNewPokemon, SUSCRIPTORES_NEW);
			free(packNewPokemon);
			break;
		case d_CATCH_POKEMON:
			log_info(LOGGER_GENERAL, "Llego un catch pokemon");
			void* packCatchPokemon = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			enviarMensajeCatchASuscriptores (packCatchPokemon, SUSCRIPTORES_CATCH);
			free(packCatchPokemon);
			break;
		case d_GET_POKEMON:
			log_info(LOGGER_GENERAL, "Llego un get pokemon");
			void* packGetPokemon = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			enviarMensajeGetASuscriptores (packGetPokemon, SUSCRIPTORES_GET);
			free(packGetPokemon);
			break;
		case d_APPEARED_POKEMON:
			log_info(LOGGER_GENERAL, "Llego un appeared pokemon");
			void* packAppearedPokemon = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			enviarMensajeAppearedASuscriptores (packAppearedPokemon, SUSCRIPTORES_APPEARED);
			free(packAppearedPokemon);
			break;
		case d_CAUGHT_POKEMON:
			log_info(LOGGER_GENERAL, "Llego un caught pokemon");
			void* packCaughtPokemon = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			enviarMensajeCaughtASuscriptores (packCaughtPokemon, SUSCRIPTORES_CAUGHT);
			free(packCaughtPokemon);
			break;
		case d_LOCALIZED_POKEMON:
			log_info(LOGGER_GENERAL, "Llego un localized pokemon");
			void* packLocalizedPokemon = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			/*uint32_t idMensajeLocalized;
			char *localizedNombrePokemon;
			t_list * poscant;
			Serialize_Unpack_LocalizedPokemon(packLocalizedPokemon, &idMensajeLocalized, localizedNombrePokemon,poscant);
			log_info(LOGGER_GENERAL,"Me llego mensaje de %i. Id: %i, Pkm: %s\n", cliente, idMensajeGet, getNombrePokemon);
			enviarMensajeLocalizedASuscriptores (packLocalizedPokemon, SUSCRIPTORES_LOCALIZED);
			free(packLocalizedPokemon);*/
			break;
		case d_ACK:
			log_info(LOGGER_GENERAL, "Llego un acknowledged");
			void* packAcknowledge = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			uint32_t idMensajeACK, resultadoACK;
			Serialize_Unpack_ACK(packAcknowledge);
			log_info(LOGGER_GENERAL,"Me llego mensaje de %i. Id: %i, Resultado: %i\n", cliente, idMensajeACK, resultadoACK);
			Serialize_PackAndSend_ACK(cliente, idMensajeACK); ///definir nosotros el id
			free(packAcknowledge);
			break;
		case d_SUBSCRIBE_QUEUE:
			log_info(LOGGER_GENERAL, "Llego un Subscribe");
			void * recibir = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			uint32_t variable = Serialize_Unpack_ACK(recibir);
			suscribir(variable, cliente);

			free(recibir);
			break;
		default:
			log_error(LOGGER_GENERAL, "Mensaje no entendido: %i\n", header);
			void* packBasura = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			free(packBasura);
			break;
	}
}

void suscribir(uint32_t variable, int cliente){
	switch (variable){
	case d_NEW_POKEMON:
		log_info (LOGGER_GENERAL, "Se agrego el suscriptor %i a la cola de NEW", cliente);
		list_add (SUSCRIPTORES_NEW, cliente);
		if (list_is_empty (ADMINISTRADOR_MEMORIA)){
			log_info (LOGGER_GENERAL, "No hay mensajes anteriore");
		}
		else{
		enviarMensajesCacheNew (cliente);
		}
		break;
	case d_CATCH_POKEMON:
		log_info (LOGGER_GENERAL, "Se agrego el suscriptor %i a la cola de CATCH", cliente);
		list_add (SUSCRIPTORES_CATCH, cliente);
		break;
	case d_GET_POKEMON:
		log_info (LOGGER_GENERAL, "Se agrego el suscriptor %i a la cola de GET", cliente);
		list_add (SUSCRIPTORES_GET, cliente);
		break;
	case d_APPEARED_POKEMON:
		log_info (LOGGER_GENERAL, "Se agrego el suscriptor %i a la cola de Appeared", cliente);
		list_add (SUSCRIPTORES_APPEARED, cliente);
		break;
	case d_CAUGHT_POKEMON:
		log_info (LOGGER_GENERAL, "Se agrego el suscriptor %i a la cola de Caught", cliente);
		list_add (SUSCRIPTORES_CAUGHT, cliente);
		break;
	case d_LOCALIZED_POKEMON:
		log_info (LOGGER_GENERAL, "Se agrego el suscriptor %i a la cola de Localized", cliente);
		list_add (SUSCRIPTORES_LOCALIZED, cliente);
		break;
	default:
		log_info (LOGGER_GENERAL, "No se suscribio");
		break;
	}
}

void enviarMensajesCacheNew (int cliente){
	estructuraAdministrativa cache;
	int lenght = list_size(ADMINISTRADOR_MEMORIA);
	for (int i = 0; i<lenght; i++){
	 int mensajeCache = list_get(ADMINISTRADOR_MEMORIA, i);
	 Serialize_PackAndSend_ACK(cliente, mensajeCache);
	 log_info (LOGGER_GENERAL, "Se envió el mensaje al suscriptor %i", cliente);
	}
}



void tratarMensajeNewASuscriptores (void *paquete, t_list* lista){
	estructuraAdministrativa * resultado = malloc (sizeof(estructuraAdministrativa));
	uint32_t posX, posY, cantidad;
	char *pokemon;
	Serialize_Unpack_NewPokemon_NoID(paquete, &pokemon, &posX, &posY, &cantidad);
	log_info(LOGGER_GENERAL,"Me llego mensaje new Pkm: %s, x: %i, y: %i, cant: %i\n", pokemon, posX, posY, cantidad);
	int ID = obtenerID();
	newEnMemoria mensajeAGuardar;
	mensajeAGuardar.cantidad = cantidad;
	mensajeAGuardar.largoDeNombre = string_length(pokemon);
	mensajeAGuardar.nombrePokemon = pokemon;
	mensajeAGuardar.posX = posX;
	mensajeAGuardar.posY = posY;
	resultado = guardarMensaje(d_NEW_POKEMON, &mensajeAGuardar);
	if (resultado){
		resultado->idMensaje = ID;
		resultado->tipoMensaje = d_NEW_POKEMON;
		if(string_equals_ignore_case(BROKER_CONFIG.ALGORITMO_MEMORIA, "particiones")){
		resultado->tamanioParticion = tamanioDeMensaje(d_NEW_POKEMON, &mensajeAGuardar);
		}
		resultado->estaOcupado = 1;
		resultado->donde = resultado;
		resultado->suscriptoresConACK = list_create();
		resultado->suscriptoresConMensajeEnviado = list_create();
		resultado->tiempo = temporal_get_string_time();
		resultado->ultimaReferencia = temporal_get_string_time();
		memcpy(resultado->donde, &mensajeAGuardar, tamanioDeMensaje(d_NEW_POKEMON, &mensajeAGuardar));
		log_info(LOGGER_GENERAL, "Se guardo el mensaje en la memoria");
	}
	int lenght = list_size(lista);
	for (int i = 0; i<lenght; i++){
		int socketCliente = list_get(lista, i);
		Serialize_PackAndSend_NEW_POKEMON(socketCliente, ID, pokemon, posX, posY, cantidad);
		actualizarEnviadosPorID(ID, socketCliente);
		log_info (LOGGER_GENERAL, "Se envió el mensaje %i al suscriptor %i", ID, socketCliente);
	}
	log_info(LOGGER_GENERAL, "No hay mas suscriptores! \n");

}


void enviarMensajeCatchASuscriptores (void* paquete, t_list* lista){
	estructuraAdministrativa cache;
	cache.suscriptoresConMensajeEnviado = list_create();
	uint32_t posicionCatchX, posicionCatchY;
	char *catchNombrePokemon;
	Serialize_Unpack_CatchPokemon_NoID(paquete, &catchNombrePokemon, &posicionCatchX, &posicionCatchY);
	log_info(LOGGER_GENERAL,"Me llego mensaje catch, Pkm: %s, x: %i, y: %i\n", catchNombrePokemon, posicionCatchX, posicionCatchY);
	int ID = obtenerID();
	int lenght = list_size(lista);
	for (int i = 0; i<lenght; i++){
		int socketCliente = list_get(lista, i);
		Serialize_PackAndSend_CATCH_POKEMON_NoID(socketCliente, catchNombrePokemon, posicionCatchX, posicionCatchY);
		log_info (LOGGER_GENERAL, "Se envió el mensaje %i al suscriptor %i\n", ID, socketCliente);
		list_add (cache.suscriptoresConMensajeEnviado, &socketCliente);
}
	cache.idMensaje = ID;
	cache.tipoMensaje = d_CATCH_POKEMON;
	list_add(ADMINISTRADOR_MEMORIA, &cache);
	log_info(LOGGER_GENERAL, "No hay suscriptores");
}

void enviarMensajeGetASuscriptores (void* paquete, t_list* lista){
	estructuraAdministrativa cache;
	cache.suscriptoresConMensajeEnviado = list_create();
	uint32_t idMensajeGet;
	char *getNombrePokemon;
	Serialize_Unpack_GetPokemon_NoID(paquete, &getNombrePokemon);
	log_info(LOGGER_GENERAL,"Me llego mensaje get, Pkm: %s\n", getNombrePokemon);
	int ID = obtenerID();
	int lenght = list_size(lista);
	for (int i = 0; i<lenght; i++){
		int socketCliente = list_get(lista, i);
		Serialize_PackAndSend_GET_POKEMON_NoID(socketCliente, getNombrePokemon);
		log_info (LOGGER_GENERAL, "Se envió el mensaje %i al suscriptor %i\n", ID, socketCliente);
		list_add (cache.suscriptoresConMensajeEnviado, &socketCliente);
	}
	cache.idMensaje = ID;
	cache.tipoMensaje = d_GET_POKEMON;
	list_add(ADMINISTRADOR_MEMORIA, &cache);
	log_info(LOGGER_GENERAL, "No hay suscriptores");
}

void enviarMensajeAppearedASuscriptores (void* paquete, t_list* lista){
	estructuraAdministrativa cache;
	cache.suscriptoresConMensajeEnviado = list_create();
	uint32_t idMensajeAppeared, posicionX, posicionY;
	char *nombrePokemon;
	Serialize_Unpack_AppearedPokemon(paquete, &idMensajeAppeared, &nombrePokemon, &posicionX, &posicionY);
	log_info(LOGGER_GENERAL,"Me llego mensaje appeared Id: %i, Pkm: %s, x: %i, y: %i\n", idMensajeAppeared, nombrePokemon, posicionX, posicionY);
	int ID = obtenerID();
	int lenght = list_size(lista);
	for (int i = 0; i<lenght; i++){
		int socketCliente = list_get(lista, i);
		Serialize_PackAndSend_APPEARED_POKEMON(socketCliente, &idMensajeAppeared, nombrePokemon, posicionX, posicionY);
		log_info (LOGGER_GENERAL, "Se envió el mensaje al suscriptor %i\n", socketCliente);
		list_add (cache.suscriptoresConMensajeEnviado, &socketCliente);
	}
	cache.idMensaje = ID;
	cache.tipoMensaje = d_APPEARED_POKEMON;
	list_add(ADMINISTRADOR_MEMORIA, &cache);
	log_info(LOGGER_GENERAL, "No hay suscriptores");
}

void enviarMensajeCaughtASuscriptores (void* paquete, t_list* lista){
	estructuraAdministrativa cache;
	cache.suscriptoresConMensajeEnviado = list_create();
	uint32_t idMensajeCaught, resultado;
	Serialize_Unpack_CaughtPokemon(paquete, idMensajeCaught, resultado);
	log_info(LOGGER_GENERAL,"Me llego mensaje caught Id: %i, Resultado: %i\n", idMensajeCaught, resultado);
	int lenght = list_size(lista);
	int ID = obtenerID();
	for (int i = 0; i<lenght; i++){
		int socketCliente = list_get(lista, i);
		Serialize_PackAndSend_CAUGHT_POKEMON(socketCliente, idMensajeCaught, resultado); ////hacer funcion sin id
		log_info (LOGGER_GENERAL, "Se envió el mensaje %i al suscriptor %i\n", ID, socketCliente);
		list_add (cache.suscriptoresConMensajeEnviado, &socketCliente);
	}
	cache.idMensaje = ID;
	cache.tipoMensaje = d_CAUGHT_POKEMON;
	list_add(ADMINISTRADOR_MEMORIA, &cache);
	log_info(LOGGER_GENERAL, "No hay suscriptores");
}

///////FUNCIONES DE INIT///////////
void Init(){
	LOGGER_GENERAL = iniciar_log("Broker");
	ConfigInit();
	log_info(LOGGER_GENERAL, "Configuracion broker levantada!");
	LOGGER_OBLIGATORIO = iniciar_log(BROKER_CONFIG.LOG_FILE);
	ListsInit();
	MemoriaPrincipalInit();
	SemaphoresInit();
}

void ConfigInit(){
	t_config* configCreator = config_create("/home/utnso/workspace/tp-2020-1c-ManaOS-/Broker/Broker.config");
	BROKER_CONFIG.ALGORITMO_REEMPLAZO = config_get_string_value(configCreator, "ALGORITMO_REEMPLAZO");
	BROKER_CONFIG.ALGORITMO_MEMORIA = config_get_string_value(configCreator, "ALGORITMO_MEMORIA");
	BROKER_CONFIG.ALGORITMO_PARTICION_LIBRE = config_get_string_value(configCreator, "ALGORITMO_PARTICION_LIBRE");
	BROKER_CONFIG.FRECUENCIA_COMPACTACION = config_get_int_value(configCreator, "FRECUENCIA_COMPACTACION");
	BROKER_CONFIG.IP_BROKER = config_get_string_value(configCreator, "IP_BROKER");
	BROKER_CONFIG.LOG_FILE = config_get_string_value(configCreator, "LOG_FILE");
	BROKER_CONFIG.PUERTO_BROKER = config_get_string_value(configCreator, "PUERTO_BROKER");
	BROKER_CONFIG.TAMANO_MEMORIA = config_get_int_value(configCreator, "TAMANO_MEMORIA");
	BROKER_CONFIG.TAMANO_MINIMO_PARTICION = config_get_int_value(configCreator, "TAMANO_MINIMO_PARTICION");
}

void ListsInit(){
	CONEXIONES = list_create();
	SUSCRIPTORES_NEW = list_create();
	SUSCRIPTORES_APPEARED = list_create();
	SUSCRIPTORES_GET = list_create();
	SUSCRIPTORES_CATCH = list_create();
	SUSCRIPTORES_CAUGHT = list_create();
	SUSCRIPTORES_LOCALIZED = list_create();
	ADMINISTRADOR_MEMORIA = list_create();
}

void MemoriaPrincipalInit(){
		estructuraAdministrativa * estructura = malloc (sizeof (estructuraAdministrativa));
		MEMORIA_PRINCIPAL = malloc(BROKER_CONFIG.TAMANO_MEMORIA);
		estructura->estaOcupado = 0;
		estructura->tamanioParticion = BROKER_CONFIG.TAMANO_MEMORIA;
		list_add (ADMINISTRADOR_MEMORIA, estructura);
		estructura = list_get(ADMINISTRADOR_MEMORIA, 0);
}

void SemaphoresInit(){
	sem_init(&MUTEX_CLIENTE, 0, 1);
	sem_init(&MUTEX_CONTADOR, 0, 1);
	sem_init(&MUTEX_MEMORIA, 0, 1);
}

/////////FUNCIONES VARIAS/////////
int obtenerID(){
	sem_wait(&MUTEX_CONTADOR);
	return CONTADOR++;
	sem_post(&MUTEX_CONTADOR);
}

//////FUNCIONES CACHE//////////
estructuraAdministrativa * guardarMensaje(d_message mensaje, void * mensajeAGuardar){
	estructuraAdministrativa* donde = malloc (sizeof (estructuraAdministrativa));
	if(string_equals_ignore_case(BROKER_CONFIG.ALGORITMO_MEMORIA, "particiones")){
	//donde = buscarParticionLibrePara(sizeof(mensajeAGuardar));
	//memcpy(donde, mensajeAGuardar, sizeof(mensajeAGuardar));
	}
	if(string_equals_ignore_case(BROKER_CONFIG.ALGORITMO_MEMORIA, "bs")){
	donde = buscarParticionLibreBS(mensaje, mensajeAGuardar);
	return donde;
	}
}

void * buscarParticionLibrePara(int tamanioMensajeAGuardar){
	int i = BROKER_CONFIG.TAMANO_MEMORIA;
	int j = 0, k = -1;
	estructuraAdministrativa* estructura = malloc(sizeof(estructuraAdministrativa));
	if(string_equals_ignore_case(BROKER_CONFIG.ALGORITMO_PARTICION_LIBRE, "ff")){
		bool hayParticionParaGuardarlo(estructuraAdministrativa* elemento) {
			return (elemento->estaOcupado == 0 && elemento->tamanioParticion >= tamanioMensajeAGuardar);
		}
		estructura =  list_find(ADMINISTRADOR_MEMORIA, (void*) hayParticionParaGuardarlo);
	}
	if(string_equals_ignore_case(BROKER_CONFIG.ALGORITMO_PARTICION_LIBRE, "bf")){
		void mejorParticion(estructuraAdministrativa* elemento) {
			int tam = elemento->tamanioParticion - tamanioMensajeAGuardar;
			if(elemento->estaOcupado == 0 && tam < i){
				i = tam;
				k = j;
			}
			j++;
		}
		list_iterate(ADMINISTRADOR_MEMORIA, (void*) mejorParticion);
		estructura =  list_get(ADMINISTRADOR_MEMORIA, k);
	}
	if (estructura != NULL){
		//respetando el tamaño mínimo, hacer la partición lo más chica posible
		return estructura->donde;
	}
	/*if(!compactorecien){
		compactar();
	}
	if(!reemplazorecien){
	reemplazar();
	}
	buscarParticionLibrePara();*/
}

void actualizarEnviadosPorID(int id, int socketCliente){
	estructuraAdministrativa* unaEstructura = buscarEstructuraAdministrativaConID(id);
	list_add(unaEstructura->suscriptoresConMensajeEnviado, &socketCliente);
	return;
}

estructuraAdministrativa* buscarEstructuraAdministrativaConID(int id){
	bool _is_the_one(estructuraAdministrativa* p) {
		return (p->idMensaje = id);
	}

	return list_find(ADMINISTRADOR_MEMORIA, (void*) _is_the_one);
}

int tamanioDeMensaje(d_message tipoMensaje, void * unMensaje){
	switch(tipoMensaje){
	case d_NEW_POKEMON:
		return (((newEnMemoria *)unMensaje)->largoDeNombre * 4 + 16);
		break;
	/*case d_CATCH_POKEMON:
		break;
	case d_GET_POKEMON:
		break;
	case d_APPEARED_POKEMON:
		break;
	case d_CAUGHT_POKEMON:
		break;
	case d_LOCALIZED_POKEMON:
		break;
	case d_ACK:
		break;
	case d_SUBSCRIBE_QUEUE:
		break;*/
	default:
		log_error(LOGGER_GENERAL, "Malardo");
		return 0;
	}
}

/////////////BUDDY SYSTEM/////////////////
estructuraAdministrativa* buscarParticionLibreBS(d_message tipoMensaje, void* mensaje){
	estructuraAdministrativa* particion = malloc (sizeof (estructuraAdministrativa));
	int tamanioMensaje = tamanioDeMensaje(tipoMensaje, mensaje);
	int i = BROKER_CONFIG.TAMANO_MEMORIA;
	int j = 0, k = -1;
	///////////FIRST FIT ///////////////////
	if(string_equals_ignore_case(BROKER_CONFIG.ALGORITMO_PARTICION_LIBRE, "ff")){
		bool hayParticionParaGuardarlo(estructuraAdministrativa* elemento) {
				return (elemento->estaOcupado == 0 && elemento->tamanioParticion >= tamanioMensaje);
			}
		particion = list_find(ADMINISTRADOR_MEMORIA, (void*) hayParticionParaGuardarlo);
		}
	//////////BEST FIT///////////////////////
	if(string_equals_ignore_case(BROKER_CONFIG.ALGORITMO_PARTICION_LIBRE, "bf")){
		void mejorParticion(estructuraAdministrativa* elemento) {
			int tam = elemento->tamanioParticion - tamanioMensaje;
			if(!elemento->estaOcupado && tam < i){
				i = tam;
				k = j;
				}
				j++;
			}
			list_iterate(ADMINISTRADOR_MEMORIA, (void*) mejorParticion);
			particion =  list_get(ADMINISTRADOR_MEMORIA, k);

		}
		if (particion != NULL){
			log_info(LOGGER_GENERAL, "Encontre particion Libre");
			estructuraAdministrativa * particionDondeGuardar = malloc (sizeof(estructuraAdministrativa));
			particionDondeGuardar = particionAMedida(tipoMensaje, mensaje, particion);
			return particionDondeGuardar;
		}
		if(particion == NULL){
			log_info (LOGGER_GENERAL, "No hay espacio, es necesario componer");
			int seRealizoLaComposicion = composicion();
			log_info (LOGGER_GENERAL, "Se realizó la composición BS");
		}
		/*
		////buscar de nuevo
		reemplazar(tipoMensaje, mensaje);
		}
		//////////buscarParticionLibre();
		 */
}

int primeraParticion(){
	estructuraAdministrativa * particionMenor;
	estructuraAdministrativa * particion;
	int i;
	particionMenor->tiempo = list_get (ADMINISTRADOR_MEMORIA, 0); ///solo los que estan ocupados van a tener tiempo
	int particionesMemoria = list_size(ADMINISTRADOR_MEMORIA);
	for (i = 1; i < particionesMemoria; i++){
		particion->tiempo = list_get(ADMINISTRADOR_MEMORIA, i);
		if (particion->tiempo < particionMenor->tiempo){
			particionMenor->tiempo = particion->tiempo;
			particionMenor->donde = particion->donde;
			i ++;
		}
	}
	return i;
}

int particionMenosReferenciada(){
	estructuraAdministrativa * particionMenor = malloc (sizeof(estructuraAdministrativa));
	estructuraAdministrativa * particion = malloc (sizeof(estructuraAdministrativa));
	int i;
	particionMenor->ultimaReferencia = list_get (ADMINISTRADOR_MEMORIA, 0); ///solo los que estan ocupados van a tener tiempo
	int particionesMemoria = list_size(ADMINISTRADOR_MEMORIA);
	for (i = 1; i < particionesMemoria; i++){
		particion->ultimaReferencia = list_get(ADMINISTRADOR_MEMORIA, i);
		if (particion->ultimaReferencia < particionMenor->ultimaReferencia){
			particionMenor->ultimaReferencia = particion->ultimaReferencia;
			particionMenor->donde = particion->donde;
			i ++;
		}
	}
	return i;
}

void reemplazar (d_message tipoMensaje, void* mensaje){
	estructuraAdministrativa* particion = malloc (sizeof (estructuraAdministrativa));
	int tamanioMensaje = tamanioDeMensaje (tipoMensaje, mensaje);
	//////////FIFO////////////////////////////////////////
	if(string_equals_ignore_case(BROKER_CONFIG.ALGORITMO_REEMPLAZO, "fifo")){
		int posicion = primeraParticion();
		particion = list_get (ADMINISTRADOR_MEMORIA, posicion);
		if (particion->tamanioParticion > tamanioMensaje){
			list_add_in_index(ADMINISTRADOR_MEMORIA, particion, mensaje);
			return;
		}
	}
	if(string_equals_ignore_case(BROKER_CONFIG.ALGORITMO_REEMPLAZO, "lru")){
		int posicion = particionMenosReferenciada();
		particion = list_get (ADMINISTRADOR_MEMORIA, posicion);
		if (particion->tamanioParticion > tamanioMensaje){
		list_add_in_index(ADMINISTRADOR_MEMORIA, particion, mensaje);
		return;
		}
	}
	particion->idMensaje = NULL;
	list_clean(particion->suscriptoresConACK);
	list_clean (particion->suscriptoresConMensajeEnviado);
	particion->tiempo = NULL;
	//particion->tipoMensaje = NULL;
	particion->ultimaReferencia = NULL;
	particion->estaOcupado = 0;
	/////se usa despues para compactar
}

int composicion(){
	int flag;
	estructuraAdministrativa * particionActual = malloc (sizeof (estructuraAdministrativa));
	estructuraAdministrativa * particionAnterior = malloc (sizeof (estructuraAdministrativa));
	estructuraAdministrativa * particionPosterior = malloc (sizeof (estructuraAdministrativa));
	particionAnterior->donde = particionActual->donde - particionActual->tamanioParticion;
	particionPosterior->donde = particionActual->donde + particionActual->tamanioParticion;
	if (particionAnterior->estaOcupado == 0 && !particionActual->estaOcupado && particionAnterior->tamanioParticion == particionActual->tamanioParticion){
		particionActual->donde = particionAnterior->donde;
		particionActual->estaOcupado = 0;
		particionActual->tamanioParticion = particionAnterior->tamanioParticion + particionActual->tamanioParticion;
		if (!particionActual->estaOcupado && !particionPosterior->estaOcupado && particionActual->tamanioParticion == particionPosterior->tamanioParticion){
			particionActual->estaOcupado = 0;
			particionActual->tamanioParticion = particionActual->tamanioParticion + particionPosterior->tamanioParticion;
		}
	flag = 1;
	}
	else{
	flag = 0;
	}
	return flag;
}

estructuraAdministrativa * particionAMedida(d_message tipoMensaje, void*mensaje, estructuraAdministrativa* particion){
	estructuraAdministrativa * particionMinima = malloc (sizeof (estructuraAdministrativa));
	particionMinima->tamanioParticion = BROKER_CONFIG.TAMANO_MINIMO_PARTICION;
	int contar = contarTamanio();
	estructuraAdministrativa * particion2 = malloc (sizeof (estructuraAdministrativa)); // la que le sigue al actual
	estructuraAdministrativa * particion3 = malloc (sizeof (estructuraAdministrativa)); // la que le sigue al actual
	int tamanioMensaje = tamanioDeMensaje(tipoMensaje, mensaje);
	if (tamanioMensaje <= particionMinima->tamanioParticion){
		return particionMinima;
		}
	while (particion->tamanioParticion / 2 > tamanioMensaje && contar == BROKER_CONFIG.TAMANO_MEMORIA){
		estructuraAdministrativa * particionAuxiliar = malloc (sizeof (estructuraAdministrativa)); // la que le sigue al actual
		particionAuxiliar->tamanioParticion = particion->tamanioParticion / 2;
		particionAuxiliar->estaOcupado = 0;
		particionAuxiliar->donde =particion->donde + particionAuxiliar->tamanioParticion;
		particionAuxiliar->idMensaje = NULL;
		particionAuxiliar->suscriptoresConACK = list_create();
		particionAuxiliar->suscriptoresConMensajeEnviado = list_create();
		particionAuxiliar->tiempo = NULL;
		particionAuxiliar->ultimaReferencia = NULL;
		list_add_in_index(ADMINISTRADOR_MEMORIA, 0, particionAuxiliar);
		particion->estaOcupado = 0;
		particion->tamanioParticion = particionAuxiliar->tamanioParticion;
		particion = list_get (ADMINISTRADOR_MEMORIA, 0);
		particionAuxiliar = list_get (ADMINISTRADOR_MEMORIA, 1);
		particion2 = list_get (ADMINISTRADOR_MEMORIA, 2);
		particion3 = list_get (ADMINISTRADOR_MEMORIA, 3);
	}
	log_info (LOGGER_GENERAL, "Se toma la particion a medida");
	return particion;
}

int contarTamanio (){
	estructuraAdministrativa * particion = malloc (sizeof(estructuraAdministrativa));
	int i, contador = 0;
	int tamanio = list_size(ADMINISTRADOR_MEMORIA);
	for (i = 0; i < tamanio; i ++){
		particion = list_get (ADMINISTRADOR_MEMORIA, i);
		contador = contador + particion->tamanioParticion;
	}
	return contador;
}

void compactacion() {
	bool estaOcupado(estructuraAdministrativa* elemento) {
			return (elemento->estaOcupado==1);
		}
	t_list* listaAuxiliar = list_filter(ADMINISTRADOR_MEMORIA, (void*) estaOcupado);
	reposicionarParticionesOcupadas(listaAuxiliar);
}

void reposicionarParticionesOcupadas(t_list * listaAuxiliar){
	void * nuevoDonde = MEMORIA_PRINCIPAL;
	void cambiarInfo(estructuraAdministrativa* elemento) {
			memcpy(nuevoDonde, elemento->donde, elemento->tamanioParticion);
			elemento->donde = nuevoDonde;
			nuevoDonde = nuevoDonde + elemento->tamanioParticion;
			return;
		}
	list_iterate(listaAuxiliar, (void*) cambiarInfo);
	list_clean_and_destroy_elements(ADMINISTRADOR_MEMORIA, (void *)estructuraAdministrativaDestroyer);
	list_add_all(ADMINISTRADOR_MEMORIA, listaAuxiliar);
	estructuraAdministrativa * espacioFaltante = malloc (sizeof(estructuraAdministrativa));
	espacioFaltante->donde = nuevoDonde;
	espacioFaltante->estaOcupado = 0;
	espacioFaltante->suscriptoresConACK = list_create();
	espacioFaltante->suscriptoresConMensajeEnviado = list_create();
	espacioFaltante->tamanioParticion = BROKER_CONFIG.TAMANO_MEMORIA - contarTamanio();
	list_add(ADMINISTRADOR_MEMORIA, espacioFaltante);
}

static void estructuraAdministrativaDestroyer(estructuraAdministrativa *self) {
    free(self->donde);
    free(self->estaOcupado);
    free(self->idMensaje);
    free(self->tamanioParticion);
    free(self->tiempo);
    //free(self->tipoMensaje);
    free(self->ultimaReferencia);
    list_clean_and_destroy_elements(self->suscriptoresConACK, (void*)suscriptorDestroyer);
    list_destroy(self->suscriptoresConACK);
    list_clean_and_destroy_elements(self->suscriptoresConMensajeEnviado, (void*)suscriptorDestroyer);
    list_destroy(self->suscriptoresConMensajeEnviado);
    free(self);
}

static void suscriptorDestroyer(int *self) {
    free(self);
}


void * levantarMensaje(d_message tipoMensaje, void * lugarDeComienzo){
	void * punteroManipulable = lugarDeComienzo;
	newEnMemoria * retorno;
	catchEnMemoria * retornoCatch;
	getEnMemoria * retornoGet;
	appearedEnMemoria * retornoAppeared;
	caughtEnMemoria * retornoCaught;
	localizedEnMemoria * retornoLocalized;
	sem_wait(&MUTEX_MEMORIA);
	switch(tipoMensaje){
	case d_NEW_POKEMON:
		retorno = malloc(sizeof(newEnMemoria));
		retorno->nombrePokemon = string_new();
		memcpy(retorno->largoDeNombre, punteroManipulable, sizeof(uint32_t));
		punteroManipulable += sizeof(uint32_t);
		memcpy(retorno->nombrePokemon, punteroManipulable, retorno->largoDeNombre);
		punteroManipulable += (sizeof(char) * retorno->largoDeNombre);
		memcpy(retorno->posX, punteroManipulable, sizeof(uint32_t));
		punteroManipulable += (sizeof(uint32_t));
		memcpy(retorno->posY, punteroManipulable, sizeof(uint32_t));
		punteroManipulable += (sizeof(uint32_t));
		memcpy(retorno->cantidad, punteroManipulable, sizeof(uint32_t));
		sem_post(&MUTEX_MEMORIA);
		return retorno;
		break;
	case d_CATCH_POKEMON:
		retornoCatch = malloc(sizeof(catchEnMemoria));
		retornoCatch->nombrePokemon = string_new();
		memcpy(retornoCatch->largoDeNombre, punteroManipulable, sizeof(uint32_t));
		punteroManipulable += sizeof(uint32_t);
		memcpy(retornoCatch->nombrePokemon, punteroManipulable, retornoCatch->largoDeNombre);
		punteroManipulable += (sizeof(char) * retornoCatch->largoDeNombre);
		memcpy(retornoCatch->posX, punteroManipulable, sizeof(uint32_t));
		punteroManipulable += (sizeof(uint32_t));
		memcpy(retornoCatch->posY, punteroManipulable, sizeof(uint32_t));
		sem_post(&MUTEX_MEMORIA);
		return retornoCatch;
		break;
	case d_GET_POKEMON:
		retornoGet = malloc(sizeof(getEnMemoria));
		retornoGet->nombrePokemon = string_new();
		memcpy(retornoGet->largoDeNombre, punteroManipulable, sizeof(uint32_t));
		punteroManipulable += sizeof(uint32_t);
		memcpy(retornoGet->nombrePokemon, punteroManipulable, retornoGet->largoDeNombre);
		sem_post(&MUTEX_MEMORIA);
		return retornoGet;
		break;
	case d_APPEARED_POKEMON:
		retornoAppeared = malloc(sizeof(appearedEnMemoria));
		retornoAppeared->nombrePokemon = string_new();
		memcpy(retornoAppeared->largoDeNombre, punteroManipulable, sizeof(uint32_t));
		punteroManipulable += sizeof(uint32_t);
		memcpy(retornoAppeared->nombrePokemon, punteroManipulable, retornoAppeared->largoDeNombre);
		punteroManipulable += (sizeof(char) * retornoAppeared->largoDeNombre);
		memcpy(retornoAppeared->posX, punteroManipulable, sizeof(uint32_t));
		punteroManipulable += (sizeof(uint32_t));
		memcpy(retornoAppeared->posY, punteroManipulable, sizeof(uint32_t));
		sem_post(&MUTEX_MEMORIA);
		return retornoAppeared;
		break;
	case d_CAUGHT_POKEMON:
		retornoCaught = malloc(sizeof(caughtEnMemoria));
		memcpy(retornoCaught->atrapado, punteroManipulable, sizeof(uint32_t));
		sem_post(&MUTEX_MEMORIA);
		return retornoCaught;
		break;
	case d_LOCALIZED_POKEMON:
		retornoLocalized = malloc(sizeof(localizedEnMemoria));
		retornoLocalized->nombrePokemon = string_new();
		retornoLocalized->puntos = list_create();
		memcpy(retornoLocalized->largoDeNombre, punteroManipulable, sizeof(uint32_t));
		punteroManipulable += sizeof(uint32_t);
		memcpy(retornoLocalized->nombrePokemon, punteroManipulable, retornoLocalized->largoDeNombre);
		punteroManipulable += (sizeof(char) * retorno->largoDeNombre);
		memcpy(retornoLocalized->cantidadDePuntos, punteroManipulable, sizeof(uint32_t));
		punteroManipulable += (sizeof(uint32_t));
		for (int i = 0; i<retornoLocalized->cantidadDePuntos; i++){
			punto * nuevoPunto = malloc(sizeof(punto));
			memcpy(nuevoPunto->posX, punteroManipulable, sizeof(uint32_t));
			punteroManipulable += (sizeof(uint32_t));
			memcpy(nuevoPunto->posY, punteroManipulable, sizeof(uint32_t));
			punteroManipulable += (sizeof(uint32_t));
			list_add(retornoLocalized->puntos, nuevoPunto);
		}
		sem_post(&MUTEX_MEMORIA);
		return retornoLocalized;
		break;
	default:
		log_error(LOGGER_GENERAL, "Error levantando data de la memoria");
		sem_post(&MUTEX_MEMORIA);
		return NULL;
	}
}

void * leerInfoYActualizarUsoPorID(int id){
	bool igualID(estructuraAdministrativa* elemento) {
				return (elemento->idMensaje == id);
			}
	estructuraAdministrativa * ElElemento = list_find(ADMINISTRADOR_MEMORIA, (void*)igualID);
	ElElemento->ultimaReferencia = string_new();
	ElElemento->ultimaReferencia = temporal_get_string_time();
	return(levantarMensaje(ElElemento->tipoMensaje, ElElemento->donde));
}

