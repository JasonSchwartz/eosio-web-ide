#include <eosio/eosio.hpp>
#include <string>
#include <include/tictactoe.hpp>

using namespace eosio;

ACTION tictactoe::welcome(name host, name user){
    require_auth(get_self());
    print("Welcome, challengers ", name(host), " and ", name(user), "!");
}

ACTION tictactoe::create(name challenger, name host){
    require_auth(host);

    // check that the host and challenger are not the same
    check(challenger != host, "Host and challenger must be different");

    game_t games{get_self(), host.value};
    
    //find the hosts games
    auto host_games = games.find(host.value);

    //find the hosts games
    auto challenger_games = games.find(challenger.value);

    check(host_games != std::end(games), "Host is playing another game...");
    check(challenger_games != std::end(games), "challenger is playing another game...");

    // Because we don't have a secondary index we need to iterate through the table and check each value

    auto it = games.begin();
    while (it != games.end()) {
        check(it->challenger != host, "Found host as challenger in other game");
        check(it->challenger != challenger, "Found challenger as challenger in other game");
    }
    // Passed our checks

    games.emplace(get_self(), [&](auto& element) {
        element.host = host; 
        element.challenger = challenger;
    });
}

ACTION tictactoe::close (name challenger, name host){
    require_auth(host);

    //check that the host and challenger are not the same
    check(challenger != host, "Host and challenger must be different");

    game_t games{get_self(), host.value};
    
    //find the hosts games
    auto host_games = games.require_find(host.value, "Not a host in the games table, only host can delete game");

    games.erase(host_games);
}