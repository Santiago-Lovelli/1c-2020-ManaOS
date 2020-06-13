/*
 * Serializacion.h
 *
 *  Created on: 10 abr. 2020
 *      Author: utnso
 */

#ifndef SERIALIZACION_SERIALIZACION_H_
#define SERIALIZACION_SERIALIZACION_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

/////////////////////////////////////////
//       Como usar esta libreria       //
/////////////////////////////////////////
//                                     //
// - Para enviar se utiliza Serialize  //
// _PackAndSend_ y el tipo de paquete  //
// correspondiente                     //
//                                     //
// - Para recibir algo primero se      //
// utiliza receiveHeader y despues     //
// con los datos obtenidos, se llama a //
// receive and unpack obteniendo asi   //
// el pack void*                       //
//                                     //
// - Para extraer datos especificos    //
// de estos paquetes se utilizan las   //
// funciones unpack                    //
//                                     //
// - NUNCA llamar a una funcion unpack //
// no correspondiente al tipo de pack  //
// que recibimos                       //
//                                     //
/////////////////////////////////////////

//////////////////////////////////////////
//           Tipos de mensajes          //
//////////////////////////////////////////
typedef enum d_messages {
	d_NEW_POKEMON,
	d_CATCH_POKEMON,
	d_GET_POKEMON,
	d_APPEARED_POKEMON,
	d_CAUGHT_POKEMON,
	d_LOCALIZED_POKEMON,
	d_ACK,
	d_SUBSCRIBE_QUEUE
} d_message;

typedef enum d_process {
	d_BROKER,
	d_TEAM,
	d_GAMECARD,
	d_SUBSCRIBER
} d_process;

////////////////////////
// Comunicacion Base //
///////////////////////

typedef struct {
	uint32_t tamanioMensaje;
	d_message tipoMensaje;
}__attribute__((packed)) HeaderDelibird; //Esta estructura es de tamaño 8

typedef struct {
	uint32_t posX;
	uint32_t posY;
	uint32_t cantidad;
}__attribute__((packed)) d_PosCant;

////////////////
// FUNCIONES //
///////////////

///////////////////////////
// FUNCIONES PARA ENVIAR //
///////////////////////////

/**
* ESTA FUNCION ENVIA UN PAQUETE DEL TAMAÑO ESPECIFICADO A TRAVES DEL SOCKET ESPECIFICADO
* ES LA FUNCION DEFAULT QUE USAMOS PARA ENVIAR UN PAQUETE Y LAS OTRAS FUNCIONES DE ENVIAR
* PAQUETES LLAMARAN A ESTA
*/

bool Serialize_PackAndSend(int socketCliente, const void*pack, uint32_t tamPack, d_message tipoMensaje);

/**
* ESTA FUNCION NOS SIRVE PARA CONTESTAR ACK
*/

bool Serialize_PackAndSend_ACK(int socketCliente, uint32_t miId);

/**
 * ESTA FUNCION NOS SIRVE PARA PEDIR SUSCRIBIRNOS A UNA COLA
 */

bool Serialize_PackAndSend_SubscribeQueue(int socketCliente, d_message queue);

/**
 * ESTA FUNCION ENVIA UN PAQUETE DEL TIPO NEW_POKEMON A TRAVES DEL SOCKET ESPECIFICADO
 */

bool Serialize_PackAndSend_NEW_POKEMON(int socketCliente, uint32_t idMensaje,const void *pokemon, uint32_t posX, uint32_t posY, uint32_t cantidad);

/**
 * ESTA FUNCION ENVIA UN PAQUETE DEL TIPO NEW_POKEMON SIN EL ID MENSAJE A TRAVES DEL SOCKET ESPECIFICADO
 */

bool Serialize_PackAndSend_NEW_POKEMON_NoID(int socketCliente,const void *pokemon, uint32_t posX, uint32_t posY, uint32_t cantidad);

/**
 * ESTA FUNCION ENVIA UN PAQUETE DEL TIPO CATCH_POKEMON A TRAVES DEL SOCKET ESPECIFICADO
 */

bool Serialize_PackAndSend_CATCH_POKEMON(int socketCliente, uint32_t idMensaje,const void *pokemon, uint32_t posX, uint32_t posY);

/**
 * ESTA FUNCION ENVIA UN PAQUETE DEL TIPO CATCH_POKEMON SIN EL ID MENSAJE A TRAVES DEL SOCKET ESPECIFICADO
 */

bool Serialize_PackAndSend_CATCH_POKEMON_NoID(int socketCliente,const void *pokemon, uint32_t posX, uint32_t posY);

