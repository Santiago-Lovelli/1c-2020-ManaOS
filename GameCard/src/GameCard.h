#ifndef GAMECARD_H_
#define GAMECARD_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <Logger/Logger.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <Conexiones/Conexiones.h>
#include <Serializacion/Serializacion.h>
#include <sys/mman.h>
#include <Bitmap/Bitmap.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <Lista/lista.h>
#include <math.h>
#include <ManejoDePunterosDobles/ManejoDePunterosDobles.h>

t_log * loggerGeneral;
t_config *archivo_de_configuracion;
t_list* pokemonsEnFiles;

typedef struct {
	uint32_t tamanioDeBloque;
	uint32_t bloques;
	char numeroMagico[11];
} m_metadata;

typedef struct {
	char* inicioDirectory;
	char* inicioSize;
	char* inicioBloques;
	char* inicioOpen;
} p_metadata;

typedef struct {
	char* nombreDePokemon;
	pthread_mutex_t semaforoDePokemon;
} p_pokemonSemaforo;

typedef struct {
	int cliente;
	t_log* log;
} p_elementoDeHilo;

m_metadata metadata;
t_bitarray * bitmap;
pthread_mutex_t bitSem;

int crearDirectorioEn(char *path);
void* atenderGameboy();
void iniciarServidorDeGameBoy();
void levantarLogYArchivoDeConfiguracion();
void atender(HeaderDelibird header, int cliente, t_log* logger);
void* recibirYAtenderUnCliente(p_elementoDeHilo* elemento);
void conectarmeColaDe(pthread_t* hilo, d_message colaDeSuscripcion);
void cargarMetadata();
void * obtenerBitmap();
void * obtenerMetadata();

char* montajeDeArchivo();
char* pathDePokemonMetadata(char * pokemon);
bool existePokemon(char* pokemon);
char *archivoMetadataPokemon(char *path, uint32_t cantidadALevantar);
char* obtenerBloquesDeMetadataPokemon(char* pkm);
char** obtenerBloquesDeMetadataPokemonEnArray(char* unPokemon);
char* obtenerDirectory(char* pkm);
uint32_t obtenerSizeDePokemon(char* pkm);
char* obtenerPathDeBloque(char* bloque);

int indiceDePokemonEnLista(char* pkm);
/*
 * Retorna un void* mmapeado al bloque de datos con un tamanio pasado, si el tamanio es nulo
 * reserva el tamanio que le pasan, si es 0 reserva el tamanio total del bloque
 * */
void* mmapeadoBloquePropio(t_log* log, uint32_t tamanioDeseado, char* numeroDeBloque);



#endif /* GAMECARD_H_ */
