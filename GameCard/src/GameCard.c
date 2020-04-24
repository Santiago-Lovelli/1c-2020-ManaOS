#include "GameCard.h"

void atender(HeaderDelibird header, int cliente) {

	switch (header.tipoMensaje) {
	case d_NEW_POKEMON:
		;
		log_info(loggerGeneral, "Llego un new pokemon");

		void* packNewPokemon = Serialize_ReceiveAndUnpack(cliente,
				header.tamanioMensaje);
		uint32_t idMensajeNew, posicionNewX, posicionNewY, newCantidad;
		char *newNombrePokemon;
		Serialize_Unpack_NewPokemon(packNewPokemon, &idMensajeNew,
				&newNombrePokemon, &posicionNewX, &posicionNewY, &newCantidad);
		log_info(loggerGeneral,
				"Me llego mensaje de %i. Id: %i, Pkm: %s, x: %i, y: %i, cant: %i\n",
				header.tipoMensaje, idMensajeNew, newNombrePokemon,
				posicionNewX, posicionNewY, newCantidad);
		// Se hace lo necesario
		free(packNewPokemon);
		break;
	case d_CATCH_POKEMON:
		;
		log_info(loggerGeneral, "Llego un catch pokemon");

		void* packCatchPokemon = Serialize_ReceiveAndUnpack(cliente,
				header.tamanioMensaje);
		uint32_t idMensajeCatch, posicionCatchX, posicionCatchY;
		char *catchNombrePokemon;
		Serialize_Unpack_CatchPokemon(packCatchPokemon, &idMensajeCatch,
				&catchNombrePokemon, &posicionCatchX, &posicionCatchY);
		log_info(loggerGeneral,
				"Me llego mensaje de %i. Id: %i, Pkm: %s, x: %i, y: %i\n",
				header.tipoMensaje, idMensajeCatch, catchNombrePokemon,
				posicionCatchX, posicionCatchY);
		// Se hace lo necesario
		free(packCatchPokemon);
		break;
	case d_GET_POKEMON:
		;
		log_info(loggerGeneral, "Llego un get pokemon");

		void* packGetPokemon = Serialize_ReceiveAndUnpack(cliente,
				header.tamanioMensaje);
		uint32_t idMensajeGet;
		char *getNombrePokemon;
		Serialize_Unpack_GetPokemon(packGetPokemon, &idMensajeGet,
				&getNombrePokemon);
		log_info(loggerGeneral, "Me llego mensaje de %i. Id: %i, Pkm: %s\n",
				header.tipoMensaje, idMensajeGet, getNombrePokemon);
		// Se hace lo necesario
		free(packGetPokemon);
		break;
	default:
		log_error(loggerGeneral, "Mensaje no entendido: %i\n", header);
		void* packBasura = Serialize_ReceiveAndUnpack(cliente,
				header.tamanioMensaje);
		free(packBasura);
		break;
	}
}

void recibirYAtenderUnCliente(int cliente) {
	while (1) {
		HeaderDelibird headerRecibido = Serialize_RecieveHeader(cliente);
		if (headerRecibido.tipoMensaje == -1) {
			log_error(loggerGeneral, "Se desconecto el GameBoy\n");
			break;
		}
		atender(headerRecibido, cliente);
	}
}

void* atenderGameBoy() {
	t_log* gameBoyLog = iniciar_log("GameBoy");
	char *puerto = config_get_string_value(archivo_de_configuracion,
			"PUERTO_GAMECARD");
	log_info(gameBoyLog, "Puerto GameCard: %s", puerto);
	char *ip = config_get_string_value(archivo_de_configuracion, "IP_GAMECARD");
	log_info(gameBoyLog, "Ip GameCard: %s", ip);

	int conexion = iniciar_servidor(ip, puerto, gameBoyLog);
	int cliente = esperar_cliente_con_accept(conexion, gameBoyLog);
	log_info(gameBoyLog, "se conecto cliente: %i", cliente);
	recibirYAtenderUnCliente(cliente);
}

void iniciarServidorDeGameBoy(pthread_t* servidor) {
	if (pthread_create(servidor, NULL, (void*) atenderGameBoy, NULL) == 0) {
		log_info(loggerGeneral, "Se creo hilo de GameBoy");
	} else {
		log_error(loggerGeneral, "No se pudo crear el hilo de GameBoy");
	}
}

void levantarLogYArchivoDeConfiguracion() {
	loggerGeneral = iniciar_log("GameCard");
	archivo_de_configuracion =
			config_create(
					"/home/utnso/workspace/tp-2020-1c-ManaOS-/GameCard/GameCard.config");
	log_info(loggerGeneral,
			"Se levanto el archivo de configuracion /home/utnso/workspace/tp-2020-1c-ManaOS-/GameCard/GameCard.config\n");
}

