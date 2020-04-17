/*
 * GameCard.h
 *
 *  Created on: 13 abr. 2020
 *      Author: utnso
 */

#ifndef GAMECARD_H_
#define GAMECARD_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <Logger/Logger.h>
#include <commons/config.h>
#include <Conexiones/Conexiones.h>
#include <Serializacion/Serializacion.h>

t_log * loggerGeneral;
t_config *archivo_de_configuracion;

void* atenderGameboy();
void iniciarServidorDeGameBoy();
void iniciar();
void atender(HeaderDelibird header, int cliente);
void recibirYAtenderUnCliente(int cliente);

#endif /* GAMECARD_H_ */
