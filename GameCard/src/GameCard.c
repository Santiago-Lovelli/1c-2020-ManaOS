#include "GameCard.h"
#include <dirent.h>

int crearDirectorioEn(char *path) {
	return mkdir(path, 0777);
	//retorna 0 en exito, -1 si fallo
}

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
	memcpy(path + desplazamiento, rutaMeta, strlen(rutaMeta) + 1);

	log_info(loggerGeneral, "Montaje de path pokemon metadata: %s \n", path);

	return path;
}

bool existePokemon(char* pokemon) {
	int indice = indiceDePokemonEnLista(pokemon);
	return indice != (-1);
}

char *archivoMetadataPokemon(char *path) {

	uint32_t tamanio_archivo_de_metadata = tamanio_archivo(path);

	log_info(loggerGeneral, "Tamanio archivo: %i", tamanio_archivo_de_metadata);

	int disco = open(path, O_RDWR, 0);

	char *archivoPokemon = mmap(NULL, tamanio_archivo_de_metadata,
	PROT_READ | PROT_WRITE,
	MAP_SHARED | MAP_FILE, disco, 0);

	log_info(loggerGeneral, "Mapeado %s", path);
	free(path);
	return archivoPokemon;
}

char* obtenerOpen(char* pkm) {

	char* path = pathDePokemonMetadata(pkm);
	char *archivoPokemon = archivoMetadataPokemon(path);
	char** separadoPorEnter = string_split(archivoPokemon, "\n");
	char** abiertoEnUno = string_split(separadoPorEnter[3], "=");

	return abiertoEnUno[1];
}

p_metadata* obtenerMetadataEnteraDePokemon(char* unPokemon) {

	//A futuro hacer refactor para no habrir archivo de metadata tantas veces

	char* path = pathDePokemonMetadata(unPokemon);
	char *archivoPokemon = archivoMetadataPokemon(path);

	char** separadoPorEnter = string_split(archivoPokemon, "\n");

	uint32_t desplazamientoDirectory = strlen("DIRECTORY") + 1;
	uint32_t desplazamientoSize = strlen(separadoPorEnter[0]) + 1
			+ strlen("SIZE") + 1;
	uint32_t desplazamientoBlocks = strlen(separadoPorEnter[0]) + 1
			+ strlen(separadoPorEnter[1]) + 1 + strlen("BLOCKS") + 1;
	uint32_t desplazamientoOpen = strlen(separadoPorEnter[0]) + 1
			+ strlen(separadoPorEnter[1]) + 1 + strlen(separadoPorEnter[2]) + 1
			+ strlen("OPEN") + 1;

	p_metadata* metadataObtenida = malloc(
			strlen(archivoPokemon + desplazamientoDirectory)
					+ strlen(archivoPokemon + desplazamientoSize)
					+ strlen(archivoPokemon + desplazamientoBlocks)
					+ strlen(archivoPokemon + desplazamientoOpen));

	metadataObtenida->inicioDirectory = archivoPokemon
			+ desplazamientoDirectory;
	metadataObtenida->inicioSize = archivoPokemon + desplazamientoSize;
	metadataObtenida->inicioBloques = archivoPokemon + desplazamientoBlocks;
	metadataObtenida->inicioOpen = archivoPokemon + desplazamientoOpen;

	return metadataObtenida;
}

bool esElPokemon(p_pokemonSemaforo* pkmSem, char* pkm) {
	return strcmp(pkmSem->nombreDePokemon, pkm) == 0;
}

int indiceDePokemonEnLista(char* pkm) {
	return list_get_index(pokemonsEnFiles, pkm, (void*) esElPokemon);
}

p_pokemonSemaforo* obtenerPokemonSemaforo(char* pokemon) {
	int indice = indiceDePokemonEnLista(pokemon);
	return list_get(pokemonsEnFiles, indice);
}

char* obtenerPathDeBloque(char* bloque) {
	char* montaje = montajeDeArchivo();
	char* bloq = "/Blocks/";
	char* bin = ".bin";
	char* path = malloc(
			strlen(montaje) + strlen(bloq) + strlen(bloque) + strlen(bin) + 1);

	int desplazamiento = 0;

	memcpy(path + desplazamiento, montaje, strlen(montaje));
	desplazamiento = desplazamiento + strlen(montaje);

	memcpy(path + desplazamiento, bloq, strlen(bloq));
	desplazamiento = desplazamiento + strlen(bloq);

	memcpy(path + desplazamiento, bloque, strlen(bloque));
	desplazamiento = desplazamiento + strlen(bloque);

	memcpy(path + desplazamiento, bin, strlen(bin));
	desplazamiento = desplazamiento + strlen(bin);

	memcpy(path + desplazamiento, "\0", 1);

	log_info(loggerGeneral, "path en funcion:%s", path);

	return path;
}

