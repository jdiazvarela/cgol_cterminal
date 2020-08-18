#ifndef biblio_terminal_directo_h
#define biblio_terminal_directo_h


#pragma once
// -- INCLUSIONES

#include <stdlib.h>
// size_t

#include <stdio.h>
// printf()
// sscanf()

#include <sys/ioctl.h>
// struct winsize
//   ws_col
//   ws_row
// #TIOCGWINSZ
// ioctl()

#include <unistd.h>
// #STDIN_FILENO
// #STDOUT_FILENO
// write()
// read()


// --- terminal (directo)

#define terminal_escribirTextoMedido( texto, longitud ) ( ( size_t) write( STDOUT_FILENO, ( texto ), ( longitud ) ) == ( longitud ) )

// obtener la posicion actual del cursor de la terminal
int terminal_leerPosicion( size_t * pColumnas, size_t * pFilas ) {

	// se hace la peticion de la posicion del cursor (temas de claves ANSI)
	if ( ! terminal_escribirTextoMedido( "\x1b[6n", 4 ) ) {
		//printf ( "\\e[6n error\n" );
		return -1;
	};

	// se lee y se carga en el buffer
	char buffer[ 32 ];
	size_t indice = 0;

	while ( indice < sizeof( buffer ) - 1 ) {

		if ( read( STDIN_FILENO, &buffer[ indice ], 1 ) != 1 ) {
			//printf ( "read 1\n" );
			break;
		};

		if ( buffer[ indice ] == 'R' ) break;

		indice++;
	};

	// char nulo al final
	buffer[ indice ] = '\0';

	// escape y [
	if ( buffer[ 0 ] != 27 || buffer[ 1 ] != '[' ) {
		//printf ( "no [\n" );
		return -1;
	};

	// se interpretan los valores devueltos
	if ( sscanf( & buffer[ 2 ], "%ld;%ld", pFilas, pColumnas ) != 2 ) {
		//printf ( "sscanf [\n" );
		return -1;
	};

	return 0;
};

// obtener las dimensiones de la terminal
int terminal_leerDimensiones( size_t * pColumnas, size_t * pFilas ) {

	/*

	// este metodo hackeado trae problemas porque necesita leer de la terminal
	// lo que pasa es que puede llegar a descartar entradas que realmente quiero interpretar

	// al fondo..
	if ( write( STDOUT_FILENO, "\x1b[9999C\x1b[9999B", 14 ) != 14 ) return -1;

	// y a ver hasta donde llego
	return terminal_leerPosicion( pColumnas, pFilas );
	*/

	struct winsize window_size;
	if ( ioctl( STDOUT_FILENO, TIOCGWINSZ, & window_size ) == -1 || window_size.ws_col == 0) {
		return -1;
	} else {
		* pColumnas = window_size.ws_col;
		* pFilas    = window_size.ws_row;
		return 0;
	};
};

// esto es para saltear lo que haga falta para tener la terminal entera disponible
// la idea es que en un emulador de terminal pueda volver a leer lo que estaba previo
// a la ejecucion de esssssssta cosssssaaa
int terminal_saltearPrevio() {

	size_t xI, xF, yI, yF;

	// posicion actual del cursor en la terminal......
	if ( terminal_leerPosicion( & xI, & yI ) < 0 ) return -1;

	// a leer las dimensiones
	// ... puede o no que muevan el cursor.. igual no importa
	if ( terminal_leerDimensiones( & xF, & yF ) < 0 ) return -1;

	// basicamente se baja la cantidad de lineas necesarias
	// para hacer desaparecer lo escrito hasta la posicion inicial del cursor
	for ( size_t y = yF - yI; y < yF; y++ )
		if ( ! terminal_escribirTextoMedido( "\r\n", 2 ) )
			return -1;

	return 0;
};


#endif