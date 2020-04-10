#include "Bitmap.h"

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
