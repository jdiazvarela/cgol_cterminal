#ifndef terminal_h
#define terminal_h

#include <stdio.h>
// printf()
// sscanf()

#include <unistd.h>
// #STDIN_FILENO
// #STDOUT_FILENO
// size_t
// write()
// read()

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

	// al fondo..
	if ( write( STDOUT_FILENO, "\x1b[9999C\x1b[9999B", 14 ) != 14 ) return -1;

	// y a ver hasta donde llego
	return terminal_leerPosicion( pColumnas, pFilas );
};

// esto es para saltear lo que haga falta para tener la terminal entera disponible
// la idea es que en un emulador de terminal pueda volver a leer lo previo
// a la ejecucion de esssssssta cosssssaaa
int terminal_saltearPrevio() {

	size_t xI, xF, yI, yF;

	if ( terminal_leerPosicion( & xI, & yI ) < 0 ) return -1;

	if ( terminal_leerDimensiones( & xF, & yF ) < 0 ) return -1;

	for ( size_t y = yF - yI; y < yF; y++ )
		if ( write( STDOUT_FILENO, "\r\n", 2 ) != 2 )
			return -1;

	return 0;
};


// -- superficie

// esto es arbitrario mal... clave para transparencia
#define terminal_colorTransparente 8

// esta clave indica que solo se transfiere el color
// significa que al dibujar las superficies una capa superior
// no transfiere los simbolos.. solo los colores
#define terminal_signoTransferencia 3

// definicion
struct struct_terminalSuperficie {

	size_t nivel;

	size_t x0;
	size_t y0;
	size_t columnas;
	size_t filas;

	char * data;
	char * color;
};

typedef struct struct_terminalSuperficie terminalSuperficie;

// define todas las partes para evitar alertas de indefinido
// y cosas asi ademas pone algunos valores por defecto
// ... es para usar despues de declarar (la idea es que se
// pueda usar esto en un puntero o directo sobre un struct :P)
#define terminalSuperficie_construir( superf ) \
  { \
	superf.nivel = 100; \
	superf.x0 = 0; \
	superf.y0 = 0; \
	superf.columnas = 0; \
	superf.filas = 0; \
	superf.data = NULL; \
	superf.color = NULL; \
  }

#define terminalSuperficie_despejar( superf ) \
  { \
  	if ( superf.data != NULL ) { \
  		free( superf.data ); \
    	superf.data = NULL; \
	}; \
	if ( superf.color != NULL ) { \
		free( superf.color ); \
		superf.color = NULL; \
	}; \
  }

// esto es pa limpiar los buffferrsssss internos
int pTerminalSuperficie_limpiar( terminalSuperficie * pSuperf ) {

	#define superf ( * pSuperf )

	// simbolos
	for ( size_t i = 0; i < superf.columnas * superf.filas; i++ )
		superf.data[ i ] = ' ';

	// color
	for ( size_t i = 0; i < 2 * superf.columnas * superf.filas; i++ )
		superf.color[ i ] = terminal_colorTransparente;

	#undef superf

	return 0;
};

#define terminalSuperficie_limpiar( pSuperf ) pTerminalSuperficie_limpiar( & ( pSuperf ) )

// dimensionar buffffsssss intersssssnssnsnss
int pTerminalSuperficie_dimensionar( terminalSuperficie * pSuperf, const size_t columnas, const size_t filas ) {

	#define superf ( * pSuperf )

	if ( superf.columnas == columnas && superf.filas == filas ) return 0;

	// primero se arman los buffers

	char * color = malloc( 2 * columnas * filas );
	if ( color == NULL ) {
		printf ( "color == NULL\n" );
		return -1;
	};

	char * data = malloc( columnas * filas );
	if ( data == NULL ) {
		printf ( "data == NULL\n" );
		return -1;
	};

	// despues se pasa a cargar todo

	if ( superf.data != NULL ) free( superf.data );
	superf.data = data;

	if ( superf.color != NULL ) free( superf.color );
	superf.color = color;

	superf.columnas = columnas;
	superf.filas = filas;

	// ya que estamos limpiamos los buffsfsfsfsffsss

	terminalSuperficie_limpiar( superf );

	#undef superf

	return 0;
};

