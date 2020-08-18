#ifndef biblio_entrada_h
#define biblio_entrada_h


#pragma once
// -- INCLUSIONES

#include <errno.h>
// errno
// #EAGAIN

#include <unistd.h>
// size_t
// #STDIN_FILENO
// read()


// -- DEFINICIONES

// claves para identificar entradas (definicion de tipo)
typedef size_t claveDeEntrada;


// -- CONSTANTES

// entradas con combinaciones de chars
#define ENTRADA_ARRIBA               260
#define ENTRADA_ABAJO                261
#define ENTRADA_IZQUIERDA            262
#define ENTRADA_DERECHA              263
#define ENTRADA_INICIO               264
#define ENTRADA_FIN                  265
#define ENTRADA_PAGINA_PREVIA        266
#define ENTRADA_PAGINA_SIGUIENTE     267
#define ENTRADA_INSERTAR             268
#define ENTRADA_SUPRIMIR             269

// estados agregados (combinacion con teclas "modificadoras")
#define ENTRADA_ERROR                511
#define ENTRADA_SOLTAR               512 // 10
#define ENTRADA_ALT                 1024 // 11
#define ENTRADA_ALT_GR              2048 // 12
#define ENTRADA_SHIFT               4096 // 13
#define ENTRADA_SHIFT_IZQUIERDO     8192 // 14
#define ENTRADA_SHIFT_DERECHO      16384 // 15
#define ENTRADA_CONTROL            32768 // 16
#define ENTRADA_CONTROL_IZQUIERDO  65536 // 17
#define ENTRADA_CONTROL_DERECHO   131072 // 18
#define ENTRADA_MOUSE_IZQUIERDO   262144 // 19
#define ENTRADA_MOUSE_DERECHO     524288 // 20

// todos los modificadores en una sola variable
#define ENTRADA_MODIFICADORES \
  ( \
    ( (claveDeEntrada) 0 ) \
    | ENTRADA_SOLTAR \
    | ENTRADA_ALT \
    | ENTRADA_ALT_GR \
    | ENTRADA_SHIFT \
    | ENTRADA_SHIFT_IZQUIERDO \
    | ENTRADA_SHIFT_DERECHO \
    | ENTRADA_CONTROL \
    | ENTRADA_CONTROL_IZQUIERDO \
    | ENTRADA_CONTROL_DERECHO \
  )


// -- FUNCIONES

// limpiar los modificadores de la entrada indicada
#define entrada_sinModificadores( entrada ) ( (entrada) & ( ~ENTRADA_MODIFICADORES ) )
// obtener los modificadores de la entrada indicada
#define entrada_soloModificadores( entrada ) ( (entrada) & ( ENTRADA_MODIFICADORES ) )

