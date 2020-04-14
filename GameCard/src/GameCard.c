#include "GameCard.h"

void* atenderGameBoy(){
	t_log* gameBoyLog = iniciar_log("GameBoy");
	char *puerto = config_get_string_value(archivo_de_configuracion, "PUERTO_GAMECARD");
	log_info(gameBoyLog, "Puerto GameCard: %s", puerto);
	char *ip = config_get_string_value(archivo_de_configuracion, "IP_GAMECARD");
	log_info(gameBoyLog, "Ip GameCard: %s", ip);

	int conexion = iniciar_servidor(ip, puerto, gameBoyLog);
	int cliente = esperar_cliente_con_accept(conexion, gameBoyLog);
	log_info(gameBoyLog, "se conecto cliente: %i", cliente);
}

void iniciarServidorDeGameBoy(){

}

void iniciar(){
	loggerGeneral = iniciar_log("GameCard");
	archivo_de_configuracion = config_create("/home/utnso/workspace/tp-2020-1c-ManaOS-/GameCard/GameCard.config");
	log_info(loggerGeneral, "Se levanto el archivo de configuracion /home/utnso/workspace/tp-2020-1c-ManaOS-/GameCard/GameCard.config\n");
}


int main(void) {
	iniciar();

	pthread_t* servidor = malloc(sizeof(pthread_t));
	if(pthread_create(servidor,NULL,(void*)atenderGameBoy,NULL) == 0){
		log_info(loggerGeneral,"Se creo hilo de GameBoy");
	}else{
		log_error(loggerGeneral,"No se pudo crear el hilode GameBoy");
	}


	pthread_join(*servidor, NULL);
	return EXIT_SUCCESS;
}
