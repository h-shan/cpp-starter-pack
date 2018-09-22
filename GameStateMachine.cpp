#include "GameStateEngine.h"

GameStateEngine(){
  _currentState = State.STANDARD;
}

GameStateEngine get_state(){
  return _currentState;
}

void set_state(State setAs){
  _currentState = setAs;
}

