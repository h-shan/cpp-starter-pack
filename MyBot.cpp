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
Player PLAYER_SELF;

const int NUM_NODES = 25;

void log_monsters() {
  int num_monsters = 0;
  for (node_id_t i = 0; i < NUM_NODES; i++) {
    if (api->has_monster(i)) {
      num_monsters++;
      Monster monster = api->get_monster(i);
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
  }
  api->log("NUM_MONSTERS:**************************\n" + to_string(num_monsters));
}

vector<Monster> get_speed_monsters() {

}

Monster get_closest(vector<Monster> monsters) {
  int smallest_length = INT_MAX;
  Monster closest_monster;
  for (Monster monster: monsters) {
    vector<vector<node_id_t> > paths = api->shortest_paths(PLAYER_SELF->_location, monster->_location);
    size_t length = paths[0].size();
    if (length < smallest_length) {
      smallest_length = length;
      closest_monster = monster;
    }
  }
  return closest_monster;
}

int main() {
	int my_player_num = 0;
	bool printed = false;
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
      if (!printed) {
        log_monsters();
        printed = true;
      }
			 //YOUR CODE HERE

      api->submit_decision(0,"Rock"); //CHANGE THIS
      fflush(stdout);
      free(buf);
    }
  }
}

