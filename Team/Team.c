#include <Team.h>

int main (void){
	iniciar();
	escucharMensajes(); ///////ESTO VA CON UN HILO
	conectarseConBroker(); //////ESTO VA CON OTRO HILO SEGUN ENTIENDO
	return EXIT_SUCCESS;
}

void iniciar(){
	TEAM_LOG = iniciar_log("Team");
	iniciarConfig();
	log_info (TEAM_LOG, "TEAM OK");
	iniciarHilos();
}

void escucharMensajes(){
	int servidor = iniciar_servidor(TEAM_CONFIG.IP_TEAM, TEAM_CONFIG.PUERTO_TEAM,TEAM_LOG);
	int espera = esperar_cliente_con_accept(servidor, TEAM_LOG); //ESTO ES CON OTRO HILO?
}

void conectarseConBroker(){
	int cliente = conectarse_a_un_servidor(TEAM_CONFIG.IP_BROKER, TEAM_CONFIG.PUERTO_BROKER,TEAM_LOG);
}

void iniciarConfig(){
	t_config* creacionConfig = config_create("/home/utnso/workspace/tp-2020-1c-ManaOS-/Team/Team.config");
	TEAM_CONFIG.POSICIONES_ENTRENADORES = config_get_array_value(creacionConfig, "POSICIONES_ENTRENADORES");
	TEAM_CONFIG.POKEMON_ENTRENADORES = config_get_array_value(creacionConfig, "POKEMON_ENTRENADORES");
	TEAM_CONFIG.OBJETIVOS_ENTRENADORES = config_get_array_value(creacionConfig, "OBJETIVOS_ENTRENADORES");
	TEAM_CONFIG.TIEMPO_RECONEXION = config_get_int_value (creacionConfig, "TIEMPO_RECONEXION");
	TEAM_CONFIG.RETARDO_CICLO_CPU = config_get_int_value (creacionConfig, "RETARDO_CICLO_CPU");
	TEAM_CONFIG.ALGORITMO_PLANIFICACION = config_get_string_value (creacionConfig, "ALGORITMO_PLANIFICACION");
	TEAM_CONFIG.QUANTUM = config_get_int_value (creacionConfig, "QUANTUM");
	TEAM_CONFIG.ESTIMACION_INICIAL = config_get_int_value (creacionConfig, "ESTIMACION_INICIAL");
	TEAM_CONFIG.IP_BROKER = config_get_string_value (creacionConfig, "IP_BROKER");
	TEAM_CONFIG.PUERTO_BROKER = config_get_int_value (creacionConfig, "PUERTO_BROKER");
	TEAM_CONFIG.LOG_FILE = config_get_string_value (creacionConfig, "LOG_FILE");
	TEAM_CONFIG.IP_TEAM = config_get_string_value (creacionConfig, "IP_TEAM");
	TEAM_CONFIG.PUERTO_TEAM = config_get_int_value (creacionConfig, "PUERTO_TEAM");
}





