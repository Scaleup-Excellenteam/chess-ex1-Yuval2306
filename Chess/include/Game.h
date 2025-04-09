#pragma once
#include "Board.h"

class Game {
private:
    Board m_board;
    bool m_isWhiteTurn;

public:
    Game(const std::string& boardString);

    // Check if the move is valid and return the response code
    int validateMove(const std::string& input);

    // Note: This method is no longer used - I directly map from input(I'll leave it for history.)
    static std::pair<int, int> algebraicToIndices(char file, char rank);

    // Get the current turn
    bool isWhiteTurn() const { return m_isWhiteTurn; }
};