#pragma once
#include <string>

using namespace std;

enum State{
  STANDARD,
  HEALING,
  PURSUIT
};

class StateMachine{
  public:
    StateMachine();
    State get_state();
    void set_state(State setAs);

  private:
    State _currentState;
};
