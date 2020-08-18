#ifndef biblio_terminal_grafico_h
#define biblio_terminal_grafico_h


// -- INCLUSIONES

#include <stdlib.h>
// size_t

#include "definicion.h"
// terminalSuperficie
// terminalBuffer

#include "basico.h"
// terminalSuperficie_contieneX()
// terminalSuperficie_contieneY()
// terminalSuperficie_signoXY()
// terminalSuperficie_colorFuenteXY()
// terminalSuperficie_colorFondoXY()


// -- LIMPIEZA

// limpiando una sola fila
int pTerminalSuperficie_limpiarFila( terminalSuperficie * pSuperf, const size_t fila ) {
	#define superf ( * pSuperf )

	for ( size_t x = 0; x < superf.columnas; x++ ) {
		// simbolos
		terminalSuperficie_signoXY(       superf, x, fila ) = terminal_signoTransparente;
		// color
		terminalSuperficie_colorFuenteXY( superf, x, fila ) = terminal_colorTransparente;
		terminalSuperficie_colorFondoXY(  superf, x, fila ) = terminal_colorTransparente;
	};

	return 0;

	#undef superf
};

#define terminalSuperficie_limpiarFila( pSuperf, fila ) pTerminalSuperficie_limpiarFila( & ( pSuperf ), fila )

// limpiando una columna (habria que tratar de usar el de fila pero bue... aca esta como alternativa)
int pTerminalSuperficie_limpiarColumna( terminalSuperficie * pSuperf, const size_t columna ) {
	#define superf ( * pSuperf )

	for ( size_t y = 0; y < superf.filas; y++ ) {
		// simbolos
		terminalSuperficie_signoXY(       superf, columna, y ) = terminal_signoTransparente;
		// color
		terminalSuperficie_colorFuenteXY( superf, columna, y ) = terminal_colorTransparente;
		terminalSuperficie_colorFondoXY(  superf, columna, y ) = terminal_colorTransparente;
	};

	return 0;

	#undef superf
};

#define terminalSuperficie_limpiarColumna( pSuperf, columna ) pTerminalSuperficie_limpiarColumna( & ( pSuperf ), columna )

// pa limpiar toooooda la superficie
int pTerminalSuperficie_limpiar( terminalSuperficie * pSuperf ) {
	#define superf ( * pSuperf )

	for ( size_t fila = 0; fila < superf.filas; fila++ )
		terminalSuperficie_limpiarFila( superf, fila );

	return 0;

	#undef superf
};

#define terminalSuperficie_limpiar( pSuperf ) pTerminalSuperficie_limpiar( & ( pSuperf ) )


// -- DIBUJO (directo sobre superficie)

// -- color

// horizontal

// esta funcion solo pinta... no cambia los simbolos
int pTerminalSuperficie_pintarHorizontal( terminalSuperficie * pSuperf, const size_t x0, const size_t y0, const char fuente, const char fondo, const size_t longitud ) {
	#define superf ( * pSuperf )

	size_t limite;
	if ( x0 + longitud >= superf.columnas )
		limite = superf.columnas - x0;
	else
		limite = longitud;

	for ( size_t i = 0; i < limite; i++ ) {
		terminalSuperficie_colorFuenteXY( superf, x0 + i, y0 ) = fuente;
		terminalSuperficie_colorFondoXY(  superf, x0 + i, y0 ) = fondo;
	};

	return 0;

	#undef superf
};

#define terminalSuperficie_pintarHorizontal( pSuperf, x0, y0, fuente, fondo, longitud ) pTerminalSuperficie_pintarHorizontal( & ( pSuperf ), x0, y0, fuente, fondo, longitud )

// vertical

// definir colores pero nada mas
int pTerminalSuperficie_pintarVertical( terminalSuperficie * pSuperf, const size_t x0, const size_t y0, const char fuente, const char fondo, const size_t longitud ) {
	#define superf ( * pSuperf )

	// un mini control para no escribir fuera de buffer
	size_t limite;
	if ( y0 + longitud >= superf.filas )
		limite = superf.filas - y0;
	else
		limite = longitud;

	for ( size_t i = 0; i < limite; i++ ) {
		terminalSuperficie_colorFuenteXY( superf, x0, y0 + i ) = fuente;
		terminalSuperficie_colorFondoXY(  superf, x0, y0 + i ) = fondo;
	};

	return 0;

	#undef superf
};

