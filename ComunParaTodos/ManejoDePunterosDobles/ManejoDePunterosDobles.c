#include "ManejoDePunterosDobles.h"

uint32_t damePosicionFinalDoblePuntero(char **puntero){
	if(puntero == NULL)
			return 0;
	if(puntero[0] == NULL)
		return 0;
	uint32_t contador = 0;
	while(puntero[contador] != NULL)
		contador = contador+1;
	return contador-1;
}

int damePosicionDeObjetoEnDoblePuntero(char **puntero, char* objeto){
	int limite = damePosicionFinalDoblePuntero(puntero);
	for(int i=0; i<=limite; i++){
		if(strcmp(objeto, puntero[i]) == 0 )
			return i;
	}
	return -1;
}

char* primerElementoEnComun(char** puntero1, char** puntero2){
	int limite1 = damePosicionFinalDoblePuntero(puntero1);
	int limite2 = damePosicionFinalDoblePuntero(puntero2);
	for(int i=0; i<=limite1; i++){
		for(int j=0; j<=limite2; j++){
			if(strcmp(puntero1[i],puntero2[j]) == 0 )
				return puntero1[i];
		}
	}
	return NULL;
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
	t_list* listaDeUno = convertirDoblePunteroEnLista(unPuntero);
	t_list* listaDeOtro = convertirDoblePunteroEnLista(otroPuntero);
	int limiteDeUno = damePosicionFinalDoblePuntero(unPuntero);
	int limiteDeDos = damePosicionFinalDoblePuntero(otroPuntero);

	if(limiteDeUno != limiteDeDos)
		return -1;

	int contador = 0;
	while(list_size(listaDeUno) != 0 && list_size(listaDeOtro) != 0){

		p_punteroEnLista* otroAux = list_get(listaDeOtro,contador);
		int unIndex = list_get_index(listaDeUno,otroAux->data,(void*)esElString);

		p_punteroEnLista* unAux = list_get(listaDeUno,contador);
		int otroIndex = list_get_index(listaDeOtro,unAux->data,(void*)esElString);
		if(unIndex == -1 || otroIndex == -1){
			list_destroy(listaDeUno);
			list_destroy(listaDeOtro);
			return -1;
		}
		list_remove(listaDeUno, contador);
		list_remove(listaDeOtro, unIndex);
	}
	list_destroy(listaDeUno);
	list_destroy(listaDeOtro);
	return 1;
}

t_list* convertirDoblePunteroEnLista(char** doble){
	int limiteDeLista = damePosicionFinalDoblePuntero(doble);
	t_list* listaDeUno = list_create();
	int i=0;
	if(doble[0] != NULL){
		while(i <= limiteDeLista){
			p_punteroEnLista* aux = malloc(strlen(doble[i])+1);
			aux->data = doble[i];
			list_add(listaDeUno,aux);
			i = i + 1;
		}
	}
	return listaDeUno;

}
