#include "button.h"
#include "eventos.h"
#include "sapi.h"

#define	cDelayDebounce 80

enum Button {WAIT,DEBOUNCE,PRESS};
enum Button est_act_PM;    //puesta en marcha
enum Button est_act_P;     //parada
enum Button est_act_R;     //rearme
enum Button est_act_L1;    //peso L1
enum Button est_act_L2;    //peso L2
enum Button est_act_Cero;  //peso cero



//	Variables Privadas
static delay_t	vDelayDeb;


/*
 * TEC1 -> ePuestaMarcha
 * TEC2 -> eParada
 * TEC3 -> eRearme
 */


//	Funciones Globales
void InitMefButton(void){
	gpioInit(TEC1, GPIO_INPUT); // @suppress("Symbol is not resolved")
	gpioInit(TEC2, GPIO_INPUT); // @suppress("Symbol is not resolved")
	gpioInit(TEC3, GPIO_INPUT); // @suppress("Symbol is not resolved")
	gpioInit(TEC4, GPIO_INPUT); //  L1   @suppress("Symbol is not resolved")
	gpioInit(GPIO6, GPIO_INPUT); //  L2  @suppress("Symbol is not resolved")
	gpioInit(GPIO5, GPIO_INPUT); //  cero  @suppress("Symbol is not resolved")
	delayInit(&vDelayDeb , cDelayDebounce);
	est_act_PM = WAIT;
	est_act_P = WAIT;
	est_act_R = WAIT;
	est_act_L1 = WAIT;
	est_act_L2 = WAIT;
	est_act_Cero = WAIT;
}


void ActualizarMefButton(void){
	switch (est_act_PM){
	case WAIT:
		if (!gpioRead(TEC1)){
			est_act_PM = DEBOUNCE;
			delayRead(&vDelayDeb);
		}
		break;
	case DEBOUNCE:
		if (delayRead(&vDelayDeb)){
			if (!gpioRead(TEC1)){
				est_act_PM = PRESS;
				insert(&colaEventos, ePuestaMarcha);
			}else{
				est_act_PM = WAIT;
			}
		}
		break;
	case PRESS:
		if (gpioRead(TEC1)){
			est_act_PM = WAIT;
		}
	}

	switch (est_act_P){
	case WAIT:
		if (!gpioRead(TEC2)){
			est_act_P = DEBOUNCE;
			delayRead(&vDelayDeb);
		}
		break;
	case DEBOUNCE:
		if (delayRead(&vDelayDeb)){
			if (!gpioRead(TEC2)){
				est_act_P = PRESS;
				insert(&colaEventos, eParada);
			}else{
				est_act_P = WAIT;
			}
		}
		break;
	case PRESS:
		if (gpioRead(TEC2)){
			est_act_P = WAIT;
		}
	}


	switch (est_act_R){
	case WAIT:
		if (!gpioRead(TEC3)){
			est_act_R = DEBOUNCE;
			delayRead(&vDelayDeb);
		}
		break;
	case DEBOUNCE:
		if (delayRead(&vDelayDeb)){
			if (!gpioRead(TEC3)){
				est_act_R = PRESS;
				insert(&colaEventos, eRearme);
			}else{
				est_act_R = WAIT;
			}
		}
		break;
	case PRESS:
		if (gpioRead(TEC3)){
			est_act_R = WAIT;
		}
	}


	switch (est_act_L1){
	case WAIT:
		if (!gpioRead(TEC4)){
			est_act_L1 = DEBOUNCE;
			delayRead(&vDelayDeb);
		}
		break;
	case DEBOUNCE:
		if (delayRead(&vDelayDeb)){
			if (!gpioRead(TEC4)){
				est_act_L1 = PRESS;
				insert(&colaEventos, eL1);
			}else{
				est_act_L1 = WAIT;
			}
		}
		break;
	case PRESS:
		if (gpioRead(TEC4)){
			est_act_L1 = WAIT;
		}
	}


	switch (est_act_L2){
	case WAIT:
		if (!gpioRead(GPIO6)){
			est_act_L2 = DEBOUNCE;
			delayRead(&vDelayDeb);
		}
		break;
	case DEBOUNCE:
		if (delayRead(&vDelayDeb)){
			if (!gpioRead(GPIO6)){
				est_act_L2 = PRESS;
				insert(&colaEventos, eL2);
			}else{
				est_act_L2 = WAIT;
			}
		}
		break;
	case PRESS:
		if (gpioRead(GPIO6)){
			est_act_L2 = WAIT;
		}
	}

	switch (est_act_Cero){
	case WAIT:
		if (!gpioRead(GPIO5)){
			est_act_Cero = DEBOUNCE;
			delayRead(&vDelayDeb);
		}
		break;
	case DEBOUNCE:
		if (delayRead(&vDelayDeb)){
			if (!gpioRead(GPIO5)){
				est_act_Cero = PRESS;
				insert(&colaEventos, eCero);
			}else{
				est_act_Cero = WAIT;
			}
		}
		break;
	case PRESS:
		if (gpioRead(GPIO5)){
			est_act_Cero = WAIT;
		}
	}
}
