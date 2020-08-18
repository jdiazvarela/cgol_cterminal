


// -- INCLUSIONES

// pa plugins
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

#include <dirent.h>
// #DIR
// struct dirent
//   ino_t d_ino    - file serial number
//   char d_name[]  - name of entry (will not exceed a size of NAME_MAX)
// oendir()
// readdir()
// closedir()



#include "biblio/biblio.h"


// -- FUNCIONES

size_t GOL_ciclo( char * * pTableros, const size_t i, const size_t columnas, const size_t filas ) {
	#define tableros ( * pTableros )
	size_t vivos = 0;
	// simulacion (y pintura) de GOL
	for ( int y = 0; y < (int)filas; y++ )
		for ( int x = 0; x < (int)columnas; x++ ) {

			// se suma todo en un area de 3x3
			size_t vecinos = 0;
			for ( int vy = 0; vy < 3; vy++ )
				for ( int vx = 0; vx < 3; vx++ )
					vecinos += tableros[ i * filas * columnas + ( ( ( y + vy + filas - 1 ) % filas ) * columnas ) + ( ( x + vx + columnas - 1 ) % columnas ) ];

			// se resta la celda misma (para que la suma sea solo de vecinos)
			vecinos -= tableros[ ( i * filas * columnas ) + ( y * columnas ) + x ];

			// vive o muere de acuerdo a los vecinos
			char resultado;
			switch ( vecinos ) {
				case 2 :  resultado = ( tableros[ ( i * filas * columnas ) + ( y * columnas ) + x ] > 0 ) ? 1 : 0; break;
				case 3 :  resultado = 1; break;
				default : resultado = 0;
			};
			tableros[ ( ( 1 - i ) * filas * columnas ) + ( y * columnas ) + x ] = resultado;

			vivos += tableros[ i * filas * columnas + ( y * columnas ) + x ];
		};
	return vivos;
	#undef tableros
};


// -- FUNCION PRINCIPAL

