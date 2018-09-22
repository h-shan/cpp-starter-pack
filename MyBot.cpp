#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "Game_Api.h"
using json = nlohmann::json;
using Player = Game_Api::Player;
using Monster = Game_Api::Monster;
using DeathEffects = Game_Api::DeathEffects;


#define RESPONSE_SECS 1
#define RESPONSE_NSECS 0

#include <iostream>
#include <string>
#include <vector>
#include <climits>
#include <time.h>
#include <map>

using namespace std;
//You may add global variables and helper functions
Game_Api * API;
Player PLAYER_SELF("", 0, 0, 0, 0, NULL);
map<string, string> WINNER_MAP;

const int NUM_NODES = 25;

void log_monsters() {
  vector<Monster> monsters = API->get_all_monsters();
  for (Monster monster : monsters) {
    char msg[1000];
    sprintf(msg, "---------------------------------------\nName: %s\nHealth: %d\nBase health: %d\nStance: %s\n Respawn Counter: %d\n Respawn Rate: %d\n Location: %d\nAttack: %d\nDeath Effects:\nRock: %d\nPaper: %d\nScissors: %d\nSpeed: %d\nHealth: %d",
      monster._name.c_str(),
      monster._health,
      monster._base_health,
      monster._stance.c_str(),
      monster._respawn_counter,
      monster._respawn_rate,
      monster._location,
      monster._attack,
      monster._death_effects._rock,
      monster._death_effects._paper,
      monster._death_effects._scissors,
      monster._death_effects._speed,
      monster._death_effects._health
      );
    API->log(string(msg));
  }
  API->log("NUM_MONSTERS:**************************\n" + to_string(monsters.size()));
}

vector<node_id_t> get_path(Monster monster) {
  return API->shortest_paths(PLAYER_SELF._location, monster._location)[0]; 
}

vector<Monster> get_speed_monsters() {
  vector <Monster> monsters = API->get_all_monsters();
  vector <Monster> speed_monsters;
  for (Monster monster : monsters){
    if (monster._death_effects._speed > 0){
      speed_monsters.push_back(monster);
    }
  }
  return speed_monsters;
}

Monster get_closest(vector<Monster> monsters) {
  int smallest_length = INT_MAX;
  Monster closest_monster;
  for (Monster monster: monsters) {
    vector<vector<node_id_t> > paths = API->shortest_paths(PLAYER_SELF._location, monster._location);
    size_t length = paths[0].size();
    if (length < smallest_length) {
      smallest_length = length;
      closest_monster = monster;
    }
  }
  return closest_monster;
}

string get_random_stance() {
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

string set_stance(node_id_t destination) {
  node_id_t node;
  if (PLAYER_SELF._movement_counter == 1) {
    node = destination;
  } else {
    node = PLAYER_SELF._location;
  }
  if (API->has_monster(node)) {
    Monster monster = API->get_monster(node);
    return WINNER_MAP[monster._stance]; 
  }
  return get_random_stance();
}

node_id_t get_step_towards_monster(Monster monster) {
  vector<vector<node_id_t> > paths = API->shortest_paths(PLAYER_SELF._location, monster._location);
  return paths[0][0];
}

int main() {
  srand (time(NULL));
  WINNER_MAP["Rock"] = "Paper";
  WINNER_MAP["Scissors"] = "Rock";
  WINNER_MAP["Paper"] = "Scissors";
  int my_player_num = 0;
  while(1){
		char* buf = NULL;
		size_t size = 0;
		getline(&buf, &size, stdin);
		json data = json::parse(buf);
		if(data["type"] == "map"){
			my_player_num = data["player_id"];
			API = new Game_Api(my_player_num, data["map"]);
		} else {
			API->update(data["game_data"]);
      PLAYER_SELF = API->get_self();
			 //YOUR CODE HERE
      vector <Monster> speedMonsters = get_speed_monsters();
      Monster closestMon = get_closest(speedMonsters);
      node_id_t target = get_step_towards_monster(closestMon);
      API->submit_decision(target, set_stance(target)); //CHANGE THIS
      fflush(stdout);
      free(buf);
    }
  }
}

