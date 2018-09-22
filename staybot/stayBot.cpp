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

using namespace std;
Game_Api * API;
Player PLAYER_SELF("", 0,0,0,0, NULL);

int main() {
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
      API->submit_decision(0, "Rock"); //CHANGE THIS
      fflush(stdout);
      free(buf);
    }
  }
}

