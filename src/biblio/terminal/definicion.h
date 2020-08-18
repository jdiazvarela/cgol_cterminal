#ifndef biblio_terminal_definicion_h
#define biblio_terminal_definicion_h


#pragma once
// -- INCLUSIONES

#include "directo.h"
// terminal_escribirTextoMedido

#include <stdlib.h>
// size_t
// NULL
// free()

#include <unistd.h>
// #STDIN_FILENO

#include <termios.h>
// strcut termios
//   c_lflag  # local flags (miscelaneos)
//   c_iflag  # input
//   c_oflag  # output
//   c_cflag  # control
// tcgetattr()
// tcsetattr()


// -- CONSTANTES

// -- configuraciones

// banderassssss de la configuracion
#define terminalConfiguracion_iniciado        256 // 9
#define terminalConfiguracion_modoRaw         512 // 10
#define terminalConfiguracion_cursorVisible  1024 // 11

// -- claves

// esto es arbitrario mal... clave para transparencia
#define terminal_colorTransparente 8

// colores por defecto (dependen del entorno... en realidad varian mas
// en caso de estar en un emulador de terminal
#define terminal_colorFuentePorDefecto 39
#define terminal_colorFondoPorDefecto 49

// esta clave indica que solo se transfiere el color
// ...significa que al dibujar las superficies una capa superior
// no transfiere los simbolos.. solo los colores
// (otra vez arbitrariedad pura)
#define terminal_signoTransparente 8


// -- TERMINAL - CONFIGURACION

// definicion
struct struct_terminalConfiguracion {

	// control de estados de la terminal
	size_t estado;

	// configuraciones...
	struct termios original;
	struct termios raw;
};

// definicion de tipo
typedef struct struct_terminalConfiguracion terminalConfiguracion;

// si se mira el proceso de definicion (completa) de variables como:
//    declarar -> reservar (solo para punteros) -> construir -> iniciar
// y la "destruccion" como:
//    terminar -> destruir (solo si contiene punteros) -> liberar (solo para punteros)
// ... si se lo piensa.. destruir no queda del todo bien.... digo nomas...
// aca tarian las funciones

// constructor
#define terminalConfiguracion_construir( configuracion ) \
  { \
    ( configuracion ).estado = 0; \
  }

// seguimos con el "constructor"
int pTerminalConfiguracion_iniciar( terminalConfiguracion * pConfiguracion ) {
	#define configuracion ( * pConfiguracion )

	// el "por si las dudas"
	if ( configuracion.estado & terminalConfiguracion_iniciado )
		return 0;

	// obtener configuracion original
	if ( tcgetattr( STDIN_FILENO, & ( configuracion ).original ) < 0 )
		return -1;

	// se indica que esta iniciado...
	configuracion.estado |= terminalConfiguracion_iniciado;

	// se asegura el cursor visible
	if ( terminal_escribirTextoMedido( "\x1b[?25h", 6 ) )
		configuracion.estado |= terminalConfiguracion_cursorVisible;

	return 0;

	#undef configuracion
};

#define terminalConfiguracion_iniciar( pConfiguracion ) pTerminalConfiguracion_iniciar( & ( pConfiguracion ) )

// sale "destructor"
#define terminalConfiguracion_terminar( configuracion ) \
  { \
    if ( (configuracion).estado & terminalConfiguracion_iniciado ) \
      tcsetattr( STDIN_FILENO, TCSAFLUSH, & (configuracion).original ); \
    if ( ! ( (configuracion).estado & terminalConfiguracion_cursorVisible ) ) { \
      terminal_escribirTextoMedido( "\x1b[?25h", 6 ); \
      configuracion.estado &= ~terminalConfiguracion_cursorVisible; \
    }; \
    terminal_escribirTextoMedido( "\x1b[9999C\x1b[9999B", 14 ); \
    terminal_escribirTextoMedido( "\r\n", 2 ); \
    terminal_escribirTextoMedido( "\r\n", 2 ); \
  }


// -- TERMINAL - SUPERFICIE

// definicionnnnn
struct struct_terminalSuperficie {

	size_t nivel;

	size_t x0;
	size_t y0;
	size_t columnas;
	size_t filas;

	char * data;
};

// definicion de tipo
typedef struct struct_terminalSuperficie terminalSuperficie;

// constructor / destructor

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
  }

// primero y principal... esto libera el contenido de la superficie y
// no la superficie en si. .. entonces se podria decir que NO esta hecho
// para utilizar sobre punteros
#define terminalSuperficie_destruir( superf ) \
  { \
    if ( (superf).data != NULL ) { \
      free( (superf).data ); \
      (superf).data = NULL; \
    }; \
  }


// -- TERMINAL - BUFFER

// definicion
struct struct_terminalBuffer {

	size_t columnas;
	size_t filas;

	// metadata... por ahora de cada fila
	char * meta;

	// superficiesssss asociadassssss a la terminal
	size_t superficies;
	terminalSuperficie * superficie;
};

// definicion de tipo
typedef struct struct_terminalBuffer terminalBuffer;

// pa largar despues de declarar (la idea es usar esto con struct y punteros con memoria asignada...)
#define terminalBuffer_construir( buffer ) \
  { \
    buffer.columnas = 0; \
    buffer.filas = 0; \
    buffer.meta = NULL; \
    buffer.superficies = 0; \
    buffer.superficie = NULL; \
  }

#define terminalBuffer_destruir( tbuff ) \
  { \
    if ( tbuff.superficie != NULL ) { \
      for ( size_t i = 0; i < tbuff.superficies; i++ ) \
        terminalSuperficie_destruir( tbuff.superficie[ i ] ); \
      free( tbuff.superficie ); \
      tbuff.superficie = NULL; \
    }; \
    if ( tbuff.meta != NULL ) { \
      free( tbuff.meta ); \
      tbuff.meta = NULL; \
    }; \
    tbuff.superficies = 0; \
  }


#endif