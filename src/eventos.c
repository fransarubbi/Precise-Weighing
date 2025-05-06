#include "eventos.h"
#include "sapi.h"


Cola colaEventos;

/*
 * Inicializar la cola
 */
void init_cola(Cola* colaEventos){
	colaEventos->tope = 0;
	colaEventos->ultimo = 0;
	colaEventos->cant = 0;
}


/*
 * Consultar el evento que esta en el tope, si la cola no esta vacia
 */
bool_t consult(Cola* colaEventos, Evento* e){
	if(colaEventos->cant == 0){
		return 0;
	}
	*e = colaEventos->buffer[colaEventos->tope];
	return 1;
}


/*
 * Insertar evento en la cola si la misma no esta llena
 */
bool_t insert(Cola* colaEventos, Evento e){
	if(colaEventos->cant == MAX_EVENTOS){
		return 0;   // Cola llena
	}
	colaEventos->buffer[colaEventos->ultimo] = e;
	colaEventos->ultimo = (colaEventos->ultimo + 1) & (MAX_EVENTOS - 1);
	colaEventos->cant++;
	return 1;
}


/*
 * Eliminar evento del tope de la pila
 */
void supress(Cola* colaEventos){
	colaEventos->tope = (colaEventos->tope + 1) & (MAX_EVENTOS - 1);
	colaEventos->cant--;
}

