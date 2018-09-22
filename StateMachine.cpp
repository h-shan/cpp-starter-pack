#include "StateMachine.h"

StateMachine::StateMachine(){
  _currentState = State.STANDARD;
}

State StateMachine::get_state(){
  return _currentState;
}

void StateMachine::set_state(State setAs){
  _currentState = setAs;
}

