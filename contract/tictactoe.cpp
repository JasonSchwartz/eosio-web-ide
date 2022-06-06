#include <eosio/eosio.hpp>
#include <string>
#include <include/tictactoe.hpp>

using namespace eosio;


bool tictactoe::find_matches(name user_a, name user_b, bool flag_del){

    bool found_match = false;

    // instance of games table
    game_t games{get_self(), get_self().value};

    auto it = games.begin();
    while (it != games.end()) {
        if (user_a == it->host && user_b == it->challenger){
            found_match = true;
            if (flag_del){
                games.erase(it);
                return found_match;
            }
            break;
        } else if (user_a == it->challenger && user_b == it->host) {
            found_match = true;
            if (flag_del){
                games.erase(it);
                return found_match;
            }
            break;
        }
        it++;
    }
    return found_match;
}


ACTION tictactoe::welcome(name host, name user){
    require_auth(get_self());
    print("Welcome, challengers ", name(host), " and ", name(user), "!");
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
        element.row_id = games.available_primary_key();
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