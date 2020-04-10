#include "Serializacion.h"

///////////////////////////
// FUNCIONES PARA ENVIAR //
///////////////////////////

bool Fuse_PackAndSend(int socketCliente, const void *path, uint32_t tamPath, f_operacion operacion) {

	uint32_t tamMessage = tamPath + sizeof(f_operacion) + sizeof(uint32_t);
	void* buffer = malloc( tamMessage );
	int desplazamiento = 0;
	memcpy(buffer, &operacion ,sizeof(f_operacion));
	desplazamiento += sizeof(f_operacion);
	memcpy(buffer+desplazamiento, &tamPath , sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer+desplazamiento, path, tamPath);
	desplazamiento += tamPath;
		if(desplazamiento != tamMessage){ return (-1); }
	int resultado = send(socketCliente, buffer, tamMessage, 0);
	free(buffer);
	return resultado;
}

bool Fuse_PackAndSend_Uint32_Response(int socketCliente, uint32_t response){
	void* pack = malloc(sizeof(uint32_t));
	memcpy(pack, &response, sizeof(uint32_t));
	int resultado = Fuse_PackAndSend(socketCliente, pack, sizeof(uint32_t), f_RESPONSE);
	free(pack);
	return resultado;
}

////////////////////////////
// FUNCIONES PARA RECIBIR //
////////////////////////////

HeaderFuse Fuse_RecieveHeader(int socketCliente){
	void* buffer=malloc(sizeof(f_operacion) + sizeof(uint32_t));
	if(recv(socketCliente, buffer, (sizeof(f_operacion) + sizeof(uint32_t)), MSG_WAITALL) == 0){
		HeaderFuse headerQueRetorna;
		headerQueRetorna.operaciones = (-1);
		headerQueRetorna.tamanioMensaje = 0;
		free(buffer);
		return headerQueRetorna;
	}
	uint32_t tamanioMensaje = 0;
	f_operacion operacion;
	memcpy(&operacion,buffer,sizeof(f_operacion));
	memcpy(&tamanioMensaje, buffer+(sizeof(f_operacion)), (sizeof(uint32_t)));
	free(buffer);
	HeaderFuse headerQueRetorna;
	headerQueRetorna.operaciones = operacion;
	headerQueRetorna.tamanioMensaje = tamanioMensaje;
	return headerQueRetorna;
}

void* Fuse_ReceiveAndUnpack(int socketCliente, uint32_t tamanio) {
	void* retorno = malloc(tamanio);
	recv(socketCliente, retorno, tamanio, MSG_WAITALL);
	return retorno;
}

//////////////////////////////////
// FUNCIONES PARA DESEMPAQUETAR //
/////////////////////////////////

uint32_t Fuse_Unpack_Response_Uint32(void *pack) {
	uint32_t response = 0;
	memcpy(&response, pack, sizeof(uint32_t));
	return response;
}

char* Fuse_Unpack_Path(void *buffer) {
	uint32_t tamPath = 0;
	memcpy(&tamPath, buffer, sizeof(uint32_t));
	char *path = malloc(tamPath);
	memcpy(path,buffer+sizeof(uint32_t),tamPath);
	return path;
}


