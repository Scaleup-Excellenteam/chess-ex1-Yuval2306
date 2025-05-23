#pragma once
#include "Board.h"
#include <vector>
#include <memory>

class ParallelMoveRecommender;
class Move;

class Game {
private:
    Board m_board;
    bool m_isWhiteTurn;
    std::unique_ptr<ParallelMoveRecommender> m_recommender;
    int m_depthLevel;
    std::vector<Move> m_lastRecommendedMoves; // Store last recommendations

public:
    Game(const std::string& boardString, int numThreads = 4);
    ~Game();

    // Set the search depth for move recommendations
    void setSearchDepth(int depth);


    // Check if the move is valid and return the response code
    int validateMove(const std::string& input);

    // Get the current turn
    bool isWhiteTurn() const { return m_isWhiteTurn; }

    // Recommend moves and return them
    std::vector<Move> recommendMoves();
};