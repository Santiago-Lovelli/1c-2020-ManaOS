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

//////////////////////////////////////////
//           Tipos de Permisos         //
//////////////////////////////////////////
typedef enum f_operaciones {
	f_HANDSHAKE,
	f_RESPONSE
} f_operacion;

////////////////////////
// Comunicacion Base //
///////////////////////

typedef struct {
	uint32_t tamanioMensaje;
	f_operacion operaciones;
}__attribute__((packed)) HeaderFuse; //Esta estructura es de tamaño 8

////////////////
// FUNCIONES //
///////////////

///////////////////////////
// FUNCIONES PARA ENVIAR //
///////////////////////////

/**
* ESTA FUNCION ENVIA UN PAQUETE DEL TAMAÑO ESPECIFICADO A TRAVES DEL SOCKET ESPECIFICADO
*/

bool Fuse_PackAndSend(int socketCliente, const void*path, uint32_t tamPath, f_operacion operacion);

/**
* ESTA FUNCION ENVIA UNA RESPONSE INT DEL SERVER AL CLI
*/

bool Fuse_PackAndSend_Uint32_Response(int socketCliente, uint32_t response);

////////////////////////////
// FUNCIONES PARA RECIBIR //
////////////////////////////

/**
* ESTA FUNCION RETORNA UNA ESTRUCTURA DEL TIPO
* HEADERFUSE DE LA CUAL PODEMOS OBTENER EL TIPO
* DE MENSAJE Y EL TAMANIO DEL MISMO, ESTE ULTIMO
* PARAMETRO DEBEREMOS PASARSELO A LA FUNCION
* Fuse_ReceiveAndUnpack PARA QUE NOS DE EL RESTO
* DEL PAQUETE
*/

HeaderFuse Fuse_RecieveHeader(int socketCliente);

/**
* ESTA FUNCION RECIBE UN PAQUETE A TRAVES DEL
* SOCKET, TENER EN CUENTA QUE SEGUN EL TIPO DE
* OPERACION, EL PAQUETE RECIBIDO SERA DISTINTO
* EN ALGUNOS CASOS SERA SOLO UN PATH ASI QUE PODREMOS
* USARLO DIRECTAMENTE, EN OTROS HARA FALTA LLAMAR
* A OTRAS FUNCIONES DE DESEMPAQUETADO
*/

void* Fuse_ReceiveAndUnpack(int socketCliente, uint32_t tamanioChar);

//////////////////////////////////
// FUNCIONES PARA DESEMPAQUETAR //
/////////////////////////////////

/**
* ESTA FUNCION SE USA SOLO EN CASO DE QUERER
* RECIBIR UN UINT32 DE UN PAQUETE ENVIADO POR
* EL SERVIDOR
*/

uint32_t Fuse_Unpack_Response_Uint32(void *pack);

/**
* ESTA FUNCION SE USA SOLO EN CASO DE QUERER
* RECIBIR UN PATH DE UN PAQUETE QUE CONTENGA ALGO
* MAS ADEMAS DEL PATH
*/

char* Fuse_Unpack_Path(void *buffer);


#endif /* SERIALIZACION_SERIALIZACION_H_ */
