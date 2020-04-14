
#include "Logger.h"

t_log* iniciar_log(char* nombreDeLog){
	char* nombreConExtensionLog = string_from_format("%s.log",nombreDeLog);
	t_log * logger = log_create(nombreConExtensionLog, nombreDeLog, 1, LOG_LEVEL_INFO);
	log_info(logger, "Logger iniciado: %s\n", nombreConExtensionLog);
	return logger;
}

