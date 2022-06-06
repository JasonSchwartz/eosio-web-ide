#include <eosio/eosio.hpp>

using namespace eosio;

CONTRACT tictactoe : public contract {
    public:
        using contract::contract;

        // define action constructors
        ACTION welcome(name host, name user);

        ACTION create(name challenger, name host);

        ACTION close(name challenger, name host);

        //ACTION delall();

    
    private:
        // Row ID is a unique ID that will allow us to iterate over all rows
        // this won't work for large tables, but will work fine here
        // obviously secondary ids are the way to go
        TABLE game_s {
            uint64_t row_id;
            name host;
            name challenger;
        uint64_t primary_key() const { return row_id; }
        };

        using game_t = eosio::multi_index<"game"_n, game_s>;
        game_t inventory{get_self(), get_self().value};

        // iterate through user_a games 
        // returns true of user_a has a game against user_b 

        bool find_matches(name user_a, name user_b, bool flag_del);

};