#define terminalSuperficie_dimensionar( pSuperf, columnas, filas ) pTerminalSuperficie_dimensionar( & ( pSuperf ), columnas, filas )

// se verifica si la coordenada XY esta contenida en la superficie dada
int pTerminalSuperficie_contieneXY( const terminalSuperficie * pSuperf, const size_t x, const size_t y ) {

	#define superf ( * pSuperf )

	// a la izquierda
	if ( x < superf.x0 ) return -1;
	// derecha
	if ( x >= superf.x0 + superf.columnas ) return -1;

	// arriba
	if ( y < superf.y0 ) return -1;
	// abajo
	if ( y >= superf.y0 + superf.filas ) return -1;

	// no queda otra... esta contenido
	return 0;

	#undef superf
};

#define terminalSuperficie_contieneXY( pSuperf, x, y ) pTerminalSuperficie_contieneXY( & ( pSuperf ), x, y )

// funcion rapida para escribir en la superficie dada (coordenadas internas)
int pTerminalSuperficie_escribir( terminalSuperficie * pSuperf, const size_t x, const size_t y, const char fuente, const char fondo, const char * texto, const size_t longitud ) {

	#define superf ( * pSuperf )

	// un mini control para no escribir fuera de buffer
	size_t limite;
	if ( x + longitud >= superf.columnas )
		limite = superf.columnas - x;
	else
		limite = longitud;

	for ( size_t i = 0; i < limite; i++ ) {
		superf.color[ ( y * superf.columnas + x + i ) * 2     ] = fuente;
		superf.color[ ( y * superf.columnas + x + i ) * 2 + 1 ] = fondo;
		superf.data[    y * superf.columnas + x + i           ] = texto[ i ];
	};

	return 0;

	#undef superf
};

#define terminalSuperficie_escribir( pSuperf, x, y, fuente, fondo, texto, longitud ) pTerminalSuperficie_escribir( & ( pSuperf ), x, y, fuente, fondo, texto, longitud )


// -- grafico


// -- linea horizontal

int pTerminalSuperficie_dibujarLineaHorizontal( terminalSuperficie * pSuperf, const size_t x, const size_t y, const size_t l, const char c ) {
	#define superf ( * pSuperf )

	// un mini control para no escribir fuera de buffer
	size_t limite;
	if ( x + l >= superf.columnas )
		limite = superf.columnas - x;
	else
		limite = l;

	for ( size_t i = 0; i < limite; i++ )
		superf.data[ y * superf.columnas + x + i ] = c;

	return 0;

	#undef superf
};

#define terminalSuperficie_dibujarLineaHorizontal( pSuperf, x, y, l, c ) pTerminalSuperficie_dibujarLineaHorizontal( & ( pSuperf ), x, y, l, c )

int pTerminalSuperficie_pintarLineaHorizontal( terminalSuperficie * pSuperf, const size_t x, const size_t y, const size_t l, const char f, const char b ) {
	#define superf ( * pSuperf )

	size_t limite;
	if ( x + l >= superf.columnas )
		limite = superf.columnas - x;
	else
		limite = l;

	for ( size_t i = 0; i < limite; i++ ) {
		superf.color[ ( y * superf.columnas + x + i ) * 2     ] = f;
		superf.color[ ( y * superf.columnas + x + i ) * 2 + 1 ] = b;
	};

	return 0;

	#undef superf
};

#define terminalSuperficie_pintarLineaHorizontal( pSuperf, x, y, l, f, b ) pTerminalSuperficie_pintarLineaHorizontal( & ( pSuperf ), x, y, l, f, b )

int pTerminalSuperficie_lineaHorizontal( terminalSuperficie * pSuperf, const size_t x, const size_t y, const size_t l, const char c, const char f, const char b ) {
	#define superf ( * pSuperf )

	terminalSuperficie_pintarLineaHorizontal( superf, x, y, l, f, b );
	terminalSuperficie_dibujarLineaHorizontal( superf, x, y, l, c );

	return 0;

	#undef superf
};

