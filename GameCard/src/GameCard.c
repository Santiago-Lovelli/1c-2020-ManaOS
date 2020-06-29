#include "GameCard.h"

int crearDirectorioEn(char *path) {
	return mkdir(path, 0777);
	//retorna 0 en exito, -1 si fallo
}

char* montajeDeArchivo() {
	char *tallgrass = config_get_string_value(archivo_de_configuracion,
			"PUNTO_MONTAJE_TALLGRASS");
	return tallgrass;
}

char* rutaDeFiles() {
	char* tallgrass = montajeDeArchivo();
	char* rutaFiles = "/Files/";
	char* path = malloc(strlen(tallgrass) + strlen(rutaFiles) + 1);
	int desplazamiento = 0;
	memcpy(path + desplazamiento, tallgrass, strlen(tallgrass));
	desplazamiento = desplazamiento + strlen(tallgrass);
	memcpy(path + desplazamiento, rutaFiles, strlen(rutaFiles) + 1);

	log_info(loggerGeneral, "Ruta de t_list pokemons: %s \n", path);

	return path;
}

char* pathDePokemonMetadata(char * pokemon) {

	char* rutaFiles = rutaDeFiles();

	char* rutaMeta = "/Metadata.bin";

	char* path = string_new();

	string_append(&path, rutaFiles);
	string_append(&path, pokemon);
	string_append(&path, rutaMeta);

	log_info(loggerGeneral, "Montaje de path pokemon metadata: %s \n", path);

	return path;
}

bool existePokemon(char* pokemon) {
	int indice = indiceDePokemonEnLista(pokemon);
	return indice != (-1);
}

void* mmapeadoBloquePropio(t_log* log, uint32_t tamanioDeseado,
		char* numeroDeBloque) {
	log_info(log, "bloque a escribir: %s", numeroDeBloque);

	char * pathBloque = obtenerPathDeBloque(numeroDeBloque);

	log_info(log, "path a escribir: %s", pathBloque);

	uint32_t tamanio_archivo_de_metadata = tamanio_archivo(pathBloque);

	if (tamanioDeseado > tamanio_archivo_de_metadata) {
		truncate(pathBloque, tamanioDeseado);
		tamanio_archivo_de_metadata = tamanioDeseado;
	}

	int bloque = open(pathBloque, O_RDWR, 0);

	free(pathBloque);

	void* bloqueConDatos = mmap(NULL, tamanio_archivo_de_metadata,
	PROT_READ | PROT_WRITE,
	MAP_SHARED | MAP_FILE, bloque, 0);

	return bloqueConDatos;
}

char *archivoMetadataPokemon(char *path, uint32_t cantidadALevantar) {

	int tamanio_archivo_de_metadata = tamanio_archivo(path);

	if (cantidadALevantar > tamanio_archivo_de_metadata) {
		truncate(path, cantidadALevantar);
		tamanio_archivo_de_metadata = tamanio_archivo(path);
	}

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
	char *archivoPokemon = archivoMetadataPokemon(path, 0);
	char** separadoPorEnter = string_split(archivoPokemon, "\n");
	char** abiertoEnUno = string_split(separadoPorEnter[3], "=");

	return abiertoEnUno[1];
}

p_metadata* obtenerMetadataEnteraDePokemon(char* unPokemon) {

	//A futuro hacer refactor para no abrir archivo de metadata tantas veces

	p_pokemonSemaforo* semaforoDePokemon = obtenerPokemonSemaforo(unPokemon);

	sem_wait(&(semaforoDePokemon->metadataDePokemon));

	char* path = pathDePokemonMetadata(unPokemon);
	char *archivoPokemon = archivoMetadataPokemon(path, 0);

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
	sem_post(&(semaforoDePokemon->metadataDePokemon));
	return metadataObtenida;
}

bool esElPokemon(p_pokemonSemaforo* pkmSem, char* pkm) {
	return strcmp(pkmSem->nombreDePokemon, pkm) == 0;
}

int indiceDePokemonEnLista(char* pkm) {
	sem_wait(&listaPokemon);
	int indice = list_get_index(pokemonsEnFiles, pkm, (void*) esElPokemon);
	sem_post(&listaPokemon);
	return indice;
}

