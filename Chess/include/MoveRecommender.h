#pragma once

#include "Board.h"
#include "Move.h"
#include "PriorityQueue.h"
#include <vector>
#include <algorithm>
#include <iostream>

// Constants for move evaluation
constexpr int PIECE_VALUES[] = {
        100,  // Pawn
        500,  // Rook
        300,  // Knight
        300,  // Bishop
        900,  // Queen
        10000 // King (very high value)
};

constexpr int CENTRAL_SQUARES_BONUS = 10;
constexpr int CAPTURE_BONUS_MULTIPLIER = 10;
constexpr int THREAT_BONUS = 5;
constexpr int PIECE_IN_DANGER_PENALTY = -15;

class PawnPromotionException : public std::exception {
public:
    PawnPromotionException(const Move& m) : move(m),
                                            message("Pawn promotion is available at " + m.toString()) {}

    const char* what() const noexcept override { return message.c_str(); }
    Move getMove() const { return move; }

private:
    Move move;
    std::string message;
};

class MoveRecommender {
private:
    // Number of top moves to keep in the priority queue
    static constexpr int TOP_MOVES_COUNT = 5;

    // Convert a piece symbol to its value index
    int getPieceValueIndex(char pieceSymbol) const {
        char upperSymbol = std::toupper(pieceSymbol);
        switch (upperSymbol) {
            case 'P': return 0; // Pawn
            case 'R': return 1; // Rook
            case 'N': return 2; // Knight
            case 'B': return 3; // Bishop
            case 'Q': return 4; // Queen
            case 'K': return 5; // King
            default: return -1;  // Not a valid piece
        }
    }

    // Check if position is in the center 4x4 of the board
    bool isInCenter(int row, int col) const {
        return (row >= 2 && row <= 5 && col >= 2 && col <= 5);
    }

    // Get piece value based on its symbol
    int getPieceValue(char pieceSymbol) const {
        int index = getPieceValueIndex(pieceSymbol);
        return (index >= 0) ? PIECE_VALUES[index] : 0;
    }

    // Calculate the score for a move at a specific depth
    int calculateMoveScore(const Board& board, const Move& move, int depth, bool isWhiteTurn, int maxDepth) {
        // Create a temporary board to simulate the move
        Board tempBoard = board.getCopy();

        // Apply the move to the temporary board
        tempBoard.movePiece(
                move.getSrcRow(), move.getSrcCol(),
                move.getDstRow(), move.getDstCol()
        );

        // Initial score for this move
        int score = 0;

        // Get the piece that was moved
        auto piece = board.getPiece(move.getSrcRow(), move.getSrcCol());
        if (!piece) return 0;

        char pieceSymbol = piece->getSymbol();

        // 1. Check if a capture occurred (piece at destination before the move)
        auto capturedPiece = board.getPiece(move.getDstRow(), move.getDstCol());
        if (capturedPiece) {
            char capturedSymbol = capturedPiece->getSymbol();
            score += CAPTURE_BONUS_MULTIPLIER * getPieceValue(capturedSymbol);
        }

        // 2. Check for control of the central squares
        if (isInCenter(move.getDstRow(), move.getDstCol())) {
            score += CENTRAL_SQUARES_BONUS;
        }

        // 3. Check if the moved piece is now in danger from a weaker piece
        bool inDangerFromWeaker = false;
        int pieceValue = getPieceValue(pieceSymbol);

        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                auto opponentPiece = tempBoard.getPiece(row, col);
                if (opponentPiece && opponentPiece->isWhite() != isWhiteTurn) {
                    // Check if opponent piece can capture our moved piece
                    if (opponentPiece->isValidMove(tempBoard, row, col, move.getDstRow(), move.getDstCol())) {
                        char opponentSymbol = opponentPiece->getSymbol();
                        int opponentValue = getPieceValue(opponentSymbol);

                        // If a weaker piece can capture our piece, add a penalty
                        if (opponentValue < pieceValue) {
                            inDangerFromWeaker = true;
                            score += PIECE_IN_DANGER_PENALTY;
                        }
                    }
                }
            }
        }

        // 4. Check if our piece threatens a stronger opponent piece
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                auto targetPiece = tempBoard.getPiece(row, col);
                if (targetPiece && targetPiece->isWhite() != piece->isWhite()) {
                    if (piece->isValidMove(tempBoard, move.getDstRow(), move.getDstCol(), row, col)) {
                        char targetSymbol = targetPiece->getSymbol();
                        int targetValue = getPieceValue(targetSymbol);

                        // If our piece threatens a stronger piece, add a bonus
                        if (targetValue > pieceValue) {
                            score += THREAT_BONUS;
                        }
                    }
                }
            }
        }

        // 5. If this is not the maximum depth, recursively evaluate opponent's responses
        if (depth < maxDepth) {
            // Find the best move the opponent can make
            auto bestOpponentResponse = findBestMove(tempBoard, !isWhiteTurn, depth + 1, maxDepth);

            if (!bestOpponentResponse.empty()) {
                Move bestOpponentMove = bestOpponentResponse.front();
                // Subtract the score of the opponent's best move
                score -= bestOpponentMove.getScore();
            }
        }

        return score;
    }

