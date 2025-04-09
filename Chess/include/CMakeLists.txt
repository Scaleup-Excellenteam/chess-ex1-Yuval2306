target_sources (Chess PRIVATE "Chess.h"
        "Piece.h"
        "Board.h"
        "Game.h")

target_include_directories (Chess PRIVATE ${CMAKE_CURRENT_LIST_DIR})