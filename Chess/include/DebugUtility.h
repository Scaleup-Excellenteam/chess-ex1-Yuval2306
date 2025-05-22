#pragma once
#include <iostream>
#include <string>
#include "Board.h"
#include "Piece.h"
#include "Move.h"

// Utility class for debugging chess engine issues
class DebugUtility {
public:
    // Convert board position to algebraic notation
    static std::string positionToAlgebraic(int row, int col) {
        std::string result;
        result += static_cast<char>('a' + col);  // File (a-h)
        result += static_cast<char>('8' - row);  // Rank (1-8)
        return result;
    }

    // Convert algebraic notation to board position
    static std::pair<int, int> algebraicToPosition(const std::string& algebraic) {
        if (algebraic.length() < 2) return {-1, -1};

        int col = algebraic[0] - 'a';
        int row = '8' - algebraic[1];

        if (col < 0 || col > 7 || row < 0 || row > 7) return {-1, -1};
        return {row, col};
    }

    // Verify that a move is valid
    static bool isValidMove(const Board& board, int srcRow, int srcCol, int dstRow, int dstCol, bool isWhiteTurn) {
        // Check if source and destination are valid board positions
        if (srcRow < 0 || srcRow > 7 || srcCol < 0 || srcCol > 7 ||
            dstRow < 0 || dstRow > 7 || dstCol < 0 || dstCol > 7) {
            std::cout << "Debug: Invalid board position" << std::endl;
            return false;
        }

        // Check if there is a piece at the source
        auto piece = board.getPiece(srcRow, srcCol);
        if (!piece) {
            std::cout << "Debug: No piece at source " << positionToAlgebraic(srcRow, srcCol) << std::endl;
            return false;
        }

        // Check if the piece belongs to the current player
        if (piece->isWhite() != isWhiteTurn) {
            std::cout << "Debug: Piece at " << positionToAlgebraic(srcRow, srcCol)
                      << " belongs to " << (piece->isWhite() ? "white" : "black")
                      << " but it's " << (isWhiteTurn ? "white's" : "black's") << " turn" << std::endl;
            return false;
        }

        // Check if there is a friendly piece at the destination
        auto destPiece = board.getPiece(dstRow, dstCol);
        if (destPiece && destPiece->isWhite() == isWhiteTurn) {
            std::cout << "Debug: Friendly piece at destination " << positionToAlgebraic(dstRow, dstCol) << std::endl;
            return false;
        }

        // Check if the move is valid for this piece
        if (!piece->isValidMove(board, srcRow, srcCol, dstRow, dstCol)) {
            std::cout << "Debug: Invalid move for " << piece->getSymbol()
                      << " from " << positionToAlgebraic(srcRow, srcCol)
                      << " to " << positionToAlgebraic(dstRow, dstCol) << std::endl;
            return false;
        }

        // Check if the move would put the player in check
        if (board.wouldBeInCheck(srcRow, srcCol, dstRow, dstCol, isWhiteTurn)) {
            std::cout << "Debug: Move would put player in check" << std::endl;
            return false;
        }

        return true;
    }

    // Print a description of a piece at given position
    static void describePosition(const Board& board, int row, int col) {
        if (row < 0 || row > 7 || col < 0 || col > 7) {
            std::cout << "Invalid position" << std::endl;
            return;
        }

        auto piece = board.getPiece(row, col);
        std::string position = positionToAlgebraic(row, col);

        if (!piece) {
            std::cout << "No piece at " << position << std::endl;
            return;
        }

        std::cout << "Position " << position << " contains: "
                  << piece->getSymbol() << " ("
                  << (piece->isWhite() ? "white" : "black") << ")" << std::endl;
    }

    // Validate a complete set of recommended moves for debugging
    static void validateMoveRecommendations(const Board& board, const std::vector<Move>& moves, bool isWhiteTurn) {
        std::cout << "\n=== VALIDATING RECOMMENDED MOVES ===" << std::endl;

        if (moves.empty()) {
            std::cout << "No moves to validate" << std::endl;
            return;
        }

        int validCount = 0;
        int invalidCount = 0;

        for (const auto& move : moves) {
            int srcRow = move.getSrcRow();
            int srcCol = move.getSrcCol();
            int dstRow = move.getDstRow();
            int dstCol = move.getDstCol();

            std::cout << "Validating move: "
                      << positionToAlgebraic(srcRow, srcCol) << " to "
                      << positionToAlgebraic(dstRow, dstCol) << " (score: " << move.getScore() << ")... ";

            if (isValidMove(board, srcRow, srcCol, dstRow, dstCol, isWhiteTurn)) {
                std::cout << "VALID" << std::endl;
                validCount++;
            } else {
                std::cout << "INVALID" << std::endl;
                invalidCount++;
            }
        }

        std::cout << "Summary: " << validCount << " valid moves, " << invalidCount << " invalid moves" << std::endl;
    }
};