#define terminalSuperficie_lineaHorizontal( pSuperf, x, y, l, c, f, b ) pTerminalSuperficie_lineaHorizontal( & ( pSuperf ), x, y, l, c, f, b )


// -- linea vertical

int pTerminalSuperficie_dibujarLineaVertical( terminalSuperficie * pSuperf, const size_t x, const size_t y, const size_t l, const char c ) {
	#define superf ( * pSuperf )

	// un mini control para no escribir fuera de buffer
	size_t limite;
	if ( y + l >= superf.filas )
		limite = superf.filas - y;
	else
		limite = l;

	for ( size_t i = 0; i < limite; i++ )
		superf.data[ ( y + i ) * superf.columnas + x ] = c;

	return 0;

	#undef superf
};

#define terminalSuperficie_dibujarLineaVertical( pSuperf, x, y, l, c ) pTerminalSuperficie_dibujarLineaVertical( & ( pSuperf ), x, y, l, c )

int pTerminalSuperficie_pintarLineaVertical( terminalSuperficie * pSuperf, const size_t x, const size_t y, const size_t l, const char f, const char b ) {
	#define superf ( * pSuperf )

	// un mini control para no escribir fuera de buffer
	size_t limite;
	if ( y + l >= superf.filas )
		limite = superf.filas - y;
	else
		limite = l;

	for ( size_t i = 0; i < limite; i++ ) {
		superf.color[ ( ( y + i ) * superf.columnas + x ) * 2     ] = f;
		superf.color[ ( ( y + i ) * superf.columnas + x ) * 2 + 1 ] = b;
	};

	return 0;

	#undef superf
};

#define terminalSuperficie_pintarLineaVertical( pSuperf, x, y, l, f, b ) pTerminalSuperficie_pintarLineaVertical( & ( pSuperf ), x, y, l, f, b )

int pTerminalSuperficie_lineaVertical( terminalSuperficie * pSuperf, const size_t x, const size_t y, const size_t l, const char c, const char f, const char b ) {
	#define superf ( * pSuperf )

	terminalSuperficie_pintarLineaVertical( superf, x, y, l, f, b );
	terminalSuperficie_dibujarLineaVertical( superf, x, y, l, c );

	return 0;

	#undef superf
};

#define terminalSuperficie_lineaVertical( pSuperf, x, y, l, c, f, b ) pTerminalSuperficie_lineaVertical( & ( pSuperf ), x, y, l, c, f, b )


// -- buffer

// definicion
struct struct_terminalBuffer {

	size_t columnas;
	size_t filas;

	size_t superficies;
	terminalSuperficie * superficie;
};

typedef struct struct_terminalBuffer terminalBuffer;

#define terminalBuffer_despejar( tbuff ) \
  { \
    if ( tbuff.superficie != NULL ) \
      for ( size_t i = 0; i < tbuff.superficies; i++ ) \
        terminalSuperficie_despejar( tbuff.superficie[ i ] ); \
    free( tbuff.superficie ); \
    tbuff.superficie = NULL; \
    tbuff.superficies = 0; \
  }

// pa largar despues de declarar (la idea es usar esto con struct y punteros...)
#define terminalBuffer_construir( buffer ) \
  { \
  	buffer.columnas = 0; \
  	buffer.filas = 0; \
  	buffer.superficies = 0; \
  	buffer.superficie = NULL; \
  }

