#include "Serializacion.h"

///////////////////////////
// FUNCIONES PARA ENVIAR //
///////////////////////////

bool Serialize_PackAndSend(int socketCliente, const void *pack, uint32_t tamPack, d_message tipoMensaje) {

	uint32_t tamMessage = tamPack + sizeof(d_message) + sizeof(uint32_t);
	printf("Tam mensaje: %d \n", tamMessage);
	fflush(stdout);
	void* buffer = malloc( tamMessage );
	int desplazamiento = 0;
	memcpy(buffer, &tipoMensaje ,sizeof(d_message));
	desplazamiento += sizeof(d_message);
	memcpy(buffer+desplazamiento, &tamPack , sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer+desplazamiento, pack, tamPack);
	desplazamiento += tamPack;
		if(desplazamiento != tamMessage){ return (-1); }
	int resultado = send(socketCliente, buffer, tamMessage, 0);
	free(buffer);
	return resultado;
}

bool Serialize_PackAndSend_ACK(int socketCliente, uint32_t miId){
	void* pack = malloc(sizeof(uint32_t));
	memcpy(pack, &miId, sizeof(uint32_t));
	int resultado = Serialize_PackAndSend(socketCliente, pack, sizeof(uint32_t), d_ACK);
	free(pack);
	return resultado;
}

bool Serialize_PackAndSend_SubscribeQueue(int socketCliente, d_message queue){
	void* pack = malloc(sizeof(d_message));
	memcpy(pack, &queue, sizeof(d_message));
	int resultado = Serialize_PackAndSend(socketCliente, pack, sizeof(d_message), d_SUBSCRIBE_QUEUE);
	free(pack);
	return resultado;
}

