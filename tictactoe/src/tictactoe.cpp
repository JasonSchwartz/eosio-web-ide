#include <tictactoe.hpp>
#include <eosio/eosio.hpp>
#include <string>

using namespace eosio;

bool tictactoe::find_matches(name user_a, name user_b, bool flag_del){

    bool found_match = false;

    // instance of games table
    game_t games{get_self(), get_self().value};

    auto itr = games.find(user_a.value);
    if (itr != std::end(games)){
    // can we find the match by host?
        if (user_b == itr -> challenger){
            found_match = true;
            if (flag_del){
                games.erase(itr);
            }
            return found_match;
        }
        // avoid assertion error if host already is hosting a game
        if (!flag_del){
            check(0, "Host cant host more than one game at a time");
        }
    }
    // if the host wasn't found in any games
    // iterate through challangers and search for host
    auto idx = games.get_index<"secid"_n>();
    for ( auto itr = idx.begin(); itr != idx.end(); itr++ ) {
        // print each row's values
        // eosio::print_f("Games Table : {%, %}\n", itr->host, itr->challenger);
        // print("print in loop");
        if (user_a == itr->challenger && user_b == itr->host){
            found_match = true;
            if (flag_del){
                auto itr_ = games.find(user_b.value);
                games.erase(itr_);
            }
           return found_match; 
        }
    } 
    return found_match;
}

ACTION tictactoe::create(name challenger, name host){
    require_auth(host);

    // check that the host and challenger are not the same
    check(challenger != host, "Host and challenger must be different");

    bool found_match_host = find_matches(host, challenger, false);

    check(!(found_match_host), "host and challanger already in match");

    game_t games{get_self(), get_self().value};
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

    bool found_match = find_matches(host, challenger, true);

    check(found_match, "Host and Challanger don't have game to remove...");

}

//for easy debugging
// ACTION tictactoe::delall(){
//     game_t games{get_self(), get_self().value};

//     auto it = games.begin();
//     while (it != games.end()) {
//         it = games.erase(it);
//     }
// }