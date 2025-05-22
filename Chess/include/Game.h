#pragma once
#include "Board.h"

class MoveRecommender;

class Game {
private:
    Board m_board;
    bool m_isWhiteTurn;
    MoveRecommender* m_recommender;
    int m_depthLevel;

    // Recommend moves based on current board state
    void recommendMoves();

public:
    Game(const std::string& boardString);
    ~Game(); // Need destructor to delete m_recommender

    // Set the search depth for move recommendations
    void setSearchDepth(int depth);

    // Check if the move is valid and return the response code
    int validateMove(const std::string& input);

    // Note: This method is no longer used - I'll leave it for history
    static std::pair<int, int> algebraicToIndices(char file, char rank);

    // Get the current turn
    bool isWhiteTurn() const { return m_isWhiteTurn; }
};