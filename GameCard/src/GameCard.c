#include "GameCard.h"

void atender(HeaderDelibird header, int cliente){

	switch (header.tipoMensaje) {
		case d_NEW_POKEMON:;
			log_info(loggerGeneral, "Llego un new pokemon");

			void* packNewPokemon = Serialize_ReceiveAndUnpack(cliente, header.tamanioMensaje);
			uint32_t idMensaje,posX,posY,cantidad;
			char *pokemon;
			Serialize_Unpack_NewPokemon(packNewPokemon, &idMensaje, &pokemon, &posX, &posY, &cantidad);
			/*
			uint32_t idMensaje = Serialize_Unpack_idMensaje(packNewPokemon);
			char *pokemon = Serialize_Unpack_pokemonName(packNewPokemon);
			uint32_t posX = Serialize_Unpack_posX(packNewPokemon);
			uint32_t posY = Serialize_Unpack_posY(packNewPokemon);
			uint32_t cantidad = Serialize_Unpack_cantidad(packNewPokemon);
			*/
			log_info(loggerGeneral,"Me llego mensaje. Id: %i, Pkm: %s, x: %i, y: %i, cant: %i\n", idMensaje,pokemon, posX, posY, cantidad);
			break;
		case d_CATCH_POKEMON:;
			log_info(loggerGeneral, "Llego un catch pokemon");
			break;
		case d_GET_POKEMON:;
			log_info(loggerGeneral, "Llego un get pokemon");
			break;
		default:
			log_error(loggerGeneral, "Mensaje no entendido: %i\n", header);
			break;
	}
}

void* atenderGameBoy(){
	t_log* gameBoyLog = iniciar_log("GameBoy");
	char *puerto = config_get_string_value(archivo_de_configuracion, "PUERTO_GAMECARD");
	log_info(gameBoyLog, "Puerto GameCard: %s", puerto);
	char *ip = config_get_string_value(archivo_de_configuracion, "IP_GAMECARD");
	log_info(gameBoyLog, "Ip GameCard: %s", ip);

	int conexion = iniciar_servidor(ip, puerto, gameBoyLog);
	int cliente = esperar_cliente_con_accept(conexion, gameBoyLog);
	log_info(gameBoyLog, "se conecto cliente: %i", cliente);

	HeaderDelibird header =  Serialize_RecieveHeader(cliente);

	atender(header,cliente);
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
