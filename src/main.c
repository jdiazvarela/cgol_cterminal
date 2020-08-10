


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



// -- DEFINICIONES


// --- entradas

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


// -- tiempo

// actualizar al tiempo actual
#define tiempo_actualizar(tiempo) clock_gettime( CLOCK_REALTIME, &(tiempo) )

// segundos
#define tiempo_segundos( tiempo ) tiempo.tv_sec

// nano segundos
#define tiempo_nanosegundos( tiempo ) tiempo.tv_nsec

// macro sencilla para simplificar codigo
// define de manera simple los segundos y nanosegundos de un tiempo dado
#define tiempo_definir( tiempo, segundos, nanosegundos ) \
  { \
    tiempo_segundos( tiempo ) = segundos; \
    tiempo_nanosegundos( tiempo ) = nanosegundos; \
  } \

// si tengo una variable de tiempo y quiero cargarla a otra
#define tiempo_igualar( receptor, objetivo ) tiempo_definir( receptor, tiempo_segundos( objetivo ), tiempo_nanosegundos( objetivo ) )

// a la variable de tiempo le resto otra
#define tiempo_restar( receptor, objetivo ) \
  { \
    tiempo_segundos( receptor ) -= tiempo_segundos( objetivo ); \
    tiempo_nanosegundos( receptor ) -= tiempo_nanosegundos( objetivo ); \
    if( tiempo_nanosegundos( receptor ) < 0 ) { \
      tiempo_nanosegundos( receptor ) += 1000000000; \
      tiempo_segundos( receptor )--; \
    }; \
  } \

// pregunta rapida para saber si una variable de tiempo es menor a otra
// en otras palabras si un tiempo es anterior a otro
#define tiempo_esMenor( primero, segundo ) \
  ( \
    tiempo_segundos( primero ) < tiempo_segundos( segundo ) \
    ? \
    1 \
    : \
    ( \
      tiempo_segundos( primero ) == tiempo_segundos( segundo ) \
      ? \
      tiempo_nanosegundos( primero ) < tiempo_nanosegundos( segundo ) \
      : \
      0 \
    ) \
  ) \


// --- terminal

// posicionar el cursor en coordenadas [X,Y] (desde [0,0])
// ret: -1 error, 0 ok
int terminal_posicionar( const size_t x, const size_t y ) {

	printf( "\x1b[%ld;%ldH", y + 1, x + 1 );

	return 0;
};

