#include <eosio/eosio.hpp>

using namespace eosio;

CONTRACT tictactoe : public contract {
    public:
        using contract::contract;

        // define action constructors
        ACTION welcome(name host, name user);

        ACTION create(name challenger, name host);

        ACTION close(name challenger, name host);

    
    private:
        TABLE game_s {
            name host;
            name challenger;
        uint64_t primary_key() const { return host.value; }
        };

        using game_t = eosio::multi_index<"game"_n, game_s>;
        game_t inventory{get_self(), get_self().value};

};