#pragma once
#include <string>

using namaespace std;

public enum State{
  STANDARD,
  HEALING,
  PURSUIT
}

class GameStateEngine{
  public:
    GameStateEngine();
    GameStateEngine get_state();
    void set_state(GameStateEngine setAs);

  private:
    GameStateEngine _currentState;
}
