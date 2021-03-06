
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
 * Esta funcion dado un objeto y un doble puntero devuelve el primer indice
 * en el que se encuentre ese objeto, si no lo encuentra retorna -1
 */

int damePosicionDeObjetoEnDoblePuntero(char **puntero, char* objeto);

/*
 * Esta funcion dados dos dobles punteros retorna el primer
 * elemento que tengan en comun
 */

char* primerElementoEnComun(char** puntero1, char** puntero2);

/*
 * Libera un puntero doble
 */
void liberarDoblePuntero(char **puntero);

/*
 * Esta funcion dados dos doble punteros compara que los contenidos
 * de ambos sean iguales sin importar el orden
 */

int sonIgualesSinInportarOrden(char** unPuntero, char** otroPuntero);

/*
 * Esta funcion dado un doble puntero
 * lo retorna convertido en lista
 * NO LIBERA EL DOBLE PUNTERO
 */

t_list* convertirDoblePunteroEnLista(char** doble);
#endif /* MANEJOSDEPUNTEROS_H_*/
