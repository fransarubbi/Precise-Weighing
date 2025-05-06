#include "led.h"
#include "button.h"
#include "eventos.h"
#include "sapi.h"


#define	cTicksLed	150


static enum {REPOSO,PRENDIDO,TITILA} estado_actual;
static enum {APAGADO,ENCENDIDO} estado_actual_titila;
static delay_t	vTickLed;
static Evento evento;



// Funcion privada
static void aLed(aLed_t accion){
	switch (accion){
	case cInit:
		gpioInit (LED1, GPIO_OUTPUT); // @suppress("Symbol is not resolved")
		gpioWrite(LED1, 0);
		break;
	case cOn:
		gpioWrite(LED1, 1);
		break;
	case cOff:
		gpioWrite(LED1, 0);
		break;
	default :
		break;
	}
}


// Funciones Globales
void InitMefTitila(void){
	aLed(cOff);
	delayInit(&vTickLed,cTicksLed);
	estado_actual_titila = APAGADO;
}


void InitMefLed( void ){
	aLed(cInit);
	estado_actual = REPOSO;
	InitMefTitila();
}


void ActualizarMefLed(void){
	switch (estado_actual){
	case REPOSO:
		if(consult(&colaEventos, &evento)){
			if(evento == ePuestaMarcha){
				supress(&colaEventos);
				estado_actual = PRENDIDO;
				aLed(cOn);
			}
		}
		break;

	case PRENDIDO:
		if(consult(&colaEventos, &evento)){
			if((evento == eError) || (evento == eParada)){
				supress(&colaEventos);
				estado_actual = TITILA;
				InitMefTitila();
				delayRead(&vTickLed);
			}
		}
		break;

	case TITILA:
		if(consult(&colaEventos, &evento)){
			if((evento == ePuestaMarcha) || (evento == eFinEmergencia)){
				supress(&colaEventos);
				estado_actual = PRENDIDO;
				estado_actual_titila = APAGADO;
				aLed(cOn);
			}
		}

		switch (estado_actual_titila){
		case APAGADO:
			if (delayRead(&vTickLed)){
				estado_actual_titila = ENCENDIDO;
				aLed(cOn);
				delayRead(&vTickLed);
			}
			break;
		case ENCENDIDO:
			if (delayRead(&vTickLed)){
				estado_actual_titila = APAGADO;
				aLed(cOff);
				delayRead(&vTickLed);
			}
			break;
		}
		break;
	}
}

