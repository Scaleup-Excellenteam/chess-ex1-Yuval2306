#pragma once
#include "MoveRecommender.h"
#include "ThreadSafePriorityQueue.h"
#include "ThreadPool.h"
#include <atomic>
#include <chrono>
#include <future>

class ParallelMoveRecommender : public MoveRecommender {
private:
    static constexpr int SCORE_THRESHOLD = 2000;
    static constexpr int MAX_TOP_MOVES = 5;
    std::atomic<bool> stopFlag{false};
    int numThreads;

    // Calculate moves for a single piece
    void calculatePieceMoves(const Board& board, int srcRow, int srcCol,
                             ThreadSafePriorityQueue<Move, MoveComparator>& bestMoves,
                             bool isWhiteTurn, int currentDepth, int maxDepth) {

        auto piece = board.getPiece(srcRow, srcCol);
        if (!piece || piece->isWhite() != isWhiteTurn) {
            return;
        }

        // Check all possible destinations for this piece
        for (int dstRow = 0; dstRow < 8; dstRow++) {
            for (int dstCol = 0; dstCol < 8; dstCol++) {
                if (stopFlag.load()) {
                    return;
                }

                if (srcRow == dstRow && srcCol == dstCol) {
                    continue;
                }

                // Check if there's a friendly piece at destination BEFORE checking valid move
                auto destPiece = board.getPiece(dstRow, dstCol);
                if (destPiece && destPiece->isWhite() == isWhiteTurn) {
                    continue;
                }

                if (piece->isValidMove(board, srcRow, srcCol, dstRow, dstCol)) {
                    if (board.wouldBeInCheck(srcRow, srcCol, dstRow, dstCol, isWhiteTurn)) {
                        continue;
                    }

                    Move move(srcRow, srcCol, dstRow, dstCol);

                    // Check for pawn promotion
                    if (piece->getSymbol() == (isWhiteTurn ? 'P' : 'p')) {
                        if ((isWhiteTurn && dstRow == 7) || (!isWhiteTurn && dstRow == 0)) {
                            move.setScore(1000);
                            if (currentDepth == 0) {
                                throw PawnPromotionException(move);
                            }
                        }
                    }

                    // Calculate score
                    int score = calculateMoveScore(board, move, currentDepth, isWhiteTurn, maxDepth);
                    move.setScore(score);

                    // Check if we found an amazing move (bonus feature)
                    if (score > SCORE_THRESHOLD) {
                        stopFlag.store(true);
                    }

                    try {
                        bestMoves.push(move);
                    } catch (const QueueFullException& e) {
                        // Queue is full, ignore
                    }
                }
            }
        }
    }

public:
    ParallelMoveRecommender(int threads = 4) : numThreads(threads) {}

    // Override the findBestMoves method (the public one) to use parallel processing
    std::vector<Move> findBestMoves(const Board& board, bool isWhiteTurn, int maxDepth = 2) {
        stopFlag.store(false);

        // Thread-safe priority queue
        ThreadSafePriorityQueue<Move, MoveComparator> bestMoves(MAX_TOP_MOVES);

        // For depth 0
        if (maxDepth > 0) {
            // Create thread pool
            ThreadPool pool(numThreads);
            std::vector<std::future<void>> futures;

            // timing
            auto start = std::chrono::high_resolution_clock::now();

            // Find all pieces and distribute work
            std::vector<std::pair<int, int>> piecesToProcess;

            for (int row = 0; row < 8; row++) {
                for (int col = 0; col < 8; col++) {
                    auto piece = board.getPiece(row, col);
                    if (piece && piece->isWhite() == isWhiteTurn) {
                        piecesToProcess.push_back({row, col});
                    }
                }
            }

            std::cout << "Processing " << piecesToProcess.size() << " pieces with "
                      << numThreads << " threads..." << std::endl;

            // Submit tasks to thread pool
            for (const auto& [row, col] : piecesToProcess) {
                futures.push_back(
                        pool.enqueue([this, &board, row, col, &bestMoves, isWhiteTurn, maxDepth]() {
                            try {
                                calculatePieceMoves(board, row, col, bestMoves, isWhiteTurn, 0, maxDepth);
                            } catch (const PawnPromotionException& e) {
                                throw;
                            }
                        })
                );
            }

            // Wait for all tasks to complete
            for (auto& future : futures) {
                future.get();
            }

            // Measure time
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            std::cout << "Move calculation completed in " << duration.count() << " ms" << std::endl;

            // Convert to vector
            std::vector<Move> result = bestMoves.getAllCopy();

            // Sort
            std::sort(result.begin(), result.end(), [](const Move& a, const Move& b) {
                return a.getScore() > b.getScore();
            });

            return result;
        } else {
            // For depth 0 or negative
            return MoveRecommender::findBestMoves(board, isWhiteTurn, maxDepth);
        }
    }
};