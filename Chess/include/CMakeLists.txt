target_sources (ChessGame  PRIVATE
        "Chess.h"
        "Piece.h"
        "Board.h"
        "Game.h"
        "MoveRecommender.h"
        "PriorityQueue.h"
        "Move.h"
)

target_include_directories (ChessGame  PRIVATE ${CMAKE_CURRENT_LIST_DIR})