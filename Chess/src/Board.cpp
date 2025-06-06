#include "Board.h"
#include <iostream>

Board::Board(const std::string& boardString) {
    // Initialize board
    m_board.resize(8);
    for (int i = 0; i < 8; i++) {
        m_board[i].resize(8, nullptr);
    }

    // Loop through the string and place pieces
    for (int i = 0; i < boardString.length() && i < 64; i++) {
        int row = i / 8;
        int col = i % 8;

        char pieceChar = boardString[i];

        if (pieceChar != '#') {
            bool isWhite = std::isupper(pieceChar);
            char upperChar = std::toupper(pieceChar);

            std::shared_ptr<Piece> piece = nullptr;

            switch (upperChar) {
                case 'P': piece = std::make_shared<Pawn>(isWhite); break;
                case 'R': piece = std::make_shared<Rook>(isWhite); break;
                case 'N': piece = std::make_shared<Knight>(isWhite); break;
                case 'B': piece = std::make_shared<Bishop>(isWhite); break;
                case 'Q': piece = std::make_shared<Queen>(isWhite); break;
                case 'K': piece = std::make_shared<King>(isWhite); break;
            }

            m_board[row][col] = piece;
        }
    }
}

std::shared_ptr<Piece> Board::getPiece(int row, int col) const {
    if (row < 0 || row >= 8 || col < 0 || col >= 8) {
        return nullptr;
    }
    return m_board[row][col];
}

void Board::setPiece(int row, int col, std::shared_ptr<Piece> piece) {
    if (row >= 0 && row < 8 && col >= 0 && col < 8) {
        m_board[row][col] = piece;
    }
}

void Board::removePiece(int row, int col) {
    if (row >= 0 && row < 8 && col >= 0 && col < 8) {
        m_board[row][col] = nullptr;
    }
}

// Original movePiece method (keep this exactly as is)
void Board::movePiece(int srcRow, int srcCol, int dstRow, int dstCol) {
    auto piece = getPiece(srcRow, srcCol);
    if (piece) {
        updatePieceMoved(srcRow, srcCol, piece);
        setPiece(dstRow, dstCol, piece);
        removePiece(srcRow, srcCol);
    }
}

// New method for pawn promotion - private helper
void Board::promotePawn(int row, int col, char promotionPiece) {
    auto pawn = getPiece(row, col);
    if (!pawn || (std::toupper(pawn->getSymbol()) != 'P')) {
        throw std::invalid_argument("No pawn at the specified position");
    }

    bool isWhite = pawn->isWhite();
    std::shared_ptr<Piece> newPiece = nullptr;

    // Convert to uppercase for consistency
    promotionPiece = std::toupper(promotionPiece);

    // Create the new piece based on the promotion choice
    switch (promotionPiece) {
        case 'Q':
            newPiece = std::make_shared<Queen>(isWhite);
            break;
        case 'R':
            newPiece = std::make_shared<Rook>(isWhite);
            break;
        case 'B':
            newPiece = std::make_shared<Bishop>(isWhite);
            break;
        case 'N':
            newPiece = std::make_shared<Knight>(isWhite);
            break;
        default:
            throw std::invalid_argument("Invalid promotion piece");
    }

    // Replace the pawn with the new piece
    setPiece(row, col, newPiece);
}

// Overloaded movePiece to handle promotion
void Board::movePiece(int srcRow, int srcCol, int dstRow, int dstCol, char promotionPiece) {
    auto piece = getPiece(srcRow, srcCol);
    if (piece) {
        updatePieceMoved(srcRow, srcCol, piece);

        // Check for castling move
        if (std::toupper(piece->getSymbol()) == 'K' && std::abs(dstCol - srcCol) == 2) {
            performCastle(srcRow, srcCol, dstCol);
            return;
        }

        setPiece(dstRow, dstCol, piece);
        removePiece(srcRow, srcCol);

        // Handle pawn promotion if needed
        if (promotionPiece != 0 && std::toupper(piece->getSymbol()) == 'P') {
            // Check if pawn is at the end of the board
            if ((piece->isWhite() && dstRow == 7) || (!piece->isWhite() && dstRow == 0)) {
                promotePawn(dstRow, dstCol, promotionPiece);
            }
        }
    }
}

void Board::updatePieceMoved(int row, int col, std::shared_ptr<Piece> piece) {
    // Track king and rook movements for castling
    char symbol = piece->getSymbol();

    if (symbol == 'K') {
        m_whiteKingMoved = true;
    } else if (symbol == 'k') {
        m_blackKingMoved = true;
    } else if (symbol == 'R') {
        if (row == 0 && col == 0) m_whiteLeftRookMoved = true;
        if (row == 0 && col == 7) m_whiteRightRookMoved = true;
    } else if (symbol == 'r') {
        if (row == 7 && col == 0) m_blackLeftRookMoved = true;
        if (row == 7 && col == 7) m_blackRightRookMoved = true;
    }
}

bool Board::hasKingMoved(bool isWhite) const {
    return isWhite ? m_whiteKingMoved : m_blackKingMoved;
}

bool Board::hasRookMoved(bool isWhite, bool isKingside) const {
    if (isWhite) {
        return isKingside ? m_whiteRightRookMoved : m_whiteLeftRookMoved;
    } else {
        return isKingside ? m_blackRightRookMoved : m_blackLeftRookMoved;
    }
}

