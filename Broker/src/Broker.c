#include "Broker.h"

int main(void) {
	Init();
	signal(SIGUSR1, dump);
	signal(SIGINT, destruirTodo);
	EsperarClientes();
	return EXIT_SUCCESS;
}

void EsperarClientes(){
	int server = iniciar_servidor(BROKER_CONFIG.IP_BROKER, BROKER_CONFIG.PUERTO_BROKER, LOGGER_GENERAL);
	while(1){
		int cliente = esperar_cliente_con_accept(server, LOGGER_GENERAL);
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
			log_info(LOGGER_OBLIGATORIO, "Llego un new pokemon");
			void* packNewPokemon = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			tratarMensaje(header.tipoMensaje, packNewPokemon);
			free(packNewPokemon);
			break;
		case d_CATCH_POKEMON:
			log_info(LOGGER_OBLIGATORIO, "Llego un catch pokemon");
			void* packCatchPokemon = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			tratarMensaje(header.tipoMensaje, packCatchPokemon);
			free(packCatchPokemon);
			break;
		case d_GET_POKEMON:
			log_info(LOGGER_OBLIGATORIO, "Llego un get pokemon");
			void* packGetPokemon = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			tratarMensaje(header.tipoMensaje, packGetPokemon);
			free(packGetPokemon);
			break;
		case d_APPEARED_POKEMON:
			log_info(LOGGER_OBLIGATORIO, "Llego un appeared pokemon");
			void* packAppearedPokemon = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			tratarMensaje(header.tipoMensaje, packAppearedPokemon);
			free(packAppearedPokemon);
			break;
		case d_CAUGHT_POKEMON:
			log_info(LOGGER_OBLIGATORIO, "Llego un caught pokemon");
			void* packCaughtPokemon = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			tratarMensaje(header.tipoMensaje, packCaughtPokemon);
			free(packCaughtPokemon);
			break;
		case d_LOCALIZED_POKEMON:
			log_info(LOGGER_OBLIGATORIO, "Llego un localized pokemon");
			//tratarMensaje(header.tipoMensaje, packCaughtPokemon);
			break;
		case d_ACK:
			log_info(LOGGER_GENERAL, "Llego un acknowledged");
			void* packACK = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			tratarMensajeACK (packACK, cliente);
			free(packACK);
			break;
		case d_SUBSCRIBE_QUEUE:
			log_info(LOGGER_GENERAL, "Llego un Subscribe");
			void * recibir = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			uint32_t variable = Serialize_Unpack_ACK(recibir);
			//funcionParaVerMemoria();
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

void suscribir(uint32_t variable, int clienteA){
	int * cliente = malloc(sizeof(int));
	*cliente=clienteA;
	switch (variable){
	case d_NEW_POKEMON:
		log_info (LOGGER_OBLIGATORIO, "Se agrego el suscriptor %i a la cola de NEW", *cliente);
		list_add (SUSCRIPTORES_NEW, cliente);
		if (list_is_empty (ADMINISTRADOR_MEMORIA)){
			log_info (LOGGER_GENERAL, "No hay mensajes anteriores");
		}
		else{
			enviarVariosMensajes(cliente, d_NEW_POKEMON);
		}
		break;
	case d_CATCH_POKEMON:
		log_info (LOGGER_OBLIGATORIO, "Se agrego el suscriptor %i a la cola de CATCH", *cliente);
		list_add (SUSCRIPTORES_CATCH, cliente);
		if (list_is_empty (ADMINISTRADOR_MEMORIA)){
			log_info (LOGGER_GENERAL, "No hay mensajes anteriores");
			}
			else{
				enviarVariosMensajes(cliente, d_CATCH_POKEMON);
			}
		break;
	case d_GET_POKEMON:
		log_info (LOGGER_OBLIGATORIO, "Se agrego el suscriptor %i a la cola de GET", *cliente);
		list_add (SUSCRIPTORES_GET, cliente);
		if (list_is_empty (ADMINISTRADOR_MEMORIA)){
			log_info (LOGGER_GENERAL, "No hay mensajes anteriores");
			}
			else{
				enviarVariosMensajes(cliente, d_GET_POKEMON);
			}
		break;
	case d_APPEARED_POKEMON:
		log_info (LOGGER_OBLIGATORIO, "Se agrego el suscriptor %i a la cola de Appeared", *cliente);
		list_add (SUSCRIPTORES_APPEARED, cliente);
		if (list_is_empty (ADMINISTRADOR_MEMORIA)){
			log_info (LOGGER_GENERAL, "No hay mensajes anteriores");
			}
			else{
				enviarVariosMensajes(cliente, d_APPEARED_POKEMON);
			}
		break;
	case d_CAUGHT_POKEMON:
		log_info (LOGGER_OBLIGATORIO, "Se agrego el suscriptor %i a la cola de Caught", *cliente);
		list_add (SUSCRIPTORES_CAUGHT, cliente);
		if (list_is_empty (ADMINISTRADOR_MEMORIA)){
			log_info (LOGGER_GENERAL, "No hay mensajes anteriores");
			}
			else{
				enviarVariosMensajes(cliente, d_CAUGHT_POKEMON);
			}
		break;
	case d_LOCALIZED_POKEMON:
		log_info (LOGGER_OBLIGATORIO, "Se agrego el suscriptor %i a la cola de Localized", *cliente);
		list_add (SUSCRIPTORES_LOCALIZED, cliente);
		if (list_is_empty (ADMINISTRADOR_MEMORIA)){
			log_info (LOGGER_GENERAL, "No hay mensajes anteriores");
			}
			else{
				enviarVariosMensajes(cliente, d_LOCALIZED_POKEMON);
			}
		break;
	default:
		log_error(LOGGER_OBLIGATORIO, "No se suscribio");
		break;
	}
}
///////////////TRATAR MENSAJES/////////////////

void tratarMensajeACK (void* paquete, int cliente){
	uint32_t idMensaje;
	log_info (LOGGER_OBLIGATORIO, "Se agrego el suscriptor %i a la cola de ACK del mensaje %i ", cliente, idMensaje);
	Serialize_Unpack_ACK(paquete);
	actualizarRecibidosPorID(idMensaje, cliente);
}

static void destruir(estructuraAdministrativa *estructura) {
    free(estructura);
}

/////////////////ENVIAR MENSAJE A SUSCRIPTORES/////////////////////////////////////
void enviarVariosMensajes(int * clienteA, d_message tipoMensaje){
	int * cliente = malloc(sizeof(int));
	cliente=clienteA;
	estructuraAdministrativa * elemento = malloc (sizeof(estructuraAdministrativa));
	t_list * mensajesNew = list_create();
	t_list * mensajesCatch = list_create();
	t_list * mensajesAppeared = list_create();
	t_list * mensajesGet = list_create();
	t_list* mensajesCaught = list_create();
	t_list * mensajesLocalized = list_create();
	newEnMemoria* mensajeNew;
	catchEnMemoria * mensajeCatch;
	appearedEnMemoria * mensajeAppeared;
	getEnMemoria* mensajeGet;
	caughtEnMemoria* mensajeCaught;
	localizedEnMemoria* mensajeLocalized;
	switch (tipoMensaje){
	case d_NEW_POKEMON:
		mensajesNew = tomarLosMensajes (d_NEW_POKEMON);
		int tamanioNew = list_size(mensajesNew);
		for (int i=0; i<tamanioNew;i++){
			elemento = list_get (mensajesNew, i);
			mensajeNew = leerInfoYActualizarUsoPorID(elemento->idMensaje);
			Serialize_PackAndSend_NEW_POKEMON(*cliente, elemento->idMensaje, mensajeNew->nombrePokemon ,mensajeNew->posX, mensajeNew->posY, mensajeNew->cantidad);
			actualizarEnviadosPorID(elemento->idMensaje, *cliente);
			log_info (LOGGER_OBLIGATORIO, "Se envió el mensaje %i (NEW) al suscriptor %i", elemento->idMensaje, *cliente);
		}
		//list_clean_and_destroy_elements(mensajesNew, (void *) destruir);
	break;
	case d_CATCH_POKEMON:
		mensajesCatch = tomarLosMensajes (d_CATCH_POKEMON);
		int tamanioCatch = list_size(mensajesCatch);
		for (int i=0; i<tamanioCatch;i++){
			elemento = list_get (mensajesCatch, i);
			mensajeCatch = leerInfoYActualizarUsoPorID(elemento->idMensaje);
			Serialize_PackAndSend_CATCH_POKEMON(*cliente, elemento->idMensaje,mensajeCatch->nombrePokemon, mensajeCatch->posX, mensajeCatch->posY);
			actualizarEnviadosPorID(elemento->idMensaje, *cliente);
			log_info (LOGGER_OBLIGATORIO, "Se envió el mensaje %i (CATCH) al suscriptor %i", elemento->idMensaje, *cliente);
		}
		list_clean_and_destroy_elements(mensajesCatch, (void *) destruir);
	break;
	case d_GET_POKEMON:
		mensajesGet = tomarLosMensajes (d_GET_POKEMON);
		int tamanioGet = list_size(mensajesGet);
		for (int i=0; i<tamanioGet;i++){
			elemento = list_get (mensajesGet, i);
			mensajeGet = leerInfoYActualizarUsoPorID(elemento->idMensaje);
			Serialize_PackAndSend_GET_POKEMON(*cliente, elemento->idMensaje, mensajeGet->nombrePokemon);
			actualizarEnviadosPorID(elemento->idMensaje, *cliente);
			log_info (LOGGER_OBLIGATORIO, "Se envió el mensaje %i (GET) al suscriptor %i", elemento->idMensaje, *cliente);
		}
		list_clean_and_destroy_elements(mensajesGet, (void *) destruir);
	break;
	case d_APPEARED_POKEMON:
		mensajesAppeared = tomarLosMensajes (d_APPEARED_POKEMON);
		int tamanioAppeared = list_size(mensajesAppeared);
		for (int i=0; i<tamanioAppeared;i++){
			elemento = list_get (mensajesAppeared, i);
			mensajeAppeared = leerInfoYActualizarUsoPorID(elemento->idMensaje);
			Serialize_PackAndSend_APPEARED_POKEMON(*cliente, elemento->idMensaje,mensajeAppeared->nombrePokemon, mensajeAppeared->posX, mensajeAppeared->posY);
			actualizarEnviadosPorID(elemento->idMensaje, *cliente);
			log_info (LOGGER_OBLIGATORIO, "Se envió el mensaje %i (APPEARED) al suscriptor %i", elemento->idMensaje, *cliente);
		}
		list_clean_and_destroy_elements(mensajesAppeared, (void *) destruir);
	break;
	case d_CAUGHT_POKEMON:
		mensajesCaught = tomarLosMensajes (d_CAUGHT_POKEMON);
		int tamanioCaught = list_size(mensajesCaught);
		for (int i=0; i<tamanioCaught;i++){
			elemento = list_get (mensajesCaught, i);
			mensajeCaught = leerInfoYActualizarUsoPorID(elemento->idMensaje);
			Serialize_PackAndSend_CAUGHT_POKEMON(*cliente, elemento->idMensaje, mensajeCaught->atrapado);
			actualizarEnviadosPorID(elemento->idMensaje, *cliente);
			log_info (LOGGER_OBLIGATORIO, "Se envió el mensaje %i (CAUGHT) al suscriptor %i", elemento->idMensaje, *cliente);
		}
		list_clean_and_destroy_elements(mensajesCaught, (void *) destruir);
	break;
	case d_LOCALIZED_POKEMON:
		mensajesLocalized = tomarLosMensajes (d_LOCALIZED_POKEMON);
		int tamanioLocalized = list_size(mensajesLocalized);
		for (int i=0; i<tamanioLocalized;i++){
			elemento = list_get (mensajesLocalized, i);
			mensajeLocalized = leerInfoYActualizarUsoPorID(elemento->idMensaje);
			Serialize_PackAndSend_LOCALIZED_POKEMON(*cliente, elemento->idMensaje, mensajeLocalized->nombrePokemon, mensajeLocalized->puntos);
			actualizarEnviadosPorID(elemento->idMensaje, *cliente);
			log_info (LOGGER_OBLIGATORIO, "Se envió el mensaje %i (LOCALIZED) al suscriptor %i", elemento->idMensaje, *cliente);
		}
		list_clean_and_destroy_elements(mensajesLocalized, (void *) destruir);
		break;
	default:
		log_error(LOGGER_OBLIGATORIO, "No existe el mensaje");
		break;
	}
}

t_list * tomarLosMensajes (d_message tipoMensaje){
	estructuraAdministrativa* elemento = malloc (sizeof(estructuraAdministrativa));
	t_list * listaTipo = list_create();
	int tamanioLista = list_size(ADMINISTRADOR_MEMORIA);
	for (int i = 0; i < tamanioLista; i++){
		elemento = list_get(ADMINISTRADOR_MEMORIA, i);
		if (elemento->tipoMensaje == tipoMensaje && elemento->estaOcupado == 1){
			list_add (listaTipo, elemento);
		}
	}
	return listaTipo;
}

void enviarUnMensaje (void* mensaje, d_message tipoMensaje, estructuraAdministrativa* resultado, t_list * lista){
	newEnMemoria* mensajeNew;
	catchEnMemoria * mensajeCatch;
	appearedEnMemoria * mensajeAppeared;
	getEnMemoria* mensajeGet;
	caughtEnMemoria* mensajeCaught;
	localizedEnMemoria* mensajeLocalized;
	switch (tipoMensaje){
	case d_NEW_POKEMON:
		mensajeNew = mensaje;
		void notificarSuscriptor(int * self){
			Serialize_PackAndSend_NEW_POKEMON(*self, resultado->idMensaje, mensajeNew->nombrePokemon, mensajeNew->posX, mensajeNew->posY, mensajeNew->cantidad);
			actualizarEnviadosPorID(resultado->idMensaje, *self);
			log_info (LOGGER_OBLIGATORIO, "Se envió el mensaje de id: %i al suscriptor %i", resultado->idMensaje, *self);
		}
		list_iterate(lista, (void*)notificarSuscriptor);
		log_info(LOGGER_GENERAL, "No hay mas suscriptores! \n");
		break;
	case d_CATCH_POKEMON:
		mensajeCatch = mensaje;
		int tamanioCatch = list_size(lista);
		for (int i = 0; i<tamanioCatch; i++){
			int socketCliente = list_get(lista, i);
			Serialize_PackAndSend_CATCH_POKEMON(socketCliente, resultado->idMensaje,mensajeCatch->nombrePokemon, mensajeCatch->posX, mensajeCatch->posY);
			actualizarEnviadosPorID(resultado->idMensaje, socketCliente);
			log_info (LOGGER_OBLIGATORIO, "Se envió el mensaje de id: %i al suscriptor %i", resultado->idMensaje, socketCliente);
		}
		log_info(LOGGER_GENERAL, "No hay mas suscriptores! \n");
		break;
	case d_GET_POKEMON:
		mensajeGet = mensaje;
		int tamanioGet = list_size(lista);
		for (int i = 0; i<tamanioGet; i++){
			int socketCliente = list_get(lista, i);
			Serialize_PackAndSend_GET_POKEMON(socketCliente, resultado->idMensaje, mensajeGet->nombrePokemon);
			actualizarEnviadosPorID(resultado->idMensaje, socketCliente);
			log_info (LOGGER_OBLIGATORIO, "Se envió el mensaje de id: %i al suscriptor %i", resultado->idMensaje, socketCliente);
			}
			log_info(LOGGER_GENERAL, "No hay mas suscriptores! \n");
			break;
	case d_APPEARED_POKEMON:
		mensajeAppeared = mensaje;
		int tamanioAppeared = list_size(lista);
		for (int i = 0; i<tamanioAppeared; i++){
			int socketCliente = list_get(lista, i);
			Serialize_PackAndSend_APPEARED_POKEMON(socketCliente, resultado->idMensaje,mensajeAppeared->nombrePokemon, mensajeAppeared->posX, mensajeAppeared->posY);
			actualizarEnviadosPorID(resultado->idMensaje, socketCliente);
			log_info (LOGGER_OBLIGATORIO, "Se envió el mensaje de id: %i al suscriptor %i", resultado->idMensaje, socketCliente);
			}
			log_info(LOGGER_GENERAL, "No hay mas suscriptores! \n");
			break;
	case d_CAUGHT_POKEMON:
		mensajeCaught = mensaje;
		int tamanioCaught = list_size(lista);
		for (int i = 0; i<tamanioCaught; i++){
			int socketCliente = list_get(lista, i);
			Serialize_PackAndSend_CAUGHT_POKEMON(socketCliente, resultado->idMensaje, mensajeCaught->atrapado);
			actualizarEnviadosPorID(resultado->idMensaje, socketCliente);
			log_info (LOGGER_OBLIGATORIO, "Se envió el mensaje de id: %i al suscriptor %i", resultado->idMensaje, socketCliente);
			}
			log_info(LOGGER_GENERAL, "No hay mas suscriptores! \n");
			break;
	case d_LOCALIZED_POKEMON:
		mensajeLocalized = mensaje;
		int tamanioLocalized = list_size(lista);
		for (int i = 0; i<tamanioLocalized; i++){
			int socketCliente = list_get(lista, i);
			Serialize_PackAndSend_LOCALIZED_POKEMON(socketCliente, resultado->idMensaje, mensajeLocalized->nombrePokemon, mensajeLocalized->puntos);
			actualizarEnviadosPorID(resultado->idMensaje, socketCliente);
			log_info (LOGGER_OBLIGATORIO, "Se envió el mensaje de id: %i al suscriptor %i", resultado->idMensaje, socketCliente);
			}
			log_info(LOGGER_GENERAL, "No hay mas suscriptores! \n");
			break;
	default:
		log_error(LOGGER_OBLIGATORIO, "No existe el mensaje");
		break;
	}
}
//////////////////FIN MENSAJES A SUSCRIPTORES/////////////////////////////////////

void tratarMensaje (d_message tipoMensaje, void *paquete){
	estructuraAdministrativa * resultado = malloc (sizeof(estructuraAdministrativa));
	void * unMensaje = cargarMensajeAGuardar(tipoMensaje, paquete);
	resultado = guardarMensaje(d_NEW_POKEMON, unMensaje);
	if (resultado){
		int ID = obtenerID();
		resultado->idMensaje = ID;
		resultado->tipoMensaje = tipoMensaje;
		resultado->estaOcupado = 1;
		resultado->suscriptoresConACK = list_create();
		resultado->suscriptoresConMensajeEnviado = list_create();
		resultado->tiempo = temporal_get_string_time();
		resultado->ultimaReferencia = temporal_get_string_time();
		memcpy(resultado->donde, unMensaje, tamanioDeMensaje(tipoMensaje, unMensaje));
		log_info(LOGGER_OBLIGATORIO, "Se guardo el mensaje en la memoria id: %i posicion: %i", resultado->idMensaje, posicionALog(resultado->donde));
	}
	t_list * subs = suscriptoresPara(tipoMensaje);
	enviarUnMensaje(unMensaje, tipoMensaje, resultado, subs);
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
	//DumpFileInit();
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
	BROKER_CONFIG.DUMP_FILE = config_get_string_value(configCreator, "DUMP_FILE");
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
		estructura->idMensaje = 0;
		estructura->estaOcupado = 0;
		estructura->tamanioParticion = BROKER_CONFIG.TAMANO_MEMORIA;
		estructura->donde = MEMORIA_PRINCIPAL;
		estructura->tiempo = string_new();
		estructura->tiempo = temporal_get_string_time();
		estructura->ultimaReferencia = string_new();
		estructura->ultimaReferencia = temporal_get_string_time();
		list_add (ADMINISTRADOR_MEMORIA, estructura);
}

void SemaphoresInit(){
	sem_init(&MUTEX_CLIENTE, 0, 1);
	sem_init(&MUTEX_CONTADOR, 0, 1);
	sem_init(&MUTEX_MEMORIA, 0, 1);
	sem_init(&MUTEX_TIEMPO, 0, 1);
}

void DumpFileInit(){
	FILE *fp;
	fp = fopen (BROKER_CONFIG.DUMP_FILE, "r");
	fclose(fp);
}

/////////FUNCIONES VARIAS/////////
int obtenerID(){
	sem_wait(&MUTEX_CONTADOR);
	CONTADOR ++;
	int i = CONTADOR;
	sem_post(&MUTEX_CONTADOR);
	return i;
}

//////FUNCIONES CACHE//////////
estructuraAdministrativa * guardarMensaje(d_message tipoMensaje, void * mensajeAGuardar){
	estructuraAdministrativa* donde = malloc (sizeof (estructuraAdministrativa));
	donde = buscarParticionLibre(tipoMensaje, mensajeAGuardar);
	return donde;
}

void actualizarEnviadosPorID(int id, int socketCliente){
	estructuraAdministrativa* unaEstructura = buscarEstructuraAdministrativaConID(id);
	list_add(unaEstructura->suscriptoresConMensajeEnviado, &socketCliente);
	return;
}

void actualizarRecibidosPorID(int id, int socketCliente){
	estructuraAdministrativa* unaEstructura = buscarEstructuraAdministrativaConID(id);
	if (unaEstructura != NULL){
	list_add(unaEstructura->suscriptoresConACK, &socketCliente);
	log_info(LOGGER_GENERAL, "Se agregó el cliente %i a la cola de ACK del mensaje %i ", socketCliente, id);
	}
	else{
		log_error (LOGGER_OBLIGATORIO, "El mensaje fue eliminado de la memoria");
	}
	return;
}

estructuraAdministrativa* buscarEstructuraAdministrativaConID(int id){
	bool _is_the_one(estructuraAdministrativa* p) {
		return (p->idMensaje = id);
	}

	return list_find(ADMINISTRADOR_MEMORIA, (void*) _is_the_one);
}

int tamanioDeMensaje(d_message tipoMensaje, void * unMensaje){
	int nombre;
	int posicionesYLargo;
	switch(tipoMensaje){
	case d_NEW_POKEMON:
		nombre = ((newEnMemoria *)unMensaje)->largoDeNombre * (sizeof(char));
		posicionesYLargo = (sizeof(uint32_t) * 4);
		return (nombre + posicionesYLargo);
		break;
	case d_CATCH_POKEMON:
		return (((catchEnMemoria *)unMensaje)->largoDeNombre * sizeof(char) + sizeof(uint32_t) * 3);
		break;
	case d_GET_POKEMON:
		return (((getEnMemoria *)unMensaje)->largoDeNombre * sizeof(char) + sizeof(uint32_t));
		break;
	case d_APPEARED_POKEMON:
		return (((appearedEnMemoria *)unMensaje)->largoDeNombre * sizeof(char) + sizeof(uint32_t) * 4);
		break;
	case d_CAUGHT_POKEMON:
		return (sizeof(uint32_t));
		break;
	case d_LOCALIZED_POKEMON:
		return (((localizedEnMemoria *)unMensaje)->largoDeNombre * sizeof(char) + sizeof(punto) * ((localizedEnMemoria *)unMensaje)->cantidadDePuntos);
		break;
	default:
		log_error(LOGGER_GENERAL, "Tamanio de mensaje de nada, no se puede");
		return 0;
	}
}

/////////////BUDDY SYSTEM & PARTICIONES/////////////////
estructuraAdministrativa* buscarParticionLibre(d_message tipoMensaje, void* mensaje){
	estructuraAdministrativa* particion = malloc (sizeof (estructuraAdministrativa));
	estructuraAdministrativa* particionPrueba = malloc (sizeof (estructuraAdministrativa));
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
			if(elemento->estaOcupado == 0 && tam < i && elemento->tamanioParticion >= tamanioMensaje){
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
			BUSQUEDAS_FALLIDAS ++;
			if(string_equals_ignore_case(BROKER_CONFIG.ALGORITMO_MEMORIA, "particiones")){
				if(BUSQUEDAS_FALLIDAS >= BROKER_CONFIG.FRECUENCIA_COMPACTACION || FLAG_COMPACTACION == 0){
					BUSQUEDAS_FALLIDAS=0;
					compactacion();
					FLAG_REEMPLAZAR = 0;
					return buscarParticionLibre(tipoMensaje, mensaje);
				}
				if(FLAG_REEMPLAZAR == 1){
				reemplazar(tipoMensaje, mensaje);
				return buscarParticionLibre(tipoMensaje, mensaje);
				}
			}
		if(string_equals_ignore_case(BROKER_CONFIG.ALGORITMO_MEMORIA, "bs")){
			if (FLAG_COMPOSICION == 0){
				composicion();
				FLAG_COMPOSICION = 1;
				FLAG_REEMPLAZAR = 0;
				return buscarParticionLibre(tipoMensaje, mensaje);
			}
			if (FLAG_REEMPLAZAR == 0){
				int posicion = reemplazar(tipoMensaje, mensaje);
				particionPrueba = list_get (ADMINISTRADOR_MEMORIA, posicion);
				FLAG_COMPOSICION = 0;
				FLAG_REEMPLAZAR = 1;
				return buscarParticionLibre(tipoMensaje, mensaje);
			}
		}
	}
}

int primeraParticion(){
	estructuraAdministrativa * particionMenor = malloc (sizeof(estructuraAdministrativa));
	estructuraAdministrativa * particion = malloc (sizeof(estructuraAdministrativa));
	int i, posicionMenor, contador = 0;
	int particionesMemoria = list_size(ADMINISTRADOR_MEMORIA);
	bool estaOcupado(estructuraAdministrativa* elemento) {
					return (elemento->estaOcupado == 1);
				}
	particionMenor = list_find(ADMINISTRADOR_MEMORIA, (void*) estaOcupado); // asi arreglo de agarrar uno que este desocupado
	void tomarParticion(estructuraAdministrativa* elemento){
					if(elemento->donde == particionMenor->donde){
					posicionMenor = contador;
				}
					contador ++;
				}
	list_iterate(ADMINISTRADOR_MEMORIA, (void*)tomarParticion);
	for (i = posicionMenor + 1; i < particionesMemoria; i++){
		particion = list_get(ADMINISTRADOR_MEMORIA, i);
		if(particion!= NULL){
		if (primerFechaEsAnterior(particion->tiempo, particionMenor->tiempo) && particion->estaOcupado == 1){
			particionMenor->tiempo = particion->tiempo;
			posicionMenor = i;
		}
		}
	}
	return posicionMenor;
}

int particionMenosReferenciada(){
	estructuraAdministrativa * particionMenor = malloc (sizeof(estructuraAdministrativa));
		estructuraAdministrativa * particion = malloc (sizeof(estructuraAdministrativa));
		int i, posicionMenor, contador = 0;
		int particionesMemoria = list_size(ADMINISTRADOR_MEMORIA);
		bool estaOcupado(estructuraAdministrativa* elemento) {
						return (elemento->estaOcupado == 1);
					}
		particionMenor = list_find(ADMINISTRADOR_MEMORIA, (void*) estaOcupado); // asi arreglo de agarrar uno que este desocupado
		void tomarParticion(estructuraAdministrativa* elemento){
						if(elemento->donde == particionMenor->donde){
						posicionMenor = contador;
					}
						contador ++;
					}
		list_iterate(ADMINISTRADOR_MEMORIA, (void*)tomarParticion);
		for (i = posicionMenor + 1; i < particionesMemoria; i++){
			particion = list_get(ADMINISTRADOR_MEMORIA, i);
			if (primerFechaEsAnterior(particion->ultimaReferencia, particionMenor->ultimaReferencia) && particion->ultimaReferencia == 1){
					particionMenor->ultimaReferencia = particion->ultimaReferencia;
					posicionMenor = i;
			}
		}
		return posicionMenor;
	}


int reemplazar (d_message tipoMensaje, void* mensaje){
	estructuraAdministrativa* particion = malloc (sizeof (estructuraAdministrativa));
	int tamanioMensaje = tamanioDeMensaje (tipoMensaje, mensaje);
	//////////FIFO////////////////////////////////////////
	if(noPuedoReemplazarMas()){
		FLAG_COMPACTACION = 0;
		log_info (LOGGER_OBLIGATORIO, "No puedo reemplazar más, necesito compactar.");
		return 0;
	}
	if(string_equals_ignore_case(BROKER_CONFIG.ALGORITMO_REEMPLAZO, "fifo")){
		int posicion = primeraParticion();
		particion = list_get (ADMINISTRADOR_MEMORIA, posicion);
		particion->estaOcupado = 0;
		particion->idMensaje = NULL;
		list_clean (particion->suscriptoresConACK); //Podemos necesitar destruir los elementos
		list_clean (particion->suscriptoresConMensajeEnviado);
		particion->tiempo = string_new();
		particion->tiempo = temporal_get_string_time();
		particion->ultimaReferencia = string_new();
		particion->ultimaReferencia = temporal_get_string_time();
		log_info (LOGGER_OBLIGATORIO, "Se limpio la particion Victima: %i, volvemos a buscar", posicionALog(particion->donde));
		return posicion;
	}
	if(string_equals_ignore_case(BROKER_CONFIG.ALGORITMO_REEMPLAZO, "lru")){
		int posicion = particionMenosReferenciada();
		particion = list_get (ADMINISTRADOR_MEMORIA, posicion);
		particion->estaOcupado = 0;
		particion->idMensaje = NULL;
		list_clean(particion->suscriptoresConACK);
		list_clean (particion->suscriptoresConMensajeEnviado);
		particion->tiempo = string_new();
		particion->tiempo = temporal_get_string_time();
		particion->ultimaReferencia = string_new();
		particion->ultimaReferencia = temporal_get_string_time();
		return posicion;
	}
	///El donde no cambia, al igual que el tamaño de la particion
	log_info (LOGGER_GENERAL, "Se limpio la particion Victima, volvemos a buscar");
}

bool noPuedoReemplazarMas(){
	bool cualquieraOcupado(estructuraAdministrativa* elemento) {
		return (elemento->estaOcupado == 1);
	}
	void * x = list_find(ADMINISTRADOR_MEMORIA, cualquieraOcupado);
	return (x==NULL);
}

void composicion(){
	estructuraAdministrativa * particionAnterior = malloc (sizeof(estructuraAdministrativa));
	estructuraAdministrativa * particionActual = malloc (sizeof(estructuraAdministrativa));
	estructuraAdministrativa * particionPosterior = malloc (sizeof(estructuraAdministrativa));
	int i = 0;
	int particionesMemoria = list_size(ADMINISTRADOR_MEMORIA);
	particionActual = list_get(ADMINISTRADOR_MEMORIA, i);
	particionPosterior = list_get (ADMINISTRADOR_MEMORIA, i+1);
	if (particionActual->estaOcupado == 0 && particionPosterior->estaOcupado == 0 && particionActual->tamanioParticion == particionPosterior->tamanioParticion){
		particionActual->estaOcupado = 0;
		particionActual->tamanioParticion = particionActual->tamanioParticion + particionPosterior->tamanioParticion;
	}
		for (i=1; i<particionesMemoria; i++){
		particionActual = list_get(ADMINISTRADOR_MEMORIA, i);
		particionAnterior = list_get (ADMINISTRADOR_MEMORIA, i-1);
		particionPosterior = list_get (ADMINISTRADOR_MEMORIA, i+1);
		if (particionAnterior->estaOcupado == 0 && particionActual->estaOcupado == 0 && particionAnterior->tamanioParticion == particionActual->tamanioParticion){
				particionActual->donde = particionAnterior->donde;
				particionActual->estaOcupado = 0;
				particionActual->tamanioParticion = particionAnterior->tamanioParticion + particionActual->tamanioParticion;
				if (particionActual->estaOcupado == 0 && particionPosterior->estaOcupado == 0 && particionActual->tamanioParticion == particionPosterior->tamanioParticion){
					particionActual->estaOcupado = 0;
					particionActual->tamanioParticion = particionActual->tamanioParticion + particionPosterior->tamanioParticion;
		}
		}
	}
}

estructuraAdministrativa * particionAMedida(d_message tipoMensaje, void*mensaje, estructuraAdministrativa* particion){
	int contador = 0, posicion = 0;
	estructuraAdministrativa * particionMinima = malloc (sizeof (estructuraAdministrativa));
	int contar = contarTamanio();
	int tamanioMensaje = tamanioDeMensaje(tipoMensaje, mensaje);
	if(string_equals_ignore_case(BROKER_CONFIG.ALGORITMO_MEMORIA, "particiones")){
		estructuraAdministrativa * particionVacia = malloc (sizeof (estructuraAdministrativa));
		int tamanioInicial = particion->tamanioParticion;
		particion->tamanioParticion = tamanioDeMensaje(tipoMensaje, mensaje);
		if(particion->tamanioParticion < BROKER_CONFIG.TAMANO_MINIMO_PARTICION){
			particion->tamanioParticion = BROKER_CONFIG.TAMANO_MINIMO_PARTICION;
		}
		particion->estaOcupado = 0;
		particion->idMensaje = NULL;
		particion->suscriptoresConACK = list_create();
		particion->suscriptoresConMensajeEnviado = list_create();
		particion->tiempo = string_new();
		particion->ultimaReferencia = string_new();
		if(tamanioInicial - particion->tamanioParticion > BROKER_CONFIG.TAMANO_MINIMO_PARTICION){
			particionVacia->tamanioParticion = tamanioInicial - particion->tamanioParticion;
			particionVacia->estaOcupado = 0;
			particionVacia->donde = particion->donde + particion->tamanioParticion;
			particionVacia->idMensaje = NULL;
			particionVacia->suscriptoresConACK = list_create();
			particionVacia->suscriptoresConMensajeEnviado = list_create();
			particionVacia->tiempo = string_new();
			particionVacia->ultimaReferencia = string_new();
			list_add(ADMINISTRADOR_MEMORIA, particionVacia);
		}
		else{
			particion->tamanioParticion = tamanioInicial;
		}
	}
	if(string_equals_ignore_case(BROKER_CONFIG.ALGORITMO_MEMORIA, "bs")){
		if (tamanioMensaje <= BROKER_CONFIG.TAMANO_MINIMO_PARTICION){
			bool tomarLaParticionMinima(estructuraAdministrativa* elemento) { // PUEDO TENER FRAG INTERNA
							return (elemento->estaOcupado == 0 && elemento->tamanioParticion ==BROKER_CONFIG.TAMANO_MINIMO_PARTICION);
						}
				particionMinima = list_find(ADMINISTRADOR_MEMORIA, (void*) tomarLaParticionMinima); // asi arreglo de agarrar uno que este desocupado
			}
		while (particion->tamanioParticion / 2 > tamanioMensaje && contar == BROKER_CONFIG.TAMANO_MEMORIA){
			estructuraAdministrativa * particionAuxiliar = malloc (sizeof (estructuraAdministrativa)); // la que le sigue al actua
			particionAuxiliar->tamanioParticion = particion->tamanioParticion / 2;
			particionAuxiliar->estaOcupado = 0;
			particionAuxiliar->donde =particion->donde + particionAuxiliar->tamanioParticion;
			particionAuxiliar->idMensaje = NULL;
			particionAuxiliar->suscriptoresConACK = list_create();
			particionAuxiliar->suscriptoresConMensajeEnviado = list_create();
			particionAuxiliar->tiempo = string_new();
			particionAuxiliar->ultimaReferencia = string_new();
			void tomarParticion(estructuraAdministrativa* elemento){
				if(elemento->donde == particion->donde){
					posicion = contador;
			}
				contador ++;
			}
			list_iterate(ADMINISTRADOR_MEMORIA, (void*)tomarParticion);
			contador = 0;
			list_add_in_index(ADMINISTRADOR_MEMORIA, posicion+1, particionAuxiliar);
			particion->estaOcupado = 0;
			particion->tamanioParticion = particionAuxiliar->tamanioParticion;
		}
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
	log_info (LOGGER_OBLIGATORIO, "Se ejecuto la compactacion");
}

void reposicionarParticionesOcupadas(t_list * listaAuxiliar){
	void * nuevoDonde = MEMORIA_PRINCIPAL;
	if(!list_is_empty(listaAuxiliar)){
		void cambiarInfo(estructuraAdministrativa* elemento) {
				memcpy(nuevoDonde, elemento->donde, elemento->tamanioParticion);
				elemento->donde = nuevoDonde;
				nuevoDonde = nuevoDonde + elemento->tamanioParticion;
				return;
		}
		list_iterate(listaAuxiliar, (void*) cambiarInfo);
	}
	list_clean(ADMINISTRADOR_MEMORIA);//list_clean_and_destroy_elements(ADMINISTRADOR_MEMORIA, (void *)estructuraAdministrativaDestroyerSinDestruirListas);
	list_add_all(ADMINISTRADOR_MEMORIA, listaAuxiliar);
	estructuraAdministrativa * espacioFaltante = malloc (sizeof(estructuraAdministrativa));
	espacioFaltante->donde = nuevoDonde;
	espacioFaltante->estaOcupado = 0;
	espacioFaltante->suscriptoresConACK = list_create();
	espacioFaltante->suscriptoresConMensajeEnviado = list_create();
	espacioFaltante->tiempo = string_new();
	espacioFaltante->ultimaReferencia = string_new();
	espacioFaltante->tiempo = temporal_get_string_time();
	espacioFaltante->ultimaReferencia = temporal_get_string_time();
	espacioFaltante->tamanioParticion = BROKER_CONFIG.TAMANO_MEMORIA - contarTamanio();
	list_add(ADMINISTRADOR_MEMORIA, espacioFaltante);
}

static void estructuraAdministrativaDestroyer(estructuraAdministrativa *self) {
/*void estructuraAdministrativaDestroyer(estructuraAdministrativa *self) {
    free(self->donde);
    free(self->estaOcupado);
    free(self->idMensaje);
    free(self->tamanioParticion);
    */
    free(self->tiempo);
    free(self->ultimaReferencia);
    list_clean_and_destroy_elements(self->suscriptoresConACK, (void*)suscriptorDestroyer);
    list_destroy(self->suscriptoresConACK);
    list_clean_and_destroy_elements(self->suscriptoresConMensajeEnviado, (void*)suscriptorDestroyer);
    list_destroy(self->suscriptoresConMensajeEnviado);
    free(self);
}

static void estructuraAdministrativaDestroyerSinDestruirListas(estructuraAdministrativa *self) {
    free(self->tiempo);
    free(self->ultimaReferencia);
    free(self);
}

static void suscriptorDestroyer(int *self) {
    free(self);
}

///Leer memoria para mandar mensaje
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
		retorno = lugarDeComienzo;
		sem_post(&MUTEX_MEMORIA);
		return retorno;
		break;
	case d_CATCH_POKEMON:
		retornoCatch = malloc(sizeof(catchEnMemoria));
		retornoCatch = lugarDeComienzo;
		return retornoCatch;
		break;
	case d_GET_POKEMON:
		retornoGet = malloc(sizeof(getEnMemoria));
		retornoGet = lugarDeComienzo;
		sem_post(&MUTEX_MEMORIA);
		return retornoGet;
		break;
	case d_APPEARED_POKEMON:
		retornoAppeared = malloc(sizeof(appearedEnMemoria));
		retornoAppeared = lugarDeComienzo;
		sem_post(&MUTEX_MEMORIA);
		return retornoAppeared;
		break;
	case d_CAUGHT_POKEMON:
		retornoCaught = malloc(sizeof(caughtEnMemoria));
		retornoCaught = lugarDeComienzo;
		sem_post(&MUTEX_MEMORIA);
		return retornoCaught;
		break;
	case d_LOCALIZED_POKEMON:
		retornoLocalized = malloc(sizeof(localizedEnMemoria));
		retornoLocalized = lugarDeComienzo;
		sem_post(&MUTEX_MEMORIA);
		return retornoLocalized;
		break;
	default:
		log_error(LOGGER_GENERAL, "Error levantando data de la memoria");
		sem_post(&MUTEX_MEMORIA);
		return NULL;
	}
}

void * leerInfoYActualizarUsoPorID(int id){ //deuelve un tipo en memoria
	bool igualID(estructuraAdministrativa* elemento) {
				return (elemento->idMensaje == id);
			}
	estructuraAdministrativa * ElElemento = list_find(ADMINISTRADOR_MEMORIA, (void*)igualID);
	ElElemento->ultimaReferencia = string_new();
	ElElemento->ultimaReferencia = temporal_get_string_time();
	return(levantarMensaje(ElElemento->tipoMensaje, ElElemento->donde));
}

void funcionParaVerMemoria(){
	for(int i = 0; i<list_size(ADMINISTRADOR_MEMORIA); i++){
		estructuraAdministrativa * ElElemento = list_get(ADMINISTRADOR_MEMORIA, i);
		if(ElElemento->estaOcupado == 1){
			void* prueba = levantarMensaje(ElElemento->tipoMensaje, ElElemento->donde);
			log_info(LOGGER_GENERAL, "Levantado de la memoria posta!!!");
		}
	}
}

void dump () {
	sem_wait(&MUTEX_MEMORIA);
	log_info(LOGGER_OBLIGATORIO, "Se requirió un dump");
	FILE * archivoDump = txt_open_for_append(BROKER_CONFIG.DUMP_FILE);
	char* unaLinea = string_new();
	char* extra = string_new();
	string_append(&unaLinea, "Dump: ");
	extra = temporal_get_string_time();
	string_append(&unaLinea, extra);
	string_append(&unaLinea, "\n");
	txt_write_in_file(archivoDump, unaLinea);
	for(int i = 1; i-1<list_size(ADMINISTRADOR_MEMORIA); i++){
		char* unaLinea = string_new();	char* nombreCola = string_new();
		estructuraAdministrativa * ElElemento = list_get(ADMINISTRADOR_MEMORIA, i-1);
		char* extra = string_new();
		if(ElElemento->estaOcupado == 0){
			string_append(&extra, "[L]");
			string_append(&nombreCola, "No tiene asignado");
		}
		else {
			string_append(&extra, "[X]");
			string_append(&nombreCola, nombresColas[ElElemento->tipoMensaje]);
		}
		void* finParticion = ElElemento->donde + ElElemento->tamanioParticion - 1;
		unaLinea = string_from_format("Partición %i: %06p - %06p %s  Size: %i b     LRU:%s Cola:%s ID:%i \n", i, ElElemento->donde, finParticion, extra, ElElemento->tamanioParticion, ElElemento->ultimaReferencia, nombreCola, ElElemento->idMensaje);
		txt_write_in_file(archivoDump, unaLinea);
	}
	txt_close_file(archivoDump);
	sem_post(&MUTEX_MEMORIA);
	return;
}

bool primerFechaEsAnterior(char* unaFecha, char* otraFecha){
	char** primerFechaSeparada = string_split(unaFecha, ":");
	char** segundaFechaSeparada = string_split(otraFecha, ":");
	for(int i = 0; primerFechaSeparada[i]!=NULL; i++){
		if (primerFechaSeparada[i] != segundaFechaSeparada[i]){
			return (atoi(primerFechaSeparada[i]) < atoi(segundaFechaSeparada[i]));
		}
	}
	return true;
}

void destruirTodo(){
	free(MEMORIA_PRINCIPAL);
	exit(0);
}

int posicionALog(void* unaPosicion){
	return unaPosicion - MEMORIA_PRINCIPAL;
}

void * cargarMensajeAGuardar(d_message tipoMensaje, void *paquete) {
	uint32_t posX, posY, cantidad, atrapado, correlativoA, id;
	char *pokemon;
	void *retorno;
	t_list * listaDePuntos = list_create();
	newEnMemoria * retornoNew;
	catchEnMemoria * retornoCatch;
	getEnMemoria * retornoGet;
	appearedEnMemoria * retornoAppeared;
	caughtEnMemoria * retornoCaught;
	localizedEnMemoria * retornoLocalized;
	switch(tipoMensaje){
	case d_NEW_POKEMON:
		retornoNew = malloc(sizeof(newEnMemoria));
		Serialize_Unpack_NewPokemon_NoID(paquete, &pokemon, &posX, &posY, &cantidad);
		log_info(LOGGER_GENERAL,"Me llego mensaje new Pkm: %s, x: %i, y: %i, cant: %i\n", pokemon, posX, posY, cantidad);
		retornoNew->cantidad = cantidad;
		retornoNew->largoDeNombre = string_length(pokemon);
		retornoNew->nombrePokemon = pokemon;
		retornoNew->posX = posX;
		retornoNew->posY = posY;
		retorno = retornoNew;
		break;
	case d_CATCH_POKEMON:
		retornoCatch = malloc(sizeof(catchEnMemoria));
		Serialize_Unpack_CatchPokemon_NoID(paquete, &pokemon, &posX, &posY);
		log_info(LOGGER_GENERAL,"Me llego mensaje catch Pkm: %s, x: %i, y: %i \n", pokemon, posX, posY);
		retornoCatch->largoDeNombre = string_length(pokemon);
		retornoCatch->nombrePokemon = pokemon;
		retornoCatch->posX = posX;
		retornoCatch->posY = posY;
		retorno = retornoCatch;
		break;
	case d_GET_POKEMON:
		retornoGet = malloc(sizeof(getEnMemoria));
		Serialize_Unpack_GetPokemon_NoID(paquete, &pokemon);
		log_info(LOGGER_GENERAL,"Me llego mensaje get Pkm: %s\n", pokemon);
		retornoGet->largoDeNombre = string_length(pokemon);
		retornoGet->nombrePokemon = pokemon;
		retorno = retornoGet;
		break;
	case d_APPEARED_POKEMON:
		retornoAppeared = malloc(sizeof(appearedEnMemoria));
		Serialize_Unpack_AppearedPokemon(paquete, &id, &pokemon, &posX, &posY);
		log_info(LOGGER_GENERAL,"Me llego mensaje appeared Pkm: %s, x: %i, y: %i , id: %i \n", pokemon, posX, posY, id);
		retornoAppeared->largoDeNombre = string_length(pokemon);
		retornoAppeared->nombrePokemon = pokemon;
		retornoAppeared->posX = posX;
		retornoAppeared->posY = posY;
		retorno = retornoAppeared;
		break;
	case d_CAUGHT_POKEMON:
		retornoCaught = malloc(sizeof(caughtEnMemoria));
		Serialize_Unpack_CaughtPokemon(paquete, &correlativoA, &atrapado);
		log_info(LOGGER_GENERAL,"Me llego mensaje caught correlativo a: %i, resultado: %i \n", correlativoA, atrapado);
		retornoCaught->atrapado = atrapado;
		retorno = retornoCaught;
		break;
	case d_LOCALIZED_POKEMON:
		retornoLocalized = malloc(sizeof(localizedEnMemoria));
		Serialize_Unpack_LocalizedPokemon(paquete, &id, &pokemon, &listaDePuntos);
		log_info(LOGGER_GENERAL,"Me llego mensaje localized correlativo a: %i, pkm: %s \n", id, pokemon);
		void mostrarPuntos(punto* self){
			log_info(LOGGER_GENERAL,"Punto en x: %i, y: %i \n", self->posX, self->posY);
		}
		list_iterate(listaDePuntos, (void*)mostrarPuntos);
		retorno = retornoLocalized;
		break;
	default:
		log_error(LOGGER_GENERAL, "Error descomponiendo el mensaje");
		return NULL;
	}
	return retorno;
}

t_list* suscriptoresPara(d_message tipoDeMensaje){
	switch(tipoDeMensaje){
	case d_NEW_POKEMON:
		return SUSCRIPTORES_NEW;
	case d_CATCH_POKEMON:
		return SUSCRIPTORES_CATCH;
	case d_GET_POKEMON:
		return SUSCRIPTORES_GET;
	case d_APPEARED_POKEMON:
		return SUSCRIPTORES_APPEARED;
	case d_CAUGHT_POKEMON:
		return SUSCRIPTORES_CAUGHT;
	case d_LOCALIZED_POKEMON:
		return SUSCRIPTORES_LOCALIZED;
	}
	return NULL;
}
