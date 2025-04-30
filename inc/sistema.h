#ifndef SISTEMA_H_
#define SISTEMA_H_

#include "sapi.h"

typedef enum {    //Todos los estados
    STATE_NONE = -1,
	ESPERA,
    STOP,
    ANDANDO,
    ERRONEO,
    PARADO,
    VACIADO_EMERGENCIA,
	PESADO,
	VACIADO,
	PRECISO,
	NO_PRECISO
} StateID;


typedef struct StateMachine StateMachine;

typedef void (*StateHandler)(StateMachine*);  // Puntero a función que representa un estado


struct StateMachine{     // Máquina de estados
    StateID current;
    StateHandler handler;
    StateID history_PESADO;
    StateID history_VACIADO;
};


typedef enum{cInitC1, cOffC1,cOnC1} aC1_t;
typedef enum{cInitC2, cOffC2,cOnC2} aC2_t;
typedef enum{cInitBas, cOffBas,cOnBas} aBasculante_t;
typedef enum{cInitBasTope, cOffBasTope,cOnBasTope} aBasculanteTope_t;
typedef enum{timer0, timer1, timer2, timer3} aTimer_t;


// Prototipos de funciones de estado
void state_ESPERA(StateMachine*);
void state_STOP(StateMachine*);
void state_ANDANDO(StateMachine*);
void state_ERRONEO(StateMachine*);
void state_PARADO(StateMachine*);
void state_VACIADO_EMERGENCIA(StateMachine*);
void state_PESADO(StateMachine*);
void state_VACIADO(StateMachine*);
void state_PRECISO(StateMachine*);
void state_NO_PRECISO(StateMachine*);
void descargaBas(StateMachine* sm);


// Prototipo de funciones de transiciones entre jerarquias
void enter_ANDANDO(StateMachine*, bool_t);
void enter_PESADO(StateMachine*);
void enter_NO_PRECISO(StateMachine*);
void enter_STOP(StateMachine*, bool_t);
void enter_ESPERA(StateMachine*);
void enter_VACIADO(StateMachine*);
void enter_PRECISO(StateMachine*);


// Ejecuciones
void transition_to(StateMachine*, StateHandler, StateID);   // Transición de estado
void run_state_machine(StateMachine*);   // Ejecutar maquina


// Inicializaciones
void init_FSM(StateMachine*);   // Inicializar la FSM
void init_Sistema(void);    // Inicializar el sistema


// Auxiliar
void disable_timer_IRQ(aTimer_t);

#endif /* SISTEMA_H_ */
