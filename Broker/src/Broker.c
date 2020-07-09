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
	uint32_t posX, posY, cantidad;
	char *pokemon;
	Serialize_Unpack_NewPokemon_NoID(paquete, &pokemon, &posX, &posY, &cantidad);
	log_info(LOGGER_GENERAL,"Me llego mensaje new Pkm: %s, x: %i, y: %i, cant: %i\n", pokemon, posX, posY, cantidad);
	int ID = obtenerID();
	cachearNew mensajeAGuardar;
	mensajeAGuardar.cantidad = cantidad;
	mensajeAGuardar.largoDeNombre = string_length(pokemon);
	mensajeAGuardar.nombrePokemon = pokemon;
	mensajeAGuardar.posX = posX;
	mensajeAGuardar.posY = posY;
	void*  resultado = guardarMensaje(&mensajeAGuardar);
	estructuraAdministrativa elemento;
	if (resultado){
		elemento.idMensaje = ID;
		elemento.tipoMensaje = d_NEW_POKEMON;
		elemento.tamanioParticion = tamanioDeMensaje(d_NEW_POKEMON, &mensajeAGuardar);
		elemento.estaOcupado = 1;
		elemento.donde = resultado;
		elemento.suscriptoresConACK = list_create();
		elemento.suscriptoresConMensajeEnviado = list_create();
		list_add(ADMINISTRADOR_MEMORIA, &elemento);
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

/*void enviarMensajeLocalizedASuscriptores (void* paquete, t_list* lista){
	uint32_t idMensaje;
	char *pokemon;
	t_list* poscant;
	int lenght = list_size(lista);
	for (int i = 0; i<lenght; i++){
		int socketCliente = list_get(lista, i);
		Serialize_PackAndSend_LOCALIZED_POKEMON(socketCliente, idMensaje, pokemon, poscant); //hacer funcion sin id
		log_info (LOGGER_GENERAL, "Se envió el mensaje al suscriptor %i\n", socketCliente);
	}
	log_info(LOGGER_GENERAL, "No hay suscriptores");
}*/


///////FUNCIONES DE INIT///////////
void Init(){
	LOGGER_GENERAL = iniciar_log("Broker");
	ConfigInit();
	log_info(LOGGER_GENERAL, "Configuracion broker levantada!");
	LOGGER_OBLIGATORIO = iniciar_log(BROKER_CONFIG.LOG_FILE);
	ListsInit();
	MemoriaPrincipalInit();
}

void ConfigInit(){
	t_config* configCreator = config_create("/home/utnso/tp-2020-1c-ManaOS-/Broker/Broker.config");
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
		MEMORIA_PRINCIPAL = malloc(BROKER_CONFIG.TAMANO_MEMORIA);
}

/////////FUNCIONES VARIAS/////////

int obtenerID(){
	//Falta semaforo
	return CONTADOR++;
}

//////FUNCIONES CACHE//////////
void * guardarMensaje(void * mensajeAGuardar){
	void * donde;
	donde = buscarParticionLibrePara(sizeof(mensajeAGuardar));
	memcpy(donde, mensajeAGuardar, sizeof(mensajeAGuardar));
	return donde;

}

void * buscarParticionLibrePara(int tamanioMensajeAGuardar){
	int i = BROKER_CONFIG.TAMANO_MEMORIA;
	int j = 0, k = -1;
	estructuraAdministrativa* estructura;
	if(string_equals_ignore_case(BROKER_CONFIG.ALGORITMO_PARTICION_LIBRE, "ff")){
	bool hayParticionParaGuardarlo(estructuraAdministrativa* elemento) {
		return (!elemento->estaOcupado && elemento->tamanioParticion >= tamanioMensajeAGuardar);
	}
	estructura =  list_find(ADMINISTRADOR_MEMORIA, (void*) hayParticionParaGuardarlo);
	}
	if(string_equals_ignore_case(BROKER_CONFIG.ALGORITMO_PARTICION_LIBRE, "bf")){
		void mejorParticion(estructuraAdministrativa* elemento) {
			int tam = elemento->tamanioParticion - tamanioMensajeAGuardar;
			if(!elemento->estaOcupado && tam < i){
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
		return (((cachearNew *)unMensaje)->largoDeNombre * 4 + 16);
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

int composicion(){
	int flag;
	estructuraAdministrativa * particionActual;
	estructuraAdministrativa * particionAnterior;
	estructuraAdministrativa * particionPosterior;
	particionAnterior->donde = particionActual->donde - particionActual->tamanioParticion;
	particionPosterior->donde = particionActual->donde + particionActual->tamanioParticion;
	if (!particionAnterior->estaOcupado && !particionActual->estaOcupado && particionAnterior->tamanioParticion == particionActual->tamanioParticion){
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






