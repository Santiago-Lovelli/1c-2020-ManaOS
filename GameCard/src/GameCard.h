#ifndef GAMECARD_H_
#define GAMECARD_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <Logger/Logger.h>
#include <commons/config.h>
#include <commons/string.h>
#include <Conexiones/Conexiones.h>
#include <Serializacion/Serializacion.h>
#include <sys/mman.h>
#include <Bitmap/Bitmap.h>
#include <unistd.h>

t_log * loggerGeneral;
t_config *archivo_de_configuracion;

typedef struct {
	uint32_t tamanioDeBloque;
	uint32_t bloques;
	char numeroMagico[11];
}m_metadata;

m_metadata metadata;
t_bitarray * bitmap;

void* atenderGameboy();
void iniciarServidorDeGameBoy();
void levantarLogYArchivoDeConfiguracion();
void atender(HeaderDelibird header, int cliente);
void recibirYAtenderUnCliente(int cliente);
void conectarmeColaDe(d_message colaDeSuscripcion);
void cargarMetadata();
void * obtenerBitmap();
void * obtenerMetadata();

#endif /* GAMECARD_H_ */
