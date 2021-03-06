#ifndef GAMEBOY_H_
#define GAMEBOY_H_

#include <stdio.h>
#include <stdlib.h>
#include <Conexiones/Conexiones.h>
#include <commons/config.h>
#include <Logger/Logger.h>
#include <Serializacion/Serializacion.h>
#include <pthread.h>

////////////////////////
// VARIABLES GLOBALES //
////////////////////////

t_log *logger;
char *puertoBroker;
char *ipBroker;
char *puertoTeam;
char *ipTeam;
char *puertoGameCard;
char *ipGamecard;

////////////////
// FUNCIONES  //
///////////////

uint32_t OKoFAIL(char *actual);
void New_pokemon(char *argv[]);
void Appeared_pokemon(char *argv[]);
void Catch_pokemon(char *argv[]);
void Caught_pokemon(char *argv[]);
void Get_pokemon(char *argv[]);
void Subscribe_Queue(char *argv[]);
int obtenerNroMensaje(char *actual);
int obtenerNroProceso(char *actual);
char* obtenerPuertoProceso(char *actual);
char* obtenerIpProceso(char *actual);
int conectarA(char *actual);
void cumplirPedido(int argc, char *argv[]);
void iniciarConfiguracion();
void iniciarEstructuras();
void atenderMensajes (HeaderDelibird headerRecibido, int socket);
void AtenderCliente (void * conexion);


#endif /* GAMEBOY_H_ */
