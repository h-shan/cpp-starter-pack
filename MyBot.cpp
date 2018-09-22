#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <string>
#include <vector>
#include <climits>
#include <math.h>
#include <time.h>
#include <map>

#include "Game_Api.h"
#include "util.h"
#include "strategy.h"
#include "StateMachine.h"

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
Monster NEXT_TARGET_MONSTER;
StateMachine STATE_MACHINE;
Strategy STRATEGY;

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

bool should_pursuit(){
  //fprintf(stderr, "Get_advantage: %f\n", get_advantage());
  return get_advantage() >= 0.75;
}

node_id_t pursuit(){
  if (SELF._location == OPPONENT._location){
    if (time_to_next_move(SELF) <= time_to_next_move(OPPONENT)){
	    return SELF._location;
    }
	} else {
    if (SELF._destination == SELF._location){
	    vector<node_id_t> adjacent = API->get_adjacent_nodes(SELF._location);
	    return adjacent[0];
    }
	  return SELF._destination;
	}

  vector<vector<node_id_t>> paths = API->shortest_paths(SELF._location, OPPONENT._location);
  for (vector<node_id_t> path : paths) {
    if (path[0] == SELF._destination)
      return path[0];
  }
  return paths[0][0];
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
  int current_progress = path[0] == SELF._destination ? time_to_next_move(SELF) : 0;
  return player_speed * path.size() - current_progress;
}

node_id_t get_step(node_id_t node) {
  vector<node_id_t> path = get_path(node);
  return path[0];
}

void update_history() {
  if (SELF._location == OPPONENT._location && SELF._stance != "Invalid Stance") {
    STRATEGY.add_history(SELF._stance, OPPONENT._stance);
  }
}

