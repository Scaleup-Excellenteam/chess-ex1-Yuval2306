#pragma once
#include <vector>
#include <memory>

class Board;

class Piece {
protected:
    bool m_isWhite;

public:
    Piece(bool isWhite) : m_isWhite(isWhite) {}
    virtual ~Piece() = default;

    // Get whether the piece is white or black
    bool isWhite() const { return m_isWhite; }

    // Check if a move is valid for this piece
    virtual bool isValidMove(const Board& board, int srcRow, int srcCol, int dstRow, int dstCol) const = 0;

    // Get the piece type
    virtual char getSymbol() const = 0;

    // Clone the piece
    virtual std::shared_ptr<Piece> clone() const = 0;

    // Check if the path is clear (used by multiple pieces)
    bool isPathClear(const Board& board, int srcRow, int srcCol, int dstRow, int dstCol) const;
};

// Pawn piece
class Pawn : public Piece {
public:
    Pawn(bool isWhite) : Piece(isWhite) {}
    bool isValidMove(const Board& board, int srcRow, int srcCol, int dstRow, int dstCol) const override;
    char getSymbol() const override { return m_isWhite ? 'P' : 'p'; }
    std::shared_ptr<Piece> clone() const override { return std::make_shared<Pawn>(*this); }
};

// Rook piece
class Rook : public Piece {
public:
    Rook(bool isWhite) : Piece(isWhite) {}
    bool isValidMove(const Board& board, int srcRow, int srcCol, int dstRow, int dstCol) const override;
    char getSymbol() const override { return m_isWhite ? 'R' : 'r'; }
    std::shared_ptr<Piece> clone() const override { return std::make_shared<Rook>(*this); }
};

// Knight piece
class Knight : public Piece {
public:
    Knight(bool isWhite) : Piece(isWhite) {}
    bool isValidMove(const Board& board, int srcRow, int srcCol, int dstRow, int dstCol) const override;
    char getSymbol() const override { return m_isWhite ? 'N' : 'n'; }
    std::shared_ptr<Piece> clone() const override { return std::make_shared<Knight>(*this); }
};

// Bishop piece
class Bishop : public Piece {
public:
    Bishop(bool isWhite) : Piece(isWhite) {}
    bool isValidMove(const Board& board, int srcRow, int srcCol, int dstRow, int dstCol) const override;
    char getSymbol() const override { return m_isWhite ? 'B' : 'b'; }
    std::shared_ptr<Piece> clone() const override { return std::make_shared<Bishop>(*this); }
};

// Queen piece
class Queen : public Piece {
public:
    Queen(bool isWhite) : Piece(isWhite) {}
    bool isValidMove(const Board& board, int srcRow, int srcCol, int dstRow, int dstCol) const override;
    char getSymbol() const override { return m_isWhite ? 'Q' : 'q'; }
    std::shared_ptr<Piece> clone() const override { return std::make_shared<Queen>(*this); }
};

// King piece
class King : public Piece {
public:
    King(bool isWhite) : Piece(isWhite) {}
    bool isValidMove(const Board& board, int srcRow, int srcCol, int dstRow, int dstCol) const override;
    char getSymbol() const override { return m_isWhite ? 'K' : 'k'; }
    std::shared_ptr<Piece> clone() const override { return std::make_shared<King>(*this); }
    bool canCastle(const Board& board, int srcRow, int srcCol, int dstRow, int dstCol) const;
};