bool Board::canCastleKingside(bool isWhite) const {
    // Check if king or kingside rook has moved
    if (hasKingMoved(isWhite) || hasRookMoved(isWhite, true)) {
        return false;
    }

    int kingRow = isWhite ? 0 : 7;
    int rookCol = 7;

    // Check if rook is still there
    auto rook = getPiece(kingRow, rookCol);
    if (!rook || std::toupper(rook->getSymbol()) != 'R' || rook->isWhite() != isWhite) {
        return false;
    }

    // Check if squares between king and rook are empty
    for (int col = 5; col <= 6; col++) {
        if (getPiece(kingRow, col) != nullptr) {
            return false;
        }
    }

    // Check if king is currently in check
    if (isKingInCheck(isWhite)) {
        return false;
    }

    // Check if king would pass through or end in check
    Board tempBoard = getCopy();
    if (tempBoard.wouldBeInCheck(kingRow, 4, kingRow, 5, isWhite) ||
        tempBoard.wouldBeInCheck(kingRow, 4, kingRow, 6, isWhite)) {
        return false;
    }

    return true;
}

bool Board::canCastleQueenside(bool isWhite) const {
    // Check if king or queenside rook has moved
    if (hasKingMoved(isWhite) || hasRookMoved(isWhite, false)) {
        return false;
    }

    int kingRow = isWhite ? 0 : 7;
    int rookCol = 0;

    // Check if rook is still there
    auto rook = getPiece(kingRow, rookCol);
    if (!rook || std::toupper(rook->getSymbol()) != 'R' || rook->isWhite() != isWhite) {
        return false;
    }

    // Check if squares between king and rook are empty
    for (int col = 1; col <= 3; col++) {
        if (getPiece(kingRow, col) != nullptr) {
            return false;
        }
    }

    // Check if king is currently in check
    if (isKingInCheck(isWhite)) {
        return false;
    }

    // Check if king would pass through or end in check
    Board tempBoard = getCopy();
    if (tempBoard.wouldBeInCheck(kingRow, 4, kingRow, 3, isWhite) ||
        tempBoard.wouldBeInCheck(kingRow, 4, kingRow, 2, isWhite)) {
        return false;
    }

    return true;
}

void Board::performCastle(int kingRow, int kingCol, int newKingCol) {
    auto king = getPiece(kingRow, kingCol);
    if (!king) return;

    // Determine if this is kingside or queenside castling
    bool isKingside = (newKingCol > kingCol);

    int rookCol = isKingside ? 7 : 0;
    int newRookCol = isKingside ? 5 : 3;

    auto rook = getPiece(kingRow, rookCol);

    // Move king
    setPiece(kingRow, newKingCol, king);
    removePiece(kingRow, kingCol);

    // Move rook
    if (rook) {
        setPiece(kingRow, newRookCol, rook);
        removePiece(kingRow, rookCol);
    }

    std::cout << "Castling performed: " << (isKingside ? "Kingside" : "Queenside") << std::endl;
}

std::pair<int, int> Board::findKing(bool isWhite) const {
    char kingSymbol = isWhite ? 'K' : 'k';

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            auto piece = getPiece(row, col);
            if (piece && piece->getSymbol() == kingSymbol) {
                return { row, col };
            }
        }
    }

    // If king not found (shouldn't happen in valid chess)
    return { -1, -1 };
}

bool Board::isKingInCheck(bool isWhite) const {
    auto [kingRow, kingCol] = findKing(isWhite);

    if (kingRow == -1 || kingCol == -1) {
        return false; // King not found
    }

    // Check if any opponent piece can capture the king
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            auto piece = getPiece(row, col);
            if (piece && piece->isWhite() != isWhite) {
                if (piece->isValidMove(*this, row, col, kingRow, kingCol)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Board::wouldBeInCheck(int srcRow, int srcCol, int dstRow, int dstCol, bool isWhiteTurn) const {
    // Create a copy of the board
    Board tempBoard = getCopy();

    // Make the move on the copy
    tempBoard.movePiece(srcRow, srcCol, dstRow, dstCol);

    // Check if the king is in check after the move
    return tempBoard.isKingInCheck(isWhiteTurn);
}

bool Board::causesCheckOnOpponent(int srcRow, int srcCol, int dstRow, int dstCol, bool isWhiteTurn) const {
    // Create a copy of the board
    Board tempBoard = getCopy();

    // Make the move on the copy
    tempBoard.movePiece(srcRow, srcCol, dstRow, dstCol);

    // Check if the opponent's king is in check after the move
    return tempBoard.isKingInCheck(!isWhiteTurn);
}

Board Board::getCopy() const {
    // Create a new board with empty string
    Board copy("");

    // Copy the board configuration
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            auto piece = getPiece(row, col);
            if (piece) {
                copy.setPiece(row, col, piece->clone());
            }
        }
    }

    // Copy castling flags
    copy.m_whiteKingMoved = m_whiteKingMoved;
    copy.m_blackKingMoved = m_blackKingMoved;
    copy.m_whiteLeftRookMoved = m_whiteLeftRookMoved;
    copy.m_whiteRightRookMoved = m_whiteRightRookMoved;
    copy.m_blackLeftRookMoved = m_blackLeftRookMoved;
    copy.m_blackRightRookMoved = m_blackRightRookMoved;

    return copy;
}

void Board::printBoard() const {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            auto piece = getPiece(row, col);
            if (piece) {
                std::cout << piece->getSymbol();
            } else {
                std::cout << '.';
            }
            std::cout << ' ';
        }
        std::cout << std::endl;
    }
}