public:
    MoveRecommender() {}

    // Find the best moves for the current player
    std::vector<Move> findBestMoves(const Board& board, bool isWhiteTurn, int maxDepth = 2) {
        return findBestMove(board, isWhiteTurn, 0, maxDepth);
    }

    // Find the best move at a specific depth
    std::vector<Move> findBestMove(const Board& board, bool isWhiteTurn, int currentDepth, int maxDepth) {
        // Create a priority queue to store the best moves
        PriorityQueue<Move, MoveComparator> bestMoves(TOP_MOVES_COUNT);

        // Find all valid moves for the current player
        for (int srcRow = 0; srcRow < 8; srcRow++) {
            for (int srcCol = 0; srcCol < 8; srcCol++) {
                auto piece = board.getPiece(srcRow, srcCol);

                // Skip if there's no piece or if it belongs to the opponent
                if (!piece || piece->isWhite() != isWhiteTurn) {
                    continue;
                }

                // Consider all possible destination squares
                for (int dstRow = 0; dstRow < 8; dstRow++) {
                    for (int dstCol = 0; dstCol < 8; dstCol++) {
                        // Skip if source and destination are the same
                        if (srcRow == dstRow && srcCol == dstCol) {
                            continue;
                        }

                        // Check if the move is valid for this piece
                        if (piece->isValidMove(board, srcRow, srcCol, dstRow, dstCol)) {
                            // Check if the move would put the player in check
                            if (board.wouldBeInCheck(srcRow, srcCol, dstRow, dstCol, isWhiteTurn)) {
                                continue;
                            }

                            // Create a move object
                            Move move(srcRow, srcCol, dstRow, dstCol);

                            // Check for pawn promotion
                            if (piece->getSymbol() == (isWhiteTurn ? 'P' : 'p')) {
                                // If a pawn reaches the opposite end of the board
                                if ((isWhiteTurn && dstRow == 7) || (!isWhiteTurn && dstRow == 0)) {
                                    // Set a high score for promotion moves
                                    move.setScore(1000);

                                    // If this is at depth 0 (current player), throw exception
                                    if (currentDepth == 0) {
                                        throw PawnPromotionException(move);
                                    }
                                }
                            }

                            // Calculate the score for this move
                            int score = calculateMoveScore(board, move, currentDepth, isWhiteTurn, maxDepth);
                            move.setScore(score);

                            // Add the move to the priority queue
                            try {
                                bestMoves.push(move);
                            } catch (const QueueFullException& e) {
                                // Queue is full, but we've already sorted by priority
                                // so we can just ignore this exception
                            }
                        }
                    }
                }
            }
        }

        // Convert priority queue to vector of moves
        std::vector<Move> result;
        while (!bestMoves.empty()) {
            result.push_back(bestMoves.poll());
        }

        // Moves are already in descending order of score because of the comparator
        return result;
    }
};