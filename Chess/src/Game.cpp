#include "Game.h"
#include "MoveRecommender.h"
#include <iostream>
#include <sstream>

Game::Game(const std::string& boardString)
        : m_board(boardString), m_isWhiteTurn(true), m_recommender(new MoveRecommender()), m_depthLevel(2) {}

Game::~Game() {
    delete m_recommender;
}

void Game::setSearchDepth(int depth) {
    if (depth >= 1 && depth <= 3) {
        m_depthLevel = depth;
    } else {
        std::cout << "Invalid depth. Using default depth of " << m_depthLevel << std::endl;
    }
}

// Convert from algebraic notation (e.g., 'a1') to array indices (0-7, 0-7)
std::pair<int, int> Game::algebraicToIndices(char file, char rank) {
    file = std::tolower(file);
    int row = '8' - rank;
    int col = file - 'a';
    return { row, col };
}

void Game::recommendMoves() {
    try {
        std::vector<Move> bestMoves = m_recommender->findBestMoves(m_board, m_isWhiteTurn, m_depthLevel);
        std::cout << "\n===== RECOMMENDED MOVES =====" << std::endl;

        int movesToShow = std::min(static_cast<int>(bestMoves.size()), 3);

        if (movesToShow == 0) {
            std::cout << "No valid moves found!" << std::endl;
        } else {
            for (int i = 0; i < movesToShow; i++) {
                std::cout << (i + 1) << ". " << bestMoves[i] << std::endl;
            }
        }

        std::cout << std::endl;
    } catch (const PawnPromotionException& e) {
        std::cout << "\n" << e.what() << std::endl;
        std::cout << "Recommended promotion move: " << e.getMove() << std::endl << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "\nError recommending moves: " << e.what() << std::endl << std::endl;
    }
}

int Game::validateMove(const std::string& input) {
    // Before validating the move, recommend moves
    recommendMoves();

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

    // Process pawn promotion if needed
    char promotionPiece = 0;
    if (lowerInput.length() > 4) {
        promotionPiece = lowerInput[4];
    }

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

    // Check for pawn promotion
    bool needsPawnPromotion = false;
    if ((piece->getSymbol() == 'P' && dstRow == 7) || (piece->getSymbol() == 'p' && dstRow == 0)) {
        needsPawnPromotion = true;

        // If no promotion piece specified, ask for one
        if (promotionPiece == 0) {
            try {
                throw PawnPromotionException(Move(srcRow, srcCol, dstRow, dstCol));
            } catch (const PawnPromotionException& e) {
                std::cout << e.what() << std::endl;
                std::cout << "Please provide a promotion piece (q, r, b, n)" << std::endl;
                return 21; // Illegal move until promotion piece is specified
            }
        }

        // Validate promotion piece
        promotionPiece = std::tolower(promotionPiece);
        if (promotionPiece != 'q' && promotionPiece != 'r' &&
            promotionPiece != 'b' && promotionPiece != 'n') {
            std::cout << "Invalid promotion piece. Use q, r, b, or n" << std::endl;
            return 21; // Illegal promotion
        }
    }

    // Check if the move would put the opponent in check
    if (m_board.causesCheckOnOpponent(srcRow, srcCol, dstRow, dstCol, m_isWhiteTurn)) {
        std::cout << "Move causes check" << std::endl;
        // The move is valid and causes check
        m_board.movePiece(srcRow, srcCol, dstRow, dstCol, promotionPiece);
        m_isWhiteTurn = !m_isWhiteTurn;
        return 41;
    }

    std::cout << "Valid move" << std::endl;
    // The move is valid
    m_board.movePiece(srcRow, srcCol, dstRow, dstCol, promotionPiece);
    m_isWhiteTurn = !m_isWhiteTurn;
    return 42;
}