uint32_t esEstaPosicion(char* linea, uint32_t posicionEnX, uint32_t posicionEnY) {
	char** posicionEnCero = string_split(linea, "=");
	char** posiciones = string_split(posicionEnCero[0], "-");
	char* posX = string_itoa(posicionEnX);
	char* posY = string_itoa(posicionEnY);
	return strcmp(posiciones[0], posX) == 0 && strcmp(posiciones[1], posY) == 0;

}

uint32_t nuevaCantidad(char* linea, uint32_t cantidad) {
	char** cantidadEnUno = string_split(linea, "=");
	uint32_t cantidadEnPokemon = atoi(cantidadEnUno[1]);
	uint32_t nuevo = cantidad + cantidadEnPokemon;
	log_info(loggerGeneral, "Cantidad Nueva: %i", nuevo);
	return nuevo;
}

char* obtenerBloqueConDatosReales(char* numeroDeBloque) {
	log_info(loggerGeneral, "bloque a escribir: %s", numeroDeBloque);

	char * pathBloque = obtenerPathDeBloque(numeroDeBloque);

	log_info(loggerGeneral, "path a escribir: %s", pathBloque);

	int bloque = open(pathBloque, O_RDWR, 0);

	free(pathBloque);

	char* bloqueConDatos = mmap(NULL, metadata.tamanioDeBloque,
	PROT_READ | PROT_WRITE,
	MAP_SHARED | MAP_FILE, bloque, 0);

	return bloqueConDatos;
}

