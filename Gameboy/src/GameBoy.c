#include "GameBoy.h"

uint32_t OKoFAIL(char *actual){
	if( string_equals_ignore_case(actual, "OK") )
		return 1;
	else if( string_equals_ignore_case(actual, "FAIL") )
		return 0;
	return 99;
}

void New_pokemon(char *argv[]){
	log_info(logger, "SE ENVIARA EL SIGUIENTE PAQUETE:");
	log_info(logger,"POKEMON: %s ", argv[3]);
	log_info(logger,"POSX: %i ", atoi( argv[4] ) );
	log_info(logger,"POSY: %i ", atoi( argv[5] ) );
	log_info(logger,"CANTIDAD: %i ", atoi( argv[6] ) );
	log_info(logger,"IDMENSAJE: %i ", atoi( argv[7] ) );
	int conexion = conectarA(argv[1]);
	d_process procesoActual = obtenerNroProceso(argv[1]);
	if( procesoActual == d_BROKER)
		Serialize_PackAndSend_NEW_POKEMON_NoID(conexion, argv[3], atoi(argv[4]), atoi(argv[5]), atoi( argv[6]) );
	else if ( procesoActual == d_GAMECARD)
		Serialize_PackAndSend_NEW_POKEMON(conexion, atoi(argv[7]), argv[3], atoi(argv[4]), atoi(argv[5]), atoi( argv[6]) );
}

void Appeared_pokemon(char *argv[]){
	log_info(logger, "SE ENVIARA EL SIGUIENTE PAQUETE:");
	log_info(logger,"POKEMON: %s ", argv[3] );
	log_info(logger,"POSX: %i ", atoi( argv[4] ) );
	log_info(logger,"POSY: %i ", atoi( argv[5] ) );
	log_info(logger,"IDMENSAJE: %i ", atoi( argv[6] ) );
	int conexion = conectarA(argv[1]);
	d_process procesoActual = obtenerNroProceso(argv[1]);
	if( procesoActual == d_BROKER)
		Serialize_PackAndSend_APPEARED_POKEMON(conexion, atoi(argv[6]) ,argv[3], atoi(argv[4]), atoi(argv[5]) );
	else if ( procesoActual == d_TEAM)
		Serialize_PackAndSend_APPEARED_POKEMON_NoID(conexion, argv[3], atoi(argv[4]), atoi(argv[5]));
}

void Catch_pokemon(char *argv[]){
	log_info(logger, "SE ENVIARA EL SIGUIENTE PAQUETE:");
	log_info(logger,"POKEMON: %s ", argv[3] );
	log_info(logger,"POSX: %i ", atoi( argv[4] ) );
	log_info(logger,"POSY: %i ", atoi( argv[5] ) );
	log_info(logger,"IDMENSAJE: %i ", atoi( argv[6] ) );
	int conexion = conectarA(argv[1]);
	Serialize_PackAndSend_CATCH_POKEMON(conexion, atoi(argv[6]) ,argv[3], atoi(argv[4]), atoi(argv[5]) );
}

void Caught_pokemon(char *argv[]){
	log_info(logger, "SE ENVIARA EL SIGUIENTE PAQUETE:");
	log_info(logger,"IDMENSAJE: %i ", atoi( argv[3] ) );
	log_info(logger,"ESTADO: %s ", argv[4] );

	int conexion = conectarA(argv[1]);
	Serialize_PackAndSend_CAUGHT_POKEMON(conexion, atoi(argv[3]), OKoFAIL(argv[4]) );

}

void Get_pokemon(char *argv[]){
	log_info(logger, "SE ENVIARA EL SIGUIENTE PAQUETE:");
	log_info(logger,"POKEMON: %s ", argv[3] );
	int conexion = conectarA(argv[1]);
	Serialize_PackAndSend_GET_POKEMON_NoID(conexion, argv[3]);
}

void Subscribe_Queue(char *argv[]){
	log_info(logger, "SE ENVIARA EL SIGUIENTE PAQUETE:");
	log_info(logger,"COLA DE MENSAJES: %s ", argv[2] );
	log_info(logger, "TIEMPO", atoi(argv[3]));
	int conexion = conectarA("BROKER");

	Serialize_PackAndSend_SubscribeQueue(conexion, obtenerNroMensaje(argv[2]));
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
	log_info(logger, "Intentando conectar a: %s", actual);
	return conectarse_a_un_servidor( obtenerIpProceso(actual) , obtenerPuertoProceso(actual) , logger);
}

