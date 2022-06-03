#include <eosio/eosio.hpp>

using namespace eosio;

CONTRACT toyhaus : public contract {
    public:
        using contract::contract;

        ACTION welcome (name arrival){
            require_auth(name("toyboss"));
            check(arrival == name("toyboss"), "Don't lie, You are not the boss!");
            print("Welcome to the toyhaus");

        }

};