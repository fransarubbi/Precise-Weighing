#include "sistema.h"
#include "sapi.h"


#define	cTicksBas	500


extern bool_t ePuestaMarcha;
extern bool_t eParada;
extern bool_t eRearme;
extern bool_t eL1;
extern bool_t eL2;
extern bool_t eCero;
bool_t eFin;
bool_t eFinEmergencia;
bool_t eError;
bool_t timerInt;
bool_t timerFlag;
bool_t inFlag;
uint8_t basFlag;
uint8_t flagVaciado;
static delay_t	vDelayBas;



/*
 * Funciones para el timer
 */
void timers_Init(void) {

	Chip_TIMER_Init(LPC_TIMER0);
	Chip_TIMER_Init(LPC_TIMER1);
	Chip_TIMER_Init(LPC_TIMER2);
	Chip_TIMER_Init(LPC_TIMER3);
	Chip_TIMER_PrescaleSet(LPC_TIMER0, 20400 - 1);  //Baja la frecuencia a 10kHz (204Mhz/20400)
	Chip_TIMER_PrescaleSet(LPC_TIMER1, 20400 - 1);
	Chip_TIMER_PrescaleSet(LPC_TIMER2, 20400 - 1);
	Chip_TIMER_PrescaleSet(LPC_TIMER3, 20400 - 1);
	Chip_TIMER_SetMatch(LPC_TIMER0, 0, 50000);  //10k*5 = 5 seg
    Chip_TIMER_SetMatch(LPC_TIMER1, 0, 50000);  //10k*5 = 5 seg
    Chip_TIMER_SetMatch(LPC_TIMER2, 0, 50000);   //10k*5 = 5 seg
    Chip_TIMER_SetMatch(LPC_TIMER3, 0, 1250);
    Chip_TIMER_ResetOnMatchEnable(LPC_TIMER0, 0);
    Chip_TIMER_ResetOnMatchEnable(LPC_TIMER1, 0);
    Chip_TIMER_ResetOnMatchEnable(LPC_TIMER2, 0);
    Chip_TIMER_ResetOnMatchEnable(LPC_TIMER3, 0);
    Chip_TIMER_MatchEnableInt(LPC_TIMER0, 0);
    Chip_TIMER_MatchEnableInt(LPC_TIMER1, 0);
    Chip_TIMER_MatchEnableInt(LPC_TIMER2, 0);
    Chip_TIMER_MatchEnableInt(LPC_TIMER3, 0);

    NVIC_DisableIRQ(TIMER0_IRQn);
    NVIC_ClearPendingIRQ(TIMER0_IRQn);
    NVIC_SetPriority(TIMER0_IRQn, 1);

    NVIC_DisableIRQ(TIMER1_IRQn);
    NVIC_ClearPendingIRQ(TIMER1_IRQn);
    NVIC_SetPriority(TIMER1_IRQn, 1);

    NVIC_DisableIRQ(TIMER2_IRQn);
    NVIC_ClearPendingIRQ(TIMER2_IRQn);
    NVIC_SetPriority(TIMER2_IRQn, 1);

    NVIC_DisableIRQ(TIMER3_IRQn);
    NVIC_ClearPendingIRQ(TIMER3_IRQn);
    NVIC_SetPriority(TIMER3_IRQn, 1);
}


void TIMER0_IRQHandler(void) {
    if (Chip_TIMER_MatchPending(LPC_TIMER0, 0)) {
    	timerInt = 1;
        Chip_TIMER_ClearMatch(LPC_TIMER0, 0);
    }
}


void TIMER1_IRQHandler(void) {
    if (Chip_TIMER_MatchPending(LPC_TIMER1, 0)) {
    	timerInt = 1;
        Chip_TIMER_ClearMatch(LPC_TIMER1, 0);
    }
}


void TIMER2_IRQHandler(void) {
    if (Chip_TIMER_MatchPending(LPC_TIMER2, 0)) {
    	timerInt = 1;
        Chip_TIMER_ClearMatch(LPC_TIMER2, 0);
    }
}


void TIMER3_IRQHandler(void) {
    if (Chip_TIMER_MatchPending(LPC_TIMER3, 0)) {
    	basFlag += 1;
    	timerFlag = 1;
        Chip_TIMER_ClearMatch(LPC_TIMER3, 0);
    }
}


/*
 * Funciones para los GPIO
 * GPIO0 -> C1
 * GPIO2 -> C2
 * GPIO4 -> Basculante
 */
static void aC1(aC1_t accion){
	switch (accion){
	case cInitC1:
		gpioInit(GPIO0, GPIO_OUTPUT); // @suppress("Symbol is not resolved")
		gpioWrite(GPIO0, 0);
		break;
	case cOnC1:
		gpioWrite(GPIO0, 1);
		break;
	case cOffC1:
		gpioWrite(GPIO0, 0);
		break;
	default :
		break;
	}
}