/**
 * ESTA FUNCION ENVIA UN PAQUETE DEL TIPO GET_POKEMON A TRAVES DEL SOCKET ESPECIFICADO
 */

bool Serialize_PackAndSend_GET_POKEMON(int socketCliente, uint32_t idMensaje,const void *pokemon);

/**
 * ESTA FUNCION ENVIA UN PAQUETE DEL TIPO GET_POKEMON SIN EL ID MENSAJE A TRAVES DEL SOCKET ESPECIFICADO
 */

bool Serialize_PackAndSend_GET_POKEMON_NoID(int socketCliente,const void *pokemon);

/**
 * ESTA FUNCION ENVIA UN PAQUETE DEL TIPO APPEARED_POKEMON A TRAVES DEL SOCKET ESPECIFICADO
 */

bool Serialize_PackAndSend_APPEARED_POKEMON(int socketCliente, uint32_t idMensaje,const void *pokemon, uint32_t posX, uint32_t posY);

/**
 * ESTA FUNCION ENVIA UN PAQUETE DEL TIPO APPEARED_POKEMON SIN EL ID MENSAJE A TRAVES DEL SOCKET ESPECIFICADO
 */

bool Serialize_PackAndSend_APPEARED_POKEMON_NoID(int socketCliente,const void *pokemon, uint32_t posX, uint32_t posY);

/**
 * ESTA FUNCION ENVIA UN PAQUETE DEL TIPO CAUGHT_POKEMON A TRAVES DEL SOCKET ESPECIFICADO
 */

bool Serialize_PackAndSend_CAUGHT_POKEMON(int socketCliente, uint32_t idMensaje, uint32_t resultado);

/**
 * ESTA FUNCION ENVIA UN PAQUETE DEL TIPO LOCALIZED_POKEMON A TRAVES DEL SOCKET ESPECIFICADO
 */

bool Serialize_PackAndSend_LOCALIZED_POKEMON(int socketCliente, uint32_t idMensaje, char *pokemon, t_list *poscant);

/**
 * ESTA FUNCION SIRVE COMO UNA ABSTRACCION PARA NO REPETIR LOGICA, SE USA A TRAVES DE
 * LAS FUNCIOENS PACK AND SEND CATCH Y APPEARED
 */

bool Serialize_PackAndSend_CATCHoAPPEARED(int socketCliente, uint32_t idMensaje, const void *pokemon, uint32_t posX, uint32_t posY, d_message tipoMensaje);

/**
 * ESTA FUNCION SIRVE COMO UNA ABSTRACCION PARA NO REPETIR LOGICA, SE USA A TRAVES DE
 * LAS FUNCIOENS PACK AND SEND CATCH Y APPEARED EN SUS VERSIONES SIN ID
 */

bool Serialize_PackAndSend_CATCHoAPPEARED_NoID(int socketCliente, const void *pokemon, uint32_t posX, uint32_t posY, d_message tipoMensaje);

////////////////////////////
// FUNCIONES PARA RECIBIR //
////////////////////////////

/**
* ESTA FUNCION RETORNA UNA ESTRUCTURA DEL TIPO
* HEADERDELIBIRD DE LA CUAL PODEMOS OBTENER EL TIPO
* DE MENSAJE Y EL TAMANIO DEL MISMO, ESTE ULTIMO
* PARAMETRO DEBEREMOS PASARSELO A LA FUNCION
* Serialize_ReceiveAndUnpack PARA QUE NOS DE EL RESTO
* DEL PAQUETE Y DESPUES DE ESO EL RESTO DEL PAQUETE PASARSELO
* A LAS FUNCIONES DE DESEMPAQUETADO CORRESPONDIENTE
*/

HeaderDelibird Serialize_RecieveHeader(int socketCliente);

/**
* ESTA FUNCION RECIBE UN PAQUETE A TRAVES DEL
* SOCKET, TENER EN CUENTA QUE SEGUN EL TIPO DE
* OPERACION, EL PAQUETE RECIBIDO SERA DISTINTO
* PARA DESEMPAQUETAR APROPIADAMENTE HARA FALTA
* LLAMAR A OTRAS FUNCIONES DE DESEMPAQUETADO
*/

void* Serialize_ReceiveAndUnpack(int socketCliente, uint32_t tamanioChar);

//////////////////////////////////
// FUNCIONES PARA DESEMPAQUETAR //
/////////////////////////////////

/**
* ESTA FUNCION DESEMPAQUETA EL ENTERO DE UN
* PAQUETE ACK, ESTE ENTERO REPRESENTARA EL
* ESTADO DE LA OPERACION
*/

uint32_t Serialize_Unpack_ACK(void *pack);


