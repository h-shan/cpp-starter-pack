#ifndef MM_UTIL_H_
#define MM_UTIL_H_
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Game_Api.h"

using json = nlohmann::json;
using Player = Game_Api::Player;
using Monster = Game_Api::Monster;
using DeathEffects = Game_Api::DeathEffects;


#define RESPONSE_SECS 1
#define RESPONSE_NSECS 0

#include <string>
#include <vector>
#include <map>

int get_stat(Player player, string stance);

int get_total_strength(Player player);

int turns_to_kill(Player player, Monster monster);

int get_play_speed(Player player);

int time_to_next_move(Player player);

string get_random_stance();

string get_weakness(string stance);

string get_strongest_stat(Player player);
#endif