static void aC2(aC2_t accion){
	switch (accion){
	case cInitC2:
		gpioInit(GPIO2, GPIO_OUTPUT); // @suppress("Symbol is not resolved")
		gpioWrite(GPIO2, 0);
		break;
	case cOnC2:
		gpioWrite(GPIO2, 1);
		break;
	case cOffC2:
		gpioWrite(GPIO2, 0);
		break;
	default :
		break;
	}
}


static void aBasculante(aBasculante_t accion){
	switch (accion){
	case cInitBas:
		gpioInit(GPIO4, GPIO_OUTPUT); // @suppress("Symbol is not resolved")
		gpioWrite(GPIO4, 0);
		break;
	case cOnBas:
		gpioWrite(GPIO4, 1);
		break;
	case cOffBas:
		gpioWrite(GPIO4, 0);
		break;
	default :
		break;
	}
}


static void aBasculanteTope(aBasculanteTope_t accion){
	switch (accion){
	case cInitBasTope:
		gpioInit(GPIO1, GPIO_OUTPUT); // @suppress("Symbol is not resolved")
		gpioWrite(GPIO1, 0);
		break;
	case cOnBasTope:
		gpioWrite(GPIO1, 1);
		break;
	case cOffBasTope:
		gpioWrite(GPIO1, 0);
		break;
	default :
		break;
	}
}


/*
 * Auxiliar
 */
void disable_timer_IRQ(aTimer_t flagTimer) {
	switch (flagTimer){
	case timer0:
		NVIC_DisableIRQ(TIMER0_IRQn);
		Chip_TIMER_Disable(LPC_TIMER0);
		break;
	case timer1:
		NVIC_DisableIRQ(TIMER1_IRQn);
		Chip_TIMER_Disable(LPC_TIMER1);
		break;
	case timer2:
		NVIC_DisableIRQ(TIMER2_IRQn);
		Chip_TIMER_Disable(LPC_TIMER2);
		break;
	case timer3:
		NVIC_DisableIRQ(TIMER3_IRQn);
		Chip_TIMER_Disable(LPC_TIMER3);
		break;
	}

}


/*
 * Funciones de estado
 */
void state_ESPERA(StateMachine* sm){
	if (ePuestaMarcha) {
		ePuestaMarcha = 0;
		enter_ANDANDO(sm, 1);
	}
}


void state_NO_PRECISO(StateMachine* sm){
	NVIC_EnableIRQ(TIMER0_IRQn);

	bool_t returning_STOP = (sm->history_PESADO == NO_PRECISO);
	aTimer_t flagTimer = timer0;

	if (!returning_STOP) {
		sm->history_PESADO = NO_PRECISO;
		Chip_TIMER_Reset(LPC_TIMER0);
	}
	Chip_TIMER_Enable(LPC_TIMER0);


	if ((timerInt == 0) && eL1) {
		eL1 = 0;
		disable_timer_IRQ(timer0);
		aC1(cOffC1);
		transition_to(sm, state_PRECISO, PRECISO);
	}
	if (timerInt) {
		timerInt = 0;
		disable_timer_IRQ(timer0);
		eError = 1;
		enter_STOP(sm, 1);
	}
	if (eParada) {
		eParada = 0;
		sm->history_PESADO = NO_PRECISO;
		disable_timer_IRQ(timer0);
		enter_STOP(sm, 0);
	}
}


void state_PRECISO(StateMachine* sm){
	NVIC_EnableIRQ(TIMER1_IRQn);

	bool_t returning_STOP = (sm->history_PESADO == PRECISO);
	aTimer_t flagTimer = timer1;

	if (!returning_STOP) {
		sm->history_PESADO = PRECISO;
		Chip_TIMER_Reset(LPC_TIMER1);
	}
	Chip_TIMER_Enable(LPC_TIMER1);

	if((timerInt == 0) && eL2){
		eL2 = 0;
		disable_timer_IRQ(timer1);
		enter_VACIADO(sm);
	}
	if(timerInt){
		timerInt = 0;
		disable_timer_IRQ(timer1);
		eError = 1;
		enter_STOP(sm, 1);
	}
	if(eParada){
		eParada = 0;
		sm->history_PESADO = PRECISO;
		disable_timer_IRQ(timer1);
		enter_STOP(sm, 0);
	}
}


void state_VACIADO(StateMachine* sm){
	NVIC_EnableIRQ(TIMER2_IRQn);

	bool_t returning_STOP = (sm->history_VACIADO == VACIADO);
	aTimer_t flagTimer = timer2;

	if (!returning_STOP){
		sm->history_VACIADO = VACIADO;
		Chip_TIMER_Reset(LPC_TIMER2);
	}
	Chip_TIMER_Enable(LPC_TIMER2);

	if((timerInt == 0) && eCero){
		eCero = 0;
		disable_timer_IRQ(timer2);
		aC1(cOffC1);
		aC2(cOffC2);
		aBasculante(cOffBas);
		delayInit(&vDelayBas, cTicksBas);
		inFlag = 1;
		transition_to(sm, descargaBas, ESPERA);

	}
	if(timerInt){
		timerInt = 0;
		disable_timer_IRQ(timer2);
		eError = 1;
		enter_STOP(sm, 1);
	}
	if(eParada){
		eParada = 0;
		sm->history_VACIADO = VACIADO;
		disable_timer_IRQ(timer2);
		enter_STOP(sm, 0);
	}
}


