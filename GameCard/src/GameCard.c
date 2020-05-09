#include "GameCard.h"

char* montajeDeArchivo() {
	char *tallgrass = config_get_string_value(archivo_de_configuracion,
			"PUNTO_MONTAJE_TALLGRASS");
	return tallgrass;
}

char* pathDePokemonMetadata(char * pokemon) {

	char *tallgrass = montajeDeArchivo();

	char* rutaFiles = "/Files/";

	char* rutaMeta = "/Metadata.bin";

	char* path = malloc(
			strlen(tallgrass) + strlen(rutaFiles) + strlen(pokemon)
					+ strlen(rutaMeta) + 1);
	int desplazamiento = 0;

	memcpy(path + desplazamiento, tallgrass, strlen(tallgrass));
	desplazamiento = desplazamiento + strlen(tallgrass);
	memcpy(path + desplazamiento, rutaFiles, strlen(rutaFiles));
	desplazamiento = desplazamiento + strlen(rutaFiles);
	memcpy(path + desplazamiento, pokemon, strlen(pokemon));
	desplazamiento = desplazamiento + strlen(pokemon);
	memcpy(path + desplazamiento, rutaMeta, strlen(rutaMeta));

	log_info(loggerGeneral, "Montaje de path pokemon metadata: %s \n", path);

	return path;
}

bool existePokemon(char* pokemon) {
	char* path = pathDePokemonMetadata(pokemon);
	FILE* archivoPokemon = fopen(path, "rb");
	bool existe = false;
	if (archivoPokemon != NULL) {
		existe = true;
		fclose(archivoPokemon);
	} else {
		log_info(loggerGeneral, "No existe el pokemon: %s en el sistema",
				pokemon);
	}
	return existe;
}

char* obtenerBloquesDeMetadataPokemon(char* pkm) {
	char* path = pathDePokemonMetadata(pkm);

	uint32_t tamanio_archivo_de_metadata = tamanio_archivo(path);

	log_info(loggerGeneral, "Tamanio archivo: %i", tamanio_archivo_de_metadata);

	int disco = open(path, O_RDWR, 0);

	char *archivoPokemon = mmap(NULL, tamanio_archivo_de_metadata,
	PROT_READ | PROT_WRITE,
	MAP_SHARED | MAP_FILE, disco, 0);

	log_info(loggerGeneral, "Mapeado %s", path);
	free(path);

	char** separadoPorEnter = string_split(archivoPokemon, "\n");
	char** bloquesEnPosicionUno = string_split(separadoPorEnter[2], "=");
	log_info(loggerGeneral, "Bloques: %s", bloquesEnPosicionUno[1]);
	return bloquesEnPosicionUno[1];

}

void agregarPosicionA(char* pkm, uint32_t posicionX, uint32_t posicionY,
		uint32_t cantidad) {
	char* bloques = obtenerBloquesDeMetadataPokemon(pkm);
	char** arrayDeBloques = string_get_string_as_array(bloques);
	int contador = 0;
	while (arrayDeBloques[contador] != NULL) {
		log_info(loggerGeneral, "%s", arrayDeBloques[contador]);
		contador = contador + 1;

	/*Agregar la posiciones
	 * Levanto el primer bloque o todos?
	 * Onda, busco en todos juntos o de a un bloque?
	 * Onda copio todo junto? o solo en el primero?
	 * mmap por cada archivo, memcpy todos a un mega char*,
	 * Buscar y despues memcpy a los mmaps de diez hasta que
	 * me quede uno mas chico a diez y ahi copio todo al ultimo bloque
	 * */

	}

}

void newPokemon(char* pkm, uint32_t posicionX, uint32_t posicionY,
		uint32_t cantidad) {

	bool existe = existePokemon(pkm);

	if (!existe) {
		log_error(loggerGeneral, "NO existe el pokemon: %s", pkm);
		//Falta: crear toda la estructura
	} else {

		log_info(loggerGeneral, "Existe: %i", existe);

		agregarPosicionA(pkm, posicionX, posicionY, cantidad);

	}
}