#define terminalSuperficie_pintarVertical( pSuperf, x0, y0, fuente, fondo, longitud ) pTerminalSuperficie_pintarVertical( & ( pSuperf ), x0, y0, fuente, fondo, longitud )


// -- "lineas"

// horizontal

// esta funcion solo traza los simbolos... no pinta
int pTerminalSuperficie_trazarHorizontal_signo( terminalSuperficie * pSuperf, const size_t x0, const size_t y0, const char signo, const size_t longitud ) {
	#define superf ( * pSuperf )

	// un mini control para no escribir fuera de buffer
	size_t limite;
	if ( x0 + longitud >= superf.columnas )
		limite = superf.columnas - x0;
	else
		limite = longitud;

	for ( size_t i = 0; i < limite; i++ )
		terminalSuperficie_signoXY( superf, x0 + i, y0 ) = signo;

	return 0;

	#undef superf
};

#define terminalSuperficie_trazarHorizontal_signo( pSuperf, x0, y0, signo, longitud ) pTerminalSuperficie_trazarHorizontal_signo( & ( pSuperf ), x0, y0, signo, longitud )

// esta funcion traza los simbolos y colorea la linea dada
int pTerminalSuperficie_dibujarHorizontal_signo( terminalSuperficie * pSuperf, const size_t x0, const size_t y0, const char fuente, const char fondo, const char signo, const size_t longitud ) {
	#define superf ( * pSuperf )

	size_t limite;
	if ( x0 + longitud >= superf.columnas )
		limite = superf.columnas - x0;
	else
		limite = longitud;

	for ( size_t i = 0; i < limite; i++ ) {
		terminalSuperficie_signoXY(       superf, x0 + i, y0 ) = signo;
		terminalSuperficie_colorFuenteXY( superf, x0 + i, y0 ) = fuente;
		terminalSuperficie_colorFondoXY(  superf, x0 + i, y0 ) = fondo;
	};

	return 0;

	#undef superf
};

#define terminalSuperficie_dibujarHorizontal_signo( pSuperf, x0, y0, fuente, fondo, signo, longitud ) pTerminalSuperficie_dibujarHorizontal_signo( & ( pSuperf ), x0, y0, fuente, fondo, signo, longitud )

// vertical

// trazar signos y nada mas
int pTerminalSuperficie_trazarVertical_signo( terminalSuperficie * pSuperf, const size_t x0, const size_t y0, const char signo, const size_t longitud ) {
	#define superf ( * pSuperf )

	// un mini control para no escribir fuera de buffer
	size_t limite;
	if ( y0 + longitud >= superf.filas )
		limite = superf.filas - y0;
	else
		limite = longitud;

	for ( size_t i = 0; i < limite; i++ )
		terminalSuperficie_signoXY( superf, x0, y0 + i ) = signo;

	return 0;

	#undef superf
};

#define terminalSuperficie_trazarVertical_signo( pSuperf, x0, y0, signo, longitud ) pTerminalSuperficie_trazarVertical_signo( & ( pSuperf ), x0, y0, signo, longitud )

// trazar y colorear la linea
int pTerminalSuperficie_dibujarVertical_signo( terminalSuperficie * pSuperf, const size_t x0, const size_t y0, const char fuente, const char fondo, const char signo, const size_t longitud ) {
	#define superf ( * pSuperf )

	// un mini control para no escribir fuera de buffer
	size_t limite;
	if ( y0 + longitud >= superf.filas )
		limite = superf.filas - y0;
	else
		limite = longitud;

	for ( size_t i = 0; i < limite; i++ ) {
		terminalSuperficie_signoXY(       superf, x0, y0 + i ) = signo;
		terminalSuperficie_colorFuenteXY( superf, x0, y0 + i ) = fuente;
		terminalSuperficie_colorFondoXY(  superf, x0, y0 + i ) = fondo;
	};

	return 0;

	#undef superf
};

#define terminalSuperficie_dibujarVertical_signo( pSuperf, x0, y0, fuente, fondo, signo, longitud ) pTerminalSuperficie_dibujarVertical_signo( & ( pSuperf ), x0, y0, fuente, fondo, signo, longitud )


// -- texto

// horizontal

int pTerminalSuperficie_trazarHorizontal_texto( terminalSuperficie * pSuperf, const size_t x0, const size_t y0, const char * texto, const size_t longitud ) {
	#define superf ( * pSuperf )

	// un mini control para no escribir fuera de buffer
	size_t limite;
	if ( x0 + longitud >= superf.columnas )
		limite = superf.columnas - x0;
	else
		limite = longitud;

	for ( size_t i = 0; i < limite; i++ )
		terminalSuperficie_signoXY( superf, x0 + i, y0 ) = texto[ i ];

	return 0;

	#undef superf
};