/**
 * ESTA FUNCION DESEMPAQUETA EL ID DEL MENSAJE DE UN
 * PAQUETE VOID* QUE LO CONTENGA
 * NO USAR ESTA FUNCION EN PAQUETES QUE NO
 * CONTENGAN DICHO DATO
 */

uint32_t Serialize_Unpack_idMensaje(void *pack);

/**
 * ESTA FUNCION DESEMPAQUETA EL NOMBRE DEL POKEMON
 * PAQUETE VOID* QUE LO CONTENGA
 * NO USAR ESTA FUNCION EN PAQUETES QUE NO
 * CONTENGAN DICHO DATO
 */

char* Serialize_Unpack_pokemonName(void *buffer);

/**
 * ESTA FUNCION DESEMPAQUETA LA POSICION X
 * PAQUETE VOID* QUE LO CONTENGA
 * NO USAR ESTA FUNCION EN PAQUETES QUE NO
 * CONTENGAN DICHO DATO
 */

uint32_t Serialize_Unpack_posX(void *pack);

/**
 * ESTA FUNCION DESEMPAQUETA LA POSICION Y
 * PAQUETE VOID* QUE LO CONTENGA
 * NO USAR ESTA FUNCION EN PAQUETES QUE NO
 * CONTENGAN DICHO DATO
 */

uint32_t Serialize_Unpack_posY(void *pack);

/**
 * ESTA FUNCION DESEMPAQUETA LA CANTIDAD
 * PAQUETE VOID* QUE LO CONTENGA
 * NO USAR ESTA FUNCION EN PAQUETES QUE NO
 * CONTENGAN DICHO DATO
 */

uint32_t Serialize_Unpack_cantidad(void *pack);

/**
 * ESTA FUNCION DESEMPAQUETA EL NOMBRE
 * PAQUETE VOID* QUE LO CONTENGA Y QUE NO TENGA ID
 * NO USAR ESTA FUNCION EN PAQUETES QUE NO
 * CONTENGAN DICHO DATO
 */

char* Serialize_Unpack_pokemonName_NoID(void *buffer);

/**
 * ESTA FUNCION DESEMPAQUETA LA POS X
 * PAQUETE VOID* QUE LO CONTENGA Y QUE NO TENGA ID
 * NO USAR ESTA FUNCION EN PAQUETES QUE NO
 * CONTENGAN DICHO DATO
 */

uint32_t Serialize_Unpack_posX_NoID(void *pack);

/**
 * ESTA FUNCION DESEMPAQUETA LA POS Y
 * PAQUETE VOID* QUE LO CONTENGA Y QUE NO TENGA ID
 * NO USAR ESTA FUNCION EN PAQUETES QUE NO
 * CONTENGAN DICHO DATO
 */

uint32_t Serialize_Unpack_posY_NoID(void *pack);

/**
 * ESTA FUNCION DESEMPAQUETA LA CANTIDAD
 * PAQUETE VOID* QUE LO CONTENGA Y QUE NO TENGA ID
 * NO USAR ESTA FUNCION EN PAQUETES QUE NO
 * CONTENGAN DICHO DATO
 */

uint32_t Serialize_Unpack_cantidad_NoID(void *pack);

/**
 * ESTA FUNCION DESEMPAQUETA EL RESULTADO
 * PAQUETE VOID* QUE LO CONTENGA
 * NO USAR ESTA FUNCION EN PAQUETES QUE NO
 * CONTENGAN DICHO DATO
 */

uint32_t Serialize_Unpack_resultado(void *pack);

//////////////////////////////////////
// FUNCIONES PARA DESEMPAQUETAR PRO //
/////////////////////////////////////

/*
 * La intencion de las funciones de desempaquetado pro
 * es facilitar el desempaquetado de los paquetes recibidos
 */

/*
 * ESTA FUNCION DADO LOS PARAMETROS PASADOS POR REFERENCIA Y
 * EL PAQUETE VOID* LLENA ESOS PARAMETROS CON LOS VALORES ADENTRO
 * DEL PAQUETE DE TIPO NEW_POKEMON (SOLO SIRVE PARA ESE TIPO DE PAQUETE)
 */

void Serialize_Unpack_NewPokemon(void *packNewPokemon, uint32_t *idMensaje, char **nombre, uint32_t *posX, uint32_t *posY, uint32_t *cantidad);

/*
 * ESTA FUNCION DADO LOS PARAMETROS PASADOS POR REFERENCIA Y
 * EL PAQUETE VOID* LLENA ESOS PARAMETROS CON LOS VALORES ADENTRO
 * DEL PAQUETE DE TIPO CATCH_POKEMON (SOLO SIRVE PARA ESE TIPO DE PAQUETE)
 */

