#ifndef GAMECARD_H_
#define GAMECARD_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <Logger/Logger.h>
#include <commons/config.h>
#include <Conexiones/Conexiones.h>
#include <Serializacion/Serializacion.h>
#include <sys/mman.h>

t_log * loggerGeneral;
t_config *archivo_de_configuracion;

void* atenderGameboy();
void iniciarServidorDeGameBoy();
void levantarLogYArchivoDeConfiguracion();
void atender(HeaderDelibird header, int cliente);
void recibirYAtenderUnCliente(int cliente);
void conectarmeColaDe(d_message colaDeSuscripcion);

#endif /* GAMECARD_H_ */