bool will_engage(node_id_t next) {
  node_id_t myLocation = SELF._location;
  if (SELF._movement_counter == SELF._speed + 1) {
    // ////fprintf(stderr, "Calculating from destination\n");
    myLocation = next;
  }
  if (myLocation == OPPONENT._location) {
    // ////fprintf(stderr, "SAME LOCATION\n");
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
  ////fprintf(stderr, "get_closest_monster start\n");
  vector<Monster> closestMonsters = API->nearest_monsters(SELF._location, 0);
  vector<node_id_t> adjacent = API->get_adjacent_nodes(SELF._location);
  if (not_current) {
    for (node_id_t node : adjacent) {
      vector<Monster> adjacentMonsters = API->nearest_monsters(node, 0);
      for (Monster monster: adjacentMonsters) {
        if (monster._location == SELF._location) {
          continue;
        }
        if (!monster._dead || monster._respawn_counter <= time_to_target(monster._location)) {
          ////fprintf(stderr, "get_closest_monster end*****************\n");
          return monster;
        }
      }  
    }
  }

  for (Monster monster: closestMonsters) {
    if (!monster._dead || monster._respawn_counter <= time_to_target(monster._location)) {
      ////fprintf(stderr, "get_closest_monster end\n");
      return monster;
    }
  }
  closestMonsters = API->nearest_monsters(SELF._location, 1);
  ////fprintf(stderr, "get_closest_monster end\n");
  return closestMonsters[rand() % closestMonsters.size()];
}

void update_game_state(){
  if (SELF._health < 75){
    STATE_MACHINE.set_state(State::HEALING);
    API->log("HEALING");
  } else if (should_pursuit()){
    STATE_MACHINE.set_state(State::PURSUIT);
    API->log("PURSUIT");
  }
  else {
    STATE_MACHINE.set_state(State::STANDARD);
    API->log("STANDARD");
  }
}

void handle_standard(){
  //fprintf(stderr, "Begin standard\n");
  node_id_t target;
  if (SELF._destination != SELF._location) {
    target = SELF._destination;
  } else {
    Monster monster = get_closest_monster(true);
    //fprintf(stderr, "Mark 1\n");
    target = monster._location;
    //fprintf(stderr, "Mark 2, %d\n", target);
    
    if (API->has_monster(SELF._location) && !API->get_monster(SELF._location)._dead) {
      if (turns_to_kill(SELF, API->get_monster(SELF._location)) < time_to_next_move(SELF)) {
        //fprintf(stderr, "Option 1\n");
        target = get_step(target);
      } else {
        //fprintf(stderr, "Option 2\n");
        target = SELF._location;
      }
    } else {
      //fprintf(stderr, "Option 3\n");
      target = get_step(target);
    }
    //fprintf(stderr, "Mark 3\n");
  }
  string stance = set_stance(target);
    //fprintf(stderr, "Mark 4\n");

  if (will_engage(target)) {
    stance = STRATEGY.get_stance(OPPONENT);
  }
  //fprintf(stderr, "End standard\n");
  API->submit_decision(target, stance);
}

void handle_pursuit(){
  //fprintf(stderr, "Begin pursuit\n");
  node_id_t target = OPPONENT._location;

  if (API->has_monster(SELF._location) && !API->get_monster(SELF._location)._dead) {
    if (turns_to_kill(SELF, API->get_monster(SELF._location)) < time_to_next_move(SELF)) {
      target = get_step(target);
    } else {
      target = SELF._location;
    }
  } else {
    target = get_step(target);
  }
  string stance = set_stance(target);

  if (will_engage(target)) {
    stance = STRATEGY.get_stance(OPPONENT);
  }
  //fprintf(stderr, "End pursuit\n");
  API->submit_decision(target, stance); //CHANGE THIS
}


void handle_healing() {
  node_id_t target;
  // check if there is a monster at my location and if it is still alive
  if (API->has_monster(SELF._location) && !API->get_monster(SELF._location)._dead) {
	  		// Check if I will kill the monster in time before next turn. 
    if (turns_to_kill(SELF, API->get_monster(SELF._location)) < time_to_next_move(SELF)) {
			target = get_step(0);

    } else {
      target = SELF._location;
    }
  }  else {
    target = get_step(0);
  }
  if (SELF._location == 0){
	  if (SELF._destination == 0){
		  int minRespawn = 50;
		  Monster bestMonster;
		  vector<int> potentialTargets{1, 6, 10};
		  for (int potentialTarget : potentialTargets){
			Monster monster = API->get_monster(potentialTarget);
			if (monster._respawn_counter < minRespawn){
				minRespawn = monster._respawn_counter;
				bestMonster = monster;
			}
		  }
		
		int timeToKill = time_to_target(bestMonster._location) + get_play_speed(SELF);
		int healthRespawn = get_monster(0)._respawn_counter;
		if (timeToKill < healthRespawn){
			target = bestMonster._location;
		}
		
	  } else {
		  target = SELF._destination;
	  }
  }
  
  string stance = set_stance(target);
  if (will_engage(target)) {
    stance = STRATEGY.get_stance(OPPONENT);
  }
  API->submit_decision(target, stance);
}

int main() {
  int my_player_num = 0;
  ////fprintf(stderr, "Mark 1\n");
  
  while(1){
		char* buf = NULL;
		size_t size = 0;
		getline(&buf, &size, stdin);
		json data = json::parse(buf);
		if(data["type"] == "map"){
			my_player_num = data["player_id"];
			API = new Game_Api(my_player_num, data["map"]);
		} else {
        ////fprintf(stderr, "Mark 2\n");

			API->update(data["game_data"]);
      SELF = API->get_self();
      OPPONENT = API->get_opponent();
      update_history();
  ////fprintf(stderr, "Mark 3\n");

      update_game_state();
      switch (STATE_MACHINE.get_state()){
        case State::STANDARD:
          handle_standard();
          break;
        case State::PURSUIT:
          handle_pursuit();
          break;
        case State::HEALING:
          handle_healing();
          break;
      }
      fflush(stdout);
      free(buf);
    }
  }
}