void state_PARADO(StateMachine* sm){
	if(eRearme){
		eRearme = 0;
		aBasculante(cOnBas);
		transition_to(sm, state_VACIADO_EMERGENCIA, VACIADO_EMERGENCIA);
	}
	if(ePuestaMarcha){
		ePuestaMarcha = 0;
		if(sm->history_VACIADO == STATE_NONE){
			enter_ANDANDO(sm, 1);
		}
		else{
			enter_ANDANDO(sm, 0);
		}
	}
}


void state_ERRONEO(StateMachine* sm){
	if(eRearme){
		eRearme = 0;
		eError = 0;
		aBasculante(cOnBas);
		transition_to(sm, state_VACIADO_EMERGENCIA, VACIADO_EMERGENCIA);
	}
}


void state_VACIADO_EMERGENCIA(StateMachine* sm){
	if(eCero){
		eCero = 0;
		aBasculante(cOffBas);
		delayInit(&vDelayBas, cTicksBas);
		inFlag = 1;
		flagVaciado = 1;
		transition_to(sm, descargaBas, ESPERA);
	}
}


void descargaBas(StateMachine* sm){
	if(inFlag){
		aBasculanteTope(cOnBasTope);
		if(delayRead(&vDelayBas)){
			aBasculanteTope(cOffBasTope);
			inFlag = 0;
		}
	}
	else{
		NVIC_EnableIRQ(TIMER3_IRQn);

		if(timerFlag){
			Chip_TIMER_Reset(LPC_TIMER3);
			gpioWrite(GPIO4, !gpioRead(GPIO4));
			timerFlag = 0;
		}
		Chip_TIMER_Enable(LPC_TIMER3);

		if(basFlag == 16){
			basFlag = 0;
			aBasculante(cOffBas);
			if(flagVaciado){
				flagVaciado = 0;
				eFinEmergencia = 1;
			}
			disable_timer_IRQ(timer3);
			sm->history_PESADO = STATE_NONE;
			sm->history_VACIADO = STATE_NONE;
			enter_ANDANDO(sm, 1);
		}
	}
}


/*
 * Funciones de transiciones entre jerarquias
 */
void enter_ESPERA(StateMachine* sm){
	aC1(cOffC1);
	aC2(cOffC2);
	aBasculante(cOffBas);
	init_FSM(sm);
}


void enter_ANDANDO(StateMachine* sm, bool_t flagAndando) {
    sm->current = ANDANDO;
    if(flagAndando){
    	enter_PESADO(sm);
    }
    else{
    	enter_VACIADO(sm);
    }
}


void enter_PESADO(StateMachine* sm) {
    sm->current = PESADO;

    switch(sm->history_PESADO){
        case NO_PRECISO:
            enter_NO_PRECISO(sm);
            break;
        case PRECISO:
            enter_PRECISO(sm);
            break;
        case STATE_NONE:
        	enter_NO_PRECISO(sm);
        	break;
    }
}


void enter_NO_PRECISO(StateMachine* sm) {
	aC1(cOnC1);
	aC2(cOnC2);
	aBasculante(cOffBas);
    transition_to(sm, state_NO_PRECISO, NO_PRECISO);
}


void enter_PRECISO(StateMachine* sm){
	aC1(cOffC1);
	aC2(cOnC2);
	aBasculante(cOffBas);
	transition_to(sm, state_PRECISO, PRECISO);
}


void enter_VACIADO(StateMachine* sm) {
	aC1(cOffC1);
	aC2(cOffC2);
	aBasculante(cOnBas);
    transition_to(sm, state_VACIADO, VACIADO);
}


void enter_STOP(StateMachine* sm, bool_t flagStop){
	aC1(cOffC1);
	aC2(cOffC2);
	aBasculante(cOffBas);
	sm->current = STOP;
	if(flagStop){    // flagStop en 1 -> ERRONEO
		transition_to(sm, state_ERRONEO, ERRONEO);
	}
	else{   // flagStop en 0 -> PARADO
		transition_to(sm, state_PARADO, PARADO);
	}
}


/*
 * Funciones de ejecucion
 */
void transition_to(StateMachine* sm, StateHandler new_state, StateID id) {
    sm->handler = new_state;
    sm->current = id;
}


void run_state_machine(StateMachine* sm){
	if (sm->handler) {
		sm->handler(sm);
	}
}


/*
 * Funciones de inicializacion
 */

void init_FSM(StateMachine* sm){
	sm->history_PESADO = STATE_NONE;
	sm->history_VACIADO = STATE_NONE;
	transition_to(sm, state_ESPERA, ESPERA);
}


void init_Sistema(void){
	eFin = 0;
	eFinEmergencia = 0;
	eError = 0;
	timerInt = 0;
	basFlag = 0;
	timerFlag = 0;
	timers_Init();
	aC1(cInitC1);
	aC2(cInitC2);
	aBasculante(cInitBas);
	aBasculanteTope(cInitBasTope);
}
