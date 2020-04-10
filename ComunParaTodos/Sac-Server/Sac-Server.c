#include "Sac-Server.h"

void cargar_bitmap(int cantidad, t_bitarray *tBitarray, t_log *logger){
	int cargado = 0;
	while(cargado < cantidad){
		bitarray_set_bit(tBitarray, cargado);
		if(!bitarray_test_bit(tBitarray, cargado)){
			log_error(logger, "cargado: %i", cargado);
		}
		cargado = cargado + 1;
	}
	log_info(logger, "cargado: %i", cargado);
}

uint64_t timestamp(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	long long result = (((long long )tv.tv_sec) * 1000 + ((long) tv.tv_usec) / 1000); //opero con milisegundos
	uint64_t timefinal = result;
	return timefinal;
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

int buscar_espacio_en_bitmap(t_bitarray *tBitarray, t_log *logger){
	int total = bitarray_get_max_bit(tBitarray);
	int indice = 0;
	while(total){
		if(!bitarray_test_bit(tBitarray,indice)){
			return indice;
		}
		total=total-1;
		indice=indice+1;
	}
	log_error(logger, "No hay bits dentro del bitmap libres");
	return -1;
}

void ocupar_bloque_en_bitmap(int indice, t_bitarray *tBitarray){
	bitarray_set_bit(tBitarray,indice);
}

void liberar_bloque_en_bitmap(int indice, t_bitarray *tBitarray){
	bitarray_clean_bit(tBitarray,indice);
}