void atender(HeaderDelibird header, int cliente, t_log* logger) {

	switch (header.tipoMensaje) {
	case d_NEW_POKEMON:
		;
		log_info(logger, "Llego un new pokemon");

		void* packNewPokemon = Serialize_ReceiveAndUnpack(cliente,
				header.tamanioMensaje);
		uint32_t idMensajeNew, posicionNewX, posicionNewY, newCantidad;
		char *newNombrePokemon;
		Serialize_Unpack_NewPokemon(packNewPokemon, &idMensajeNew,
				&newNombrePokemon, &posicionNewX, &posicionNewY, &newCantidad);
		log_info(logger,
				"Me llego mensaje de %i. Id: %i, Pkm: %s, x: %i, y: %i, cant: %i\n",
				header.tipoMensaje, idMensajeNew, newNombrePokemon,
				posicionNewX, posicionNewY, newCantidad);

		newPokemon(newNombrePokemon, posicionNewX, posicionNewY, newCantidad);

		free(packNewPokemon);
		break;
	case d_CATCH_POKEMON:
		;
		log_info(logger, "Llego un catch pokemon");

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
		free(packCatchPokemon);
		break;
	case d_GET_POKEMON:
		;
		log_info(logger, "Llego un get pokemon");

		void* packGetPokemon = Serialize_ReceiveAndUnpack(cliente,
				header.tamanioMensaje);
		uint32_t idMensajeGet;
		char *getNombrePokemon;
		Serialize_Unpack_GetPokemon(packGetPokemon, &idMensajeGet,
				&getNombrePokemon);
		log_info(logger, "Me llego mensaje de %i. Id: %i, Pkm: %s\n",
				header.tipoMensaje, idMensajeGet, getNombrePokemon);
		// Se hace lo necesario
		free(packGetPokemon);
		break;
	default:
		log_error(logger, "Mensaje no entendido: %i\n", header);
		void* packBasura = Serialize_ReceiveAndUnpack(cliente,
				header.tamanioMensaje);
		free(packBasura);
		break;
	}
}

