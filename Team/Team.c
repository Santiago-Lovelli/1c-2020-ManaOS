#include <Team.h>

int main (void){
	iniciarConfig();
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
}



