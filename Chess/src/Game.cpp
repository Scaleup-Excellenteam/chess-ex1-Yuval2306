#include "Game.h"
#include <iostream>

Game::Game(const std::string& boardString) : m_board(boardString), m_isWhiteTurn(true) {}

// Convert from algebraic notation (e.g., 'a1') to array indices (0-7, 0-7)
std::pair<int, int> Game::algebraicToIndices(char file, char rank) {

    file = std::tolower(file);

    int row = '8' - rank;
    int col = file - 'a';

    return { row, col };
}

int Game::validateMove(const std::string& input) {
    // Convert input to lowercase for consistency
    std::string lowerInput = input;
    for (char& c : lowerInput) {
        c = std::tolower(c);
    }

    // In Chess.cpp, the input is adjusted so 'a'-'h' are rows 0-7
    // and '1'-'8' are columns 0-7, but rotated 90 degrees from standard notation

    int srcRow = lowerInput[0] - 'a';
    int srcCol = lowerInput[1] - '1';
    int dstRow = lowerInput[2] - 'a';
    int dstCol = lowerInput[3] - '1';


    // Get the piece at the source position
    auto piece = m_board.getPiece(srcRow, srcCol);

    // Check if there is a piece at the source
    if (piece == nullptr) {
        std::cout << "No piece at source [" << srcRow << "," << srcCol << "]" << std::endl;
        return 11; // There is no piece at the source
    }

    std::cout << "Piece at source: " << piece->getSymbol() << std::endl;

    // Check if the piece belongs to the current player
    if (piece->isWhite() != m_isWhiteTurn) {
        std::cout << "Piece belongs to opponent" << std::endl;
        return 12; // The piece belongs to the opponent
    }

    // Check if there is a friendly piece at the destination
    auto destPiece = m_board.getPiece(dstRow, dstCol);
    if (destPiece != nullptr && destPiece->isWhite() == m_isWhiteTurn) {
        std::cout << "Friendly piece at destination" << std::endl;
        return 13; // There is a friendly piece at the destination
    }

    // Check if the move is valid for this piece
    if (!piece->isValidMove(m_board, srcRow, srcCol, dstRow, dstCol)) {
        std::cout << "Invalid move for this piece" << std::endl;
        return 21; // Illegal movement for this piece
    }

    // Check if the move would put the player in check
    if (m_board.wouldBeInCheck(srcRow, srcCol, dstRow, dstCol, m_isWhiteTurn)) {
        std::cout << "Move would put player in check" << std::endl;
        return 31; // Move would result in check for the current player
    }

    // Check if the move would put the opponent in check
    if (m_board.causesCheckOnOpponent(srcRow, srcCol, dstRow, dstCol, m_isWhiteTurn)) {
        std::cout << "Move causes check" << std::endl;
        // The move is valid and causes check
        m_board.movePiece(srcRow, srcCol, dstRow, dstCol);
        m_isWhiteTurn = !m_isWhiteTurn;
        return 41;
    }

    std::cout << "Valid move" << std::endl;
    // The move is valid
    m_board.movePiece(srcRow, srcCol, dstRow, dstCol);
    m_isWhiteTurn = !m_isWhiteTurn;
    return 42;
}