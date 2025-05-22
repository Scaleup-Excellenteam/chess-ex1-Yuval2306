#pragma once
#include <vector>
#include <memory>
#include "Piece.h"

class Board {
private:
    std::vector<std::vector<std::shared_ptr<Piece>>> m_board;
    bool m_whiteKingMoved = false;
    bool m_blackKingMoved = false;
    bool m_whiteLeftRookMoved = false;
    bool m_whiteRightRookMoved = false;
    bool m_blackLeftRookMoved = false;
    bool m_blackRightRookMoved = false;

    // Helper method for pawn promotion
    void promotePawn(int row, int col, char promotionPiece);

public:
    Board(const std::string& boardString);

    // Get piece at a specific position
    std::shared_ptr<Piece> getPiece(int row, int col) const;

    // Set piece at a specific position
    void setPiece(int row, int col, std::shared_ptr<Piece> piece);

    // Remove piece at a specific position
    void removePiece(int row, int col);

    // Move piece from source to destination
    void movePiece(int srcRow, int srcCol, int dstRow, int dstCol);

    // Overloaded version for pawn promotion
    void movePiece(int srcRow, int srcCol, int dstRow, int dstCol, char promotionPiece);

    // Check if king is in check
    bool isKingInCheck(bool isWhite) const;

    // Find the king's position
    std::pair<int, int> findKing(bool isWhite) const;

    // Check if the move would put the player in check
    bool wouldBeInCheck(int srcRow, int srcCol, int dstRow, int dstCol, bool isWhiteTurn) const;

    // Check if the move causes check on opponent
    bool causesCheckOnOpponent(int srcRow, int srcCol, int dstRow, int dstCol, bool isWhiteTurn) const;

    // Print the board (for debugging)
    void printBoard() const;

    // Get a copy of the board
    Board getCopy() const;

    // Track king and rook movements for castling
    void updatePieceMoved(int row, int col, std::shared_ptr<Piece> piece);
};