// leer entradas de la terminal
claveDeEntrada entrada_leer() {

	int leidos;
	char buffer[ 6 ] = { '\0', '\0', '\0', '\0', '\0', '\0' };

	while ( 1 ) {
		leidos = read( STDIN_FILENO, & buffer[ 0 ], 1 );
		if ( leidos == -1 && errno != EAGAIN )
			return ENTRADA_ERROR;
		if ( leidos == 0 ) return 0;
		if ( leidos == 1 ) break;
	};

	if ( buffer[ 0 ] == 27 ) {

		// 27 ?? ?? ?? / ?? ??

		// si no hay nada... era solo la tecla de escape
		if ( read( STDIN_FILENO, & buffer[ 1 ], 1 ) != 1 ) return 27;
		if ( read( STDIN_FILENO, & buffer[ 2 ], 1 ) != 1 ) return 27;

		// 27 xx xx ?? / ?? ??

		if ( buffer[ 1 ] == 91 ) {

			// 27 91 xx ?? / ?? ??

			if ( ! (
				( ( buffer[ 2 ] > 48 ) && ( buffer[ 2 ] < 55 ) && ( buffer[ 2 ] != 52 ) )
				||
				( ( buffer[ 2 ] > 64 ) && ( buffer[ 2 ] < 73 ) && ( buffer[ 2 ] != 69 ) && ( buffer[ 2 ] != 71 ) )
			) ) return 27;

			// 27 91 [49-54,!=52,65-72,!=69,!=71] ?? / ?? ??

			if ( buffer[ 2 ] == 49 ) {

				// se buscan solo combos de 6 chars
				// 27 91 49 ?? ?? ??

				if ( read( STDIN_FILENO, & buffer[ 3 ], 1 ) != 1 ) return 27;
				if ( read( STDIN_FILENO, & buffer[ 4 ], 1 ) != 1 ) return 27;
				if ( read( STDIN_FILENO, & buffer[ 5 ], 1 ) != 1 ) return 27;

				// 27 91 49 xx xx xx

				if ( buffer[ 3 ] != 59 ) return 27;

				// 27 91 49 59 xx xx

				size_t salida = 0;

				// se identifica la salida
				switch( buffer[ 5 ] ) {
					case 65 : salida = ENTRADA_ARRIBA; break;
					case 66 : salida = ENTRADA_ABAJO; break;
					case 67 : salida = ENTRADA_DERECHA; break;
					case 68 : salida = ENTRADA_IZQUIERDA; break;
					case 70 : salida = ENTRADA_FIN; break;
					case 72 : salida = ENTRADA_INICIO; break;
					default : return 27;
				};

				// 27 91 49 59 xx [65-72,!=69,!=71]

				// se identifica el modificador
				switch( buffer[ 4 ] ) {
					case 50 : return salida | ENTRADA_SHIFT; break;
					case 51 : return salida | ENTRADA_ALT; break;
					case 52 : return salida | ENTRADA_ALT + ENTRADA_SHIFT; break;
					case 53 : return salida | ENTRADA_CONTROL; break;
					default : return 27;
				};

				return salida;
			}
			else {

				// 27 91 [49-54,!=52,65-72,!=69,!=71] ?? / ?? ??

				if ( ( buffer[ 2 ] > 49 ) && ( buffer[ 2 ] < 55 ) && ( buffer[ 2 ] != 52 ) ) {

					// 27 91 [50,54,!=52] ?? / ?? ??

					if ( read( STDIN_FILENO, & buffer[ 3 ], 1 ) != 1 ) return 27;

					// 27 91 [50,54,!=52] xx / ?? ??

					if ( buffer[ 3 ] == 126 ) {

						// solo combos de 4 chars
						// 27 91 [50,54,!=52] 126

						switch( buffer[ 2 ] ) {
							case 50 : return ENTRADA_INSERTAR; break;
							case 51 : return ENTRADA_SUPRIMIR; break;
							case 53 : return ENTRADA_PAGINA_PREVIA; break;
							case 54 : return ENTRADA_PAGINA_SIGUIENTE; break;
							default : return 27;
						};
					}
					else if ( buffer[ 3 ] == 59 ) {

						// 27 91 [50,54,!=52] 59 ?? ??

						if ( read( STDIN_FILENO, & buffer[ 4 ], 1 ) != 1 ) return 27;
						if ( read( STDIN_FILENO, & buffer[ 5 ], 1 ) != 1 ) return 27;

						// 27 91 [50,54,!=52] 59 xx xx

						if ( buffer[ 5 ] != 126 ) return 27;

						// 27 91 [50,54,!=52] 59 xx 126

						size_t salida = 0;

						// se identifica la salida
						switch( buffer[ 2 ] ) {
							case 50 : return salida = ENTRADA_INSERTAR; break;
							case 51 : return salida = ENTRADA_SUPRIMIR; break;
							case 53 : return salida = ENTRADA_PAGINA_PREVIA; break;
							case 54 : return salida = ENTRADA_PAGINA_SIGUIENTE; break;
							default : return 27;
						};

						// 27 91 [50,54,!=52] 59 xx 126

						// y los modificadores
						switch( buffer[ 4 ] ) {
							case 50 : return salida | ENTRADA_SHIFT; break;
							case 51 : return salida | ENTRADA_ALT; break;
							case 52 : return salida | ENTRADA_ALT + ENTRADA_SHIFT; break;
							case 53 : return salida | ENTRADA_CONTROL; break;
							default : return 27;
						};
					}
					else {
						return 27;
					};
				}
				else {

					// combinaciones de 3 chars????

					// 27 91 [65-72,!=69,!=71]

					switch ( buffer[ 2 ] ) {
						case 65 : return ENTRADA_ARRIBA;
						case 66 : return ENTRADA_ABAJO;
						case 67 : return ENTRADA_DERECHA;
						case 68 : return ENTRADA_IZQUIERDA;
						case 70 : return ENTRADA_FIN;
						case 72 : return ENTRADA_INICIO;
						default : return 27;
					};
				};
			};
		};
		return 27;
	}
	else {
		return buffer[ 0 ];
	};
};


#endif