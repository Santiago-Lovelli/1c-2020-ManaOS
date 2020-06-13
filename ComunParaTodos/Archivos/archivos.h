#ifndef ARCHIVOS_H_
#define ARCHIVOS_H_

#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdint.h>
#include <commons/log.h>


typedef uint32_t ptrGBloque;


typedef struct {
	/*unsigned*/ char bytes [4096];
} Bloque;


typedef struct {
	ptrGBloque bloques_de_datos [1024];
} Bloque_de_puntero;


////////////////////////////////////////
//       Funciones De Archivos       //
///////////////////////////////////////



/*
* Esta funcion retorna el tamanio de
* el archivo pasado por parametro
*/
uint32_t tamanio_archivo(char *archivo);

/*
* Esta funcion retorna el tamanio de
* el archivo pasado por parametro en bloques
*/
int tamanio_archivo_en_bloques(uint32_t tamanio);


/*
 * Retorna un char* con el ultimo valor del path despues de la ultima "/"
 * Ejemplo 1: 
 * get_filename("/casa/hola") => "hola"
 * Ejemplo2: 
 * get_filename("foo/casa/chau") => "chau"
 * Ejemplo3: 
 * get_filename("foo") => ""
 */
const char* get_filename(const char* path);

/*
 * Retorna el ultimo nombre del path despues de la ultima "."
 * Ejemplo 1: 
 * get_file_extension("casa.exe") => "exe"
 * Ejemplo2: 
 * get_file_extension("casa.exe.txt") => "txt"
 * Ejemplo3:  
 * get_file_extension("casa") => ""
 */
const char* get_file_extension(const char* path);

/*
 * Retorna un void* mmapeado al bloque de datos con un tamanio pasado, si el tamanio es nulo
 * reserva el tamanio que le pasan, si es 0 reserva el tamanio total del bloque
 * */
void* mmapeadoBloquePropio(t_log* log, uint32_t tamanioDeseado, char* numeroDeBloque);

#endif /* ARCHIVOS_H_ */
