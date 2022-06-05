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

    game_t games{get_self(), get_self().value};
    
    //find the hosts games
    auto host_games = games.find(host.value);

    //find the hosts games
    auto challenger_games = games.find(challenger.value);

    check(host_games == std::end(games), "Host is playing another game...");
    check(challenger_games == std::end(games), "challenger is playing another game...");

    // Because we don't have a secondary index we need to iterate through the table and check each value
    auto it = games.begin();
    while (it != games.end()) {
        check(it->challenger != host, "Found host as challenger in other game");
        check(it->challenger != challenger, "Found challenger as challenger in other game");
        it++;
    }
    // Passed our checks

    games.emplace(get_self(), [&](auto& element) {
        element.host = host; 
        element.challenger = challenger;
    });
}

ACTION tictactoe::close (name challenger, name host){
    check(has_auth(host) || has_auth(challenger), "Only host or challanger can delete game");

    //check that the host and challenger are not the same
    check(challenger != host, "Host and challenger must be different");

    game_t games{get_self(), get_self().value};
    
    //Both host and challanger can delete a game
    auto host_games = games.find(host.value);
    auto challenger_games = games.find(challenger.value);

    //If the host is found iterate through records to find challanger
    if (host_games != std::end(games)){
        while (host_games != games.end()) {
            if (challenger == host_games->challenger){
                 games.erase(host_games);
                 break;
            }
            host_games++;
        }
    } else if (challenger_games != std::end(games){
        while (challenger_games != games.end()) {
            if (host == challenger_games->host){
                 games.erase(challenger_games);
                 break;
            }
            it++;
        }

    }




    


    check(host_games == std::end(games) , "Host is playing another game...");
    check(challenger_games == std::end(games), "challenger is playing another game...");


   
}