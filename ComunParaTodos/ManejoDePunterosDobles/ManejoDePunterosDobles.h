
#include <stdlib.h>
#include <stdint.h>

#ifndef MANEJOSDEPUNTEROS_H_
#define MANEJOSDEPUNTEROS_H_

/*
 * Devuelve el ultimo indice del puntero doble
 */
uint32_t damePosicionFinalDoblePuntero(char **puntero);

/*
 * Libera un puntero doble
 */
void liberarDoblePuntero(char **puntero);

#endif /* MANEJOSDEPUNTEROS_H_*/