bool Serialize_PackAndSend_NEW_POKEMON(int socketCliente, uint32_t idMensaje,const void *pokemon, uint32_t posX, uint32_t posY, uint32_t cantidad) {
	uint32_t tamMessage = strlen(pokemon) + 1 + (5*sizeof(uint32_t)); //+1 por el /0
	uint32_t tamNombrePokemon = strlen(pokemon) + 1;
	void* buffer = malloc( tamMessage );
	int desplazamiento = 0;
	memcpy(buffer, &idMensaje, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento , &tamNombrePokemon, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, pokemon, tamNombrePokemon);
	desplazamiento += tamNombrePokemon;
	memcpy(buffer + desplazamiento, &posX, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &posY, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &cantidad, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	int resultado = Serialize_PackAndSend(socketCliente, buffer, tamMessage, d_NEW_POKEMON);
	free(buffer);
	return resultado;

}

bool Serialize_PackAndSend_NEW_POKEMON_NoID(int socketCliente,const void *pokemon, uint32_t posX, uint32_t posY, uint32_t cantidad) {
	uint32_t tamMessage = strlen(pokemon) + 1 + (4*sizeof(uint32_t)); //+1 por el /0
	uint32_t tamNombrePokemon = strlen(pokemon) + 1;
	void* buffer = malloc( tamMessage );
	int desplazamiento = 0;
	memcpy(buffer + desplazamiento , &tamNombrePokemon, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, pokemon, tamNombrePokemon);
	desplazamiento += tamNombrePokemon;
	memcpy(buffer + desplazamiento, &posX, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &posY, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &cantidad, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	int resultado = Serialize_PackAndSend(socketCliente, buffer, tamMessage, d_NEW_POKEMON);
	free(buffer);
	return resultado;

}

bool Serialize_PackAndSend_CATCHoAPPEARED(int socketCliente, uint32_t idMensaje, const void *pokemon, uint32_t posX, uint32_t posY, d_message tipoMensaje){
	uint32_t tamMessage = strlen(pokemon) + 1 + (4*sizeof(uint32_t)); //+1 por el /0
	uint32_t tamNombrePokemon = strlen(pokemon) + 1;
	void* buffer = malloc( tamMessage );
	int desplazamiento = 0;
	memcpy(buffer, &idMensaje, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &tamNombrePokemon, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, pokemon, tamNombrePokemon);
	desplazamiento += tamNombrePokemon;
	memcpy(buffer + desplazamiento, &posX, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &posY, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	int resultado = Serialize_PackAndSend(socketCliente, buffer, tamMessage, tipoMensaje);
	free(buffer);
	return resultado;
}

bool Serialize_PackAndSend_APPEARED_POKEMON_IDCorrelativo(int socketCliente, uint32_t idMensaje,uint32_t idCorrelativo, const void *pokemon, uint32_t posX, uint32_t posY){
	uint32_t tamMessage = strlen(pokemon) + 1 + (4*sizeof(uint32_t)); //+1 por el /0
	uint32_t tamNombrePokemon = strlen(pokemon) + 1;
	void* buffer = malloc( tamMessage );
	int desplazamiento = 0;
	memcpy(buffer, &idMensaje, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &tamNombrePokemon, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, pokemon, tamNombrePokemon);
	desplazamiento += tamNombrePokemon;
	memcpy(buffer + desplazamiento, &posX, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &posY, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer, &idCorrelativo, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	int resultado = Serialize_PackAndSend(socketCliente, buffer, tamMessage, d_APPEARED_POKEMON);
	free(buffer);
	return resultado;
}

bool Serialize_PackAndSend_CATCHoAPPEARED_NoID(int socketCliente, const void *pokemon, uint32_t posX, uint32_t posY, d_message tipoMensaje){
	uint32_t tamMessage = strlen(pokemon) + 1 + (3*sizeof(uint32_t)); //+1 por el /0
	uint32_t tamNombrePokemon = strlen(pokemon) + 1;
	void* buffer = malloc( tamMessage );
	int desplazamiento = 0;
	memcpy(buffer + desplazamiento, &tamNombrePokemon, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, pokemon, tamNombrePokemon);
	desplazamiento += tamNombrePokemon;
	memcpy(buffer + desplazamiento, &posX, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &posY, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	int resultado = Serialize_PackAndSend(socketCliente, buffer, tamMessage, tipoMensaje);
	free(buffer);
	return resultado;
}

bool Serialize_PackAndSend_CATCH_POKEMON(int socketCliente, uint32_t idMensaje,const void *pokemon, uint32_t posX, uint32_t posY) {
	return Serialize_PackAndSend_CATCHoAPPEARED(socketCliente,idMensaje,pokemon,posX,posY,d_CATCH_POKEMON);

}

bool Serialize_PackAndSend_CATCH_POKEMON_NoID(int socketCliente,const void *pokemon, uint32_t posX, uint32_t posY) {
	return Serialize_PackAndSend_CATCHoAPPEARED_NoID(socketCliente,pokemon,posX,posY,d_CATCH_POKEMON);
}

bool Serialize_PackAndSend_GET_POKEMON(int socketCliente, uint32_t idMensaje,const void *pokemon) {
	uint32_t tamMessage = strlen(pokemon) + 1 + (2*sizeof(uint32_t)); //+1 por el /0
	uint32_t tamNombrePokemon = strlen(pokemon) + 1;
	void* buffer = malloc( tamMessage );
	int desplazamiento = 0;
	memcpy(buffer, &idMensaje, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &tamNombrePokemon, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, pokemon, tamNombrePokemon);
	desplazamiento += tamNombrePokemon;
	int resultado = Serialize_PackAndSend(socketCliente, buffer, tamMessage, d_GET_POKEMON);
	free(buffer);
	return resultado;

}

bool Serialize_PackAndSend_GET_POKEMON_NoID(int socketCliente,const void *pokemon) {
	uint32_t tamMessage = strlen(pokemon) + 1 + (1*sizeof(uint32_t)); //+1 por el /0
	uint32_t tamNombrePokemon = strlen(pokemon) + 1;
	void* buffer = malloc( tamMessage );
	int desplazamiento = 0;
	memcpy(buffer + desplazamiento, &tamNombrePokemon, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, pokemon, tamNombrePokemon);
	desplazamiento += tamNombrePokemon;
	int resultado = Serialize_PackAndSend(socketCliente, buffer, tamMessage, d_GET_POKEMON);
	free(buffer);
	return resultado;

}

bool Serialize_PackAndSend_APPEARED_POKEMON(int socketCliente, uint32_t idMensaje,const void *pokemon, uint32_t posX, uint32_t posY) {
	return Serialize_PackAndSend_CATCHoAPPEARED(socketCliente,idMensaje,pokemon,posX,posY,d_APPEARED_POKEMON);
}

bool Serialize_PackAndSend_APPEARED_POKEMON_NoID(int socketCliente,const void *pokemon, uint32_t posX, uint32_t posY) {
	return Serialize_PackAndSend_CATCHoAPPEARED_NoID(socketCliente,pokemon,posX,posY,d_APPEARED_POKEMON);
}

bool Serialize_PackAndSend_CAUGHT_POKEMON(int socketCliente, uint32_t idMensaje, uint32_t resultado){
	void* pack = malloc(2*sizeof(uint32_t));
	memcpy(pack, &idMensaje, sizeof(uint32_t));
	memcpy(pack+sizeof(uint32_t), &resultado, sizeof(uint32_t));
	int resultadoenvio = Serialize_PackAndSend(socketCliente, pack, 2*sizeof(uint32_t), d_CAUGHT_POKEMON);
	free(pack);
	return resultadoenvio;
}

bool Serialize_PackAndSend_CAUGHT_POKEMON_IDCorrelativo(int socketCliente, uint32_t idMensaje, uint32_t idCorrelativo, uint32_t resultado){
	void* pack = malloc(3*sizeof(uint32_t));
	memcpy(pack, &idMensaje, sizeof(uint32_t));
	memcpy(pack+sizeof(uint32_t), &resultado, sizeof(uint32_t));
	memcpy(pack+sizeof(uint32_t)+sizeof(uint32_t), &idCorrelativo, sizeof(uint32_t));
	int resultadoenvio = Serialize_PackAndSend(socketCliente, pack, 3*sizeof(uint32_t), d_CAUGHT_POKEMON);
	free(pack);
	return resultadoenvio;
}

bool Serialize_PackAndSend_LOCALIZED_POKEMON(int socketCliente, uint32_t idMensaje, char *pokemon, d_PosCant** posiciones) {
	uint32_t cantidadPunteros = damePosicionFinalDoblePuntero(posiciones);
	uint32_t tamNombrePoke = strlen(pokemon)+1;
	uint32_t tamPack = (3*sizeof(uint32_t)) + tamNombrePoke + ((cantidadPunteros+1)*(2*sizeof(uint32_t)));
	uint32_t desplazamiento = 0;
	void* pack = malloc(tamPack);
	memcpy(pack+desplazamiento, &idMensaje, sizeof(uint32_t)); //Memcpy el IDMENSAJE
	desplazamiento += sizeof(uint32_t); //Me muevo la longitud del IDMENSAJE
	memcpy(pack+desplazamiento, &tamNombrePoke, sizeof(uint32_t)); //Memcpy el TAMAÑO del NOMBRE del POKEMON
	desplazamiento += sizeof(uint32_t); //Me muevo la longitud del TAMAÑO del NOMBRE del POKEMON
	memcpy(pack+desplazamiento, pokemon, tamNombrePoke); //memcpy el nombre del pokemon
	desplazamiento += tamNombrePoke; //Me muevo lo escrito por el nombre del pokemon
	memcpy(pack+desplazamiento, &cantidadPunteros, sizeof(uint32_t)); //Memcpy la CANTIDAD de PUNTEROS
	desplazamiento += sizeof(uint32_t);
	for(int i=0; i<=cantidadPunteros; i++){
		memcpy(pack+desplazamiento, &(posiciones[i]->posX), sizeof(uint32_t)); //memcpy posX
		desplazamiento += sizeof(uint32_t); //Me muevo posX
		memcpy(pack+desplazamiento, &(posiciones[i]->posY), sizeof(uint32_t)); //memcpy posY
		desplazamiento += sizeof(uint32_t); //Me muevo posY
	}
	if( desplazamiento != tamPack){
		printf(" HUBO PROBLEMAS ");
		fflush(stdout);
		sleep(10);
	}
	int resultadoEnvio = Serialize_PackAndSend(socketCliente, pack, tamPack, d_LOCALIZED_POKEMON);
	free(pack);
	return resultadoEnvio;
}

bool Serialize_PackAndSend_LOCALIZED_POKEMON_IDCorrelativo(int socketCliente, uint32_t idMensaje, uint32_t idCorrelativo, char *pokemon, d_PosCant** posiciones) {
	uint32_t cantidadPunteros = damePosicionFinalDoblePuntero(posiciones);
	uint32_t tamNombrePoke = strlen(pokemon)+1;
	uint32_t tamPack = (4*sizeof(uint32_t)) + tamNombrePoke + ((cantidadPunteros+1)*(2*sizeof(uint32_t)));
	uint32_t desplazamiento = 0;
	void* pack = malloc(tamPack);
	memcpy(pack+desplazamiento, &idMensaje, sizeof(uint32_t)); //Memcpy el IDMENSAJE
	desplazamiento += sizeof(uint32_t); //Me muevo la longitud del IDMENSAJE
	memcpy(pack+desplazamiento, &tamNombrePoke, sizeof(uint32_t)); //Memcpy el TAMAÑO del NOMBRE del POKEMON
	desplazamiento += sizeof(uint32_t); //Me muevo la longitud del TAMAÑO del NOMBRE del POKEMON
	memcpy(pack+desplazamiento, pokemon, tamNombrePoke); //memcpy el nombre del pokemon
	desplazamiento += tamNombrePoke; //Me muevo lo escrito por el nombre del pokemon
	memcpy(pack+desplazamiento, &cantidadPunteros, sizeof(uint32_t)); //Memcpy la CANTIDAD de PUNTEROS
	desplazamiento += sizeof(uint32_t);
	for(int i=0; i<=cantidadPunteros; i++){
		memcpy(pack+desplazamiento, &(posiciones[i]->posX), sizeof(uint32_t)); //memcpy posX
		desplazamiento += sizeof(uint32_t); //Me muevo posX
		memcpy(pack+desplazamiento, &(posiciones[i]->posY), sizeof(uint32_t)); //memcpy posY
		desplazamiento += sizeof(uint32_t); //Me muevo posY
	}
	memcpy(pack+desplazamiento, &idCorrelativo, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	if( desplazamiento != tamPack){
		printf(" HUBO PROBLEMAS ");
		fflush(stdout);
		sleep(10);
	}
	int resultadoEnvio = Serialize_PackAndSend(socketCliente, pack, tamPack, d_LOCALIZED_POKEMON);
	free(pack);
	return resultadoEnvio;
}

////////////////////////////
// FUNCIONES PARA RECIBIR //
////////////////////////////

HeaderDelibird Serialize_RecieveHeader(int socketCliente){
	void* buffer=malloc(sizeof(d_message) + sizeof(uint32_t));
	if(recv(socketCliente, buffer, (sizeof(d_message) + sizeof(uint32_t)), MSG_WAITALL) == 0){
		HeaderDelibird headerQueRetorna;
		headerQueRetorna.tipoMensaje = (-1);
		headerQueRetorna.tamanioMensaje = 0;
		free(buffer);
		return headerQueRetorna;
	}
	uint32_t tamanioMensaje = 0;
	d_message operacion;
	memcpy(&operacion,buffer,sizeof(d_message));
	memcpy(&tamanioMensaje, buffer+(sizeof(d_message)), (sizeof(uint32_t)));
	free(buffer);
	HeaderDelibird headerQueRetorna;
	headerQueRetorna.tipoMensaje = operacion;
	headerQueRetorna.tamanioMensaje = tamanioMensaje;
	return headerQueRetorna;
}

void* Serialize_ReceiveAndUnpack(int socketCliente, uint32_t tamanio) {
	void* retorno = malloc(tamanio);
	recv(socketCliente, retorno, tamanio, MSG_WAITALL);
	return retorno;
}

//////////////////////////////////
// FUNCIONES PARA DESEMPAQUETAR //
/////////////////////////////////

uint32_t Serialize_Unpack_ACK(void *pack) {
	uint32_t response = 0;
	memcpy(&response, pack, sizeof(uint32_t));
	return response;
}

uint32_t Serialize_Unpack_idMensaje(void *pack) {
	uint32_t response = 0;
	memcpy(&response, pack, sizeof(uint32_t));
	return response;
}

char* Serialize_Unpack_pokemonName(void *buffer) {
	uint32_t tamPokemon = 0;
	memcpy(&tamPokemon, buffer+sizeof(uint32_t), sizeof(uint32_t));
	char *pokemon = malloc(tamPokemon);
	memcpy(pokemon,buffer+(2*sizeof(uint32_t)),tamPokemon);
	return pokemon;
}

uint32_t Serialize_Unpack_posX(void *pack) {
	uint32_t response = 0;
	uint32_t tamPokemon = 0;
	memcpy(&tamPokemon, pack+sizeof(uint32_t), sizeof(uint32_t));
	memcpy(&response, pack + (2*sizeof(uint32_t)) + tamPokemon , sizeof(uint32_t));
	return response;
}

uint32_t Serialize_Unpack_posY(void *pack) {
	uint32_t response = 0;
	uint32_t tamPokemon = 0;
	memcpy(&tamPokemon, pack+sizeof(uint32_t), sizeof(uint32_t));
	memcpy(&response, pack + (3*sizeof(uint32_t)) + tamPokemon , sizeof(uint32_t));
	return response;
}

uint32_t Serialize_Unpack_cantidad(void *pack) {
	uint32_t response = 0;
	uint32_t tamPokemon = 0;
	memcpy(&tamPokemon, pack+sizeof(uint32_t), sizeof(uint32_t));
	memcpy(&response, pack + (4*sizeof(uint32_t)) + tamPokemon , sizeof(uint32_t));
	return response;
}

char* Serialize_Unpack_pokemonName_NoID(void *buffer) {
	uint32_t tamPokemon = 0;
	memcpy(&tamPokemon, buffer, sizeof(uint32_t));
	char *pokemon = malloc(tamPokemon);
	memcpy(pokemon,buffer+sizeof(uint32_t),tamPokemon);
	return pokemon;
}

uint32_t Serialize_Unpack_posX_NoID(void *pack) {
	uint32_t response = 0;
	uint32_t tamPokemon = 0;
	memcpy(&tamPokemon, pack, sizeof(uint32_t));
	memcpy(&response, pack + sizeof(uint32_t) + tamPokemon , sizeof(uint32_t));
	return response;
}

uint32_t Serialize_Unpack_posY_NoID(void *pack) {
	uint32_t response = 0;
	uint32_t tamPokemon = 0;
	memcpy(&tamPokemon, pack, sizeof(uint32_t));
	memcpy(&response, pack + (2*sizeof(uint32_t)) + tamPokemon , sizeof(uint32_t));
	return response;
}

uint32_t Serialize_Unpack_cantidad_NoID(void *pack) {
	uint32_t response = 0;
	uint32_t tamPokemon = 0;
	memcpy(&tamPokemon, pack, sizeof(uint32_t));
	memcpy(&response, pack + (3*sizeof(uint32_t)) + tamPokemon , sizeof(uint32_t));
	return response;
}

uint32_t Serialize_Unpack_resultado(void *pack){
	uint32_t resultado = 0;
	memcpy(&resultado, pack+sizeof(uint32_t), sizeof(uint32_t));
	return resultado;
}

//////////////////////////////////////
// FUNCIONES PARA DESEMPAQUETAR PRO //
/////////////////////////////////////

void Serialize_Unpack_NewPokemon(void *packNewPokemon, uint32_t *idMensaje, char **nombre, uint32_t *posX, uint32_t *posY, uint32_t *cantidad){
	*idMensaje = Serialize_Unpack_idMensaje(packNewPokemon);
	*nombre = Serialize_Unpack_pokemonName(packNewPokemon);
	*posX = Serialize_Unpack_posX(packNewPokemon);
	*posY = Serialize_Unpack_posY(packNewPokemon);
	*cantidad = Serialize_Unpack_cantidad(packNewPokemon);
}

void Serialize_Unpack_CatchPokemon(void *packCatchPokemon, uint32_t *idMensaje, char **nombre, uint32_t *posX, uint32_t *posY){
	*idMensaje = Serialize_Unpack_idMensaje(packCatchPokemon);
	*nombre = Serialize_Unpack_pokemonName(packCatchPokemon);
	*posX = Serialize_Unpack_posX(packCatchPokemon);
	*posY = Serialize_Unpack_posY(packCatchPokemon);
}

void Serialize_Unpack_GetPokemon(void *packGetPokemon, uint32_t *idMensaje, char **nombre){
	*idMensaje = Serialize_Unpack_idMensaje(packGetPokemon);
	*nombre = Serialize_Unpack_pokemonName(packGetPokemon);
}

void Serialize_Unpack_AppearedPokemon(void *packAppearedPokemon, uint32_t *idMensaje, char **nombre, uint32_t *posX, uint32_t *posY){
	*idMensaje = Serialize_Unpack_idMensaje(packAppearedPokemon);
	*nombre = Serialize_Unpack_pokemonName(packAppearedPokemon);
	*posX = Serialize_Unpack_posX(packAppearedPokemon);
	*posY = Serialize_Unpack_posY(packAppearedPokemon);
}

void Serialize_Unpack_CaughtPokemon(void *packCaughtPokemon, uint32_t *idMensaje, uint32_t *resultado){
	*idMensaje = Serialize_Unpack_idMensaje(packCaughtPokemon);
	*resultado = Serialize_Unpack_resultado(packCaughtPokemon);
}

void Serialize_Unpack_LocalizedPokemon(void *packLocalizedPokemon, uint32_t *idMensaje, char **nombre, t_list **poscant){
	*idMensaje = Serialize_Unpack_idMensaje(packLocalizedPokemon);
	*nombre = Serialize_Unpack_pokemonName(packLocalizedPokemon);
	uint32_t desplazamiento = (2*sizeof(uint32_t)) + strlen(*nombre) + 1;
	uint32_t cantidadPunteros = 0;
	memcpy(&cantidadPunteros, packLocalizedPokemon+desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	d_PosCant* posicionActual;
	for(int i=0; i<=cantidadPunteros; i++){
		posicionActual = malloc(sizeof(typeof(d_PosCant)));
		memcpy(&(posicionActual->posX),packLocalizedPokemon+desplazamiento,sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(&(posicionActual->posY),packLocalizedPokemon+desplazamiento,sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		list_add(*poscant, posicionActual);
	}
}

void Serialize_Unpack_NewPokemon_NoID(void *packNewPokemon, char **nombre, uint32_t *posX, uint32_t *posY, uint32_t *cantidad){
	*nombre = Serialize_Unpack_pokemonName_NoID(packNewPokemon);
	*posX = Serialize_Unpack_posX_NoID(packNewPokemon);
	*posY = Serialize_Unpack_posY_NoID(packNewPokemon);
	*cantidad = Serialize_Unpack_cantidad_NoID(packNewPokemon);
}


void Serialize_Unpack_AppearedPokemon_NoID(void *packAppearedPokemon, char **nombre, uint32_t *posX, uint32_t *posY){
	*nombre = Serialize_Unpack_pokemonName_NoID(packAppearedPokemon);
	*posX = Serialize_Unpack_posX_NoID(packAppearedPokemon);
	*posY = Serialize_Unpack_posY_NoID(packAppearedPokemon);
}

void Serialize_Unpack_CatchPokemon_NoID(void *packCatchPokemon, char **nombre, uint32_t *posX, uint32_t *posY){
	*nombre = Serialize_Unpack_pokemonName_NoID(packCatchPokemon);
	*posX = Serialize_Unpack_posX_NoID(packCatchPokemon);
	*posY = Serialize_Unpack_posY_NoID(packCatchPokemon);
}

void Serialize_Unpack_GetPokemon_NoID(void *packGetPokemon, char **nombre){
	*nombre = Serialize_Unpack_pokemonName_NoID(packGetPokemon);
}

void Serialize_Unpack_AppearedPokemon_IDCorrelativo(void *packAppearedPokemon, uint32_t *idMensaje,uint32_t *idCorrelativo, char **nombre, uint32_t *posX, uint32_t *posY){
	*idMensaje = Serialize_Unpack_idMensaje(packAppearedPokemon);
	*nombre = Serialize_Unpack_pokemonName(packAppearedPokemon);
	*posX = Serialize_Unpack_posX(packAppearedPokemon);
	*posY = Serialize_Unpack_posY(packAppearedPokemon);
	int desplazamiento = strlen(*nombre) + 1 + (4*sizeof(uint32_t));
	memcpy(idCorrelativo, packAppearedPokemon+desplazamiento, sizeof(uint32_t));
}

void Serialize_Unpack_LocalizedPokemon_IDCorrelativo(void *packLocalizedPokemon, uint32_t *idMensaje, uint32_t *idCorrelativo, char **nombre, t_list **poscant){
	*idMensaje = Serialize_Unpack_idMensaje(packLocalizedPokemon);
	*nombre = Serialize_Unpack_pokemonName(packLocalizedPokemon);
	uint32_t desplazamiento = (2*sizeof(uint32_t)) + strlen(*nombre) + 1;
	uint32_t cantidadPunteros = 0;
	memcpy(&cantidadPunteros, packLocalizedPokemon+desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	d_PosCant* posicionActual;
	for(int i=0; i<=cantidadPunteros; i++){
		posicionActual = malloc(sizeof(typeof(d_PosCant)));
		memcpy(&(posicionActual->posX),packLocalizedPokemon+desplazamiento,sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(&(posicionActual->posY),packLocalizedPokemon+desplazamiento,sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		list_add(*poscant, posicionActual);
	}
	memcpy(idCorrelativo, packLocalizedPokemon+desplazamiento, sizeof(uint32_t));
}

void Serialize_Unpack_CaughtPokemon_IDCorrelativo(void *packCaughtPokemon, uint32_t *idMensaje, uint32_t *idCorrelativo, uint32_t *resultado){
	*idMensaje = Serialize_Unpack_idMensaje(packCaughtPokemon);
	*resultado = Serialize_Unpack_resultado(packCaughtPokemon);
	memcpy(idCorrelativo, packCaughtPokemon+sizeof(uint32_t)+sizeof(uint32_t), sizeof(uint32_t));
}