void conectarmeColaDe(d_message colaDeSuscripcion) {

	char *puerto = config_get_string_value(archivo_de_configuracion,
			"PUERTO_BROKER");
	char *ip = config_get_string_value(archivo_de_configuracion, "IP_BROKER");

	uint32_t reconectar = config_get_int_value(archivo_de_configuracion,
			"TIEMPO_DE_REINTENTO_CONEXION");
	int conexion;

	while (1) {
		conexion = conectarse_a_un_servidor(ip, puerto, loggerGeneral);
		if (conexion == -1) {
			log_error(loggerGeneral, "No se pudo conectar con el Broken");
			sleep(reconectar);
		} else {
			break;
		}
	}
	Serialize_PackAndSend_SubscribeQueue(conexion, colaDeSuscripcion);
}

void * obtenerMetadata() {
	char *tallgrass = config_get_string_value(archivo_de_configuracion,
			"PUNTO_MONTAJE_TALLGRASS");
	log_info(loggerGeneral, "El montaje es: %s", tallgrass);

	char* rutaMetadata = "/Metadata/Metadata.bin";

	char* montajeMetadata = malloc(
			strlen(tallgrass) + strlen(rutaMetadata) + 1);

	memcpy(montajeMetadata, tallgrass, strlen(tallgrass));
	memcpy(montajeMetadata + strlen(tallgrass), rutaMetadata,
			strlen(rutaMetadata) + 1);

	log_info(loggerGeneral, "Montaje de metadata: %s \n", montajeMetadata);

	uint32_t tamanio_archivo_de_metadata = tamanio_archivo(montajeMetadata);
	log_info(loggerGeneral, "Tamanio archivo: %i", tamanio_archivo_de_metadata);

	int disco = open(montajeMetadata, O_RDWR, 0);
	void *metadata = mmap(NULL, tamanio_archivo_de_metadata,
			PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_FILE, disco, 0);
	return metadata;
}

void * obtenerBitmap() {
	char *tallgrass = config_get_string_value(archivo_de_configuracion,
			"PUNTO_MONTAJE_TALLGRASS");
	log_info(loggerGeneral, "El montaje es: %s", tallgrass);

	char* rutaMetadata = "/Metadata/Bitmap.bin";

	char* montajeMetadata = malloc(
			strlen(tallgrass) + strlen(rutaMetadata) + 1);

	memcpy(montajeMetadata, tallgrass, strlen(tallgrass));
	memcpy(montajeMetadata + strlen(tallgrass), rutaMetadata,
			strlen(rutaMetadata) + 1);

	log_info(loggerGeneral, "Montaje de bitmap: %s \n", montajeMetadata);

	uint32_t tamanio_archivo_de_bitmap = tamanio_archivo(montajeMetadata);
	log_info(loggerGeneral, "Tamanio archivo: %i", tamanio_archivo_de_bitmap);

	int disco = open(montajeMetadata, O_RDWR, 0);
	void *bitmap = mmap(NULL, tamanio_archivo_de_bitmap, PROT_READ | PROT_WRITE,
	MAP_SHARED | MAP_FILE, disco, 0);
	return bitmap;
}

void cargarMetadata() {

	char *meta = obtenerBitmap();

	char **separadoPorIguales = string_split(meta, "=");

	char **separadorParaBlockSize = string_split(separadoPorIguales[1], "B");
	uint32_t blockSize = atoi(separadorParaBlockSize[0]);
	log_info(loggerGeneral, "BLOCK_SIZE: %i", blockSize);

	char **separadorParaBlocks = string_split(separadoPorIguales[2], "M");
	uint32_t blocks = atoi(separadorParaBlocks[0]);
	log_info(loggerGeneral, "BLOCKS: %i", blocks);

	char *magicNumber = malloc(strlen(separadoPorIguales[3]) + 1);
	memcpy(magicNumber, separadoPorIguales[3],
			strlen(separadoPorIguales[3]) + 1);

	log_info(loggerGeneral, "MAGIC_NUMBER: %s", separadoPorIguales[3]);

	metadata metadata;
	metadata.tamanioDeBloque = blockSize;
	metadata.bloques = blocks;
	memcpy(metadata.numeroMagico, magicNumber, strlen(magicNumber) + 1);
}

void iniciarBitmap() {
	t_bitarray * bitmap = obtenerBitmap();
}

int main(void) {
	levantarLogYArchivoDeConfiguracion();
	cargarMetadata();
	iniciarBitmap();

	pthread_t* servidor = malloc(sizeof(pthread_t));
	iniciarServidorDeGameBoy(servidor);

	pthread_t* suscriptoNewPokemon = malloc(sizeof(pthread_t));
	conectarmeColaDe(d_NEW_POKEMON);

	pthread_t* suscriptoCatchPokemon = malloc(sizeof(pthread_t));
	conectarmeColaDe(d_CATCH_POKEMON);

	pthread_t* suscriptoGetPokemon = malloc(sizeof(pthread_t));
	conectarmeColaDe(d_GET_POKEMON);

	pthread_join(*servidor, NULL);
	pthread_join(*suscriptoNewPokemon, NULL);
	pthread_join(*suscriptoCatchPokemon, NULL);
	pthread_join(*suscriptoGetPokemon, NULL);

	return EXIT_SUCCESS;
}