void cumplirPedido(int argc, char *argv[]){
	log_info(logger, "ARGC: %i ARG0: %s", argc, argv[0]);
	if(argc == 1){
		log_info(logger, "Argumentos insuficientes");
		return;
	}
	switch( obtenerNroMensaje(argv[2]) ) {

	case d_NEW_POKEMON:;
		if(argc < 8){
		    log_info(logger,"Argumentos insuficientes para la operacion: %s", argv[2]);
		    break;
		}
		log_info(logger,"RECIBI EL PEDIDO: %s PARA EL PROCESO: %s", argv[2], argv[1]);
		New_pokemon(argv);
		break;

	case d_APPEARED_POKEMON:;
	    if(argc < 7){
	    	log_info(logger,"Argumentos insuficientes para la operacion: %s", argv[2]);
	    	break;
	    }
		log_info(logger,"RECIBI EL PEDIDO: %s PARA EL PROCESO: %s", argv[2], argv[1]);
		Appeared_pokemon(argv);
		break;

	case d_CATCH_POKEMON:;
		if(argc < 7){
			log_info(logger,"Argumentos insuficientes para la operacion: %s", argv[2]);
			break;
		}
		log_info(logger,"RECIBI EL PEDIDO: %s PARA EL PROCESO: %s", argv[2], argv[1]);
		Catch_pokemon(argv);
		break;

	case d_CAUGHT_POKEMON:;
		if(argc < 5){
			log_info(logger,"Argumentos insuficientes para la operacion: %s", argv[2]);
			break;
		}
		if(OKoFAIL(argv[4]) == 99){
			log_info(logger, "Argumentos erroneo: %s", argv[4]);
			break;
		}
		log_info(logger,"RECIBI EL PEDIDO: %s PARA EL PROCESO: %s", argv[2], argv[1]);
		Caught_pokemon(argv);
		break;

	case d_GET_POKEMON:;
		if(argc < 4){
			log_info(logger,"Argumentos insuficientes para la operacion: %s", argv[2]);
			break;
		}
		log_info(logger,"RECIBI EL PEDIDO: %s PARA EL PROCESO: %s", argv[2], argv[1]);
		Get_pokemon(argv);
		break;
	default:;
		if( string_equals_ignore_case(argv[1],"SUSCRIPTOR") ){
			if(argc < 4){
				log_info(logger,"Argumentos insuficientes para la operacion: %s", argv[2]);
				break;
			}
			log_info(logger,"RECIBI EL PEDIDO: %s PARA LA COLA DE MENSAJES: %s", argv[1], argv[2]);
			Subscribe_Queue(argv);
			break;
		}
		log_info(logger,"DEFAULT");
		break;
	}
}

void iniciarConfiguracion(){
	t_config* archivo_de_configuracion = config_create("../Gameboy.config");
	puertoBroker = config_get_string_value(archivo_de_configuracion, "PUERTO_BROKER");
	ipBroker = config_get_string_value(archivo_de_configuracion, "IP_BROKER");
	puertoTeam = config_get_string_value(archivo_de_configuracion, "PUERTO_TEAM");
	ipTeam = config_get_string_value(archivo_de_configuracion, "IP_TEAM");
	puertoGameCard = config_get_string_value(archivo_de_configuracion, "PUERTO_GAMECARD");
	ipGamecard = config_get_string_value(archivo_de_configuracion, "IP_GAMECARD");
}

void iniciarEstructuras(){
	iniciarConfiguracion();
	logger = iniciar_log("GameBoy");
	log_info(logger,"PUERTO BROKER: %s IP BROKER: %s\n", puertoBroker, ipBroker);
	log_info(logger,"PUERTO TEAM: %s IP TEAM: %s\n", puertoTeam, ipTeam);
	log_info(logger,"PUERTO GAMECARD: %s IP GAMECARD: %s\n", puertoGameCard, ipGamecard);
}

int main(int argc, char *argv[]) {
	iniciarEstructuras();
	cumplirPedido(argc,argv);
	return EXIT_SUCCESS;
}
