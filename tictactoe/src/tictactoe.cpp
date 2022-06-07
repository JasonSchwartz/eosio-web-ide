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

bool is_draw(std::vector<uint8_t> board){
    return (std::find(board.begin(), board.end(), 0) == board.end());

}

bool is_empty_cell(std::vector<uint8_t> board, uint16_t position){
    return board[position] == 0;
}

bool is_winning_move(std::vector<uint8_t> board, uint8_t symbol){
    // check rows first
    for (int rows = 0; rows<3; rows++){
        uint16_t counter = 0;
        for (int columns = 0; columns<3; columns++){
            uint16_t position = columns + 3*rows;
            if (board[position] == symbol) counter ++;
        }
        if (counter == 3) return true;
    }
    // check columns
    for (int columns = 0; columns<3; columns++){
        uint16_t counter = 0;
        for (int rows = 0; rows<3; rows++){
            uint16_t position = columns + 3*rows;
            if (board[position] == symbol) counter ++;
        }
        if (counter == 3) return true;
    }
    // get diagnals manually
    if (board[4] == symbol){
        if (board[0] == symbol &&  board[8] == symbol) return true;
        if (board[6] == symbol &&  board[8] == symbol) return true;
    }
    return false;
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
        element.winner = "none"_n;
        element.board = initBoard();
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

    game_t games{get_self(), get_self().value};

    // find current game
    auto itr_game = games.find(host.value + challenger.value);
    check(itr_game != std::end(games), "No match between host and challenger to reset");

    check(itr_game->winner == name("none"), "Game has already been won");
    check(itr_game->turn == by, "It's not your turn");

    auto board = itr_game->board;

    check(column < 3 && row < 3, "proposed position out of bounds");

    uint16_t position = column + 3*row;
    check(is_empty_cell(board, position), "Position on board already in use");

    // game play
    uint8_t symbol;
    name next_turn;
    if (by == host) {
        symbol = 1;
        next_turn = challenger;
    }
    if (by == challenger) {
        symbol = 2;
        next_turn = host;
    }

    //place symbol on board
    board[position] = symbol;

    bool is_won = is_winning_move(board, symbol);

    bool is_draw_game = false;
    if (!is_won) is_draw_game = is_draw(board); 

    if (!is_won && !is_draw_game){
        games.modify(itr_game, get_self(), [&](auto& element) {
            element.turn = next_turn; // host has the first turn
            element.board = board;  // re-initialize the board
        });
    } else if (is_won) {
        games.modify(itr_game, get_self(), [&](auto& element) {
            element.turn = next_turn; // host has the first turn
            element.winner = by;
            element.board = board; 
        });
    } else if (is_draw_game){
        games.modify(itr_game, get_self(), [&](auto& element) {
            element.turn = next_turn; // host has the first turn
            element.board = board; 
        });
    }
}

ACTION tictactoe::restart(name challenger, name host, name by){
    require_auth(by);
    check(by == challenger || by == host, "You are not authorized to restart game");

    //find the game between challanger and host
    game_t games{get_self(), get_self().value};

    auto itr_game = games.find(host.value + challenger.value);

    check(itr_game != std::end(games), "No match between host and challenger to reset");
    
    // Passed our checks
    games.modify(itr_game, get_self(), [&](auto& element) {
        element.turn = host; // host has the first turn
        element.winner = "none"_n; // host has the first turn
        element.board = initBoard();  // re-initialize the board
    });

}
//for easy debugging
// ACTION tictactoe::delall(){
//     game_t games{get_self(), get_self().value};

//     auto it = games.begin();
//     while (it != games.end()) {
//         it = games.erase(it);
//     }
// }