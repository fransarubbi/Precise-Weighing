#include "programa.h"
#include "sistema.h"
#include "sapi.h"
#include "eventos.h"

StateMachine sm;

void main(void){
	boardConfig();
	init_cola(&colaEventos);
	InitMefLed();
	InitMefButton();
	init_Sistema();
	init_FSM(&sm);

	while(1){
		ActualizarMefButton();
		run_state_machine(&sm);
		ActualizarMefLed();
	}
}