#define terminalSuperficie_trazarHorizontal_texto( pSuperf, x0, y0, texto, longitud ) pTerminalSuperficie_trazarHorizontal_texto( & ( pSuperf ), x0, y0, texto, longitud )

// funcion rapida para escribir en la superficie dada (coordenadas internas)
int pTerminalSuperficie_dibujarHorizontal_texto( terminalSuperficie * pSuperf, const size_t x0, const size_t y0, const char fuente, const char fondo, const char * texto, const size_t longitud ) {
	#define superf ( * pSuperf )

	// un mini control para no escribir fuera de buffer
	size_t limite;
	if ( x0 + longitud >= superf.columnas )
		limite = superf.columnas - x0;
	else
		limite = longitud;

	for ( size_t i = 0; i < limite; i++ ) {
		terminalSuperficie_signoXY(       superf, x0 + i, y0 ) = texto[ i ];
		terminalSuperficie_colorFuenteXY( superf, x0 + i, y0 ) = fuente;
		terminalSuperficie_colorFondoXY(  superf, x0 + i, y0 ) = fondo;
	};

	return 0;

	#undef superf
};

#define terminalSuperficie_dibujarHorizontal_texto( pSuperf, x0, y0, fuente, fondo, texto, longitud ) pTerminalSuperficie_dibujarHorizontal_texto( & ( pSuperf ), x0, y0, fuente, fondo, texto, longitud )

// vertical

// escribir texxto con orientacion vertical
int pTerminalSuperficie_trazarVertical_texto( terminalSuperficie * pSuperf, const size_t x0, const size_t y0, const char * texto, const size_t longitud ) {
	#define superf ( * pSuperf )

	// un mini control para no escribir fuera de buffer
	size_t limite;
	if ( y0 + longitud >= superf.filas )
		limite = superf.filas - y0;
	else
		limite = longitud;

	for ( size_t i = 0; i < limite; i++ )
		terminalSuperficie_signoXY( superf, x0, y0 + i ) = texto[ i ];

	return 0;

	#undef superf
};

#define terminalSuperficie_trazarVertical_texto( pSuperf, x0, y0, texto, longitud ) pTerminalSuperficie_trazarVertical_texto( & ( pSuperf ), x0, y0, texto, longitud )

// escribir texxto con orientacion vertical
int pTerminalSuperficie_dibujarVertical_texto( terminalSuperficie * pSuperf, const size_t x0, const size_t y0, const char fuente, const char fondo, const char * texto, const size_t longitud ) {
	#define superf ( * pSuperf )

	// un mini control para no escribir fuera de buffer
	size_t limite;
	if ( y0 + longitud >= superf.filas )
		limite = superf.filas - y0;
	else
		limite = longitud;

	for ( size_t i = 0; i < limite; i++ ) {
		terminalSuperficie_signoXY(       superf, x0, y0 + i ) = texto[ i ];
		terminalSuperficie_colorFuenteXY( superf, x0, y0 + i ) = fuente;
		terminalSuperficie_colorFondoXY(  superf, x0, y0 + i ) = fondo;
	};

	return 0;

	#undef superf
};

#define terminalSuperficie_dibujarVertical_texto( pSuperf, x0, y0, fuente, fondo, texto, longitud ) pTerminalSuperficie_dibujarVertical_texto( & ( pSuperf ), x0, y0, fuente, fondo, texto, longitud )


// -- DIBUJO (desde buffer)

// las coordenadas indicadas corresponden a la superficie... no son "globales"

// -- limpieza

int pTerminalBuffer_superficie_limpiarFila( terminalBuffer * pTBuff, const size_t indice, const size_t fila ) {
	#define tbuff ( * pTBuff )
	#define superf ( tbuff.superficie[ indice ] )

	// sep...
	if ( ! ( terminalSuperficie_contieneY( superf, fila + superf.y0 ) ) )
		return -1;

	if ( terminalSuperficie_limpiarFila( superf, fila ) != 0 )
		return -1;

	tbuff.meta[ fila + superf.y0 ] = 1;

	return 0;

	#undef superf
	#undef tbuff
};

#define terminalBuffer_superficie_limpiarFila( pTBuff, indice, fila ) pTerminalBuffer_superficie_limpiarFila( & ( pTBuff ), indice, fila )

