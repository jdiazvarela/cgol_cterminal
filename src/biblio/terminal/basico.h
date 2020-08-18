#ifndef biblio_terminal_basico_h
#define biblio_terminal_basico_h


#pragma once
// -- INCLUSIONES

#include "directo.h"
// terminal_escribirTextoMedido()

#include "definicion.h"
// #terminalConfiguracion_modoRaw
// #terminalConfiguracion_cursorVisible
// terminalConfiguracion
//   size_t estado
//   struct termios original
//   struct termios raw
// terminalConfiguracion_iniciar()
// #terminal_colorTransparente
// #terminal_colorFuentePorDefecto
// #terminal_colorFondoPorDefecto
// #terminal_signoTransparente
// terminalSuperficie
//   size_t nivel
//   size_t x0
//   size_t y0
//   size_t columnas
//   size_t filas
//   char * data
// terminalBuffer
//   size_t columnas
//   size_t filas
//   char * meta
//   size_t superficies
//   terminalSuperficie * superficie

#include <stdlib.h>
// NULL
// size_t

#include <stdio.h>
// free()
// malloc()
// snprintf()

#include <unistd.h>
// #STDIN_FILENO

#include <termios.h>
// strcut termios
//   c_lflag  # local flags (miscelaneos)
//   c_iflag  # input
//   c_oflag  # output
//   c_cflag  # control
// tcsetattr()
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


// -- TERMINAL - CONFIGURACION

// modo crudo
int pTerminalConfiguracion_configurarModoRaw( terminalConfiguracion * pConfiguracion ) {
	#define configuracion ( * pConfiguracion )

	if ( terminalConfiguracion_iniciar( configuracion ) < 0 )
		return -1;

	configuracion.raw = ( configuracion ).original;

	configuracion.raw.c_iflag &= ~( BRKINT | ICRNL | ISTRIP | IXON );
	configuracion.raw.c_oflag &= ~( OPOST );
	configuracion.raw.c_cflag |= ( CS8 );
	configuracion.raw.c_lflag &= ~( ECHO | ICANON | IEXTEN | ISIG );
	configuracion.raw.c_cc[ VMIN  ] = 0;
	configuracion.raw.c_cc[ VTIME ] = 1;

	if ( tcsetattr( STDIN_FILENO, TCSAFLUSH, & ( configuracion.raw ) ) < 0 )
		return -1;

	configuracion.estado |= terminalConfiguracion_modoRaw;

	return 0;

	#undef configuracion
};

#define terminalConfiguracion_configurarModoRaw( pConfiguracion ) pTerminalConfiguracion_configurarModoRaw( & ( pConfiguracion ) )

// miscelanos
#define terminalConfiguracion_ocultarCursor( configuracion ) \
  { \
    terminal_escribirTextoMedido( "\x1b[?25l", 6 ); \
    configuracion.estado &= ~terminalConfiguracion_cursorVisible; \
  }

#define terminalConfiguracion_mostrarCursor( configuracion ) \
  { \
    terminal_escribirTextoMedido( "\x1b[?25h", 6 ); \
    configuracion.estado |= terminalConfiguracion_cursorVisible; \
  }


// -- TERMINAL - SUPERFICIE

// -- geters

// se verifica si la superficie dada contiene las coordenadas
#define terminalSuperficie_contieneX(     superf, x    ) ( ( (x) >= (superf).x0 ) && ( (x) < (superf).x0 + (superf).columnas ) )
#define terminalSuperficie_contieneY(     superf, y    ) ( ( (y) >= (superf).y0 ) && ( (y) < (superf).y0 + (superf).filas    ) )
#define terminalSuperficie_contieneXY(    superf, x, y ) ( terminalSuperficie_contieneX( superf, x ) && terminalSuperficie_contieneY( superf, y ) )
// data en las coordenadas dadasss
#define terminalSuperficie_signoXY(       superf, x, y ) (superf).data[ ( (y) * (superf).columnas + (x) ) * 3     ]
#define terminalSuperficie_colorFuenteXY( superf, x, y ) (superf).data[ ( (y) * (superf).columnas + (x) ) * 3 + 1 ]
#define terminalSuperficie_colorFondoXY(  superf, x, y ) (superf).data[ ( (y) * (superf).columnas + (x) ) * 3 + 2 ]

// dimensionar buffffsssss intersssssnssnsnss
int pTerminalSuperficie_dimensionar( terminalSuperficie * pSuperf, const size_t columnas, const size_t filas ) {
	#define superf ( * pSuperf )

	if ( superf.columnas == columnas && superf.filas == filas ) return 0;

	// primero laaaaa punteiirooossss

	char * data = malloc( 3 * columnas * filas );
	if ( data == NULL )
		return -1;

	// despues se pasa a cargar todo

	if ( superf.data != NULL )
		free( superf.data );

	superf.data = data;
	superf.columnas = columnas;
	superf.filas = filas;

	return 0;

	#undef superf
};

#define terminalSuperficie_dimensionar( pSuperf, columnas, filas ) pTerminalSuperficie_dimensionar( & ( pSuperf ), columnas, filas )