// obtener la posicion actual del cursor de la terminal
int terminal_leerPosicion( size_t * pColumnas, size_t * pFilas ) {

	// se hace la peticion de la posicion del cursor (temas de claves ANSI)
	if ( write( STDOUT_FILENO, "\x1b[6n", 4 ) != 4 ) {
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

	if ( write( STDOUT_FILENO, "\x1b[9999C\x1b[9999B", 14 ) != 14 ) return -1;

	return terminal_leerPosicion( pColumnas, pFilas );
};


// -- FUNCION PRINCIPAL

int main( const int argumentos_cantidad, const char ** argumentos_lista ) {

	srand(time(NULL));

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

			size_t terminal_columnas = 0;
			size_t terminal_filas = 0;

			size_t x0;
			size_t y0;

			if ( terminal_leerPosicion( & x0, & y0 ) < 0 ) {
				printf ( "AL LEER DIMS pa saltear huevadas\n" );
				return 1;
			};

			if ( terminal_leerDimensiones( & terminal_columnas, & terminal_filas ) < 0 ) {
				printf ( "AL LEER DIMS\n" );
				return -1;
			};

			for ( size_t y = terminal_filas - y0; y < terminal_filas; y++ )
				write( STDOUT_FILENO, "\r\n", 2 );

			terminal_columnas = 0;
			terminal_filas = 0;

			char * tableros = NULL;
			char * colores = NULL;
			char * salida = NULL;

			y0 = 3;
			x0 = 1;

			size_t xm = 1;
			size_t ym = 1;

			size_t i = 0;
			size_t escrito = 0;
			size_t continuar = 0;
			size_t paso = 1;

			// esconder cursor de terminal
			write( STDOUT_FILENO, "\x1b[?25l", 6 );

			struct timespec terminal_control_ultimo;
			tiempo_definir( terminal_control_ultimo, 0, 0 );

			struct timespec terminal_contol_delay;
			tiempo_definir( terminal_contol_delay, 0, 400000000 );

			while ( 1 ) {

				struct timespec terminal_contol;
				tiempo_actualizar( terminal_contol );
				struct timespec terminal_contol_diferencia;
				tiempo_igualar( terminal_contol_diferencia, terminal_contol );
				tiempo_restar( terminal_contol_diferencia, terminal_contol_delay );
				if ( tiempo_esMenor( terminal_control_ultimo, terminal_contol_diferencia ) ) {

					size_t columnas;
					size_t filas;
					if ( ! ( terminal_leerDimensiones( & columnas, & filas ) < 0 ) ) {
						tiempo_igualar( terminal_control_ultimo, terminal_contol );
						if ( columnas - x0 - xm != terminal_columnas || filas - y0 - ym != terminal_filas ) {

							terminal_columnas = columnas - x0 - xm;
							terminal_filas = filas - y0 - ym;

							if ( colores != NULL ) free( colores );
							colores = malloc( 2 * terminal_columnas * terminal_filas );
							if ( colores == NULL ) {
								printf ( "colores == NULL\n" );
								break;
							};
							if ( tableros != NULL ) free( tableros );
							tableros = malloc( 2 * terminal_filas * terminal_columnas );
							if ( tableros == NULL ) {
								printf ( "tableros == NULL\n" );
								break;
							};

							for ( size_t t = 0; t < 2; t++)
								for ( size_t y = 0; y < terminal_filas; y++ )
									for ( size_t x = 0; x < terminal_columnas; x++ ) 
										tableros[ t * terminal_filas * terminal_columnas + y * terminal_columnas + x ] = rand() % 2;

							if ( salida != NULL ) free( salida );
							salida = malloc( terminal_filas * terminal_columnas * 13 + terminal_filas * 4 + 8 + 8 );
							if ( salida == NULL ) {
								printf ( "salida == NULL\n" );
								break;
							};
						};
					};
				};

				entrada_clave entrada = entrada_leer();
				if ( entrada == ENTRADA_ERROR ) break;
				if ( entrada == 3 || entrada == 4 || entrada == 113 ) break;
				if ( entrada == 112 ) continuar = 1 - continuar;
				if ( entrada == 102 ) paso = 1;
				if ( entrada == 114 ) {
					paso = 1;
					for ( size_t t = 0; t < 2; t++)
						for ( size_t y = 0; y < terminal_filas; y++ )
							for ( size_t x = 0; x < terminal_columnas; x++ )
								tableros[ t * terminal_filas * terminal_columnas + y * terminal_columnas + x ] = rand() % 2;
				};

				if ( continuar || paso ) {

					paso = 0;
					size_t vivos = 0;

					size_t tempoal_limite = 128;
					char temporal[ tempoal_limite ];
					size_t l;
					escrito = 0;

					for ( int y = 0; y < (int)terminal_filas; y++ ) {

						l = snprintf( temporal, tempoal_limite, "\x1b[%ld;%ldH", y0 + y + 1, x0 + 1 );
						memcpy( salida + escrito, temporal, l );

						escrito += l;

						for ( int x = 0; x < (int)terminal_columnas; x++ ) {

							size_t vecinos = 0;
							int dy = 3;
							int dx = 3;
							for ( int vy = 0; vy < dy; vy++ )
								for ( int vx = 0; vx < dx; vx++ )
									vecinos += tableros[
										i * terminal_filas * terminal_columnas
										+
										( ( ( y + vy + terminal_filas    - (dy / 2) ) % terminal_filas    ) * terminal_columnas )
										+
										  ( ( x + vx + terminal_columnas - (dx / 2) ) % terminal_columnas )
									];

							vecinos -= tableros[ ( i * terminal_filas * terminal_columnas ) + ( y * terminal_columnas ) + x ];

							if ( tableros[ ( i * terminal_filas * terminal_columnas ) + ( y * terminal_columnas ) + x ] > 0 ) {
								switch ( vecinos ) {
									case 2 :
									case 3 :
										tableros[ ( ( 1 - i ) * terminal_filas * terminal_columnas ) + ( y * terminal_columnas ) + x ] = 1;
									break;
									default :
										tableros[ ( ( 1 - i ) * terminal_filas * terminal_columnas ) + ( y * terminal_columnas ) + x ] = 0;
								};
							}
							else {
								switch ( vecinos ) {
									case 3 :
										tableros[ ( ( 1 - i ) * terminal_filas * terminal_columnas ) + ( y * terminal_columnas ) + x ] = 1;
									break;
									default :
										tableros[ ( ( 1 - i ) * terminal_filas * terminal_columnas ) + ( y * terminal_columnas ) + x ] = 0;
								};
							};
							vivos += tableros[ i * terminal_filas * terminal_columnas + ( y * terminal_columnas ) + x ];

							colores[ y * terminal_columnas * 2 + x * 2     ] = ( tableros[ i * terminal_filas * terminal_columnas + ( y * terminal_columnas ) + x ] ) ? 30 : 30;
							colores[ y * terminal_columnas * 2 + x * 2 + 1 ] = ( tableros[ i * terminal_filas * terminal_columnas + ( y * terminal_columnas ) + x ] ) ? 43 : 43;

							l = snprintf(
								temporal, tempoal_limite, "\x1b[%d;%dm",
								(int)(colores[ y * terminal_columnas * 2 + x * 2     ]),
								(int)(colores[ y * terminal_columnas * 2 + x * 2 + 1 ])
							);

							if ( l > 0 ) {
								memcpy( salida + escrito, temporal, l );
								escrito += l;
							};
							
							salida[ escrito ] = ( tableros[ i * terminal_filas * terminal_columnas + ( y * terminal_columnas ) + x ] ) ? ' ' : 'X';
							escrito += 1;
						};

						memcpy( salida + escrito, "\x1b[0m", 4 );
						escrito += 4;
					};

					// inicio
					write( STDOUT_FILENO, "\x1b[H", 3 );
					// limpieza
					write( STDOUT_FILENO, "\x1b[J", 3 );
					// lavado de color
					write( STDOUT_FILENO, "\x1b[0m", 4 );
					write( STDOUT_FILENO, "\x1b[0;0m", 6 );

					// data
					l = snprintf( temporal, tempoal_limite, "\x1b[1;1H[q]: salir; [r]: reset; [p]: pausar; [f]: paso;" );
					write( STDOUT_FILENO, temporal, l );
					l = snprintf( temporal, tempoal_limite, "\x1b[2;1Hdims: %ld x %ld; minimo: %ld; vivos: %ld", terminal_columnas, terminal_filas, terminal_filas * terminal_columnas / 14, vivos );
					write( STDOUT_FILENO, temporal, l );

					// al inicio de la "ventana"
					//l = snprintf( temporal, 24, "\x1b[%ld;H", terminal_filas - ( y0 + 1 ) );
					l = snprintf( temporal, tempoal_limite, "\x1b[%ld;H", y0 );
					write( STDOUT_FILENO, temporal, l );

					// tablero
					write( STDOUT_FILENO, salida, escrito );

					i = 1 - i;
					if ( vivos * 14 < terminal_filas * terminal_columnas )
						for ( size_t t = 0; t < 2; t++)
							for ( size_t y = 0; y < terminal_filas; y++ )
								for ( size_t x = 0; x < terminal_columnas; x++ )
									tableros[ t * terminal_filas * terminal_columnas + y * terminal_columnas + x ] = rand() % 2;
				};
			};

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
