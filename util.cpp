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

int get_total_strength(Player player) {
  return player._rock + player._paper + player._scissors;
}

string get_strongest_stat(Player player) {
  if (player._rock >= player._scissors && player._rock > player._paper) {
    return "Rock";
  }
  if (player._scissors >= player._rock && player._scissors > player._paper) {
    return "Scissors";
  }
  if (player._scissors >= player._rock && player._scissors > player._paper) {
    return "Paper";
  }
  return get_random_stance();
}

int turns_to_kill(Player player, Monster monster) {
  int health = monster._health;
  int stat = get_stat(player, get_weakness(monster._stance));
  return (health + stat - 1) / stat;
}

int get_play_speed(Player player) {
  return 7 - player._speed;
}

int time_to_next_move(Player player) {
  return player._movement_counter - player._speed;
}

string get_random_stance() {
  srand (time(NULL));
  int r = rand() % 3;
  switch (r) {
    case 0:
      return "Rock";
    case 1:
      return "Paper";
    case 2:
      return "Scissors";
    default:
      return "";
  }
}

string get_weakness(string stance) {
  map<string, string> winnerMap;
  winnerMap["Scissors"] = "Rock";
  winnerMap["Rock"] = "Paper";
  winnerMap["Paper"] = "Scissors";
  return winnerMap[stance];
}