int pTerminalBuffer_superficie_limpiarColumna( terminalBuffer * pTBuff, const size_t indice, const size_t columna ) {
	#define tbuff ( * pTBuff )
	#define superf ( tbuff.superficie[ indice ] )

	// sep...
	if ( ! ( terminalSuperficie_contieneX( superf, columna + superf.x0 ) ) )
		return -1;

	size_t limite;
	if ( superf.y0 + superf.columnas >= tbuff.filas )
		limite = tbuff.filas - superf.y0;
	else
		limite = superf.filas;

	for ( size_t i = 0; i < limite; i++ ) {

		terminalSuperficie_signoXY(       superf, columna, i ) = terminal_signoTransparente;
		terminalSuperficie_colorFuenteXY( superf, columna, i ) = terminal_colorTransparente;
		terminalSuperficie_colorFondoXY(  superf, columna, i ) = terminal_colorTransparente;

		tbuff.meta[ i + superf.y0 ] = 1;
	};

	return 0;

	#undef superf
	#undef tbuff
};

#define terminalBuffer_superficie_limpiarColumna( pTBuff, indice, columna ) pTerminalBuffer_superficie_limpiarColumna( & ( pTBuff ), indice, columna )

int pTerminalBuffer_superficie_limpiar( terminalBuffer * pTBuff, const size_t indice ) {
	#define tbuff ( * pTBuff )
	#define superf ( tbuff.superficie[ indice ] )

	for( size_t fila = 0; fila < superf.filas; fila++ )
		terminalBuffer_superficie_limpiarFila( tbuff, indice, fila );

	return 0;

	#undef superf
	#undef tbuff
};

#define terminalBuffer_superficie_limpiar( pTBuff, indice ) pTerminalBuffer_superficie_limpiar( & ( pTBuff ), indice )


// -- color

// horizontal

int pTerminalBuffer_superficie_pintarHorizontal( terminalBuffer * pTBuff, const size_t indice, const size_t x0, const size_t y0, const char fuente, const char fondo, const size_t longitud ) {
	#define tbuff ( * pTBuff )
	#define superf ( tbuff.superficie[ indice ] )

	// sep...
	if ( ! terminalSuperficie_contieneY( superf, y0 + superf.y0 ) )
		return -1;

	if ( terminalSuperficie_pintarHorizontal( superf, x0, y0, fuente, fondo, longitud ) != 0 )
		return -1;

	tbuff.meta[ y0 + superf.y0 ] = 1;

	return 0;

	#undef superf
	#undef tbuff
};

#define terminalBuffer_superficie_pintarHorizontal( pTBuff, indice, x0, y0, fuente, fondo ) pTerminalBuffer_superficie_pintarHorizontal( & ( pTBuff ), indice, x0, y0, fuente, fondo, longitud )

// vertical

int pTerminalBuffer_superficie_pintarVertical( terminalBuffer * pTBuff, const size_t indice, const size_t x0, const size_t y0, const char fuente, const char fondo, const size_t longitud ) {
	#define tbuff ( * pTBuff )
	#define superf ( tbuff.superficie[ indice ] )

	// sep...
	if ( ! terminalSuperficie_contieneX( superf, x0 + superf.x0 ) )
		return -1;

	// un mini control para no escribir fuera de buffer
	size_t limite;
	if ( y0 + longitud >= superf.filas )
		limite = superf.filas - y0;
	else
		limite = longitud;

	for ( size_t i = 0; i < limite; i++ ) {

		terminalSuperficie_colorFuenteXY( superf, x0, y0 + i ) = fuente;
		terminalSuperficie_colorFondoXY(  superf, x0, y0 + i ) = fondo;

		tbuff.meta[ y0 + i + superf.y0 ] = 1;
	};

	return 0;

	#undef superf
	#undef tbuff
};

#define terminalBuffer_superficie_pintarVertical( pTBuff, indice, x0, y0, fuente, fondo, longitud ) pTerminalBuffer_superficie_pintarVertical( & ( pTBuff ), indice, x0, y0, fuente, fondo, longitud )


// -- lineas

