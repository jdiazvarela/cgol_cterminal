


// -- INCLUSIONES

#include <errno.h>
// errno
// #EAGAIN

#include <stdio.h>
// #NAME_MAX  limite de caracteres para el nombre de un directorio
// #stderr
// #NULL
// malloc()
// free()
// fprintf()
// snprintf()
// sscanf()

//#include <dlfcn.h>
// dlopen()
// dlsym()
// dlclose()

#include <stdlib.h>
// #EXIT_FAILURE
// rand()

#include <string.h>
// strlen()
// memcpy()
// strcmp()

#include <termios.h>
// strcut termios
//   c_lflag  # local flags (miscelaneos)
//   c_iflag  # input
//   c_oflag  # output
//   c_cflag  # control
// tcsetattr()
// tcgetattr()
// #TCSAFLUSH
// #ECHO
// #ICANON
// #ISIG
// #IEXTEN
// #IXON
// #BRKINT
// #ICRNL
// #ISTRIP
// #OPOST
// #CS8
// #VMIN
// #VTIME

#include <unistd.h>
// #STDIN_FILENO
// #STDOUT_FILENO
// size_t
// write()
// read()

#include <dirent.h>
// #DIR
// struct dirent
//   ino_t d_ino    - file serial number
//   char d_name[]  - name of entry (will not exceed a size of NAME_MAX)
// oendir()
// readdir()
// closedir()

#include <time.h>
// time()

#include "entrada.h"
#include "tiempo.h"
#include "terminal.h"


// -- DEFINICIONES


// -- FUNCION PRINCIPAL

