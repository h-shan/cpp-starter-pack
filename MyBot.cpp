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

using namespace std;
//You may add global variables and helper functions
Game_Api * api;
Player PLAYER_SELF("", 0, 0, 0, 0, NULL);

const int NUM_NODES = 25;

void log_monsters() {
  vector<Monster> monsters = api->get_all_monsters();
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
    api->log(string(msg));
  }
  api->log("NUM_MONSTERS:**************************\n" + to_string(monsters.size()));
}

vector<Monster> get_speed_monsters() {
  return vector<Monster>(); 
}

Monster get_closest(vector<Monster> monsters) {
  int smallest_length = INT_MAX;
  Monster closest_monster;
  for (Monster monster: monsters) {
    vector<vector<node_id_t> > paths = api->shortest_paths(PLAYER_SELF._location, monster._location);
    size_t length = paths[0].size();
    if (length < smallest_length) {
      smallest_length = length;
      closest_monster = monster;
    }
  }
  return closest_monster;
}

node_it_t get_step_towards_monster(Monster monster) {
  vector<vector<node_id_t> > paths = api->shortest_paths(PLAYER_SELF._location, monster._location);
  return paths[0][0];
}

int main() {
	int my_player_num = 0;
  while(1){
		char* buf = NULL;
		size_t size = 0;
		getline(&buf, &size, stdin);
		json data = json::parse(buf);
		if(data["type"] == "map"){
			my_player_num = data["player_id"];
			api = new Game_Api(my_player_num, data["map"]);
		} else {
			api->update(data["game_data"]);
      PLAYER_SELF = api->get_self();
			 //YOUR CODE HERE
      <vector> Monster speedMonsters = get_speed_monsters();
      Monster closestMon = get_closest(speedMonsters);
      node_id_t target = get_step_towards_monster(closestMon);
      api->submit_decision(target, "Rock"); //CHANGE THIS
      fflush(stdout);
      free(buf);
    }
  }
}