// horizontal
int pTerminalBuffer_superficie_trazarHorizontal_signo( terminalBuffer * pTBuff, const size_t indice, const size_t x0, const size_t y0, const char signo, const size_t longitud ) {
	#define tbuff ( * pTBuff )
	#define superf ( tbuff.superficie[ indice ] )

	// sep...
	if ( ! ( terminalSuperficie_contieneY( superf, y0 + superf.y0 ) ) )
		return -1;

	if ( terminalSuperficie_trazarHorizontal_signo( superf, x0, y0, signo, longitud ) != 0 )
		return -1;

	tbuff.meta[ y0 + superf.y0 ] = 1;

	return 0;

	#undef superf
	#undef tbuff
};

#define terminalBuffer_superficie_trazarHorizontal_signo( pTBuff, indice, x0, y0, signo, longitud ) pTerminalBuffer_superficie_trazarHorizontal_signo( & ( pTBuff ), indice, x0, y0, signo, longitud )

int pTerminalBuffer_superficie_dibujarHorizontal_signo( terminalBuffer * pTBuff, const size_t indice, const size_t x0, const size_t y0, const char fuente, const char fondo, const char signo, const size_t longitud ) {
	#define tbuff ( * pTBuff )
	#define superf ( tbuff.superficie[ indice ] )

	// sep...
	if ( ! ( terminalSuperficie_contieneY( superf, y0 + superf.y0 ) ) )
		return -1;

	if ( terminalSuperficie_dibujarHorizontal_signo( superf, x0, y0, fuente, fondo, signo, longitud ) != 0 )
		return -1;

	tbuff.meta[ y0 + superf.y0 ] = 1;

	return 0;

	#undef superf
	#undef tbuff
};

#define terminalBuffer_superficie_dibujarHorizontal_signo( pTBuff, indice, x0, y0, fuente, fondo, signo, longitud ) pTerminalBuffer_superficie_dibujarHorizontal_signo( & ( pTBuff ), indice, x0, y0, fuente, fondo, signo, longitud )

// vertical

int pTerminalBuffer_superficie_trazarVertical_signo( terminalBuffer * pTBuff, const size_t indice, const size_t x0, const size_t y0, const char signo, const size_t longitud ) {
	#define tbuff ( * pTBuff )
	#define superf ( tbuff.superficie[ indice ] )

	// sep...
	if ( ! ( terminalSuperficie_contieneX( superf, x0 + superf.x0 ) ) )
		return -1;

	// un mini control para no escribir fuera de buffer
	size_t limite;
	if ( y0 + longitud >= superf.filas )
		limite = superf.filas - y0;
	else
		limite = longitud;

	for ( size_t i = 0; i < limite; i++ ) {

		terminalSuperficie_signoXY( superf, x0, y0 + i ) = signo;

		tbuff.meta[ y0 + i + superf.y0 ] = 1;
	};

	return 0;

	#undef superf
	#undef tbuff
};

#define terminalBuffer_superficie_trazarVertical_signo( pTBuff, indice, x0, y0, signo, longitud ) pTerminalBuffer_superficie_trazarVertical_signo( & ( pTBuff ), indice, x0, y0, signo, longitud )

int pTerminalBuffer_superficie_dibujarVertical_signo( terminalBuffer * pTBuff, const size_t indice, const size_t x0, const size_t y0, const char fuente, const char fondo, const char signo, const size_t longitud ) {
	#define tbuff ( * pTBuff )
	#define superf ( tbuff.superficie[ indice ] )

	// sep...
	if ( ! terminalSuperficie_contieneX( superf, x0 + superf.x0 ) )
		return -1;

	// un mini control para no escribir fuera de buffer
	size_t limite;
	if ( y0 + longitud >= superf.filas )
		limite = superf.filas - y0;
	else
		limite = longitud;

	for ( size_t i = 0; i < limite; i++ ) {

		terminalSuperficie_signoXY(       superf, x0, y0 + i ) = signo;
		terminalSuperficie_colorFuenteXY( superf, x0, y0 + i ) = fuente;
		terminalSuperficie_colorFondoXY(  superf, x0, y0 + i ) = fondo;

		tbuff.meta[ y0 + i + superf.y0 ] = 1;
	};

	return 0;

	#undef superf
	#undef tbuff
};

#define terminalBuffer_superficie_dibujarVertical_signo( pTBuff, indice, x0, y0, fuente, fondo, signo, longitud ) pTerminalBuffer_superficie_dibujarVertical_signo( & ( pTBuff ), indice, x0, y0, fuente, fondo, signo, longitud )


// -- texto

// horizontal