// obtener los datos que correspnden a las coordenadas XY
// se miran las superficies etc etc....
int pTerminalBuffer_datosEnXY( terminalBuffer * pTBuff, const size_t x, const size_t y, char * pF, char * pB, char * pC ) {

	#define tbuff ( * pTBuff )
	#define f ( * pF )
	#define b ( * pB )
	#define c ( * pC )

	size_t mayorNivel = 0;

	// se averigua el contenido a definir de acuerdo a la superficie (no transparente) de mayor nivel en el lugar indicado
	for ( size_t s = 0; s < tbuff.superficies; s++ )

		#define superf ( tbuff.superficie[ s ] )

		// el char objetivo corresponde a la superficie??????
		if ( ! ( terminalSuperficie_contieneXY( superf, x, y ) < 0 ) )

			// la superficie es de mayor nivel???
			if ( mayorNivel < superf.nivel ) {

				// se ubican las coordenadas de XY dentro de la superficie
				const size_t xi = x - superf.x0;
				const size_t yi = y - superf.y0;
				// indice dentro del arreglo de data de la superficie
				const size_t xyi = yi * superf.columnas + xi;

				const char sf = superf.color[ xyi * 2     ];
				const char sb = superf.color[ xyi * 2 + 1 ];

				// si no es transparente.. se anota el cambio de color
				if ( sf != terminal_colorTransparente ) f = sf;
				if ( sb != terminal_colorTransparente ) b = sb;

				// si no se indica explicitamente que solo se tranfieran los colores....
				if ( superf.data[ xyi ] != terminal_signoTransferencia )
					// si no es transparente.. ya se va registrando el char ... total...
					if ( sf != terminal_colorTransparente && sb != terminal_colorTransparente ) {
						c = superf.data[ xyi ];
						// y ya se anota a que nivel esta el contenido
						mayorNivel = superf.nivel;
					};
			};

		#undef superf

	if ( mayorNivel == 0 ) {
		f = 0;
		b = 0;
		c = ' ';
	};

	#undef c
	#undef b
	#undef f
	#undef tbuff

	return 0;
};

#define terminalBuffer_datosEnXY( pTBuff, x, y, pF, pB, pC ) pTerminalBuffer_datosEnXY( & ( pTBuff ), x, y, & ( pF ), & ( pB ), & ( pC ) )

// basicamente se plasmas nas superficies al buffer
int pTerminalBuffer_pintarSuperficies( terminalBuffer * pTBuff ) {

	#define tbuff ( * pTBuff )

	write( STDOUT_FILENO, "\x1b[H\x1b[J", 6 );
	// inicio
	//write( STDOUT_FILENO, "\x1b[H", 3 );
	// limpieza
	//write( STDOUT_FILENO, "\x1b[J", 3 );

	//                    1 2 34 5 67 8
	// chars para color  \e [ xx ; xx m

	// cada "pixel" ocupa   1 char para el signo visible y 8 para color
	
	// despues se agregan 8 chars mas para asegurar la ubicacion en la linea y 4 para limpiar color
	const size_t data_limite = tbuff.filas * ( tbuff.columnas * ( 1 + 8 ) ) + 8;

	char data[ data_limite ];

	for ( int y = 0; y < (int)tbuff.filas; y++ ) {

		// posicion al inicio de la fila (color limpio)
		size_t escrito = snprintf( data, data_limite, "\x1b[%d;1H\x1b[0m", y + 1 );

		// ultimo color
		char fP = 0;
		char bP = 0;

		// a trabajar la fila
		for ( int x = 0; x < (int)tbuff.columnas; x++ ) {

			char f = fP;
			char b = bP;
			char c = ' ';

			terminalBuffer_datosEnXY( tbuff, x, y, f, b, c );

			// si hay un cambio de color....
			if ( f != fP || b != bP ) {
				// se escribe
				if ( f == 0 || b == 0 )
					escrito += snprintf( data + escrito, data_limite, "\x1b[0m" );
				else
					escrito += snprintf( data + escrito, data_limite, "\x1b[%d;%dm", f, b );
				// y se toma registro del nuevo color
				fP = f;
				bP = b;
			};
			// a no olvidar el signo
			data[ escrito ] = c;
			escrito++;
		};

		// sale linea
		write( STDOUT_FILENO, data, escrito );
	};

	return 0;

	#undef tbuff
};

#define terminalBuffer_pintarSuperficies( pTBuffer ) pTerminalBuffer_pintarSuperficies( & ( pTBuffer ) )

#endif