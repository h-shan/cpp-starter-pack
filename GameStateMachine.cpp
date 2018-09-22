#include "GameStateEngine.h"

GameStateEngine(){
  _currentState = GameStateEngine.STANDARD;
}

GameStateEngine get_state(){
  return _currentState;
}

void set_state(GameStateEngine setAs){
  _currentState = setAs;
}

