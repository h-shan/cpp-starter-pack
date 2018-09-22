#pragma once
#include <string>

using namespace std;

public enum State{
  STANDARD,
  HEALING,
  PURSUIT
}

class StateMachine{
  public:
    StateMachine();
    State get_state();
    void set_state(State setAs);

  private:
    GameStateEngine _currentState;
}
