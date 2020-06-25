
#include <stdlib.h>
#include <stdint.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <stdbool.h>
#include "../Lista/lista.h"
#include <string.h>

#ifndef MANEJOSDEPUNTEROS_H_
#define MANEJOSDEPUNTEROS_H_

typedef struct {
	char* data;
} p_punteroEnLista;

/*
 * Devuelve el ultimo indice del puntero doble
 */
uint32_t damePosicionFinalDoblePuntero(char **puntero);

/*
 * Libera un puntero doble
 */
void liberarDoblePuntero(char **puntero);

int sonIgualesSinInportarOrden(char** unPuntero, char** otroPuntero);

#endif /* MANEJOSDEPUNTEROS_H_*/
