#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <math.h>


#ifndef SAC_SERVER_H_
#define SAC_SERVER_H_


//Estructuras para FileSystem

typedef uint32_t ptrGBloque;


typedef struct {
	/*unsigned*/ char bytes [4096];
} Bloque;

typedef struct {
	ptrGBloque bloques_de_datos [1024];
} Bloque_de_puntero;

typedef struct {
	char identificador [3];
	uint32_t version;
	ptrGBloque inicio_bitmap;
	uint32_t tamanio_bitmap;
	unsigned char rellenuto[4081];
}Header;

typedef struct{
	char *bitArray;
}Bitmap;

/*
* Esta funcion se ocupa de 
* colocar la cantidad pasada por 
* parametros de bits correspondientes
* al bitmap en 1
*/
void cargar_bitmap(int cantidad, t_bitarray *tBitarray, t_log *logger);

///////////////////////////////
// FUNCIONES ADMINISTRATIVAS //
//////////////////////////////

/*
* Esta funcion se ocupa de
* retornar la hora actual 
*/
uint64_t timestamp();

/*
* Esta funcion retorna el tamanio de
* el archivo pasado por parametro
*/
uint32_t tamanio_archivo(char *archivo);

/*
* Esta funcion retorna el tamanio de
* el archivo pasado por parametro en bloques
*/
int tamanio_archivo_en_bloques(uint32_t tamanio);

//////////////////////////
// FUNCIONES DEL BITMAP //
/////////////////////////

/*
* Esta funcion retorna el primer
* espacio vacio que encuentre en el bitmap
*/
int buscar_espacio_en_bitmap(t_bitarray *tBitarray, t_log *logger);

/*
* Esta funcion se encarga de ocupar el bit
* indicado por el indice dado en el bitmap
*/
void ocupar_bloque_en_bitmap(int indice, t_bitarray *tBitarray);

/*
* Esta funcion se encarga de liberar el bit
* indicado por el indice dado en el bitmap
*/
void liberar_bloque_en_bitmap(int indice, t_bitarray *tBitarray);

#endif /* SAC_SERVER_H_ */
