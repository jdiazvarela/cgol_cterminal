#ifndef biblio_tiempo_h
#define biblio_tiempo_h


#pragma once
// -- INCLUSIONES

#include <time.h>
// struct timespec
//   tv_sec
//   tv_nsec
// clock_gettime()


// -- DEFINICION

// -- tiempo

typedef struct timespec tiempo;

// segundos
#define tiempo_segundos( tiempo ) tiempo.tv_sec

// nano segundos
#define tiempo_nanosegundos( tiempo ) tiempo.tv_nsec


// -- ASIGNACION / IGUALACION

// macro sencilla para simplificar codigo
// define de manera simple los segundos y nanosegundos de un tiempo dado
#define tiempo_definir( tiempo, segundos, nanosegundos ) \
  { \
    tiempo_segundos( tiempo ) = segundos; \
    tiempo_nanosegundos( tiempo ) = nanosegundos; \
  }

// si tengo una variable de tiempo y quiero cargarla a otra
#define tiempo_igualar( receptor, objetivo ) tiempo_definir( receptor, tiempo_segundos( objetivo ), tiempo_nanosegundos( objetivo ) )


// -- COMPARACION

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
  )


// -- ARITMETICO

// a la variable de tiempo le resto otra
#define tiempo_restar( receptor, objetivo ) \
  { \
    tiempo_segundos( receptor ) -= tiempo_segundos( objetivo ); \
    tiempo_nanosegundos( receptor ) -= tiempo_nanosegundos( objetivo ); \
    if( tiempo_nanosegundos( receptor ) < 0 ) { \
      tiempo_nanosegundos( receptor ) += 1000000000; \
      tiempo_segundos( receptor )--; \
    }; \
  }


// -- FUNCIONES

// actualizar al tiempo actual
#define tiempo_actualizar( tiempo ) clock_gettime( CLOCK_REALTIME, &(tiempo) )

// controla si trancurrio un delay desde el tiempo marcado
// y se actualiza en caso afffffirrrrrrmativo
int pTiempo_controlar( tiempo * pPrevio, const tiempo delay ) {

	#define previo ( * pPrevio )

	tiempo control;
	tiempo delta;

	tiempo_actualizar( control );
	tiempo_igualar( delta, control );
	tiempo_restar( delta, delay );

	return tiempo_esMenor( previo, delta );

	#undef previo
};

// macro para hacer mas bonito el codigo
#define tiempo_controlar( pPrevio, delay ) pTiempo_controlar( & ( pPrevio ), delay )


#endif