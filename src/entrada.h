#ifndef entrada_h
#define entrada_h

#include <errno.h>
// errno
// #EAGAIN

#include <unistd.h>
// size_t
// #STDIN_FILENO
// read()

// claves para identificar entradas
typedef size_t entrada_clave;

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

#define ENTRADA_MODIFICADORES \
  ( \
    ( (entrada_clave) 0 ) \
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

#define entrada_sinModificadores( entrada ) \
  ( \
    (entrada) & ( ~ENTRADA_MODIFICADORES ) \
  )

// leer entradas de la terminal
entrada_clave entrada_leer() {

	int leidos;
	char buffer[ 6 ];

	while ( 1 ) {
		leidos = read( STDIN_FILENO, & buffer[ 0 ], 1 );
		if ( leidos == -1 && errno != EAGAIN )
			return ENTRADA_ERROR;
		if ( leidos == 0 ) return 0;
		if ( leidos == 1 ) break;
	};

	//printf ( "char leido %d '%c'\r\n", buffer[ 0 ], buffer[ 0 ] );

	if ( buffer[ 0 ] == 27 ) {

		// si no hay nada... era solo la tecla de escape
		if ( read( STDIN_FILENO, & buffer[ 1 ], 1 ) != 1 ) return 27;
		if ( read( STDIN_FILENO, & buffer[ 2 ], 1 ) != 1 ) return 27;

		// \e[
		if ( buffer[ 1 ] == '[' ) {

			if ( ! (
				( ( buffer[ 2 ] > 48 ) && ( buffer[ 2 ] < 55 ) && ( buffer[ 2 ] != 52 ) )
				||
				( ( buffer[ 2 ] > 64 ) && ( buffer[ 2 ] < 73 ) && ( buffer[ 2 ] != 69 ) && ( buffer[ 2 ] != 71 ) )
			) ) return 27;

			if ( buffer[ 2 ] == 49 ) {

				// 27 91 49 59 [50-53] [65-72,!=71]

				if ( read( STDIN_FILENO, & buffer[ 3 ], 1 ) != 1 ) return 27;
				if ( read( STDIN_FILENO, & buffer[ 4 ], 1 ) != 1 ) return 27;
				if ( read( STDIN_FILENO, & buffer[ 5 ], 1 ) != 1 ) return 27;

				if ( buffer[ 3 ] != 59 ) return 27;

				size_t salida = 0;

				switch( buffer[ 5 ] ) {
					case 65 : salida = ENTRADA_ARRIBA; break;
					case 66 : salida = ENTRADA_ABAJO; break;
					case 67 : salida = ENTRADA_DERECHA; break;
					case 68 : salida = ENTRADA_IZQUIERDA; break;
					case 70 : salida = ENTRADA_FIN; break;
					case 72 : salida = ENTRADA_INICIO; break;
					default : return 27;
				};

				switch( buffer[ 4 ] ) {
					case 50 : return salida + ENTRADA_SHIFT; break;
					case 51 : return salida + ENTRADA_ALT; break;
					case 52 : return salida + ENTRADA_ALT + ENTRADA_SHIFT; break;
					case 53 : return salida + ENTRADA_CONTROL; break;
					default : return 27;
				};

				return salida;
			}
			else {

				if ( ( buffer[ 2 ] > 49 ) && ( buffer[ 2 ] < 53 ) && ( buffer[ 2 ] != 52 ) ) {

					// 27 91 [50,54,!=52] xx / xx xx

					if ( read( STDIN_FILENO, & buffer[ 3 ], 1 ) != 1 ) return 27;

					if ( buffer[ 3 ] == 126 ) {

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

						// 27 91 [50,54,!=52] 59 [50,53] 126

						if ( read( STDIN_FILENO, & buffer[ 4 ], 1 ) != 1 ) return 27;
						if ( read( STDIN_FILENO, & buffer[ 5 ], 1 ) != 1 ) return 27;

						if ( buffer[ 5 ] != 126 ) return 27;

						size_t salida = 0;

						switch( buffer[ 2 ] ) {
							case 50 : return salida = ENTRADA_INSERTAR; break;
							case 51 : return salida = ENTRADA_SUPRIMIR; break;
							case 53 : return salida = ENTRADA_PAGINA_PREVIA; break;
							case 54 : return salida = ENTRADA_PAGINA_SIGUIENTE; break;
							default : return 27;
						};

						switch( buffer[ 4 ] ) {
							case 50 : return salida + ENTRADA_SHIFT; break;
							case 51 : return salida + ENTRADA_ALT; break;
							case 52 : return salida + ENTRADA_ALT + ENTRADA_SHIFT; break;
							case 53 : return salida + ENTRADA_CONTROL; break;
							default : return 27;
						};
					}
					else {
						return 27;
					};
				}
				else {
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