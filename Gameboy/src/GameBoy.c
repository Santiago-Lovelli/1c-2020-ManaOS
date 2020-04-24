#include "GameBoy.h"

void New_pokemon(char *argv[]){
	int conexion = conectarA(argv[2]);
	//Serialize
}

void Appeared_pokemon(char *argv[]){
	int conexion = conectarA(argv[2]);
	//Serialize
}

void Catch_pokemon(char *argv[]){
	int conexion = conectarA(argv[2]);
	//Serialize
}

void Caught_pokemon(char *argv[]){
	int conexion = conectarA(argv[2]);
	//Serialize
}

void Get_pokemon(char *argv[]){
	int conexion = conectarA(argv[2]);
	//Serialize
}

int obtenerNroMensaje(char *actual){
	if( string_equals_ignore_case(actual,"NEW_POKEMON") )
		return d_NEW_POKEMON;
	else if( string_equals_ignore_case(actual,"APPEARED_POKEMON") )
		return d_APPEARED_POKEMON;
	else if ( string_equals_ignore_case(actual,"CATCH_POKEMON") )
		return d_CATCH_POKEMON;
	else if ( string_equals_ignore_case(actual,"CAUGHT_POKEMON") )
		return d_CAUGHT_POKEMON;
	else if ( string_equals_ignore_case(actual,"GET_POKEMON") )
		return d_GET_POKEMON;
	return 99;
}

int obtenerNroProceso(char *actual){
	if( string_equals_ignore_case(actual,"BROKER") )
		return d_BROKER;
	else if( string_equals_ignore_case(actual,"TEAM") )
		return d_TEAM;
	else if( string_equals_ignore_case(actual,"GAMECARD") )
		return d_GAMECARD;
	else if( string_equals_ignore_case(actual,"SUSCRIPTOR") )
		return d_SUBSCRIBER;
	return 99;
}

char* obtenerPuertoProceso(char *actual){
	switch ( obtenerNroProceso(actual) ){
	case d_BROKER:;
		return puertoBroker;
		break;
	case d_TEAM:;
		return puertoTeam;
		break;
	case d_GAMECARD:;
		return puertoGameCard;
		break;
	default:;
		return "-1";
		break;
	}
}

char* obtenerIpProceso(char *actual){
	switch ( obtenerNroProceso(actual) ){
		case d_BROKER:;
			return ipBroker;
			break;
		case d_TEAM:;
			return ipTeam;
			break;
		case d_GAMECARD:;
			return ipGamecard;
			break;
		default:;
			return "-1";
			break;
		}
}

int conectarA(char *actual){
	return conectarse_a_un_servidor( obtenerIpProceso(actual) , obtenerPuertoProceso(actual) , logger);
}

void cumplirPedido(int argc, char *argv[]){
	switch( obtenerNroMensaje(argv[3]) ) {

	case d_NEW_POKEMON:;
		if(argc < 8){
		    log_info(logger,"Argumentos insuficientes para la operacion: %s", argv[3]);
		    break;
		}
		log_info(logger,"RECIBI EL PEDIDO: %s PARA EL PROCESO: %s", argv[3], argv[2]);
		New_pokemon(argv);
		break;

	case d_APPEARED_POKEMON:;
	    if(argc < 7){
	    	log_info(logger,"Argumentos insuficientes para la operacion: %s", argv[3]);
	    	break;
	    }
		log_info(logger,"RECIBI EL PEDIDO: %s PARA EL PROCESO: %s", argv[3], argv[2]);
		Appeared_pokemon(argv);
		/*log_info(logger,"POKEMON: %s ", argv[4]);
		log_info(logger,"POSX: %i ", atoi( argv[5] ) );
		log_info(logger,"POSY: %i ", atoi( argv[6] ) );*/
		break;

	case d_CATCH_POKEMON:;
		if(argc < 7){
			log_info(logger,"Argumentos insuficientes para la operacion: %s", argv[3]);
			break;
		}
		log_info(logger,"RECIBI EL PEDIDO: %s PARA EL PROCESO: %s", argv[3], argv[2]);
		Catch_pokemon(argv);
		break;

	case d_CAUGHT_POKEMON:;
		if(argc < 6){
			log_info(logger,"Argumentos insuficientes para la operacion: %s", argv[3]);
			break;
		}
		log_info(logger,"RECIBI EL PEDIDO: %s PARA EL PROCESO: %s", argv[3], argv[2]);
		Caught_pokemon(argv);
		break;

	case d_GET_POKEMON:;
		if(argc < 5){
			log_info(logger,"Argumentos insuficientes para la operacion: %s", argv[3]);
			break;
		}
		log_info(logger,"RECIBI EL PEDIDO: %s PARA EL PROCESO: %s", argv[3], argv[2]);
		Get_pokemon(argv);
		break;
	case d_SUBSCRIBE_QUEUE:;
		//pendiente
		break;
	default:;
		log_info(logger,"DEFAULT");
		break;
	}
}

int main(int argc, char *argv[]) {
	t_config* archivo_de_configuracion = config_create("../Gameboy.config");

	puertoBroker = config_get_string_value(archivo_de_configuracion, "PUERTO_BROKER");
	ipBroker = config_get_string_value(archivo_de_configuracion, "IP_BROKER");

	puertoTeam = config_get_string_value(archivo_de_configuracion, "PUERTO_TEAM");
	ipTeam = config_get_string_value(archivo_de_configuracion, "IP_TEAM");

	puertoGameCard = config_get_string_value(archivo_de_configuracion, "PUERTO_GAMECARD");
	ipGamecard = config_get_string_value(archivo_de_configuracion, "IP_GAMECARD");

	logger = iniciar_log("GameBoy");

	log_info(logger,"PUERTO BROKER: %s IP BROKER: %s\n", puertoBroker, ipBroker);
	log_info(logger,"PUERTO TEAM: %s IP TEAM: %s\n", puertoTeam, ipTeam);
	log_info(logger,"PUERTO GAMECARD: %s IP GAMECARD: %s\n", puertoGameCard, ipGamecard);

	cumplirPedido(argc,argv);

	/*
	int conexionBroker = conectarse_a_un_servidor(ipBroker,puertoBroker,logger);
	int conexionTeam = conectarse_a_un_servidor(ipTeam,puertoTeam,logger);
	int conexionGamecard = conectarse_a_un_servidor(ipGamecard,puertoGameCard,logger);
	*/

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
