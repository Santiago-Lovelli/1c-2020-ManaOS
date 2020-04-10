#ifndef ARCHIVOS_H_
#define ARCHIVOS_H_
#include <string.h>
#include <stdlib.h>

////////////////////////////////////////
//       Funciones De Archivos       //
///////////////////////////////////////

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


#endif /* ARCHIVOS_H_ */
