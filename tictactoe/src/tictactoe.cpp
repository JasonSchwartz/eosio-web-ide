#include <tictactoe.hpp>
#include <eosio/eosio.hpp>
#include <string>

using namespace eosio;

// helper functions

std::vector<uint8_t> initBoard(){
    std::vector<uint8_t> board;
    board.assign(9, 0);
    return board;
}

bool tictactoe::find_matches(name user_a, name user_b, bool flag_del){

    bool found_match = false;

    // instance of games table
    game_t games{get_self(), get_self().value};

    auto itr = games.find(user_a.value + user_b.value);
    if (itr != std::end(games)){
        // can we find the match by host?
        found_match = true;
        if (flag_del){
            games.erase(itr);
        }
        return found_match;
        // avoid assertion error if host already is hosting a game
        if (!flag_del){
            check(0, "Host cant host more than one game at a time");
        }
    }
    // if the host wasn't found in any games
    // iterate through challangers and search for host
    auto idx = games.get_index<"byhost"_n>();
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

    auto idx_ch = games.get_index<"bychallenger"_n>();
    for ( auto itr = idx_ch.begin(); itr != idx_ch.end(); itr++ ) {
        // print each row's values
        // eosio::print_f("Games Table : {%, %}\n", itr->host, itr->challenger);
        // print("print in loop");
        if (user_a == itr->host && user_b == itr->challenger){
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
        element.row_id = host.value + challenger.value;
        element.challenger = challenger;
        element.host = host; 
        element.turn = host; // host has the first turn
        //element.winner = "none"_n;
        //element.board = initBoard();
    });
}

ACTION tictactoe::close (name challenger, name host){
    check(has_auth(host) || has_auth(challenger), "Only host or challanger can delete game");

    //check that the host and challenger are not the same
    check(challenger != host, "Host and challenger must be different");

    bool found_match = find_matches(host, challenger, true);

    check(found_match, "Host and Challanger don't have game to remove...");

}

ACTION tictactoe::move(name challenger, name host, name by, uint16_t row, uint16_t column){
    require_auth(by);
    check(by == challenger || by == host, "You are not authorized to move in this game");

}

ACTION tictactoe::restart(name challenger, name host, name by){
    require_auth(by);
    check(by == challenger || by == host, "You are not authorized to restart game");

}

//for easy debugging
ACTION tictactoe::delall(){
    game_t games{get_self(), get_self().value};

    auto it = games.begin();
    while (it != games.end()) {
        it = games.erase(it);
    }
}