#pragma once
#include <chrono>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <memory>
#include "Game.h"
#include "Chess.h"
#include "AutoPlayer.h"

struct BenchmarkResult {
    int numThreads;
    long long totalTimeMs;
    std::vector<long long> moveTimes;
};

class Benchmark {
public:
    // Main function that runs benchmarks with different thread counts and prints comparison
    static void runBenchmarks(const std::string& boardString, int searchDepth, int numMoves) {
        std::cout << "\n====== PERFORMANCE BENCHMARKS ======" << std::endl;
        std::cout << "Search depth: " << searchDepth << std::endl;
        std::cout << "Number of moves: " << numMoves << std::endl;

        std::vector<int> threadCounts = {0, 2, 4, 8};
        std::vector<BenchmarkResult> results;

        for (int threads : threadCounts) {
            results.push_back(runSingleBenchmark(boardString, searchDepth, numMoves, threads));
        }

        printComparison(results);
    }

private:
    // Runs a single benchmark test with specified number of threads and measures performance
    static BenchmarkResult runSingleBenchmark(const std::string& boardString,
                                              int searchDepth, int numMoves, int numThreads) {
        BenchmarkResult result;
        result.numThreads = numThreads;

        // Use sequential MoveRecommender for 0 threads
        std::unique_ptr<Game> game;
        if (numThreads == 0) {
            std::cout << "\nRunning sequential benchmark..." << std::endl;
            game = std::make_unique<Game>(boardString, 1);
        } else {
            std::cout << "\nRunning parallel benchmark with " << numThreads << " threads..." << std::endl;
            game = std::make_unique<Game>(boardString, numThreads);
        }

        game->setSearchDepth(searchDepth);

        AutoPlayer autoPlayer(false);
        Chess chessUI(boardString);

        auto totalStart = std::chrono::high_resolution_clock::now();

        int moveCount = 0;
        int codeResponse = 0;

        while (moveCount < numMoves) {
            auto moveStart = std::chrono::high_resolution_clock::now();

            // Get recommended moves
            auto recommendedMoves = game->recommendMoves();

            if (recommendedMoves.empty()) {
                std::cout << "No more valid moves available!" << std::endl;
                break;
            }

            // Get move from auto player
            std::string move = autoPlayer.getNextMove(recommendedMoves);

            // Validate and execute move
            codeResponse = game->validateMove(move);

            auto moveEnd = std::chrono::high_resolution_clock::now();
            auto moveDuration = std::chrono::duration_cast<std::chrono::milliseconds>(moveEnd - moveStart);

            if (codeResponse == 41 || codeResponse == 42) {
                result.moveTimes.push_back(moveDuration.count());
                moveCount++;

                // Update the board string for the Chess UI
                if (numThreads == 0) {
                    // For sequential benchmark, recreate the game to reset state
                    game = std::make_unique<Game>(boardString, 1);
                    game->setSearchDepth(searchDepth);

                }
            } else {
                std::cout << "Invalid move generated! Code: " << codeResponse << std::endl;
                std::cout << "Attempted move: " << move << std::endl;
                // Try to continue with another move from the recommendations
                if (recommendedMoves.size() > 1) {
                    // Try the second-best move
                    move = recommendedMoves[1].toString();
                    codeResponse = game->validateMove(move);
                    if (codeResponse == 41 || codeResponse == 42) {
                        result.moveTimes.push_back(moveDuration.count());
                        moveCount++;
                    } else {
                        break;
                    }
                } else {
                    break;
                }
            }
        }

        auto totalEnd = std::chrono::high_resolution_clock::now();
        result.totalTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(totalEnd - totalStart).count();

        return result;
    }
    // Prints a formatted comparison table of benchmark results with speedup calculations
    static void printComparison(const std::vector<BenchmarkResult>& results) {
        std::cout << "\n====== PERFORMANCE COMPARISON ======" << std::endl;
        std::cout << std::setw(10) << "Threads"
                  << std::setw(15) << "Total Time"
                  << std::setw(15) << "Avg/Move"
                  << std::setw(15) << "Speedup" << std::endl;
        std::cout << std::string(55, '-') << std::endl;

        if (results.empty()) return;

        long long sequentialTime = results[0].totalTimeMs;

        for (const auto& result : results) {
            long long avgTime = result.moveTimes.empty() ? 0 :
                                result.totalTimeMs / result.moveTimes.size();
            double speedup = sequentialTime > 0 ?
                             static_cast<double>(sequentialTime) / result.totalTimeMs : 0;

            std::cout << std::setw(10) << (result.numThreads == 0 ? "Sequential" : std::to_string(result.numThreads))
                      << std::setw(15) << result.totalTimeMs << " ms"
                      << std::setw(15) << avgTime << " ms"
                      << std::setw(15) << std::fixed << std::setprecision(2) << speedup << "x" << std::endl;
        }
    }
};