int pTerminalBuffer_superficie_trazarHorizontal_texto( terminalBuffer * pTBuff, const size_t indice, const size_t x0, const size_t y0, const char * texto, const size_t longitud ) {
	#define tbuff ( * pTBuff )
	#define superf ( tbuff.superficie[ indice ] )

	// sep...
	if ( ! terminalSuperficie_contieneY( superf, y0 + superf.y0 ) )
		return -1;

	if ( terminalSuperficie_trazarHorizontal_texto( superf, x0, y0, texto, longitud ) != 0 )
		return -1;

	tbuff.meta[ y0 + superf.y0 ] = 1;

	return 0;

	#undef superf
	#undef tbuff
};

#define terminalBuffer_superficie_trazarHorizontal_texto( pTBuff, indice, x0, y0, texto, longitud ) pTerminalBuffer_superficie_trazarHorizontal_texto( & ( pTBuff ), indice, x0, y0, texto, longitud )

int pTerminalBuffer_superficie_dibujarHorizontal_texto( terminalBuffer * pTBuff, const size_t indice, const size_t x0, const size_t y0, const char fuente, const char fondo, const char * texto, const size_t longitud ) {
	#define tbuff ( * pTBuff )
	#define superf ( tbuff.superficie[ indice ] )

	// sep...
	if ( ! terminalSuperficie_contieneY( superf, y0 + superf.y0 ) )
		return -1;

	if ( terminalSuperficie_dibujarHorizontal_texto( superf, x0, y0, fuente, fondo, texto, longitud ) != 0 )
		return -1;

	tbuff.meta[ y0 + superf.y0 ] = 1;

	return 0;

	#undef superf
	#undef tbuff
};

#define terminalBuffer_superficie_dibujarHorizontal_texto( pTBuff, indice, x0, y0, fuente, fondo, texto, longitud ) pTerminalBuffer_superficie_dibujarHorizontal_texto( & ( pTBuff ), indice, x0, y0, fuente, fondo, texto, longitud )

// vertical

int pTerminalBuffer_superficie_trazarVertical_texto( terminalBuffer * pTBuff, const size_t indice, const size_t x0, const size_t y0, const char fuente, const char fondo, const char * texto, const size_t longitud ) {
	#define tbuff ( * pTBuff )
	#define superf ( tbuff.superficie[ indice ] )

	// sep...
	if ( ! terminalSuperficie_contieneX( superf, x0 + superf.x0 ) )
		return -1;

	// un mini control para no escribir fuera de buffer
	size_t limite;
	if ( y0 + longitud >= superf.filas )
		limite = superf.filas - y0;
	else
		limite = longitud;

	for ( size_t i = 0; i < limite; i++ ) {

		terminalSuperficie_signoXY( superf, x0, y0 + i ) = texto[ i ];

		tbuff.meta[ y0 + i + superf.y0 ] = 1;
	};

	return 0;

	#undef superf
	#undef tbuff
};

#define terminalBuffer_superficie_trazarVertical_texto( pTBuff, indice, x0, y0, fuente, fondo, texto, longitud ) pTerminalBuffer_superficie_trazarVertical_texto( & ( pTBuff ), indice, x0, y0, fuente, fondo, texto, longitud )

int pTerminalBuffer_superficie_dibujarVertical_texto( terminalBuffer * pTBuff, const size_t indice, const size_t x0, const size_t y0, const char fuente, const char fondo, const char * texto, const size_t longitud ) {
	#define tbuff ( * pTBuff )
	#define superf ( tbuff.superficie[ indice ] )

	// sep...
	if ( ! terminalSuperficie_contieneX( superf, x0 + superf.x0 ) )
		return -1;

	// un mini control para no escribir fuera de buffer
	size_t limite;
	if ( y0 + longitud >= superf.filas )
		limite = superf.filas - y0;
	else
		limite = longitud;

	for ( size_t i = 0; i < limite; i++ ) {

		terminalSuperficie_signoXY(       superf, x0, y0 + i ) = texto[ i ];
		terminalSuperficie_colorFuenteXY( superf, x0, y0 + i ) = fuente;
		terminalSuperficie_colorFondoXY(  superf, x0, y0 + i ) = fondo;

		tbuff.meta[ y0 + i + superf.y0 ] = 1;
	};

	return 0;

	#undef superf
	#undef tbuff
};

#define terminalBuffer_superficie_dibujarVertical_texto( pTBuff, indice, x0, y0, fuente, fondo, texto, longitud ) pTerminalBuffer_superficie_dibujarVertical_texto( & ( pTBuff ), indice, x0, y0, fuente, fondo, texto, longitud )


#endif