void recibirYAtenderUnCliente(int cliente, t_log* log) {
	while (1) {
		HeaderDelibird headerRecibido = Serialize_RecieveHeader(cliente);
		if (headerRecibido.tipoMensaje == -1) {
			log_error(log, "Se desconecto el GameBoy\n");
			break;
		}
		atender(headerRecibido, cliente, log);
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
	recibirYAtenderUnCliente(cliente, gameBoyLog);

	log_destroy(gameBoyLog);
}

void iniciarServidorDeGameBoy(pthread_t* servidor) {
	if (pthread_create(servidor, NULL, (void*) atenderGameBoy, NULL) == 0) {
		log_info(loggerGeneral, "::::Se creo hilo de GameBoy::::");
	} else {
		log_error(loggerGeneral, "::::No se pudo crear el hilo de GameBoy::::");
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

void* suscribirme(d_message colaDeSuscripcion) {
	char *puerto = config_get_string_value(archivo_de_configuracion,
			"PUERTO_BROKER");
	char *ip = config_get_string_value(archivo_de_configuracion, "IP_BROKER");

	uint32_t reconectar = config_get_int_value(archivo_de_configuracion,
			"TIEMPO_DE_REINTENTO_CONEXION");
	int conexion;

	while (1) {
		conexion = conectarse_a_un_servidor(ip, puerto, loggerGeneral);
		if (conexion == -1) {
			log_error(loggerGeneral,
					"No se pudo conectar con el Broken a la cola de: %i\n",
					colaDeSuscripcion);
			sleep(reconectar);
		} else {
			break;
		}
	}
	Serialize_PackAndSend_SubscribeQueue(conexion, colaDeSuscripcion);

	recibirYAtenderUnCliente(conexion, loggerGeneral);
}

void conectarmeColaDe(pthread_t* hilo, d_message colaDeSuscripcion) {

	if (pthread_create(hilo, NULL, (void*) suscribirme,
			(void*) colaDeSuscripcion) == 0) {
		log_info(loggerGeneral, "::::Se creo hilo de suscripcion para: %i::::",
				colaDeSuscripcion);
	} else {
		log_error(loggerGeneral,
				"::::No se pudo crear el hilo de suscripcion para: %i::::",
				colaDeSuscripcion);
	}

}

void * obtenerMetadata() {
	char *tallgrass = montajeDeArchivo();

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

	free(montajeMetadata);

	void *metadata = mmap(NULL, tamanio_archivo_de_metadata,
	PROT_READ | PROT_WRITE,
	MAP_SHARED | MAP_FILE, disco, 0);

	return metadata;
}

void cargarMetadata() {

	char *meta = obtenerMetadata();

	char **separadoPorEnters = string_split(meta, "\n");

	char **blockSizeEnPosicionUno = string_split(separadoPorEnters[0], "=");
	uint32_t blockSize = atoi(blockSizeEnPosicionUno[1]);
	log_info(loggerGeneral, "BLOCK_SIZE: %i", blockSize);

	char **blocksEnPosicionUno = string_split(separadoPorEnters[1], "=");
	uint32_t blocks = atoi(blocksEnPosicionUno[1]);
	log_info(loggerGeneral, "BLOCKS: %i", blocks);

	char **magicNumberEnPosicionUno = string_split(separadoPorEnters[2], "=");
	char *magicNumber = malloc(strlen(magicNumberEnPosicionUno[1]) + 1);

	memcpy(magicNumber, magicNumberEnPosicionUno[1],
			strlen(magicNumberEnPosicionUno[1]) + 1);

	log_info(loggerGeneral, "MAGIC_NUMBER: %s", magicNumber);

	metadata.tamanioDeBloque = blockSize;
	metadata.bloques = blocks;
	memcpy(metadata.numeroMagico, magicNumber, strlen(magicNumber) + 1);

	log_info(loggerGeneral, "ESTRUCTURA METADATA: %i, %i, %s",
			metadata.tamanioDeBloque, metadata.bloques, metadata.numeroMagico);

	free(magicNumber);

}

char* montajeDeBitmap() {

	char *tallgrass = montajeDeArchivo();

	char* rutaBitmap = "/Metadata/Bitmap.bin";

	char* montajeBitmap = malloc(strlen(tallgrass) + strlen(rutaBitmap) + 1);

	memcpy(montajeBitmap, tallgrass, strlen(tallgrass));
	memcpy(montajeBitmap + strlen(tallgrass), rutaBitmap,
			strlen(rutaBitmap) + 1);

	log_info(loggerGeneral, "Montaje de bitmap: %s \n", montajeBitmap);
	return montajeBitmap;
}

void * obtenerBitmap() {

	char *montajeBitmap = montajeDeBitmap();

	truncate(montajeBitmap, metadata.bloques);

	int disco = open(montajeBitmap, O_RDWR, 0);
	void *bitmapLevantado = mmap(NULL, metadata.bloques,
	PROT_READ | PROT_WRITE,
	MAP_SHARED | MAP_FILE, disco, 0);
	free(montajeBitmap);
	return bitmapLevantado;
}

void crearBitmap() {
	void* bitmapAuxiliar = obtenerBitmap();

	bitmap = bitarray_create_with_mode(bitmapAuxiliar, metadata.bloques,
			MSB_FIRST);
}

void iniciarBitmap() {

	crearBitmap();

	limpiar_bitmap(0, metadata.bloques, bitmap, loggerGeneral);

	log_info(loggerGeneral, "::::Bitmap Iniciado::::");

}

void levantarBitmap() {

	crearBitmap();

	log_info(loggerGeneral, "::::Bitmap Levantado::::");

}

void finalizar() {
	log_destroy(loggerGeneral);
	config_destroy(archivo_de_configuracion);
}

int main(void) {

	levantarLogYArchivoDeConfiguracion();
	cargarMetadata();
	iniciarBitmap();

	pthread_t* servidor = malloc(sizeof(pthread_t));
	iniciarServidorDeGameBoy(servidor);

	pthread_t* suscriptoNewPokemon = malloc(sizeof(pthread_t));
	conectarmeColaDe(suscriptoNewPokemon, d_NEW_POKEMON);

	pthread_t* suscriptoCatchPokemon = malloc(sizeof(pthread_t));
	conectarmeColaDe(suscriptoCatchPokemon, d_CATCH_POKEMON);

	pthread_t* suscriptoGetPokemon = malloc(sizeof(pthread_t));
	conectarmeColaDe(suscriptoGetPokemon, d_GET_POKEMON);

	pthread_join(*servidor, NULL);
	pthread_join(*suscriptoNewPokemon, NULL);
	pthread_join(*suscriptoCatchPokemon, NULL);
	pthread_join(*suscriptoGetPokemon, NULL);

	finalizar();

	return EXIT_SUCCESS;
}
