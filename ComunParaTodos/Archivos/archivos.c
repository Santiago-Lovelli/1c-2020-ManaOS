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
