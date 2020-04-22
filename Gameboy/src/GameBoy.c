#include "GameBoy.h"

int main(void) {
	t_config* archivo_de_configuracion = config_create("../Gameboy.config");

	char *puertoBroker = config_get_string_value(archivo_de_configuracion, "PUERTO_BROKER");
	char *ipBroker = config_get_string_value(archivo_de_configuracion, "IP_BROKER");

	char *puertoTeam = config_get_string_value(archivo_de_configuracion, "PUERTO_TEAM");
	char *ipTeam = config_get_string_value(archivo_de_configuracion, "IP_TEAM");

	char *puertoGameCard = config_get_string_value(archivo_de_configuracion, "PUERTO_GAMECARD");
	char *ipGamecard = config_get_string_value(archivo_de_configuracion, "IP_GAMECARD");

	t_log *logger = iniciar_log("GameBoy");

	log_info(logger,"PUERTO BROKER: %s IP BROKER: %s\n", puertoBroker, ipBroker);

	log_info(logger,"PUERTO TEAM: %s IP TEAM: %s\n", puertoTeam, ipTeam);

	log_info(logger,"PUERTO GAMECARD: %s IP GAMECARD: %s\n", puertoGameCard, ipGamecard);



	//int conexion = conectarse_a_un_servidor(ip,puerto,logger);

	/*
	Serialize_PackAndSend_NEW_POKEMON(conexion, 1,"PIKACHU", 5, 5, 10);
	log_info(logger,"Se envio mensaje: %i, %s, x: %i, y: %i, cant: %i\n", 1,"PIKACHU", 5, 5, 10);

	Serialize_PackAndSend_CATCH_POKEMON(conexion, 2,"CHARIZARD", 2, 5);
	log_info(logger,"Se envio mensaje: %i, %s, x: %i, y: %i\n", 2,"CHARIZARD", 2, 5);

	Serialize_PackAndSend_GET_POKEMON(conexion, 3,"BULBASUAR");
	log_info(logger,"Se envio mensaje: %i, %s\n", 3,"BULBASUAR");

	Serialize_PackAndSend_GET_POKEMON(conexion, 3,"BULBASUAR");
	log_info(logger,"Se envio mensaje: %i, %s\n", 3,"BULBASUAR");

	Serialize_PackAndSend_CAUGHT_POKEMON(conexion, 4, 0);
	log_info(logger,"Se envio mensaje: %i, %s, %i\n", 3,"BULBASUAR", 0);
	*/

	return EXIT_SUCCESS;
}
