#include <eosio/eosio.hpp>

using namespace eosio;

CONTRACT tictactoe : public contract {
    public:
        using contract::contract;

        // define action constructors
        ACTION create(name challenger, name host);

        ACTION close(name challenger, name host);

        ACTION move(name challenger, name host, name by, uint16_t row, uint16_t column);

        ACTION restart(name challenger, name host, name by);

        //ACTION delall();
    
    private:
        // Row ID is a unique ID that will allow us to iterate over all rows
        // for simplicity our PK will be host.value + challanger.value
        // This is bad practice because potential for clash
        // for example 2 + 3 = 5 and 1 + 4 = 5, 
        // a benefit of this is that a record can always be found
        // host.value + challanger.value ==  challanger.value + host.value
        // concatinating names won't work because EOSIO length limits
        TABLE game_s {
            uint64_t row_id;
            name challenger;
            name host;
            name turn;
            name winner;
            std::vector<uint8_t> board;
            uint64_t time_of_move;
        uint64_t primary_key() const { return row_id; }
        uint64_t get_secondary_1 () const { return host.value;}
        uint64_t get_secondary_2 () const { return challenger.value;}
        };

        //using game_t = eosio::multi_index<"game"_n, game_s>;

        typedef eosio::multi_index<"game"_n, game_s
            ,eosio::indexed_by<"byhost"_n,eosio::const_mem_fun<game_s, uint64_t, &game_s::get_secondary_1>>
            ,eosio::indexed_by<"bychallenger"_n,eosio::const_mem_fun<game_s, uint64_t, &game_s::get_secondary_2>>> game_t;
        
        game_t game{get_self(), get_self().value};


        // Leaderboard Table
        // Scoped to contract account
        TABLE leaderboard_s {
            name player;
            uint32_t num_wins;
            uint32_t num_losses;
            uint32_t num_ties;
            uint64_t primary_key() const { return player.value; }
        };
        using leaderboard_t = eosio::multi_index<"leaderboard"_n, leaderboard_s>;
        leaderboard_t leaderboard{get_self(), get_self().value};



        // iterate through user_a games 
        // returns true of user_a has a game against user_b 

        bool find_matches(name user_a, name user_b, bool flag_del);
        void end_of_game(name by, name next_turn, bool is_won);
};