void agregarPosicionA(char* pkm, uint32_t posicionX, uint32_t posicionY,
		uint32_t cantidad) {

	p_metadata* metadataDePokemon = obtenerMetadataEnteraDePokemon(pkm);

	uint32_t reconectar = config_get_int_value(archivo_de_configuracion,
			"TIEMPO_DE_REINTENTO_OPERACION");

	p_pokemonSemaforo* semaforoDePokemon = obtenerPokemonSemaforo(pkm);

	while (1) {
		pthread_mutex_lock(&semaforoDePokemon->semaforoDePokemon);
		if (strcmp(metadataDePokemon->inicioOpen, "N") == 0) {

			memcpy(metadataDePokemon->inicioOpen, "Y", strlen("Y"));
			pthread_mutex_unlock(&semaforoDePokemon->semaforoDePokemon);
			log_info(loggerGeneral, "Se puede escribir %s", pkm);

			/*----------------------------------------------------------------*/
			char** bloquesEnUno = string_split(metadataDePokemon->inicioBloques,
					"\n");
			char** arrayConBloques = string_get_string_as_array(
					bloquesEnUno[0]);
			int i = 0;

			/*-------------------------------Recorriendo bloques---------------------------------*/

			char* megaChar = string_new();

			while (arrayConBloques[i] != NULL) {
				log_info(loggerGeneral, "bloques %s", arrayConBloques[i]);

				char* pathBloque = obtenerPathDeBloque(arrayConBloques[i]);
				log_info(loggerGeneral, "path %s", pathBloque);

				int bloque = open(pathBloque, O_RDWR, 0);

				free(pathBloque);

				log_info(loggerGeneral, "Tamanio de bloque %i",
						metadata.tamanioDeBloque);

				char *bloqueConDatos = mmap(NULL, metadata.tamanioDeBloque,
				PROT_READ | PROT_WRITE,
				MAP_SHARED | MAP_FILE, bloque, 0);

				log_info(loggerGeneral, "bloqueConDatos %s", bloqueConDatos);

				string_append(&megaChar, bloqueConDatos);

				i = i + 1;
			}

			log_info(loggerGeneral, "levantado \n%s", megaChar);

			char** lineasDeBloque = string_split(megaChar, "\n");

			int cantidadDeLineas = 0;
			int leido = 0;

			char* cantidadNueva;

			char* cantidadVieja;

			while (lineasDeBloque[cantidadDeLineas] != NULL) {

				log_info(loggerGeneral, "lineas %i: %s mide: %i",
						cantidadDeLineas, lineasDeBloque[cantidadDeLineas],
						strlen(lineasDeBloque[cantidadDeLineas]));

				if (esEstaPosicion(lineasDeBloque[cantidadDeLineas], posicionX,
						posicionY)) {

					log_info(loggerGeneral, "Esta en linea: %s",
							lineasDeBloque[cantidadDeLineas]);

					char** cantidadViejaEnUno = string_split(
							lineasDeBloque[cantidadDeLineas], "=");
					cantidadVieja = cantidadViejaEnUno[1];

					uint32_t cantidadActual = nuevaCantidad(
							lineasDeBloque[cantidadDeLineas], cantidad);

					log_info(loggerGeneral, "Nueva Cantidad: %i",
							cantidadActual);
					cantidadNueva = string_itoa(cantidadActual);
					break;
				}

				leido = leido + strlen(lineasDeBloque[cantidadDeLineas]) + 1;
				cantidadDeLineas = cantidadDeLineas + 1;
			}

			log_info(loggerGeneral, "leido: %i", leido);
			uint32_t bloquesIntactos = leido / metadata.tamanioDeBloque;
			log_info(loggerGeneral, "bloquesIntactos: %i", bloquesIntactos);

			i = 0;

			uint32_t escrito = 0;

			while (i < bloquesIntactos) {
				log_info(loggerGeneral, "bloque a escribir: %s",
						arrayConBloques[i]);

				char* pathBloque = obtenerPathDeBloque(arrayConBloques[i]);
				log_info(loggerGeneral, "path a escribir: %s", pathBloque);

				int bloque = open(pathBloque, O_RDWR, 0);

				free(pathBloque);

				char *bloqueConDatos = mmap(NULL, metadata.tamanioDeBloque,
				PROT_READ | PROT_WRITE,
				MAP_SHARED | MAP_FILE, bloque, 0);

				memcpy(bloqueConDatos, megaChar + escrito,
						metadata.tamanioDeBloque);
				escrito = escrito + metadata.tamanioDeBloque;

				i = i + 1;
			}

			log_info(loggerGeneral, "bloque a escribir: %s",
					arrayConBloques[i]);

			char* pathBloque = obtenerPathDeBloque(arrayConBloques[i]);

			i = i + 1;

			log_info(loggerGeneral, "path a escribir: %s", pathBloque);

			int bloque = open(pathBloque, O_RDWR, 0);

			free(pathBloque);

			char *bloqueConDatos = mmap(NULL, metadata.tamanioDeBloque,
			PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_FILE, bloque, 0);

			uint32_t faltante = leido - escrito;

			memcpy(bloqueConDatos, megaChar + escrito, faltante);
			escrito = escrito + faltante;

			char** hastaIgual = string_split(lineasDeBloque[cantidadDeLineas],
					"=");

			uint32_t quedaConIgual = strlen(hastaIgual[0]) + 1;
			uint32_t quedaEnBloque = metadata.tamanioDeBloque - faltante;

			uint32_t quedaValor = strlen(cantidadNueva);

			if (quedaEnBloque >= quedaConIgual) {

				memcpy(bloqueConDatos, megaChar + escrito, quedaConIgual);
				escrito = escrito + quedaConIgual;
				quedaEnBloque = quedaEnBloque - quedaConIgual;

			} else {
				memcpy(bloqueConDatos, megaChar + escrito, quedaEnBloque);
				escrito = escrito + quedaEnBloque;
				quedaEnBloque = 0;
				uint32_t quedaParaIgual = quedaConIgual - quedaEnBloque;

				/*--Nuevo bloque--*/
				log_info(loggerGeneral, "bloque a escribir: %s",
						arrayConBloques[i]);

				pathBloque = obtenerPathDeBloque(arrayConBloques[i]);
				i = i + 1;
				log_info(loggerGeneral, "path a escribir: %s", pathBloque);

				bloque = open(pathBloque, O_RDWR, 0);

				free(pathBloque);

				bloqueConDatos = mmap(NULL, metadata.tamanioDeBloque,
				PROT_READ | PROT_WRITE,
				MAP_SHARED | MAP_FILE, bloque, 0);

				memcpy(bloqueConDatos, megaChar + escrito, quedaParaIgual);
				escrito = escrito + quedaParaIgual;
				quedaEnBloque = quedaEnBloque - quedaParaIgual;
			}

			if (quedaEnBloque >= quedaValor) {

				memcpy(bloqueConDatos, cantidadNueva, quedaValor);
				escrito = escrito + strlen(cantidadVieja);
				//me estoy salteando la cantidad vieja para cuando lea del megachar
				quedaEnBloque = quedaEnBloque - quedaValor;

				memcpy(bloqueConDatos, megaChar + escrito, quedaEnBloque);
				escrito = escrito + quedaEnBloque;
				quedaEnBloque = 0;

			} else {
				memcpy(bloqueConDatos, cantidadNueva, quedaEnBloque);
				quedaValor = quedaValor - quedaEnBloque;

				/*--Nuevo bloque--*/
				log_info(loggerGeneral, "bloque a escribir: %s",
						arrayConBloques[i]);

				pathBloque = obtenerPathDeBloque(arrayConBloques[i]);
				i = i + 1;
				log_info(loggerGeneral, "path a escribir: %s", pathBloque);

				bloque = open(pathBloque, O_RDWR, 0);

				free(pathBloque);

				bloqueConDatos = mmap(NULL, metadata.tamanioDeBloque,
				PROT_READ | PROT_WRITE,
				MAP_SHARED | MAP_FILE, bloque, 0);

				memcpy(bloqueConDatos, cantidadNueva, quedaValor);
				escrito = escrito + strlen(cantidadVieja);
				//me estoy salteando la cantidad vieja para cuando lea del megachar
				quedaEnBloque = metadata.tamanioDeBloque - quedaValor;

				memcpy(bloqueConDatos, megaChar + escrito, quedaEnBloque);
				escrito = escrito + quedaEnBloque;

				quedaEnBloque = 0;
			}

			//A copiar la info que falta en el archivo

			char** tam = string_split(metadataDePokemon->inicioSize, "\n");
			uint32_t tamanioOriginal = atoi(tam[0]);
			uint32_t faltanteDelMegaChar = tamanioOriginal - escrito;
			char* bloquesNuevos = string_new();

			while (escrito <= faltanteDelMegaChar) {
				if (arrayConBloques[i] != NULL) {
					char* numeroDeBloqueNuevo = string_itoa(
							buscar_espacio_en_bitmap(bitmap, loggerGeneral));

					string_append(&bloquesNuevos, numeroDeBloqueNuevo);

					log_info(loggerGeneral, "bloque a escribir: %s",
							numeroDeBloqueNuevo);

					pathBloque = obtenerPathDeBloque(numeroDeBloqueNuevo);
					log_info(loggerGeneral, "path a escribir: %s", pathBloque);

					bloque = open(pathBloque, O_RDWR, 0);

					free(pathBloque);

					bloqueConDatos = mmap(NULL, metadata.tamanioDeBloque,
					PROT_READ | PROT_WRITE,
					MAP_SHARED | MAP_FILE, bloque, 0);

					quedaEnBloque = metadata.tamanioDeBloque;
				} else {
					log_info(loggerGeneral, "bloque a escribir: %s",
							arrayConBloques[i]);

					pathBloque = obtenerPathDeBloque(arrayConBloques[i]);
					log_info(loggerGeneral, "path a escribir: %s", pathBloque);

					bloque = open(pathBloque, O_RDWR, 0);

					free(pathBloque);

					bloqueConDatos = mmap(NULL, metadata.tamanioDeBloque,
					PROT_READ | PROT_WRITE,
					MAP_SHARED | MAP_FILE, bloque, 0);

					quedaEnBloque = metadata.tamanioDeBloque;
				}

				if (faltanteDelMegaChar > quedaEnBloque) {
					memcpy(bloqueConDatos, megaChar + escrito,
							metadata.tamanioDeBloque);
					escrito = escrito + metadata.tamanioDeBloque;
					faltanteDelMegaChar = faltanteDelMegaChar - quedaEnBloque;
					quedaEnBloque = 0;
				} else {
					memcpy(bloqueConDatos, megaChar + escrito,
							faltanteDelMegaChar);
					escrito = escrito + faltanteDelMegaChar;
					faltanteDelMegaChar = 0;
					quedaEnBloque = quedaEnBloque - faltanteDelMegaChar;
				}

				i = i + 1;
			}

			char* metadataPost = string_duplicate("DIRECTORY=N\nSIZE=");

			uint32_t tamanioNuevoFinal = tamanioOriginal + strlen(cantidadVieja) - strlen(cantidadNueva);
			string_append(&metadataPost,string_itoa(tamanioNuevoFinal));
			string_append(&metadataPost,"\n");
			string_append(&metadataPost,"BLOCKS=[");
			int aux = 0;
			for (int j = 0; j < i; ++j) {
				if(arrayConBloques[j] != NULL){
					string_append(&metadataPost,arrayConBloques[j]);
				}else{
					string_append(&metadataPost,&bloquesNuevos[aux]);
					aux = aux +1;
				}
			}
			string_append(&metadataPost,"]\nOPEN=N");

			/*----------------------------------------------------------------*/
			sleep(100);
			break;
		} else {
			pthread_mutex_unlock(&semaforoDePokemon->semaforoDePokemon);
			log_error(loggerGeneral,
					"El archivo %s ya esta abierto por otro proceso", pkm);
			sleep(reconectar);
		}
	}

	/*Agregar la posiciones
	 * Levanto el primer bloque o todos?
	 * Onda, busco en todos juntos o de a un bloque?
	 * Onda copio todo junto? o solo en el primero?
	 * mmap por cada archivo, memcpy todos a un mega char*,
	 * Buscar y despues memcpy a los mmaps de diez hasta que
	 * me quede uno mas chico a diez y ahi copio todo al ultimo bloque
	 * */
}

