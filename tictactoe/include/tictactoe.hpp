#include <eosio/eosio.hpp>

using namespace eosio;

CONTRACT tictactoe : public contract {
    public:
        using contract::contract;

        // define action constructors
        ACTION welcome(name challenger, name host);

        ACTION create(name challenger, name host);

        ACTION close(name challenger, name host);

        //ACTION delall();

    
    private:
        // Row ID is a unique ID that will allow us to iterate over all rows
        // this won't work for large tables, but will work fine here
        // obviously secondary ids are the way to go
        TABLE game_s {
            name host;
            name challenger;
        uint64_t primary_key() const { return host.value; }
        uint64_t by_secondary () const { return challenger.value;}
        };

        //using game_t = eosio::multi_index<"game"_n, game_s>;

        typedef eosio::multi_index<"game"_n, game_s, eosio::indexed_by<"secid"_n, eosio::const_mem_fun<game_s, uint64_t, &game_s::by_secondary>>> game_t;
        game_t inventory{get_self(), get_self().value};

        // iterate through user_a games 
        // returns true of user_a has a game against user_b 

        bool find_matches(name user_a, name user_b, bool flag_del);

};