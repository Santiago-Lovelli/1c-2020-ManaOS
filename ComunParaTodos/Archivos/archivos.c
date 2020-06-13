#include "archivos.h"

const char* get_filename(const char* path){
	const char *file = strrchr(path,'/');
	if(!file || file == path) return "";
	return file +1;
}

const char* get_file_extension(const char* filename){
	const char *extension = strrchr(filename,'.');
	if(!extension || extension == filename) return "";
	return extension +1;
}


uint32_t tamanio_archivo(char *archivo){
	FILE* file = fopen(archivo,"r");
	fseek(file, 0L, SEEK_END);
	uint32_t tamanio = ftell(file);
	fclose(file);
	return tamanio;
}

int tamanio_archivo_en_bloques(uint32_t tamanio){
	if(tamanio%sizeof(Bloque)>0){
		return tamanio/sizeof(Bloque);
	}return (tamanio/sizeof(Bloque))+1;
}

void* mmapeadoBloquePropio(t_log* log, uint32_t tamanioDeseado, char* numeroDeBloque) {
	log_info(log, "bloque a escribir: %s", numeroDeBloque);

	char * pathBloque = obtenerPathDeBloque(numeroDeBloque);

	log_info(log, "path a escribir: %s", pathBloque);

	uint32_t tamanio_archivo_de_metadata = 0;

	if(tamanioDeseado == 0){
		tamanio_archivo_de_metadata = tamanio_archivo(pathBloque);
	}else{
		tamanio_archivo_de_metadata = tamanioDeseado;
	}

	int bloque = open(pathBloque, O_RDWR, 0);

	free(pathBloque);

	void* bloqueConDatos = mmap(NULL, tamanio_archivo_de_metadata,
	PROT_READ | PROT_WRITE,
	MAP_SHARED | MAP_FILE, bloque, 0);

	return bloqueConDatos;
}
