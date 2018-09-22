#pragma once
#include <string>

using namespace std;

public enum State{
  STANDARD,
  HEALING,
  PURSUIT
}

class GameStateEngine{
  public:
    GameStateEngine();
    GameStateEngine get_state();
    void set_state(State setAs);

  private:
    GameStateEngine _currentState;
}
