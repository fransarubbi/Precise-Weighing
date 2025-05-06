#ifndef EVENTOS_H_
#define EVENTOS_H_

#include "sapi.h"

#define MAX_EVENTOS 8


typedef enum{
	ePuestaMarcha,
	eL1,
	eL2,
	eCero,
	eParada,
	eError,
	eRearme,
	eFinEmergencia
} Evento;


typedef struct{
	Evento buffer[MAX_EVENTOS];
	uint8_t tope;
	uint8_t ultimo;
	uint8_t cant;
} Cola;


extern Cola colaEventos;


void init_cola(Cola*);
bool_t consult(Cola*, Evento*);
bool_t insert(Cola*, Evento);
void supress(Cola*);


#endif /* EVENTOS_H_ */