void Serialize_Unpack_CatchPokemon(void *packCatchPokemon, uint32_t *idMensaje, char **nombre, uint32_t *posX, uint32_t *posY);

/*
 * ESTA FUNCION DADO LOS PARAMETROS PASADOS POR REFERENCIA Y
 * EL PAQUETE VOID* LLENA ESOS PARAMETROS CON LOS VALORES ADENTRO
 * DEL PAQUETE DE TIPO GETPOKEMON (SOLO SIRVE PARA ESE TIPO DE PAQUETE)
 */

void Serialize_Unpack_GetPokemon(void *packGetPokemon, uint32_t *idMensaje, char **nombre);

/*
 * ESTA FUNCION DADO LOS PARAMETROS PASADOS POR REFERENCIA Y
 * EL PAQUETE VOID* LLENA ESOS PARAMETROS CON LOS VALORES ADENTRO
 * DEL PAQUETE DE TIPO APPEARED_POKEMON (SOLO SIRVE PARA ESE TIPO DE PAQUETE)
 */

void Serialize_Unpack_AppearedPokemon(void *packAppearedPokemon, uint32_t *idMensaje, char **nombre, uint32_t *posX, uint32_t *posY);

/*
 * ESTA FUNCION DADO LOS PARAMETROS PASADOS POR REFERENCIA Y
 * EL PAQUETE VOID* LLENA ESOS PARAMETROS CON LOS VALORES ADENTRO
 * DEL PAQUETE DE TIPO CAUGHT_POKEMON (SOLO SIRVE PARA ESE TIPO DE PAQUETE)
 */

void Serialize_Unpack_CaughtPokemon(void *packCaughtPokemon, uint32_t *idMensaje, uint32_t *resultado);

/*
 * ESTA FUNCION DADO LOS PARAMETROS PASADOS POR REFERENCIA Y
 * EL PAQUETE VOID* LLENA ESOS PARAMETROS CON LOS VALORES ADENTRO
 * DEL PAQUETE DE TIPO LOCALIZED_POKEMON (SOLO SIRVE PARA ESE TIPO DE PAQUETE)
 */

void Serialize_Unpack_LocalizedPokemon(void *packLocalizedPokemon, uint32_t *idMensaje, char **nombre, t_list **poscant);

/*
 * ESTA FUNCION DADO LOS PARAMETROS PASADOS POR REFERENCIA Y
 * EL PAQUETE VOID* LLENA ESOS PARAMETROS CON LOS VALORES ADENTRO
 * DEL PAQUETE DE TIPO NEW_POKEMON EN SU VERSION
 * SIN ID (SOLO SIRVE PARA ESE TIPO DE PAQUETE)
 */

void Serialize_Unpack_NewPokemon_NoID(void *packNewPokemon, char **nombre, uint32_t *posX, uint32_t *posY, uint32_t *cantidad);

/*
 * ESTA FUNCION DADO LOS PARAMETROS PASADOS POR REFERENCIA Y
 * EL PAQUETE VOID* LLENA ESOS PARAMETROS CON LOS VALORES ADENTRO
 * DEL PAQUETE DE TIPO APPEARED_POKEMON EN SU VERSION
 * SIN ID (SOLO SIRVE PARA ESE TIPO DE PAQUETE)
 */

void Serialize_Unpack_AppearedPokemon_NoID(void *packAppearedPokemon, char **nombre, uint32_t *posX, uint32_t *posY);

/*
 * ESTA FUNCION DADO LOS PARAMETROS PASADOS POR REFERENCIA Y
 * EL PAQUETE VOID* LLENA ESOS PARAMETROS CON LOS VALORES ADENTRO
 * DEL PAQUETE DE TIPO CATCH_POKEMON EN SU VERSION
 * SIN ID (SOLO SIRVE PARA ESE TIPO DE PAQUETE)
 */

void Serialize_Unpack_CatchPokemon_NoID(void *packCatchPokemon, char **nombre, uint32_t *posX, uint32_t *posY);

/*
 * ESTA FUNCION DADO LOS PARAMETROS PASADOS POR REFERENCIA Y
 * EL PAQUETE VOID* LLENA ESOS PARAMETROS CON LOS VALORES ADENTRO
 * DEL PAQUETE DE TIPO GET_POKEMON EN SU VERSION
 * SIN ID (SOLO SIRVE PARA ESE TIPO DE PAQUETE)
 */

void Serialize_Unpack_GetPokemon_NoID(void *packGetPokemon, char **nombre);

#endif /* SERIALIZACION_SERIALIZACION_H_ */
