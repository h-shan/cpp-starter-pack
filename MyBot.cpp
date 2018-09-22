#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "Game_Api.h"
#include "util.h"
#include "strategy.h"
#include <iostream>
#include <string>
#include <vector>
#include <climits>
#include <math.h>
#include <time.h>
#include <map>
#include "GameStateEngine.h"

using json = nlohmann::json;
using Player = Game_Api::Player;
using Monster = Game_Api::Monster;
using DeathEffects = Game_Api::DeathEffects;

#define RESPONSE_SECS 1
#define RESPONSE_NSECS 0

using namespace std;
//You may add global variables and helper functions
Game_Api * API;
Player SELF("", 0, 0, 0, 0, NULL);
Player OPPONENT("", 0, 0, 0, 0, NULL);
Monster CURRENT_TARGET_MONSTER;

// return 0 if we have disadvantage, 0.5 - 1 based on sigmoid if we have advantage
double get_advantage(){
  int self_health = SELF._health;
  int self_strength = get_total_strength(SELF);
  int opponent_health = OPPONENT._health;
  int opponent_strength = get_total_strength(OPPONENT);
  // ratio: how many rounds can we survive
  double my_rounds = ((double)self_health) / (opponent_strength);
  double opponent_rounds = ((double)opponent_health)/(self_strength);
  // ratio: how many more rounds can we survive than the opponent
  double advantage = my_rounds / opponent_rounds;
  // if we have no chance, return flag "-999.0"
  if (advantage < 1)
    return 0;
  // calculate and return sigmoid function if there is a chance
  double sigmoid = (exp(advantage)) / (exp(advantage) + 1); 
  return sigmoid;
}



int get_remaining_health(Monster monster){
  int monster_health = monster._health;
  int monster_damage = monster._attack;
  int health = SELF._health;
  string my_stance = get_weakness(monster._stance);
  int my_damage = get_stat(SELF, my_stance);
  while (monster_health > 0){
    monster_health -= my_damage;
    health -= monster_damage;
  }
  return health;
}

vector<node_id_t> get_path(node_id_t node) {
  return API->shortest_paths(SELF._location, node)[0]; 
}

vector<node_id_t> get_path(node_id_t source, node_id_t node) {
  return API->shortest_paths(source, node)[0]; 
}

vector<Monster> get_health_monsters() {
  vector <Monster> monsters = API->get_all_monsters();
  vector <Monster> health_monsters;
  for (Monster monster : monsters){
    if (!monster._dead && monster._death_effects._health > 0){
      health_monsters.push_back(monster);
    }
  }
  return health_monsters;
}

Monster get_closest(vector<Monster> monsters) {
  int smallest_length = 25;
  Monster closest_monster;
  for (Monster monster: monsters) {
    vector<node_id_t> path = get_path(monster._location);
    size_t length = path.size();
    if (length < smallest_length) {
      smallest_length = length;
      closest_monster = monster;
    }
  }
  return closest_monster;
}

string set_stance(node_id_t destination) {
  node_id_t node = SELF._location;
  if (SELF._movement_counter == SELF._speed + 1) {
    node = destination;
  }
  if (API->has_monster(node)) {
    Monster monster = API->get_monster(node);
    return get_weakness(monster._stance); 
  }
  return get_random_stance();
}

int time_to_target(node_id_t node) {
  vector<node_id_t> path = get_path(node);
  int player_speed = 7 - SELF._speed;
  int current_progress = path[0] == SELF._desitnation ? time_to_next_move(SELF) : 0;
  return player_speed * path.size() - current_progress;
}

node_id_t get_path_player() {
  vector<node_id_t> path = get_path(OPPONENT._location);
  return path[0];
}

node_id_t get_step_towards_monster(Monster monster) {
  vector<node_id_t> path = get_path(monster._location);
  return path[0];
}

void update_history(Strategy &strategy) {
  if (SELF._location == OPPONENT._location && SELF._stance != "Invalid Stance") {
    strategy.add_history(SELF._stance, OPPONENT._stance);
  }
}

bool will_engage(node_id_t next) {
  node_id_t myLocation = SELF._location;
  if (SELF._movement_counter == SELF._speed + 1) {
    // fprintf(stderr, "Calculating from destination\n");
    myLocation = next;
  }
  if (myLocation == OPPONENT._location) {
    // fprintf(stderr, "SAME LOCATION\n");
    return true;
  }
  vector<node_id_t> neighbors = API->get_adjacent_nodes(myLocation);
  if (OPPONENT._movement_counter == 1) {
    for (node_id_t node : neighbors) {
      if (node == OPPONENT._location) {
        return true;
      }
    }
  }
  return false;
}

Monster get_closest_monster(bool not_current = false) {
  vector<Monster> closestMonsters = API->nearest_monsters(SELF._location, 0);
  vector<node_id_t> adjacent = API->get_adjacent_nodes(SELF._location);
  if (not_current) {
    for (node_id_t node : adjacent) {
      vector<Monster> adjacentMonsters = API->nearest_monsters(node, 0);
      for (Monster monster: adjacentMonsters) {
        if (monster._location == SELF._location) {
          continue;
        }
        if (!monster._dead || monster._respawn_counter <= time_to_target(SELF._location, monster._location)) {
          return monster;
        }
      }  
    }
  }

  for (Monster monster: closestMonsters) {
    if (!monster._dead || monster._respawn_counter <= time_to_target(SELF._location, monster._location)) {
      return monster;
    }
  }
  closestMonsters = API->nearest_monsters(SELF._location, 1);
  return closestMonsters[rand() % closestMonsters.size()];
}

int main() {
  Strategy strategy;
  GameStateEngine gameState;
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
      SELF = API->get_self();
      OPPONENT = API->get_opponent();
      update_history(strategy);
			 //YOUR CODE HERE
      // vector <Monster> healthMonsters = get_health_monsters();
      // Monster closestMon = get_closest(healthMonsters);

      bool find_next = false;
      if (CURRENT_TARGET_MONSTER._name != "") {
        CURRENT_TARGET_MONSTER = API->nearest_monsters(SELF._location, CURRENT_TARGET_MONSTER._name, 0)[0];
        bool isHealing = CURRENT_TARGET_MONSTER._name == "Health 0" && SELF._health < 50;
        if (CURRENT_TARGET_MONSTER._dead && !isHealing) {
          if (SELF._health < 50) {
            CURRENT_TARGET_MONSTER = API->nearest_monsters(SELF._location, "Health 0", 0)[0];
          } else {
            CURRENT_TARGET_MONSTER = get_closest_monster();
          }
        }
        // if we're at the same place already
        if (CURRENT_TARGET_MONSTER._location == SELF._location) {
          if (turns_to_kill(SELF, CURRENT_TARGET_MONSTER) < time_to_next_move(SELF)) {
            find_next = true;
          }
        }
      } else {
        CURRENT_TARGET_MONSTER = get_closest_monster();
      }

      node_id_t target = get_step_towards_monster(CURRENT_TARGET_MONSTER);
      // still want stance to depend on current monster
      string stance = set_stance(target);

      if (find_next) {
        if (self._destination != self._location) {
          target = self._destination;
        }
        target = get_step_towards_monster(get_closest_monster(true));
      }

      if (will_engage(target)) {
        stance = strategy.get_stance(OPPONENT);
      }
      API->submit_decision(target, stance); //CHANGE THIS
      fflush(stdout);
      free(buf);
    }
  }
}