// -- TERMINAL - BUFFER

// daaimmennnshoooneeeeeeirrrrr
int pTerminalBuffer_dimensionar( terminalBuffer * pTBuff, const size_t columnas, const size_t filas ) {
	#define tbuff ( * pTBuff )

	// si no hay cambio... no se cambia nada... pero se da por cambiado xD
	if ( tbuff.columnas == columnas && tbuff.filas == filas )
		return 0;

	// si el cambio era solo en las columnas........
	if ( tbuff.filas == filas ) {
		// ta...
		tbuff.columnas = columnas;
		return 0;
	};

	// armando el nuevo buffffff de data
	char * meta = malloc( sizeof( char ) * filas );
	if ( meta == NULL )
		return -1;

	if ( tbuff.meta != NULL )
		free( tbuff.meta );

	tbuff.meta = meta;
	tbuff.columnas = columnas;
	tbuff.filas = filas;

	return 0;

	#undef tbuff
};

#define terminalBuffer_dimensionar( pTBuff, columnas, filas ) pTerminalBuffer_dimensionar( & ( pTBuff ), columnas, filas )

// gettersss???

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
		if ( terminalSuperficie_contieneXY( superf, x, y ) )

			// la superficie es de mayor nivel???
			if ( superf.nivel > mayorNivel ) {

				#define xi ( x - superf.x0 )
				#define yi ( y - superf.y0 )

				char sf = terminalSuperficie_colorFuenteXY( superf, xi, yi );
				char sb = terminalSuperficie_colorFondoXY(  superf, xi, yi );

				// si no es transparente.. es que hay un cambio y se anota el nuevo color
				if ( sf != terminal_colorTransparente ) f = sf;
				if ( sb != terminal_colorTransparente ) b = sb;

				// si hubo cambio de color ya se registra el nivel
				if ( sf != terminal_colorTransparente && sb != terminal_colorTransparente )
					// y ya se anota a que nivel esta el contenido
					mayorNivel = superf.nivel;

				// si no se indica explicitamente que solo se tranfieran los colores (basicamente ignorar el signo)....
				if ( terminalSuperficie_signoXY( superf, xi, yi ) != terminal_signoTransparente ) {
					c = terminalSuperficie_signoXY( superf, xi, yi );
					mayorNivel = superf.nivel;
				};

				#undef yi
				#undef xi
			};

		#undef superf

	if ( mayorNivel == 0 ) {
		c = ' ';
		f = 0;
		b = 0;
	};

	return 0;

	#undef c
	#undef b
	#undef f
	#undef tbuff
};

#define terminalBuffer_datosEnXY( pTBuff, x, y, f, b, c ) pTerminalBuffer_datosEnXY( & ( pTBuff ), x, y, & ( f ), & ( b ), & ( c ) )

// -- "renderizar" ... aunq en realidad seria rasterizado.... a googlear loco

int pTerminalBuffer_pintarFila( terminalBuffer * pTBuff, const size_t fila ) {
	#define tbuff ( * pTBuff )

	//                    1 2 34 5 67 8
	// chars para color  \e [ xx ; xx m

	// cada "pixel" ocupa   1 char para el signo visible y 8 para color
	
	// despues se agregan 8 chars mas para asegurar la ubicacion en la linea y 4 para limpiar color
	const size_t data_limite = tbuff.filas * ( tbuff.columnas * ( 1 + 8 ) ) + 8;

	char data[ data_limite ];

	// posicion al inicio de la fila (y color limpio)
	size_t escrito = snprintf( data, data_limite, "\x1b[%ld;1H\x1b[0m", fila + 1 );

	// ultimo color
	char fP = 0;
	char bP = 0;

	// a trabajar la fila
	for ( size_t x = 0; x < tbuff.columnas; x++ ) {

		char f = fP;
		char b = bP;
		char c = ' ';

		// se cargan los datos correspondientes a la coordenada
		terminalBuffer_datosEnXY( tbuff, x, fila, f, b, c );

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
	terminal_escribirTextoMedido( data, escrito );

	return 0;

	#undef tbuff
};

#define terminalBuffer_pintarFila( pTBuff, fila ) pTerminalBuffer_pintarFila( & ( pTBuff ), fila )

// basicamente se plasmas nas superficies al buffer
int pTerminalBuffer_pintarSuperficies( terminalBuffer * pTBuff ) {
	#define tbuff ( * pTBuff )

	// al inicio y limpieza total
	terminal_escribirTextoMedido( "\x1b[H\x1b[J", 6 );

	// se grafica linea a linea para "mayor eficiencia"
	for ( size_t fila = 0; fila < tbuff.filas; fila++ )
		// si hubo alguna modificacion en la linea
		//if ( tbuff.meta[ fila ] != 0 ) {
			// sale que vale
			terminalBuffer_pintarFila( tbuff, fila );
			//tbuff.meta[ fila ] = 0;
		//};

	return 0;

	#undef tbuff
};

#define terminalBuffer_pintarSuperficies( pTBuffer ) pTerminalBuffer_pintarSuperficies( & ( pTBuffer ) )


#endif