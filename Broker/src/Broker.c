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
		pthread_create(hiloDeAtencion, NULL, AtenderCliente, cliente);
		/*int pthreadResponse = pthread_create(hiloDeAtencion, NULL, AtenderCliente, cliente);
			if(pthreadResponse == 0){
				pthread_detach(*hiloDeAtencion);
				log_info(LOGGER_GENERAL,"Se creo el hilo de atencion de cliente sin problema, cliente: %i", cliente);
			}else{
				log_error(LOGGER_GENERAL,"No se pudo crear el hilo de atencion de cliente, cliente: %i", cliente);
			}
			*/
	}
}

void* AtenderCliente(void* cliente) {
	while(1){
		puts("Llegué a atender un cliente lince!");
		HeaderDelibird headerRecibido =  Serialize_RecieveHeader(cliente);
		if(headerRecibido.tipoMensaje == -1){
			log_error(LOGGER_GENERAL, "Se desconecto el cliente %i: \n", cliente);
			break;
		}
		ActuarAnteMensaje(headerRecibido,cliente);
	}
}

void ActuarAnteMensaje(HeaderDelibird header, int cliente){
	switch (header.tipoMensaje) {
		case d_NEW_POKEMON:
			log_info(LOGGER_GENERAL, "Llego un new pokemon");

			void* packNewPokemon = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			uint32_t idMensajeNew,posicionNewX,posicionNewY,newCantidad;
			char *newNombrePokemon;
			Serialize_Unpack_NewPokemon(packNewPokemon, &idMensajeNew, &newNombrePokemon, &posicionNewX, &posicionNewY, &newCantidad);
			log_info(LOGGER_GENERAL,"Me llego mensaje de %i. Id: %i, Pkm: %s, x: %i, y: %i, cant: %i\n", header.tipoMensaje,idMensajeNew,newNombrePokemon, posicionNewX, posicionNewY, newCantidad);
			// Se hace lo necesario
			free(packNewPokemon);
			break;
		case d_CATCH_POKEMON:
			log_info(LOGGER_GENERAL, "Llego un catch pokemon");

			void* packCatchPokemon = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			uint32_t idMensajeCatch,posicionCatchX,posicionCatchY;
			char *catchNombrePokemon;
			Serialize_Unpack_CatchPokemon(packCatchPokemon, &idMensajeCatch, &catchNombrePokemon, &posicionCatchX, &posicionCatchY);
			log_info(LOGGER_GENERAL,"Me llego mensaje de %i. Id: %i, Pkm: %s, x: %i, y: %i\n", header.tipoMensaje,idMensajeCatch,catchNombrePokemon, posicionCatchX, posicionCatchY);
			// Se hace lo necesario
			free(packCatchPokemon);
			break;
		case d_GET_POKEMON:
			log_info(LOGGER_GENERAL, "Llego un get pokemon");

			void* packGetPokemon = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			uint32_t idMensajeGet;
			char *getNombrePokemon;
			Serialize_Unpack_GetPokemon(packGetPokemon, &idMensajeGet, &getNombrePokemon);
			log_info(LOGGER_GENERAL,"Me llego mensaje de %i. Id: %i, Pkm: %s\n", header.tipoMensaje, idMensajeGet, getNombrePokemon);
			// Se hace lo necesario
			free(packGetPokemon);
			break;
		case d_APPEARED_POKEMON:
			log_info(LOGGER_GENERAL, "Llego un appeared pokemon");

			void* packAppearedPokemon = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			uint32_t idMensajeAppeared, posicionX, posicionY;
			char *nombrePokemon;
			Serialize_Unpack_AppearedPokemon(packAppearedPokemon, &idMensajeAppeared, &nombrePokemon, &posicionX, &posicionY);
			log_info(LOGGER_GENERAL,"Me llego mensaje de %i. Id: %i, Pkm: %s, x: %i, y: %i\n", header.tipoMensaje, idMensajeAppeared, nombrePokemon, posicionX, posicionY);
			// Se hace lo necesario
			free(packAppearedPokemon);
			break;
		case d_CAUGHT_POKEMON:
			log_info(LOGGER_GENERAL, "Llego un caught pokemon");

			void* packCaughtPokemon = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			uint32_t idMensajeCaught, resultado;
			Serialize_Unpack_CaughtPokemon(packCaughtPokemon, &idMensajeCaught, &resultado);
			log_info(LOGGER_GENERAL,"Me llego mensaje de %i. Id: %i, Resultado: %i\n", header.tipoMensaje, idMensajeCaught, resultado);
			// Se hace lo necesario
			free(packCaughtPokemon);
			break;
		/*case d_LOCALIZED_POKEMON:
			log_info(LOGGER_GENERAL, "Llego un localized pokemon");

			void* packLocalizedPokemon = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			uint32_t idMensajeLocalized;
			char *localizedNombrePokemon;
			Serialize_Unpack_LocalizedPokemon(packGetPokemon, &idMensajeGet, &getNombrePokemon);
			log_info(LOGGER_GENERAL,"Me llego mensaje de %i. Id: %i, Pkm: %s\n", header.tipoMensaje, idMensajeGet, getNombrePokemon);
			// Se hace lo necesario
			free(packGetPokemon);
			break;
		*/
		case d_ACK:
			log_info(LOGGER_GENERAL, "Llego un acknowledged");
			break;

			/*
			void* packAcknowledge = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			uint32_t idMensajeCaught, resultado;
			Serialize_Unpack_ACK();
			log_info(LOGGER_GENERAL,"Me llego mensaje de %i. Id: %i, Resultado: %i\n", header.tipoMensaje, idMensajeCaught, resultado);
			// Se hace lo necesario
			free(packCaughtPokemon);
			*/
		case d_SUBSCRIBE_QUEUE:
			log_info(LOGGER_GENERAL, "Llego un Subscribe");
			break;
		default:
			log_error(LOGGER_GENERAL, "Mensaje no entendido: %i\n", header);
			void* packBasura = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			free(packBasura);
			break;
	}
}

///////FUNCIONES DE INIT///////////
void Init(){
	LOGGER_GENERAL = iniciar_log("Broker");
	ConfigInit();
	log_info(LOGGER_GENERAL, "Configuracion broker levantada!");
	LOGGER_OBLIGATORIO = iniciar_log(BROKER_CONFIG.LOG_FILE);
	ListsInit();
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

void ListsInit () {
	CONEXIONES = list_create();
	SUSCRIPTORES_NEW = list_create();
	SUSCRIPTORES_APPEARED = list_create();
	SUSCRIPTORES_GET = list_create();
	SUSCRIPTORES_CATCH = list_create();
	SUSCRIPTORES_CAUGHT = list_create();
	SUSCRIPTORES_LOCALIZED = list_create();
}

void agregarSuscriptor(int id_suscriptor, t_list* lista){
	list_add(lista, id_suscriptor);
}
