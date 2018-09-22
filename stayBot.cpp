#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "Game_Api.h"
#define RESPONSE_SECS 1
#define RESPONSE_NSECS 0
#include <iostream>
#include <string>
#include <vector>
#include <climits>
#include <math.h>
#include <time.h>
#include <map>

using namespace std;
//You may add global variables and helper functions
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