p_pokemonSemaforo* obtenerPokemonSemaforo(char* pokemon) {

	int indice = indiceDePokemonEnLista(pokemon);
	sem_wait(&listaPokemon);
	p_pokemonSemaforo* aux = list_get(pokemonsEnFiles, indice);
	sem_post(&listaPokemon);
	return aux;
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

void cambiarMetadata(char* unPokemon, char* metadataNueva) {

	p_pokemonSemaforo* semaforoDePokemon = obtenerPokemonSemaforo(unPokemon);

	sem_wait(&(semaforoDePokemon->metadataDePokemon));

	char* path = pathDePokemonMetadata(unPokemon);

	int cantidadARellenar = tamanio_archivo(path) - strlen(metadataNueva);

	char *archivoPokemon = archivoMetadataPokemon(path, strlen(metadataNueva));

	memcpy(archivoPokemon, metadataNueva, strlen(metadataNueva));

	for (int i = 0; i < cantidadARellenar; ++i) {
		memcpy(archivoPokemon + strlen(metadataNueva) + i, "\0", 1);
	}
	sem_post(&(semaforoDePokemon->metadataDePokemon));
}

void limpiarBloque(int numeroDeBloque) {
	sem_wait(&bitSem);

	liberar_bloque_en_bitmap(numeroDeBloque, bitmap);

	sem_post(&bitSem);
}

char* obtenerBloqueReservadoEnChar() {
	sem_wait(&bitSem);
	int bloqueReservado = buscar_espacio_en_bitmap(bitmap, loggerGeneral);

	if (bloqueReservado == -1) {
		log_error(loggerGeneral, "Error al buscar un nuevo bloque");
		EXIT_FAILURE;
	} else {
		ocupar_bloque_en_bitmap(bloqueReservado, bitmap);
	}

	sem_post(&bitSem);

	char* numeroDeBloqueNuevo = string_itoa(bloqueReservado);

	return numeroDeBloqueNuevo;
}

int minimo(int unNumero, int otroNumero) {
	if (unNumero < otroNumero) {
		return unNumero;
	}
	return otroNumero;
}

int maximo(int unNumero, int otroNumero) {
	if (unNumero > otroNumero) {
		return unNumero;
	}
	return otroNumero;
}

char* leerBloques(t_list* bloques, uint32_t tamanio) {
	char* leido = string_new();
	int i = 0;
	int desplazamiento = 0;
	while (list_get(bloques, i) != NULL) {
		log_info(loggerGeneral, "bloques %s", list_get(bloques, i));

		int aLevantar = minimo(metadata.tamanioDeBloque,
				tamanio - desplazamiento);

		char *bloqueConDatos = mmapeadoBloquePropio(loggerGeneral, aLevantar,
				list_get(bloques, i));

		log_info(loggerGeneral, "bloqueConDatos %s", bloqueConDatos);

		string_append(&leido, bloqueConDatos);

		desplazamiento = desplazamiento + aLevantar;

		i = i + 1;
	}

	log_info(loggerGeneral, "El archivo del pokemon es:\n%s", leido);

	return leido;
}

int numeroDeLineas(char **lineasDeBloque, int posicionX, int posicionY,
		char** linea) {
	int cantidadDeLineas = 0;
	while (lineasDeBloque[cantidadDeLineas] != NULL) {

		log_info(loggerGeneral, "lineas %i: %s mide: %i", cantidadDeLineas,
				lineasDeBloque[cantidadDeLineas],
				strlen(lineasDeBloque[cantidadDeLineas]));

		if (esEstaPosicion(lineasDeBloque[cantidadDeLineas], posicionX,
				posicionY)) {
			string_append(linea, lineasDeBloque[cantidadDeLineas]);
			log_info(loggerGeneral, "Esta en linea: %s",
					lineasDeBloque[cantidadDeLineas]);

			return cantidadDeLineas;
		}
		cantidadDeLineas = cantidadDeLineas + 1;
	}
	log_info(loggerGeneral, "No se ha encontrado la posicion");
	return -1;
}

int escribirEnBloquesDiciendoEnQueBloqueEstoy(int tamanioAEscribir,
		t_list* bloques, int contadorDeBloque, char* megaChar, int escrito,
		int desplazamiento) {
	int escritoDeLoPedido = 0;
	log_info(loggerGeneral, "Bloque a escribir: %s",
			list_get(bloques, contadorDeBloque));

	if (list_get(bloques, contadorDeBloque) == NULL) {
		char* numero = obtenerBloqueReservadoEnChar();
		list_add(bloques, numero);
	}
	char* bloqueAEscribir = mmapeadoBloquePropio(loggerGeneral,
			metadata.tamanioDeBloque, list_get(bloques, contadorDeBloque));
	int escritoEnBloque = escrito % metadata.tamanioDeBloque;
	int faltanteEnBloque = metadata.tamanioDeBloque - escritoEnBloque;

	while (escritoDeLoPedido < tamanioAEscribir) {

		if (escritoEnBloque == metadata.tamanioDeBloque) {

			if (list_get(bloques, contadorDeBloque + 1) == NULL) {
				char* numero = obtenerBloqueReservadoEnChar();
				list_add(bloques, numero);
			}
			contadorDeBloque = contadorDeBloque + 1;
			bloqueAEscribir = mmapeadoBloquePropio(loggerGeneral,
					metadata.tamanioDeBloque,
					list_get(bloques, contadorDeBloque));
			escritoEnBloque = 0;
			faltanteEnBloque = metadata.tamanioDeBloque;
		}

		int aEscribir = minimo(tamanioAEscribir - escritoDeLoPedido,
				faltanteEnBloque);

		memcpy(bloqueAEscribir + escritoEnBloque, megaChar + desplazamiento,
				aEscribir);

		escritoEnBloque = escritoEnBloque + aEscribir;
		desplazamiento = desplazamiento + aEscribir;
		escritoDeLoPedido = escritoDeLoPedido + aEscribir;
		faltanteEnBloque = faltanteEnBloque - aEscribir;
	}
	if (escritoEnBloque == metadata.tamanioDeBloque) {
		contadorDeBloque = contadorDeBloque + 1;
	}
	return contadorDeBloque;
}

void cargarListaDeBloques(char** arrayConBloques, t_list* listaDeBloques) {
	int i = 0;
	while (arrayConBloques[i] != NULL) {
		list_add(listaDeBloques, arrayConBloques[i]);
		i = i + 1;
	}
}

void abrirArchivoPokemon(char*pkm) {
	uint32_t reconectar = config_get_int_value(archivo_de_configuracion,
			"TIEMPO_DE_REINTENTO_OPERACION");
	p_pokemonSemaforo* semaforoDePokemon = obtenerPokemonSemaforo(pkm);
	while (1) {
		p_metadata* metadataDePokemon = obtenerMetadataEnteraDePokemon(pkm);
		sem_wait(&semaforoDePokemon->semaforoDePokemon);
		if (strcmp(metadataDePokemon->inicioOpen, "N") == 0) {
			memcpy(metadataDePokemon->inicioOpen, "Y", strlen("Y"));
			sem_post(&semaforoDePokemon->semaforoDePokemon);
			log_info(loggerGeneral, "Se puede abrir %s", pkm);
			break;
		}
		sem_post(&semaforoDePokemon->semaforoDePokemon);
		log_error(loggerGeneral,
				"El archivo %s ya esta abierto por otro proceso", pkm);
		sleep(reconectar);
	}

}

char* leerUnPokemon(char* pkm, t_list* listaDeBloques) {

	p_metadata* metadataDePokemon = obtenerMetadataEnteraDePokemon(pkm);

	char** bloquesEnUno = string_split(metadataDePokemon->inicioBloques, "\n");
	char** arrayConBloques = string_get_string_as_array(bloquesEnUno[0]);
	cargarListaDeBloques(arrayConBloques, listaDeBloques);

	char** tamEnCero = string_split(metadataDePokemon->inicioSize, "\n");
	int tamanioDelPokemon = atoi(tamEnCero[0]);

	char* archivoDePokemon = leerBloques(listaDeBloques, tamanioDelPokemon);

	return archivoDePokemon;
}

char* metadataNueva(int tamanioDelPokemon, uint32_t tamanioNuevoFinal,
		char**lineaSeparadaPorIgual, t_list* listaDeBloques) {
	char* metadataPost = string_duplicate("DIRECTORY=N\nSIZE=");

	string_append(&metadataPost, string_itoa(tamanioNuevoFinal));
	string_append(&metadataPost, "\n");

	string_append(&metadataPost, "BLOCKS=[");

	uint32_t bloquesTotales = ceil(
			(float) tamanioNuevoFinal / metadata.tamanioDeBloque);

	for (int j = 0; j < bloquesTotales; ++j) {
		if (list_get(listaDeBloques, j) != NULL) {
			string_append(&metadataPost, list_get(listaDeBloques, j));
			if (list_get(listaDeBloques, j + 1) != NULL)
				string_append(&metadataPost, ",");
		} else {
			log_error(loggerGeneral, "No hay bloque en posicion: %i", j);
		}
	}

	string_append(&metadataPost, "]\nOPEN=N");

	return metadataPost;
}

void enviarAperedPokemon(char* pkm, uint32_t posicionX, uint32_t posicionY,
		uint32_t cantidad, uint32_t idMensajeNew) {
	char *ip = config_get_string_value(archivo_de_configuracion, "IP_BROKER");
	char *puerto = config_get_string_value(archivo_de_configuracion,
			"PUERTO_BROKER");

	int conexion = conectarse_a_un_servidor(ip, puerto, loggerGeneral);
	if (conexion == -1) {
		log_error(loggerGeneral,
				"No se pudo conectar al Broker para un aperedPokemon");
	}
	Serialize_PackAndSend_APPEARED_POKEMON(conexion, idMensajeNew, pkm,
			posicionX, posicionY);
}

void escribirUnPokemon(int cantidadDeLineas, char** lineasDeBloque,
		t_list* listaDeBloques, char* megaChar, int cantidadNueva) {
	int contadorDeBloque = 0;
	int escrito = 0;

	for (int lineaActual = 0; lineaActual < cantidadDeLineas; ++lineaActual) {
		int lineaEntera = strlen(lineasDeBloque[lineaActual]) + 1;

		contadorDeBloque = escribirEnBloquesDiciendoEnQueBloqueEstoy(
				lineaEntera, listaDeBloques, contadorDeBloque, megaChar,
				escrito, escrito);

		escrito = escrito + lineaEntera;
	}

	char** lineaSeparadaPorIgual = string_split(
			lineasDeBloque[cantidadDeLineas], "=");

	/*					Empiezo a escribir la linea					*/
	int desplazamiento = 0;
	if (cantidadNueva > 0) {
		int faltanteDeLineaHastaIgual = strlen(lineaSeparadaPorIgual[0]) + 1;

		contadorDeBloque = escribirEnBloquesDiciendoEnQueBloqueEstoy(
				faltanteDeLineaHastaIgual, listaDeBloques, contadorDeBloque,
				megaChar, escrito, escrito);
		escrito = escrito + faltanteDeLineaHastaIgual;

		char* cantidadAEscribir = string_itoa(cantidadNueva);
		faltanteDeLineaHastaIgual = strlen(cantidadAEscribir);

		contadorDeBloque = escribirEnBloquesDiciendoEnQueBloqueEstoy(
				faltanteDeLineaHastaIgual, listaDeBloques, contadorDeBloque,
				cantidadAEscribir, escrito, 0);
		desplazamiento = escrito + faltanteDeLineaHastaIgual;
		escrito = escrito + strlen(lineaSeparadaPorIgual[1]); //salteo lo viejo

	} else {
		escrito = escrito + strlen(lineasDeBloque[cantidadDeLineas]); //salteo la linea
	}

	int faltanteEscribir = strlen(megaChar) - escrito;

	contadorDeBloque = escribirEnBloquesDiciendoEnQueBloqueEstoy(
			faltanteEscribir, listaDeBloques, contadorDeBloque, megaChar,
			desplazamiento, escrito);
	desplazamiento = desplazamiento + faltanteEscribir;
	escrito = escrito + faltanteEscribir;

	int bloquesBasura = listaDeBloques->elements_count - contadorDeBloque - 1;

	for (int aLimpiar = 0; aLimpiar < bloquesBasura; ++aLimpiar) {
		log_info(loggerGeneral, "\n::  NOP  ::\n");
		log_info(loggerGeneral, "\n::  bloquesBasura  ::\n",bloquesBasura);
		log_info(loggerGeneral, "\n::  listaDeBloques->elements_count  ::\n",listaDeBloques->elements_count);
		log_info(loggerGeneral, "\n::  contadorDeBloque  ::\n",contadorDeBloque);
		int numeroDeBloque = atoi(list_get(listaDeBloques, contadorDeBloque - 1));
		limpiarBloque(numeroDeBloque);
		list_remove(listaDeBloques, contadorDeBloque - 1);
	}
}

void agregarPokemonesNuevos(char* pkm, uint32_t posicionX, uint32_t posicionY,
		uint32_t cantidad, uint32_t idMensajeNew) {

	abrirArchivoPokemon(pkm);

	p_metadata* metadataDePokemon = obtenerMetadataEnteraDePokemon(pkm);

	t_list* listaDeBloques = list_create();

	char** tamEnCero = string_split(metadataDePokemon->inicioSize, "\n");
	int tamanioDelPokemon = atoi(tamEnCero[0]);

	char* megaChar = leerUnPokemon(pkm, listaDeBloques);

	char** lineasDeBloque = string_split(megaChar, "\n");

	int cantidadDeLineas = 0;

	char* lineaConDePosicion = string_new();

	cantidadDeLineas = numeroDeLineas(lineasDeBloque, posicionX, posicionY,
			&lineaConDePosicion);

	if (cantidadDeLineas == -1) {

		char* lineaHastaIgual = string_new();
		string_append(&lineaHastaIgual, string_itoa(posicionX));
		string_append(&lineaHastaIgual, "-");
		string_append(&lineaHastaIgual, string_itoa(posicionY));
		string_append(&lineaHastaIgual, "=0\n");

		tamanioDelPokemon = tamanioDelPokemon + strlen(lineaHastaIgual);

		string_append(&megaChar, lineaHastaIgual);

		lineasDeBloque = string_split(megaChar, "\n");

		cantidadDeLineas = numeroDeLineas(lineasDeBloque, posicionX, posicionY,
				&lineaConDePosicion);
	}

	char** lineaSeparadaPorIgual = string_split(
			lineasDeBloque[cantidadDeLineas], "=");

	int cantidadNueva = cantidad + atoi(lineaSeparadaPorIgual[1]);

	escribirUnPokemon(cantidadDeLineas, lineasDeBloque, listaDeBloques,
			megaChar, cantidadNueva);
	char* cantidadAEscribir = string_itoa(cantidadNueva);

	uint32_t tamanioNuevoFinal = tamanioDelPokemon + strlen(cantidadAEscribir)
				- strlen(lineaSeparadaPorIgual[1]);

	char* metadataPost = metadataNueva(tamanioDelPokemon, tamanioNuevoFinal,
			lineaSeparadaPorIgual, listaDeBloques);
	cambiarMetadata(pkm, metadataPost);

	enviarAperedPokemon(pkm, posicionX, posicionY, cantidad, idMensajeNew);
}

char* metadataVacia() {
	return "DIRECTORY=N\nSIZE=0\nBLOCKS=[]\nOPEN=N";
}

void cerrarArchivoPokemon(char* pkm) {
	p_pokemonSemaforo* semaforoDePokemon = obtenerPokemonSemaforo(pkm);
	p_metadata* metadataDePokemon = obtenerMetadataEnteraDePokemon(pkm);
	sem_wait(&semaforoDePokemon->semaforoDePokemon);
	memcpy(metadataDePokemon->inicioOpen, "N", strlen("N"));
	sem_post(&semaforoDePokemon->semaforoDePokemon);
	log_info(loggerGeneral, "Se cerro el pokemon: %s", pkm);
}

void atraparPokemon(char* pkm, uint32_t posicionX, uint32_t posicionY,
		uint32_t idMensajeNew) {

	abrirArchivoPokemon(pkm);

	p_metadata* metadataDePokemon = obtenerMetadataEnteraDePokemon(pkm);

	t_list* listaDeBloques = list_create();

	char** tamEnCero = string_split(metadataDePokemon->inicioSize, "\n");
	int tamanioDelPokemon = atoi(tamEnCero[0]);

	char* megaChar = leerUnPokemon(pkm, listaDeBloques);

	char** lineasDeBloque = string_split(megaChar, "\n");

	int cantidadDeLineas = 0;

	char* lineaConDePosicion = string_new();

	cantidadDeLineas = numeroDeLineas(lineasDeBloque, posicionX, posicionY,
			&lineaConDePosicion);

	if (cantidadDeLineas == -1) {
		log_error(loggerGeneral,
				"No se a encontrado la posicion del pokemond: %s", pkm);
		cerrarArchivoPokemon(pkm);
	}

	char** lineaSeparadaPorIgual = string_split(
			lineasDeBloque[cantidadDeLineas], "=");

	int cantidadNueva = atoi(lineaSeparadaPorIgual[1]) - 1;

	escribirUnPokemon(cantidadDeLineas, lineasDeBloque, listaDeBloques,
			megaChar, cantidadNueva);

	uint32_t tamanioNuevoFinal = 0;

	if(cantidadNueva == 0){
		tamanioNuevoFinal = tamanioDelPokemon - strlen(lineasDeBloque[cantidadDeLineas]);
	} else{
		char* cantidadAEscribir = string_itoa(cantidadNueva);

		tamanioNuevoFinal = tamanioDelPokemon + strlen(cantidadAEscribir)
							- strlen(lineaSeparadaPorIgual[1]);
	}

	char* metadataPost = metadataNueva(tamanioDelPokemon, tamanioNuevoFinal,
			lineaSeparadaPorIgual, listaDeBloques);
	cambiarMetadata(pkm, metadataPost);

}

void crearMetadata(char* pkm) {
	char* path = pathDePokemonMetadata(pkm);
	FILE *fileMeta;
	fileMeta = fopen(path, "w");
	fclose(fileMeta);

	int meta = open(path, O_RDWR, 0);

	truncate(path, strlen(metadataVacia()) + 1);

	free(path);

	char* metadataCreada = mmap(NULL, strlen(metadataVacia()),
	PROT_READ | PROT_WRITE,
	MAP_SHARED | MAP_FILE, meta, 0);

	char * metadataAux = metadataVacia();

	memcpy(metadataCreada, metadataAux, strlen(metadataAux) + 1);
}

void crearPokemon(char* pokemon) {

	char* rutaFiles = rutaDeFiles();

	char* path = string_new();

	string_append(&path, rutaFiles);
	string_append(&path, pokemon);

	log_info(loggerGeneral, "Montaje de path pokemon a crear: %s \n", path);

	if (crearDirectorioEn(path) == -1) {
		log_error(loggerGeneral, "Error al crear el path %s \n", path);
		EXIT_FAILURE;
	}

	crearMetadata(pokemon);

	agregarAPokemosEnLista(pokemon);

}

void newPokemon(char* pkm, uint32_t posicionX, uint32_t posicionY,
		uint32_t cantidad, uint32_t idMensajeNew) {
	sem_wait(&existencia);
	bool existe = existePokemon(pkm);

	if (!existe) {
		log_error(loggerGeneral,
				"NO existe el pokemon... pero se esta creando: %s", pkm);
		crearPokemon(pkm);
	} else {
		log_info(loggerGeneral, "Existe el pokemon %s", pkm);
	}
	sem_post(&existencia);
	agregarPokemonesNuevos(pkm, posicionX, posicionY, cantidad, idMensajeNew);
}

void catchPokemon(char* pkm, uint32_t posicionX, uint32_t posicionY,
		uint32_t idMensajeNew) {
	sem_wait(&existencia);
	bool existe = existePokemon(pkm);

	if (!existe) {
		log_error(loggerGeneral, "NO existe el pokemon: %s", pkm);
	} else {
		log_info(loggerGeneral, "Existe el pokemon %s", pkm);
	}
	sem_post(&existencia);
	atraparPokemon(pkm, posicionX, posicionY, idMensajeNew);
}

void atender(HeaderDelibird header, p_elementoDeHilo* elemento) {
	int cliente = elemento->cliente;
	t_log* logger = elemento->log;
	switch (header.tipoMensaje) {
	case d_NEW_POKEMON:
		;
		log_info(logger, "Llego un new pokemon");
		void* packNewPokemon = Serialize_ReceiveAndUnpack(cliente,
				header.tamanioMensaje);
		sem_post(&sock);
		uint32_t idMensajeNew, posicionNewX, posicionNewY, newCantidad;
		char *newNombrePokemon;
		Serialize_Unpack_NewPokemon(packNewPokemon, &idMensajeNew,
				&newNombrePokemon, &posicionNewX, &posicionNewY, &newCantidad);
		log_info(logger,
				"Me llego mensaje de %i. Id: %i, Pkm: %s, x: %i, y: %i, cant: %i\n",
				header.tipoMensaje, idMensajeNew, newNombrePokemon,
				posicionNewX, posicionNewY, newCantidad);

		sem_post(&mutexCliente);
		newPokemon(newNombrePokemon, posicionNewX, posicionNewY, newCantidad,
				idMensajeNew);

		free(packNewPokemon);
		break;
	case d_CATCH_POKEMON:
		;
		log_info(logger, "Llego un catch pokemon");

		void* packCatchPokemon = Serialize_ReceiveAndUnpack(cliente,
				header.tamanioMensaje);
		sem_post(&sock);
		uint32_t idMensajeCatch, posicionCatchX, posicionCatchY;
		char *catchNombrePokemon;
		Serialize_Unpack_CatchPokemon(packCatchPokemon, &idMensajeCatch,
				&catchNombrePokemon, &posicionCatchX, &posicionCatchY);
		log_info(logger,
				"Me llego mensaje de %i. Id: %i, Pkm: %s, x: %i, y: %i\n",
				header.tipoMensaje, idMensajeCatch, catchNombrePokemon,
				posicionCatchX, posicionCatchY);
		sem_post(&mutexCliente);
		// Se hace lo necesario
		free(packCatchPokemon);

		break;
	case d_GET_POKEMON:
		;
		log_info(logger, "Llego un get pokemon");

		void* packGetPokemon = Serialize_ReceiveAndUnpack(cliente,
				header.tamanioMensaje);
		sem_post(&sock);
		uint32_t idMensajeGet;
		char *getNombrePokemon;
		Serialize_Unpack_GetPokemon(packGetPokemon, &idMensajeGet,
				&getNombrePokemon);
		log_info(logger, "Me llego mensaje de %i. Id: %i, Pkm: %s\n",
				header.tipoMensaje, idMensajeGet, getNombrePokemon);
		sem_post(&mutexCliente);
		// Se hace lo necesario
		free(packGetPokemon);
		break;
	default:
		log_error(logger, "Mensaje no entendido: %i\n", header);
		void* packBasura = Serialize_ReceiveAndUnpack(cliente,
				header.tamanioMensaje);
		sem_post(&sock);
		sem_post(&mutexCliente);
		free(packBasura);
		break;
	}
}

void* recibirYAtenderUnCliente(p_elementoDeHilo* elemento) {
	while (1) {
		sem_wait(&sock);
		HeaderDelibird headerRecibido = Serialize_RecieveHeader(
				elemento->cliente);
		if (headerRecibido.tipoMensaje == -1) {
			log_error(elemento->log, "Se desconecto el Cliente\n");
			sem_post(&sock);
			sem_post(&mutexCliente);
			break;
		}
		atender(headerRecibido, elemento);
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

		sem_wait(&mutexCliente);

		log_info(gameBoyLog, "se conecto cliente: %i", cliente);
		pthread_t* dondeSeAtiende = malloc(sizeof(pthread_t));

		p_elementoDeHilo *elemento = malloc(sizeof(p_elementoDeHilo));
		elemento->log = gameBoyLog;
		elemento->cliente = cliente;

		if (pthread_create(dondeSeAtiende, NULL,
				(void*) recibirYAtenderUnCliente, elemento) == 0) {
			log_info(gameBoyLog, ":::: Se creo hilo para cliente ::::");
		} else {
			log_error(gameBoyLog,
					":::: No se pudo crear el hilo para cliente ::::");
			sem_post(&mutexCliente);
		}
		pthread_detach(*dondeSeAtiende);

	}
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

	liberarDoblePuntero(blockSizeEnPosicionUno);

	log_info(loggerGeneral, "BLOCK_SIZE: %i", blockSize);

	char **blocksEnPosicionUno = string_split(separadoPorEnters[1], "=");
	uint32_t blocks = atoi(blocksEnPosicionUno[1]);

	liberarDoblePuntero(blocksEnPosicionUno);

	log_info(loggerGeneral, "BLOCKS: %i", blocks);

	char **magicNumberEnPosicionUno = string_split(separadoPorEnters[2], "=");
	char *magicNumber = malloc(strlen(magicNumberEnPosicionUno[1]) + 1);


	memcpy(magicNumber, magicNumberEnPosicionUno[1],
			strlen(magicNumberEnPosicionUno[1]) + 1);

	liberarDoblePuntero(magicNumberEnPosicionUno);
	liberarDoblePuntero(separadoPorEnters);

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

	int bytes = ceil((float) metadata.bloques / 8);

	truncate(montajeBitmap, bytes);

	int bit = open(montajeBitmap, O_RDWR, 0);

	void *bitmapLevantado = mmap(NULL, bytes,
	PROT_READ | PROT_WRITE,
	MAP_SHARED | MAP_FILE, bit, 0);

	free(montajeBitmap);

	return bitmapLevantado;
}

void crearBitmap() {
	void* bitmapAuxiliar = obtenerBitmap();
	int bytes = ceil((float) metadata.bloques / 8);
	log_info(loggerGeneral, "Tengo un tamanio de bitmap bytes: %i", bytes);
	bitmap = bitarray_create_with_mode(bitmapAuxiliar, bytes, MSB_FIRST);
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

void agregarAPokemosEnLista(char* nombrePokemon) {
	p_pokemonSemaforo* auxPokemon = malloc(
			strlen(nombrePokemon) + 1 + sizeof(sem_t) + sizeof(sem_t));

	auxPokemon->nombreDePokemon = malloc(strlen(nombrePokemon) + 1);

	memcpy(auxPokemon->nombreDePokemon, nombrePokemon, strlen(nombrePokemon));
	memcpy(auxPokemon->nombreDePokemon + strlen(nombrePokemon), "\0", 1);
	sem_init(&(auxPokemon->semaforoDePokemon), 0, 1);
	sem_init(&(auxPokemon->metadataDePokemon), 0, 1);
	sem_wait(&listaPokemon);
	list_add(pokemonsEnFiles, auxPokemon);
	sem_post(&listaPokemon);
}

void listaActual(t_list* lista) {
	DIR *dir;
	struct dirent *directorio;
	char* path = rutaDeFiles();

	dir = opendir(path);
	int cont = 0;
	if (dir) {
		while ((directorio = readdir(dir)) != NULL) {
			if (directorio->d_type == 4 && strcmp(directorio->d_name, ".") != 0
					&& strcmp(directorio->d_name, "..") != 0) {

				char* nombre = malloc(strlen(directorio->d_name)+1);
				memcpy(nombre,directorio->d_name,strlen(directorio->d_name)+1);
				log_info(loggerGeneral, "Existe el pokemon, %s", nombre);
				list_add(lista, nombre);

				cont++;
			}
		}
		closedir(dir);
	}
	free(directorio);

}

void crearListaDePokemons(t_list* pokemons){
	pokemonsEnFiles = list_create();
	for (int i = 0; i < pokemons->elements_count; ++i) {
		agregarAPokemosEnLista(list_get(pokemons,i));
	}
}

void cargarListaAtual() {
	t_list* nombres = list_create();
	listaActual(nombres);
	crearListaDePokemons(nombres);
	list_destroy(nombres);
}

char* pathBlock() {
	char* path = string_new();
	string_append(&path, montajeDeArchivo());
	string_append(&path, "/Blocks/");
	return path;
}

void iniciarBloques() {
	char* rutaBlock = pathBlock();

	DIR *dir;
	struct dirent *directorio;
	dir = opendir(rutaBlock);
	if (dir) {
		while ((directorio = readdir(dir)) != NULL) {
			char* nombre = directorio->d_name;
			if (directorio->d_type == 8 && strcmp(nombre, ".") != 0
					&& strcmp(nombre, "..") != 0) {

				char* ruta = string_new();
				string_append(&ruta,rutaBlock);
				string_append(&ruta,nombre);
				log_info(loggerGeneral, "remuevo: %s -- %i", nombre,remove(ruta));
				free(ruta);
			}
		}
		closedir(dir);
	}

	for (int i = 0; i < metadata.bloques; ++i) {
		char* path = string_new();
		string_append(&path, rutaBlock);
		string_append(&path, string_itoa(i));
		string_append(&path, ".bin");

		log_info(loggerGeneral, "path: %s", path);

		FILE *block;
		block = fopen(path, "w");
		fclose(block);

		truncate(path, metadata.tamanioDeBloque);
		free(path);

	}
	free(rutaBlock);

}

void finalizar() {
	log_destroy(loggerGeneral);
	config_destroy(archivo_de_configuracion);
	sem_destroy(&bitSem);
	sem_destroy(&sock);
	sem_destroy(&mutexCliente);
	sem_destroy(&listaPokemon);
	sem_destroy(&existencia);
	bitarray_destroy(bitmap);
	list_destroy(pokemonsEnFiles);
}

void iniciarSemaforos() {
	sem_init(&bitSem, 0, 1);
	sem_init(&sock, 0, 1);
	sem_init(&mutexCliente, 0, 1);
	sem_init(&listaPokemon, 0, 1);
	sem_init(&existencia, 0, 1);
}

void limpiarFiles() {
	t_list* pkms = list_create();
	listaActual(pkms);

	for (int i = 0; i < pkms->elements_count; ++i) {
		char* rutaFile = rutaDeFiles();

		log_info(loggerGeneral, "get : %s", list_get(pkms, i));
		char* nombre = list_get(pkms, i);
		log_info(loggerGeneral, "nombre : %s", nombre);

		string_append(&rutaFile, nombre);

		char* rutameta = string_duplicate(rutaFile);
		string_append(&rutameta, "/Metadata.bin");


		log_info(loggerGeneral, "Se Elimino la metadata: %s, %i", rutameta, remove(rutameta));

		log_info(loggerGeneral, "Se Elimino el archivo: %s, %i", rutaFile, rmdir(rutaFile));

		free(rutaFile);
		free(rutameta);
	}

	list_destroy(pkms);

	pokemonsEnFiles = list_create();
}

void formatear() {
	iniciarBitmap();
	iniciarBloques();
	limpiarFiles();
}

void levantar() {
	levantarBitmap();
	cargarListaAtual();
}

void inicio() {
	levantarLogYArchivoDeConfiguracion();
	iniciarSemaforos();
	cargarMetadata();
}

void suscribirse() {
	pthread_t* suscriptoNewPokemon = malloc(sizeof(pthread_t));
	conectarmeColaDe(suscriptoNewPokemon, d_NEW_POKEMON);

	pthread_t* suscriptoCatchPokemon = malloc(sizeof(pthread_t));
	conectarmeColaDe(suscriptoCatchPokemon, d_CATCH_POKEMON);

	pthread_t* suscriptoGetPokemon = malloc(sizeof(pthread_t));
	conectarmeColaDe(suscriptoGetPokemon, d_GET_POKEMON);

	pthread_join(*suscriptoNewPokemon, NULL);
	pthread_join(*suscriptoCatchPokemon, NULL);
	pthread_join(*suscriptoGetPokemon, NULL);
}

int main(int argc, char *argv[]) {

	char *format = "";
	format = argv[1];

	inicio();

	if (argc > 1 && strcmp("-f", format) == 0) {
		log_info(loggerGeneral, "Formateando");
		formatear();
	} else if (argc == 1) {
		log_info(loggerGeneral, "Levantando");
		levantar();
	} else {
		log_error(loggerGeneral, "Argumentos invalidos");
		return EXIT_FAILURE;
	}

	pthread_t* servidor = malloc(sizeof(pthread_t));
	iniciarServidorDeGameBoy(servidor);

	suscribirse();

	pthread_join(*servidor, NULL);

	finalizar();

	return EXIT_SUCCESS;
}
