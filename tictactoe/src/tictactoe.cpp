#include <tictactoe.hpp>
#include <eosio/eosio.hpp>
#include <eosio/time.hpp>
#include <eosio/system.hpp>
#include <string>

using namespace eosio;

// helper functions

uint64_t epoch_time_seconds() {
   auto now = eosio::current_time_point();
   auto epoch_time_seconds = now.sec_since_epoch();
   return epoch_time_seconds;
}


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

void tictactoe::end_of_game(name by, name next_turn, bool is_won){
    leaderboard_t leaderboard{get_self(), get_self().value};
    auto find_by = leaderboard.find(by.value);
    auto find_next_turn = leaderboard.find(next_turn.value);

    uint8_t game_won = 0;
    uint8_t game_tie = 0;
    if (is_won){
        game_won = 1;
    } else {
        game_tie = 1;
    }
    // Update for the current player.
    // Only two possible outcomes, win or tie
    if (find_by == std::end(leaderboard)){
        leaderboard.emplace(get_self(), [&](auto& element) {
            element.player = by;
            element.num_wins = game_won;
            element.num_losses = 0;
            element.num_ties = game_tie; 
        });
    } else {
        leaderboard.modify(find_by, get_self(), [&](auto& element) {
            element.num_wins = find_by -> num_wins + game_won;
            element.num_ties = find_by -> num_ties + game_tie; 
        });
    }
    // Update for the current player.
    // Only two possible outcomes, loss or tie
    if (find_next_turn == std::end(leaderboard)){
        leaderboard.emplace(get_self(), [&](auto& element) {
            element.player = next_turn;
            element.num_wins = 0;
            element.num_losses = game_won;  // A win for current player is a loss
            element.num_ties = game_tie; 
        });
    } else {
        leaderboard.modify(find_next_turn, get_self(), [&](auto& element) {
            element.num_losses = find_next_turn -> num_losses + game_won;
            element.num_ties = find_next_turn -> num_ties + game_tie; 
        });
    }
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
        element.time_of_move = epoch_time_seconds();
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

    //Has it been too long between moves?
    uint64_t time_delta = epoch_time_seconds()- itr_game -> time_of_move;

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

    // You lose if you take more than 60 seconds
    // to make your move
    if (time_delta > 60){

        print("You took too long, the winner is {%}", next_turn);

        //write a blank board and declare a winner
        games.modify(itr_game, get_self(), [&](auto& element) {
            element.turn = next_turn; // host has the first turn
            element.winner = next_turn;
            element.board = initBoard(); 
            element.time_of_move = epoch_time_seconds();
        });

        end_of_game(next_turn, by, true);
        return;
    }

    //place symbol on board
    board[position] = symbol;

    bool is_won = is_winning_move(board, symbol);
    bool is_draw_game = false;
    if (!is_won) is_draw_game = is_draw(board); 

    // Update leaderboard on game end
    bool is_game_end = is_won || is_draw_game;

    if (!is_won && !is_draw_game){
        games.modify(itr_game, get_self(), [&](auto& element) {
            element.turn = next_turn; // host has the first turn
            element.board = board;  // re-initialize the board
            element.time_of_move = epoch_time_seconds();
        });
    } else if (is_won) {
        games.modify(itr_game, get_self(), [&](auto& element) {
            element.turn = next_turn; // host has the first turn
            element.winner = by;
            element.board = board; 
            element.time_of_move = epoch_time_seconds();
        });
    } else if (is_draw_game){
        games.modify(itr_game, get_self(), [&](auto& element) {
            element.turn = next_turn; // host has the first turn
            element.board = board;
            element.time_of_move = epoch_time_seconds();
        });
    }

    if (is_game_end) {
        end_of_game(by, next_turn, is_won);
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
        element.time_of_move = epoch_time_seconds();
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