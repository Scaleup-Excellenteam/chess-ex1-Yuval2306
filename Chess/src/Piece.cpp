#include "Piece.h"
#include "Board.h"
#include <cmath>
#include <iostream>

bool Piece::isPathClear(const Board& board, int srcRow, int srcCol, int dstRow, int dstCol) const {
    int rowDir = 0;
    if (srcRow < dstRow) rowDir = 1;
    else if (srcRow > dstRow) rowDir = -1;

    int colDir = 0;
    if (srcCol < dstCol) colDir = 1;
    else if (srcCol > dstCol) colDir = -1;

    int row = srcRow + rowDir;
    int col = srcCol + colDir;

    while (row != dstRow || col != dstCol) {
        if (board.getPiece(row, col) != nullptr) {
            return false;
        }
        row += rowDir;
        col += colDir;
    }

    return true;
}

bool Pawn::isValidMove(const Board& board, int srcRow, int srcCol, int dstRow, int dstCol) const {
    // In the board representation used by Chess
    int direction = m_isWhite ? 1 : -1;  // White moves down, black moves up

    // Normal move: one square forward
    if (srcCol == dstCol && srcRow + direction == dstRow) {
        bool valid = board.getPiece(dstRow, dstCol) == nullptr;
        return valid;
    }

    // First move: two squares forward
    if (srcCol == dstCol &&
        ((m_isWhite && srcRow == 1 && dstRow == 3) ||
         (!m_isWhite && srcRow == 6 && dstRow == 4))) {
        bool middleEmpty = board.getPiece(srcRow + direction, srcCol) == nullptr;
        bool destEmpty = board.getPiece(dstRow, dstCol) == nullptr;
        bool valid = middleEmpty && destEmpty;
        std::cout << "Two squares forward: " << (valid ? "valid" : "invalid") << std::endl;
        return valid;
    }

    // Capture: one square diagonally
    if (std::abs(srcCol - dstCol) == 1 && srcRow + direction == dstRow) {
        auto targetPiece = board.getPiece(dstRow, dstCol);
        bool valid = targetPiece != nullptr && targetPiece->isWhite() != m_isWhite;
        std::cout << "Diagonal capture: " << (valid ? "valid" : "invalid") << std::endl;
        return valid;
    }

    std::cout << "No valid pawn move found" << std::endl;
    return false;
}

bool Rook::isValidMove(const Board& board, int srcRow, int srcCol, int dstRow, int dstCol) const {
    // Rook can only move in straight lines
    if (srcRow != dstRow && srcCol != dstCol) {
        return false;
    }

    // Check if the path is clear
    bool pathClear = isPathClear(board, srcRow, srcCol, dstRow, dstCol);
    std::cout << "Path clear: " << (pathClear ? "yes" : "no") << std::endl;
    return pathClear;
}

bool Knight::isValidMove(const Board& board, int srcRow, int srcCol, int dstRow, int dstCol) const {
    // Knight moves: 2 squares in one direction and 1 square perpendicular
    int rowDiff = std::abs(srcRow - dstRow);
    int colDiff = std::abs(srcCol - dstCol);

    // Knight can move two squares in one direction and one square perpendicular
    return (rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2);
}

bool Bishop::isValidMove(const Board& board, int srcRow, int srcCol, int dstRow, int dstCol) const {
    // Bishop can only move diagonally
    if (std::abs(srcRow - dstRow) != std::abs(srcCol - dstCol)) {
        return false;
    }

    // Check if the path is clear
    return isPathClear(board, srcRow, srcCol, dstRow, dstCol);
}

bool Queen::isValidMove(const Board& board, int srcRow, int srcCol, int dstRow, int dstCol) const {
    // Queen can move like a rook (straight) or a bishop (diagonally)
    bool isStraight = srcRow == dstRow || srcCol == dstCol;
    bool isDiagonal = std::abs(srcRow - dstRow) == std::abs(srcCol - dstCol);

    if (!isStraight && !isDiagonal) {
        return false;
    }

    // Check if the path is clear
    return isPathClear(board, srcRow, srcCol, dstRow, dstCol);
}

bool King::isValidMove(const Board& board, int srcRow, int srcCol, int dstRow, int dstCol) const {
    // King can move one square in any direction
    int rowDiff = std::abs(srcRow - dstRow);
    int colDiff = std::abs(srcCol - dstCol);

    if (rowDiff <= 1 && colDiff <= 1) {
        return true;
    }

    // Check for castling
    return canCastle(board, srcRow, srcCol, dstRow, dstCol);
}

// Castling is not implemented in this simplified version
bool King::canCastle(const Board& board, int srcRow, int srcCol, int dstRow, int dstCol) const {
    // For a full implementation, we would check king and rook movement history and ensure the king doesn't move through check
    return false;
}