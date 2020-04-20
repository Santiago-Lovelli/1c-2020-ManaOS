#include "GameBoy.h"

int main(void) {
	t_config* archivo_de_configuracion = config_create("/home/utnso/workspace/tp-2020-1c-ManaOS-/GameCard/GameCard.config");
	char *puerto = config_get_string_value(archivo_de_configuracion, "PUERTO_GAMECARD");
	char *ip = config_get_string_value(archivo_de_configuracion, "IP_GAMECARD");
	t_log *logger = iniciar_log("GameBoy");
	int conexion = conectarse_a_un_servidor(ip,puerto,logger);

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

	return EXIT_SUCCESS;
}
