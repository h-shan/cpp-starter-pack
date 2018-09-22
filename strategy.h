#ifndef STRATEGY_H_
#define STRATEGY_H_

#include <vector>
#include <string>
#include "Game_Api.h"
#include "util.h"

using Player = Game_Api::Player;

using namespace std;

class Strategy {
  
  private:
    vector<vector<string>> _history;
    string _find_match(int length);
  
  public:
    string get_stance(Player opponent);
    void add_history(string selfStance, string opponentStance);
};

#endif
