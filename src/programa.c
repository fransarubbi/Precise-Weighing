#include "programa.h"
#include "sistema.h"
#include "sapi.h"

StateMachine sm;

void main(void){
	boardConfig();
	InitMefLed();
	InitMefButton();
	init_Sistema();
	init_FSM(&sm);

	while(1){
		ActualizarMefButton();
		ActualizarMefLed();
		run_state_machine(&sm);
	}
}
