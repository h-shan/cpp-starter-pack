#include "util.h"

using namespace std;

// stance is "Rock", "Paper", or "Scissors"
int get_stat(Player player, string stance) {
  if (stance == "Rock") {
    return player._rock;
  } else if (stance == "Paper") {
    return player._paper;
  } else if (stance == "Scissors") {
    return player._scissors;
  }
  return 0;
}

int get_strength(Player player) {
  return player._rock + player._paper + player._scissors;
}

int turns_to_kill(Player player, Monster monster) {
  int health = monster._health;
  map<string, string> winnerMap;
  winnerMap["Scissors"] = "Rock";
  winnerMap["Rock"] = "Paper";
  winnerMap["Paper"] = "Scissors";
  int stat = get_stat(player, winnerMap[monster._stance]);
  return (health + stat - 1) / stat;
}


