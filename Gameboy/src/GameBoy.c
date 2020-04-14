#include "GameBoy.h"

int main(void) {
	t_config* archivo_de_configuracion = config_create("/home/utnso/workspace/tp-2020-1c-ManaOS-/GameCard/GameCard.config");
	char *puerto = config_get_string_value(archivo_de_configuracion, "PUERTO_GAMECARD");
	char *ip = config_get_string_value(archivo_de_configuracion, "IP_GAMECARD");
	t_log *logger = iniciar_log("GameBoy");
	conectarse_a_un_servidor(ip,puerto,logger);

	return EXIT_SUCCESS;
}
