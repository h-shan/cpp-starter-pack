//
// Created by alvar on 9/1/2018.
//

#ifndef GAME_API_GAME_API_H
#define GAME_API_GAME_API_H
#include "includes/json.hpp"

#define BASE_MOVEMENT_COUNTER 7
#define INIT_PLAYER_HEALTH 100

using std::string;
using std::vector;
using json = nlohmann::json;

typedef int node_id_t;
class Game_Api {

public:

    struct DeathEffects {
        int _rock;
        int _paper;
        int _scissors;
        int _speed;
        int _health;
    };

    struct Monster {
        Monster(string name, int health, string stance, int speed, node_id_t location, int attack, json deathfx, Game_Api * api);
        Monster();
        void update(json unit_json);
        bool _dead;
        string _name;
        int _health;
        int _base_health;
        string _stance;
        int _respawn_counter;
        int _respawn_rate;
        node_id_t _location;
        int _attack;
        DeathEffects _death_effects;
        Game_Api* _api;
    };

    struct Player {
        Player(string name, int health, int speed, node_id_t location, int movement_counter, Game_Api* api);
        void update(json unit_json);
        bool _dead;
        string _name;
        int _health;
        string _stance;
        int _speed;
        node_id_t _location;
        node_id_t _destination;
        int _movement_counter;
        int _rock;
        int _paper;
        int _scissors;
        Game_Api* _api;
    };

    struct Node {
        vector<node_id_t> adjacent;
        vector<Player*> players;
        vector<Monster*> monsters;
    };

public:
    explicit Game_Api(int player_number, string json_str);
    void update(json json_str);
    void log(string message);
    void submit_decision(node_id_t destination, string stance);
    Player get_self();
    Player get_opponent();
    vector<node_id_t> get_adjacent_nodes(node_id_t location);
    vector<vector<node_id_t>> shortest_paths(node_id_t start, node_id_t destination);
    int get_duel_turn_num();
    int get_turn_num();
    vector<Monster> get_all_monsters();
    bool has_monster(node_id_t node);
    Monster get_monster(node_id_t node);
    vector<Monster> nearest_monsters(node_id_t node, int mode);
    vector<Monster> nearest_monsters(node_id_t node, string monster_type, int search_mode);

private:
    int _this_player_number;
    vector<Monster> all_monsters;
    vector<Node> nodes;
    Player _player1;
    Player _player2;
    int _turn_number;
};


#endif //MM2018_GAME_LOGIC_GAME_API_H