int main( const int argumentos_cantidad, const char ** argumentos_lista ) {

	srand( time( NULL ) );

	struct termios terminal_configuracion_original;

	if ( ! ( tcgetattr( STDIN_FILENO, & terminal_configuracion_original ) < 0 ) ) {

		struct termios terminal_configuracion_raw = terminal_configuracion_original;

		terminal_configuracion_raw.c_iflag &= ~( BRKINT | ICRNL | ISTRIP | IXON );
		terminal_configuracion_raw.c_oflag &= ~( OPOST );
		terminal_configuracion_raw.c_cflag |= ( CS8 );
		terminal_configuracion_raw.c_lflag &= ~( ECHO | ICANON | IEXTEN | ISIG );
		terminal_configuracion_raw.c_cc[ VMIN ] = 0;
		terminal_configuracion_raw.c_cc[ VTIME ] = 1;

		if ( ! ( tcsetattr( STDIN_FILENO, TCSAFLUSH, & terminal_configuracion_raw ) < 0 ) )  {

			if ( terminal_saltearPrevio() < 0 ) {
				printf ( "al saltear pantalla inicial\n" );
				return 1;
			};

			terminalBuffer tbuff;
			terminalBuffer_construir( tbuff );

			tbuff.superficies = 2;
			tbuff.superficie = malloc( sizeof( terminalSuperficie ) * tbuff.superficies );
			if ( tbuff.superficie == NULL ) {
				printf ( "al crear superficie\n" );
				return 1;
			};

			for ( size_t s = 0; s < tbuff.superficies; s++ ) {
				terminalSuperficie_construir( tbuff.superficie[ s ] );
			};

			#define supGOL tbuff.superficie[ 0 ]
			#define supGUI tbuff.superficie[ 1 ]

			char * tableros = NULL;

			size_t i = 0;
			size_t continuar = 1;
			size_t paso = 0;

			const size_t columnas_minimo = 8;
			const size_t filas_minimo = 8;

			size_t editando = 0;

			supGOL.x0 = 1;
			supGOL.y0 = 4;

			supGUI.nivel = 9999;

			size_t cX = supGOL.x0;
			size_t cY = supGOL.y0;

			// esconder cursor de terminal
			write( STDOUT_FILENO, "\x1b[?25l", 6 );

			tiempo previo;
			tiempo_definir( previo, 0, 0 );
			tiempo delay;
			tiempo_definir( delay, 3, 300000000 );

			size_t vivos = 0;

			while ( 1 ) {

				// control de dimensiones...
				if ( tiempo_controlar( previo, delay ) ) {
					size_t columnas;
					size_t filas;
					if ( ! ( terminal_leerDimensiones( & columnas, & filas ) < 0 ) ) {
						if( columnas < columnas_minimo || filas < filas_minimo ) break;
						tiempo_igualar( previo, delay );
						if ( columnas != tbuff.columnas || filas != tbuff.filas ) {

							cX = supGOL.x0 + supGOL.columnas  / 2;
							cY = supGOL.y0 + supGOL.filas / 2;

							if ( terminalSuperficie_dimensionar( supGUI, columnas, filas ) < 0 ) {
								printf ( "terminalSuperficie_dimensionar < 0\n" );
								break;
							};

							if ( terminalSuperficie_dimensionar( supGOL, columnas - 2, filas - 5 ) < 0 ) {
								printf ( "terminalSuperficie_dimensionar < 0\n" );
								break;
							};

							if ( tableros != NULL ) free( tableros );
							tableros = malloc( 2 * supGOL.columnas * supGOL.filas );
							if ( tableros == NULL ) {
								printf ( "tableros == NULL\n" );
								break;
							};

							for ( size_t t = 0; t < 2 * supGOL.filas * supGOL.columnas; t++) tableros[ t ] = rand() % 2;

							tbuff.columnas = columnas;
							tbuff.filas = filas;

							paso = 1;
						};
					};
				};

				// lectura de entradassss

				entrada_clave entrada = entrada_leer();
				if ( entrada == ENTRADA_ERROR ) break;

				// ctrl + c  ; ctrl + c
				if ( entrada == 3 || entrada == 4 || entrada == 'q' ) break;

				// pausa/continuar
				if ( entrada == 'p' ) {
					continuar = 1 - continuar;
					if ( continuar > 0 ) editando = 0;
				};

				// tirate un paso
				if ( entrada == 'f' ) {
					paso = 1;
					editando = 0;
				};

				// 'r'   reset / random
				if ( entrada == 'r' ) {
					for ( size_t t = 0; t < 2 * supGOL.filas * supGOL.columnas; t++) tableros[ t ] = rand() % 2;
					paso = 1;
					editando = 0;
				};

				if ( entrada == 'w' || entrada == 'a' || entrada == 's' || entrada == 'd' || entrada == ENTRADA_IZQUIERDA || entrada == ENTRADA_DERECHA || entrada == ENTRADA_ARRIBA || entrada == ENTRADA_ABAJO ) {
					switch( entrada ) {
						case ENTRADA_IZQUIERDA : case 'a' : if ( cX != supGOL.x0 ) cX--; break;
						case ENTRADA_DERECHA : case 'd' : cX++; if ( cX >= supGOL.x0 + supGOL.columnas ) cX = supGOL.x0 + supGOL.columnas - 1; break;
						case ENTRADA_ARRIBA : case 'w' : if ( cY != supGOL.y0 ) cY--; break;
						case ENTRADA_ABAJO : case 's' : cY++; if ( cY >= supGOL.y0 + supGOL.filas ) cY = supGOL.y0 + supGOL.filas - 1; break;
					};
					editando = 1;
					paso = 0;
					continuar = 0;
				};

				// enter / espacio
				if ( entrada == 13 || entrada == ' ' || entrada == 'c' ) {
					editando = 1;
					paso = 0;
					continuar = 0;
				};

				// 'n'   clear ... despejar limpiar,,.. . etc...
				if ( entrada == 'n' ) {
					for ( size_t t = 0; t < 2 * supGOL.filas * supGOL.columnas; t++) tableros[ t ] = 0;
					paso = 1;
					editando = 1;
					continuar = 0;
				};

				// paso de simulacion
				if ( continuar || paso ) {
					vivos = 0;
					// simulacion (y pintura) de GOL
					for ( int y = 0; y < (int)supGOL.filas; y++ )
						for ( int x = 0; x < (int)supGOL.columnas; x++ ) {

							size_t vecinos = 0;
							for ( int vy = 0; vy < 3; vy++ )
								for ( int vx = 0; vx < 3; vx++ )
									vecinos += tableros[ i * supGOL.filas * supGOL.columnas + ( ( ( y + vy + supGOL.filas    - 1 ) % supGOL.filas    ) * supGOL.columnas ) + ( ( x + vx + supGOL.columnas - 1 ) % supGOL.columnas ) ];

							vecinos -= tableros[ ( i * supGOL.filas * supGOL.columnas ) + ( y * supGOL.columnas ) + x ];

							switch ( vecinos ) {
								case 2 : tableros[ ( ( 1 - i ) * supGOL.filas * supGOL.columnas ) + ( y * supGOL.columnas ) + x ] = ( tableros[ ( i * supGOL.filas * supGOL.columnas ) + ( y * supGOL.columnas ) + x ] > 0 ) ? 1 : 0;
								break;
								case 3 : tableros[ ( ( 1 - i ) * supGOL.filas * supGOL.columnas ) + ( y * supGOL.columnas ) + x ] = 1;
								break;
								default : tableros[ ( ( 1 - i ) * supGOL.filas * supGOL.columnas ) + ( y * supGOL.columnas ) + x ] = 0;
							};

							vivos += tableros[ i * supGOL.filas * supGOL.columnas + ( y * supGOL.columnas ) + x ];
						};
				};

				// actualizacion de GUIIIIIIIIIIIIIIIIII
				terminalSuperficie_limpiar( supGUI );

				if ( editando ) {
					const char f = 0;
					const char b = 0;

					// superior
					terminalSuperficie_lineaHorizontal( supGUI, supGOL.x0, supGOL.y0 - 1, supGOL.columnas, '-', 0, 43 );
					// inferior
					terminalSuperficie_lineaHorizontal( supGUI, supGOL.x0, supGOL.y0 + supGOL.filas, supGOL.columnas, '-', 0, 43 );

					// izquierda
					terminalSuperficie_lineaVertical( supGUI, supGOL.x0 - 1, supGOL.y0, supGOL.filas, '|', 0, 43 );
					// derecha
					terminalSuperficie_lineaVertical( supGUI, supGOL.x0 + supGOL.columnas, supGOL.y0, supGOL.filas , '|', 0, 43 );

					//terminalSuperficie_pintarLineaHorizontal( supGUI, supGOL.x0, cY, supGOL.columnas, 35, 43 );
					terminalSuperficie_lineaHorizontal( supGUI, supGOL.x0, cY, supGOL.columnas, terminal_signoTransferencia, 93, 100 );
					terminalSuperficie_lineaVertical( supGUI, cX, supGOL.y0, supGOL.filas, terminal_signoTransferencia, 93, 100 );


					terminalSuperficie_escribir( supGUI, cX, supGOL.y0 - 1, f, b, "v", 1 );
					terminalSuperficie_escribir( supGUI, cX, supGOL.y0 + supGOL.filas, f, b, "^", 1 );
					terminalSuperficie_escribir( supGUI, supGOL.x0 - 1, cY, f, b, ">", 1 );
					terminalSuperficie_escribir( supGUI, supGOL.x0 + supGOL.columnas, cY, f, b, "<", 1 );

					if ( entrada == 13 || entrada == ' ' || entrada == 'c' ) {
						size_t x = cX - supGOL.x0;
						size_t y = cY - supGOL.y0;

						vivos += ( tableros[ i * supGOL.filas * supGOL.columnas + ( y * supGOL.columnas ) + x ] ) ? -1 : 1;
						tableros[ i * supGOL.filas * supGOL.columnas + ( y * supGOL.columnas ) + x ] = 1 - tableros[ i * supGOL.filas * supGOL.columnas + ( y * supGOL.columnas ) + x ];

						terminalSuperficie_escribir( supGOL, x, y, 36, 40, ( tableros[ i * supGOL.filas * supGOL.columnas + ( y * supGOL.columnas ) + x ] ) ? "o" : " ", 1 );
					};
				};

				size_t temporal_limite = 128;
				char temporal[ temporal_limite ];
				size_t l;

				// data
				l = snprintf( temporal, temporal_limite, "[q]: salir; [r]: reinicio; [f]: paso; [n]: despejar [p]: pausar/continuar;" );
				terminalSuperficie_escribir( supGUI, 0, 0, 0, 0, temporal, l );
				l = snprintf( temporal, temporal_limite, "[flechas/wasd]: mover; [enter/barra/c]: cambiar;" );
				terminalSuperficie_escribir( supGUI, 0, 1, 0, 0, temporal, l );
				l = snprintf( temporal, temporal_limite, "dims: %ld x %ld; cursor: %ld,%ld; vivos: %ld;", supGOL.columnas, supGOL.filas, cX, cY, vivos );
				terminalSuperficie_escribir( supGUI, 0, 2, 0, 0, temporal, l );

				terminalBuffer_pintarSuperficies( tbuff );

				// pintar tablero
				for ( int y = 0; y < (int)supGOL.filas; y++ )
					for ( int x = 0; x < (int)supGOL.columnas; x++ )
						terminalSuperficie_escribir( supGOL, x, y, 36, 40, ( tableros[ i * supGOL.filas * supGOL.columnas + ( y * supGOL.columnas ) + x ] ) ? "o" : " ", 1 );


				if ( continuar || paso ) {
					// si era solo un paso se descontinua
					if ( paso ) continuar = 0;
					// siempre se detiene despues de un paso
					paso = 0;
					// cambio de tablero
					i = 1 - i;
				};
			};

			#undef supGOL_i
			#undef supGOL

			// mostrando el cursor
			write( STDOUT_FILENO, "\x1b[?25h", 6 );
			write( STDOUT_FILENO, "\r\n", 2 );
			write( STDOUT_FILENO, "\r\n", 2 );
		};

		if ( tcsetattr( STDIN_FILENO, TCSAFLUSH, & terminal_configuracion_original ) == -1 ) {
			printf( "\r\n%s\n", ">[ Error ]: tcsetattr; al reestablecer configuracion original" );
		};
	}
	else {
		printf( "%s\n", ">[ Error ]: tcgetattr; al obtener configuracion actual" );
	};

	return 0;
};


// -- FIN
