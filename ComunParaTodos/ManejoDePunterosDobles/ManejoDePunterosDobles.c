#include "ManejoDePunterosDobles.h"

uint32_t damePosicionFinalDoblePuntero(char **puntero){
	uint32_t contador = 0;
	while(puntero[contador] != NULL)
		contador = contador+1;
	return contador-1;
}

void liberarDoblePuntero(char **puntero){
	uint32_t posicionFinal = damePosicionFinalDoblePuntero(puntero);
	for(int i=0; i<=posicionFinal; i=i+1)
	{
		free(puntero[i]);
	}
	free(puntero);
}

