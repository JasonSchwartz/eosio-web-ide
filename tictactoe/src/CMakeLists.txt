project(tictactoe)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( tictactoe tictactoe tictactoe.cpp )
target_include_directories( tictactoe PUBLIC ${CMAKE_SOURCE_DIR}/../include )
target_ricardian_directory( tictactoe ${CMAKE_SOURCE_DIR}/../ricardian )