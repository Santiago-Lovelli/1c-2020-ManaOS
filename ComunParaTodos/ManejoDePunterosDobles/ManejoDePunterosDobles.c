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

bool esElString(p_punteroEnLista* unString, char* otroString){
	int rest = strcmp(unString->data, otroString);
	return rest== 0;
}

int sonIgualesSinInportarOrden(char** unPuntero, char** otroPuntero){
	t_list* listaDeUno = list_create();
	t_list* listaDeOtro = list_create();
	int i = 0;
	int j = 0;
	int limiteDeUno = damePosicionFinalDoblePuntero(unPuntero);
	int limiteDeDos = damePosicionFinalDoblePuntero(otroPuntero);

	if(limiteDeUno != limiteDeDos)
		return -1;

	while(i <= limiteDeUno){
		p_punteroEnLista* aux = malloc(strlen(unPuntero[i])+1);
		aux->data = unPuntero[i];
		list_add(listaDeUno,aux);
		i = i + 1;
	}
	while(j <= limiteDeDos){
		p_punteroEnLista* aux = malloc(strlen(otroPuntero[j])+1);
		aux->data = otroPuntero[j];
		list_add(listaDeOtro,aux);
		j = j + 1;
	}

	int contador = 0;
	while(contador < list_size(listaDeUno) && contador < list_size(listaDeOtro)){

		p_punteroEnLista* otroAux = list_get(listaDeOtro,contador);
		int unIndex = list_get_index(listaDeUno,otroAux->data,(void*)esElString);

		p_punteroEnLista* unAux = list_get(listaDeUno,contador);
		int otroIndex = list_get_index(listaDeOtro,unAux->data,(void*)esElString);
		if(unIndex == -1 || otroIndex == -1){
			list_destroy(listaDeUno);
			list_destroy(listaDeOtro);
			return -1;
		}
		contador = contador+1;
	}
	list_destroy(listaDeUno);
	list_destroy(listaDeOtro);
	return 1;
}
