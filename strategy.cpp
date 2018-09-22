#include "strategy.h"

#include <unordered_map>

using namespace std;

const string INVALID = "INVALID";

string Strategy::_find_match(int length) {
  if (_history.size() <= (size_t)length) {
    return "";
  }
 
  unordered_map<string, int> map;
  map["Rock"] = 0;
  map["Paper"] = 0;
  map["Scissors"] = 0;
  int max_counter = 0;  
  string best = "";

  size_t num_to_review = 40;
  for (size_t i = _history.size() - num_to_review; i < _history.size() - 1 - length; i++) {
    vector<vector<string>>::iterator iter = _history.end() - length;
    bool match = true;
    for (size_t j = i; j < i + length; j++) {
      vector<string> vec = *iter;
      if (_history[j][0] != vec[0] || _history[j][1] != vec[1]) {
        match = false;
        break;
      }
      iter++;
    }
    if (match) {
      string pred = _history[i + length][1]; 
      if (++map[pred] >= max_counter) {
        best = pred;
      }
    }
  }
  return best;
}

string Strategy::get_stance(Player opponent) {
  if (_history.empty()) {
    return get_weakness(get_strongest_stat(opponent)); 
  }
  string pred = "";
  for (size_t i = 1; i < 10; i++) {
    string res = _find_match(i);
    if (res.length()) {
      pred = res;
    } else {
      break;
    }
  }

  if (pred.length()) {
    return get_weakness(pred);
  }
  return get_weakness(_history[_history.size() - 1][1]); 
}

void Strategy::add_history(string self_stance, string opponent_stance) {
  vector<string> vec{ self_stance, opponent_stance };
  _history.push_back(vec);
}

