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


#ifndef BITMAP_H_
#define BITMAP_H_

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

/*
* Limpia el bitamp desde una posicion una cantidad
*/
void limpiar_bitmap(int desdePosision, int cantidad, t_bitarray *tBitarray, t_log *logger);

#endif /* BITMAP_H_ */