int main( const int argumentos_cantidad, const char ** argumentos_lista ) {

	size_t semilla = time( NULL );
	srand( semilla );

	terminalConfiguracion tConf;
	terminalConfiguracion_construir( tConf );

	if ( terminalConfiguracion_configurarModoRaw( tConf ) < 0 ) return 1;

	// esconder cursor de terminal
	terminalConfiguracion_ocultarCursor( tConf );

	// salteando primeras lineas para no sobreescribir contenido previo de la terminal
	if ( terminal_saltearPrevio() < 0 ) {
		printf ( "al saltear pantalla inicial\n" );
		goto salida;
	};

	// control de flujo del programa
	size_t continuar = 1;
	size_t paso = 0;
	size_t editando = 0;

	// armando buffers
	terminalBuffer tbuff;
	terminalBuffer_construir( tbuff );

	// armando superficies de dibujo (2)
	tbuff.superficies = 2;
	tbuff.superficie = malloc( sizeof( terminalSuperficie ) * tbuff.superficies );
	if ( tbuff.superficie == NULL ) {
		printf ( "al crear superficie\n" );
		goto salida;
	};

	for ( size_t s = 0; s < tbuff.superficies; s++ )
		terminalSuperficie_construir( tbuff.superficie[ s ] );

	// para facilitar ya declaro algunos nombres para cada superficie
	#define supGOL_i 0
	#define supGOL tbuff.superficie[ supGOL_i ]
	#define supGUI_i 1
	#define supGUI tbuff.superficie[ 1 ]

	// ubicacion del tablero de GOL
	supGOL.x0 = 1;
	supGOL.y0 = 4;
	// se mueve la GUI al frente
	supGUI.nivel = 9999;

	// data ...tableros para el juego
	char * tableros = NULL;
	// indice de tablero
	size_t i = 0;
	// contador de celdas vivas
	size_t vivos = 0;

	// algunos limites...
	const size_t columnas_minimo = 8;
	const size_t filas_minimo = 8;

	// posicion del cursor (editor)
	size_t cX = supGOL.x0;
	size_t cY = supGOL.y0;

	// control de frecuencia de redimensionamiento de la terminal
	tiempo previo;
	tiempo_definir( previo, 0, 0 );
	tiempo delay;
	// se verifican las dimensiones cada tanto tiempo
	tiempo_definir( delay, 3, 300000000 );
	tiempo t0;

	size_t ciclos = 0;

	// sale ciclo
	while ( 1 ) {

		tiempo_actualizar( t0 );

		// lectura de entradassss

		claveDeEntrada entrada = entrada_leer();
		if ( entrada == ENTRADA_ERROR ) break;

		// separo para facilitarrrr
		claveDeEntrada modificadores = entrada_soloModificadores( entrada );
		entrada = entrada_sinModificadores( entrada );


		// -- ENTRADAS

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

					for ( size_t t = 0; t < 2 * supGOL.filas * supGOL.columnas; t++ ) tableros[ t ] = rand() % 2;

					if ( terminalBuffer_dimensionar( tbuff, columnas, filas ) < 0 ) {
						printf ( "terminalBuffer_dimensionar == NULL\n" );
						break;
					};

					terminalBuffer_superficie_limpiar( tbuff, supGOL_i );
					terminalBuffer_superficie_limpiar( tbuff, supGUI_i );

					for ( size_t fila = 0; fila < tbuff.filas; fila++ )
						tbuff.meta[ fila ] = 1;

					if ( continuar < 1 ) paso = 1;
					ciclos = 0;
				};
			};
		};

		// ctrl + c  ; ctrl + d; esc; q
		if ( entrada == 3 || entrada == 4 || entrada == 27 || entrada == 'q' ) break;

		// barra  -> pausa/continuar
		if ( entrada == ' ' ) { continuar = 1 - continuar; if ( continuar > 0 ) editando = 0; };

		// tirate un paso
		if ( entrada == 'f' ) { paso = 1; editando = 0; };

		// 'r'   reset / random
		if ( entrada == 'r' ) {
			for ( size_t t = 0; t < 2 * supGOL.filas * supGOL.columnas; t++) tableros[ t ] = rand() % 2;
			if ( continuar < 1 ) paso = 1;
			ciclos = 0;
		};

		// movimiento del cursor flechas y wasd
		if ( entrada == 'w' || entrada == 'a' || entrada == 's' || entrada == 'd' || entrada == ENTRADA_IZQUIERDA || entrada == ENTRADA_DERECHA || entrada == ENTRADA_ARRIBA || entrada == ENTRADA_ABAJO ) {

			terminalSuperficie_limpiarColumna( supGUI, cX );
			terminalSuperficie_limpiarFila(    supGUI, cY );

			size_t rep = 1;
			if ( modificadores == ENTRADA_SHIFT ) rep = 5;

			for ( size_t r = 0; r < rep; r++ ) {
				switch( entrada ) {
					case 'a' : case ENTRADA_IZQUIERDA : if ( cX != supGOL.x0 ) cX--; else cX = supGOL.x0 + supGOL.columnas - 1; break;
					case 'w' : case ENTRADA_ARRIBA :    if ( cY != supGOL.y0 ) cY--; else cY = supGOL.y0 + supGOL.filas - 1;    break;
					case 'd' : case ENTRADA_DERECHA :   if ( cX + 1 < supGOL.x0 + supGOL.columnas ) cX++; else cX = supGOL.x0; break;
					case 's' : case ENTRADA_ABAJO :     if ( cY + 1 < supGOL.y0 + supGOL.filas )    cY++; else cY = supGOL.y0; break;
				};
			};
			editando = 1;
			paso = 0;
			continuar = 0;
		};

		// "dibujar" <- enter / 'e' 
		if ( entrada == 13 || entrada == 'e' ) { editando = 1; paso = 0; continuar = 0; };

		// 'z'   clear ... despejar limpiar,,.. . etc...
		if ( entrada == 'z' ) {
			for ( size_t t = 0; t < 2 * supGOL.filas * supGOL.columnas; t++) tableros[ t ] = 0;
			paso = 1;
			editando = 1;
			continuar = 0;
			ciclos = 0;
		};


		// -- SIMULACION

		// paso de simulacion
		if ( continuar || paso ) {
			vivos = GOL_ciclo( & tableros, i, supGOL.columnas, supGOL.filas );
			ciclos++;
		};


		// -- GRAFICO

		// actualizacion de GUIIIIIIIIIIIIIIIIII

		if ( editando ) {

			// barras (cuadro)
			// superior
			terminalBuffer_superficie_dibujarHorizontal_signo( tbuff, supGUI_i, supGOL.x0,                   supGOL.y0 - 1,            39, 49, '-', supGOL.columnas );
			// inferior
			terminalBuffer_superficie_dibujarHorizontal_signo( tbuff, supGUI_i, supGOL.x0,                   supGOL.y0 + supGOL.filas, 39, 49, '-', supGOL.columnas );
			// izquierda
			terminalBuffer_superficie_dibujarVertical_signo(   tbuff, supGUI_i, supGOL.x0 - 1,               supGOL.y0,                39, 49, '|', supGOL.filas );
			// derecha
			terminalBuffer_superficie_dibujarVertical_signo(   tbuff, supGUI_i, supGOL.x0 + supGOL.columnas, supGOL.y0,                39, 49, '|', supGOL.filas );

			// ejes indicadores
			terminalBuffer_superficie_dibujarHorizontal_signo( tbuff, supGUI_i, supGOL.x0,                   cY,                       93, 100, terminal_signoTransparente, supGOL.columnas );
			terminalBuffer_superficie_dibujarVertical_signo(   tbuff, supGUI_i, cX,                          supGOL.y0,                93, 100, terminal_signoTransparente, supGOL.filas );

			// indicadores en las barras
			terminalBuffer_superficie_dibujarHorizontal_signo( tbuff, supGUI_i, cX,                          supGOL.y0 - 1,            91, 49, 'v', 1 );
			terminalBuffer_superficie_dibujarHorizontal_signo( tbuff, supGUI_i, cX,                          supGOL.y0 + supGOL.filas, 91, 49, '^', 1 );
			terminalBuffer_superficie_dibujarHorizontal_signo( tbuff, supGUI_i, supGOL.x0 - 1,               cY,                       91, 49, '>', 1 );
			terminalBuffer_superficie_dibujarHorizontal_signo( tbuff, supGUI_i, supGOL.x0 + supGOL.columnas, cY,                       91, 49, '<', 1 );

			if ( entrada == 13 || entrada == 'e' ) {
				#define xi ( cX - supGOL.x0 )
				#define yi ( cY - supGOL.y0 )

				vivos += ( tableros[ i * supGOL.filas * supGOL.columnas + ( yi * supGOL.columnas ) + xi ] ) ? -1 : 1;
				tableros[ i * supGOL.filas * supGOL.columnas + ( yi * supGOL.columnas ) + xi ] = 1 - tableros[ i * supGOL.filas * supGOL.columnas + ( yi * supGOL.columnas ) + xi ];

				terminalBuffer_superficie_dibujarHorizontal_signo( tbuff, supGOL_i, xi, yi, 96, 40, ( tableros[ i * supGOL.filas * supGOL.columnas + ( yi * supGOL.columnas ) + xi ] ) ? 'o' : ' ', 1 );

				#undef xi
				#undef yi
			};
		}
		else {
			// se pone todo en transssparente
			terminalBuffer_superficie_dibujarHorizontal_signo( tbuff, supGUI_i, supGOL.x0,                   supGOL.y0 - 1,            terminal_signoTransparente, terminal_colorTransparente, terminal_colorTransparente, supGOL.columnas );
			terminalBuffer_superficie_dibujarHorizontal_signo( tbuff, supGUI_i, supGOL.x0,                   supGOL.y0 + supGOL.filas, terminal_signoTransparente, terminal_colorTransparente, terminal_colorTransparente, supGOL.columnas );
			terminalBuffer_superficie_dibujarVertical_signo(   tbuff, supGUI_i, supGOL.x0 - 1,               supGOL.y0,                terminal_signoTransparente, terminal_colorTransparente, terminal_colorTransparente, supGOL.filas );
			terminalBuffer_superficie_dibujarVertical_signo(   tbuff, supGUI_i, supGOL.x0 + supGOL.columnas, supGOL.y0,                terminal_signoTransparente, terminal_colorTransparente, terminal_colorTransparente, supGOL.filas );
			terminalBuffer_superficie_dibujarHorizontal_signo( tbuff, supGUI_i, supGOL.x0,                   cY,                       terminal_signoTransparente, terminal_colorTransparente, terminal_colorTransparente, supGOL.columnas );
			terminalBuffer_superficie_dibujarVertical_signo(   tbuff, supGUI_i, cX,                          supGOL.y0,                terminal_signoTransparente, terminal_colorTransparente, terminal_colorTransparente, supGOL.filas );
		};

		size_t temporal_limite = 128;
		char temporal[ temporal_limite ];
		size_t l;

		// info

		l = snprintf( temporal, temporal_limite, "[esc/q/ctrl+c/ctrl+d]: salir; [flechas/wasd]: mover; [enter/e]: cambiar;" );
		terminalBuffer_superficie_limpiarFila( tbuff, supGUI_i, 0 );
		terminalBuffer_superficie_dibujarHorizontal_texto( tbuff, supGUI_i, 0, 0, 0, 0, temporal, l );

		l = snprintf( temporal, temporal_limite, "[f]: paso; [barra]: pausar/continuar; [z]: despejar; [r]: reinicio; semilla: %ld;", semilla );
		terminalBuffer_superficie_limpiarFila( tbuff, supGUI_i, 1 );
		terminalBuffer_superficie_dibujarHorizontal_texto( tbuff, supGUI_i, 0, 1, 0, 0, temporal, l );

		l = snprintf( temporal, temporal_limite, "dims: %ld x %ld; entradas: [%4ld][%6ld]; cursor: %ld,%ld; ciclos: %ld; vivos: %ld;", supGOL.columnas, supGOL.filas, entrada, modificadores, cX, cY, ciclos, vivos );
		terminalBuffer_superficie_limpiarFila( tbuff, supGUI_i, 2 );
		terminalBuffer_superficie_dibujarHorizontal_texto( tbuff, supGUI_i, 0, 2, 0, 0, temporal, l );

		// pintar tablero de GOL en la superficie

		if ( continuar || paso )
			for ( size_t y = 0; y < supGOL.filas; y++ )
				for ( size_t x = 0; x < supGOL.columnas; x++ )
					terminalBuffer_superficie_dibujarHorizontal_signo( tbuff, supGOL_i, x, y, 96, 40, ( tableros[ i * supGOL.filas * supGOL.columnas + ( y * supGOL.columnas ) + x ] > 0 ) ? 'o' : ' ', 1 );

		// pintar superficies
		terminalBuffer_pintarSuperficies( tbuff );

		// mas huevadas de control de flujo del programa
		if ( continuar || paso ) {
			// si era solo un paso se descontinua
			if ( paso ) continuar = 0;
			// siempre se detiene despues de un paso
			paso = 0;
			editando = 0;
			// cambio de tablero
			i = 1 - i;
		};
	};

	#undef supGOL_i
	#undef supGOL

	salida:

	// se libera toda la meeeeemoryyyy
	terminalBuffer_destruir( tbuff );

	terminalConfiguracion_terminar( tConf );

	return 0;
};


// -- FIN