void newPokemon(char* pkm, uint32_t posicionX, uint32_t posicionY,
		uint32_t cantidad) {

	bool existe = existePokemon(pkm);

	if (!existe) {
		log_error(loggerGeneral, "NO existe el pokemon: %s", pkm);
		puts("**************  A HACER  **************");
		//Falta: crear toda la estructura
	} else {
		log_info(loggerGeneral, "Existe el pokemon %s", pkm);
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

void* recibirYAtenderUnCliente(p_elementoDeHilo* elemento) {
	while (1) {
		HeaderDelibird headerRecibido = Serialize_RecieveHeader(
				elemento->cliente);
		if (headerRecibido.tipoMensaje == -1) {
			log_error(elemento->log, "Se desconecto el Cliente\n");
			break;
		}
		atender(headerRecibido, elemento->cliente, elemento->log);
	}
	return 0;
}

void* atenderGameBoy() {
	t_log* gameBoyLog = iniciar_log("GameBoy");

	char *puerto = config_get_string_value(archivo_de_configuracion,
			"PUERTO_GAMECARD");

	log_info(gameBoyLog, "Puerto GameCard: %s", puerto);

	char *ip = config_get_string_value(archivo_de_configuracion, "IP_GAMECARD");

	log_info(gameBoyLog, "Ip GameCard: %s", ip);

	int conexion = iniciar_servidor(ip, puerto, gameBoyLog);
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

	p_elementoDeHilo* elemento;
	elemento->cliente = conexion;
	elemento->log = loggerGeneral;

	recibirYAtenderUnCliente(elemento);
	return 0;
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

void cargarListaAtual() {
	DIR *dir;
	struct dirent *dirrectorio;

	char *tallgrass = montajeDeArchivo();

	char* rutaFiles = "/Files/";

	char* path = malloc(strlen(tallgrass) + strlen(rutaFiles) + 1);
	int desplazamiento = 0;

	memcpy(path + desplazamiento, tallgrass, strlen(tallgrass));
	desplazamiento = desplazamiento + strlen(tallgrass);
	memcpy(path + desplazamiento, rutaFiles, strlen(rutaFiles) + 1);

	log_info(loggerGeneral, "Ruta de t_list pokemons: %s \n", path);

	pokemonsEnFiles = list_create();

	dir = opendir(path);
	int cont = 0;
	if (dir) {
		while ((dirrectorio = readdir(dir)) != NULL) {
			char* nombre = dirrectorio->d_name;
			if (dirrectorio->d_type == 4 && strcmp(nombre, ".") != 0
					&& strcmp(nombre, "..") != 0) {

				p_pokemonSemaforo* auxPokemon = malloc(
						strlen(nombre) + 1 + sizeof(pthread_mutex_t));

				auxPokemon->nombreDePokemon = malloc(strlen(nombre) + 1);

				memcpy(auxPokemon->nombreDePokemon, nombre, strlen(nombre));
				memcpy(auxPokemon->nombreDePokemon + strlen(nombre), "\0", 1);
				pthread_mutex_init(&(auxPokemon->semaforoDePokemon), NULL);

				list_add(pokemonsEnFiles, auxPokemon);
				cont++;
			}
		}
		closedir(dir);
		for (int i = 0; i < cont; ++i) {
			p_pokemonSemaforo* elDeLaLista = list_get(pokemonsEnFiles, i);
			log_info(loggerGeneral, "Elemento en index %i: %s\n", i,
					elDeLaLista->nombreDePokemon);
		}
	}
}

void finalizar() {
	log_destroy(loggerGeneral);
	config_destroy(archivo_de_configuracion);
}

int main(void) {

	levantarLogYArchivoDeConfiguracion();
	cargarListaAtual();
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
