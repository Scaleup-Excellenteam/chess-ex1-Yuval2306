#pragma once
#include <chrono>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

struct BenchmarkResult {
    int numThreads;
    long long totalTimeMs;
    std::vector<long long> moveTimes;

    void print() const {
        std::cout << "\n=== Benchmark Results for " << numThreads << " threads ===" << std::endl;
        std::cout << "Total time: " << totalTimeMs << " ms" << std::endl;
        std::cout << "Individual move times:" << std::endl;

        for (size_t i = 0; i < moveTimes.size(); ++i) {
            std::cout << "  Move " << (i + 1) << ": " << moveTimes[i] << " ms" << std::endl;
        }

        if (!moveTimes.empty()) {
            long long avg = totalTimeMs / moveTimes.size();
            std::cout << "Average time per move: " << avg << " ms" << std::endl;
        }
    }
};

class Benchmark {
public:
    static void runBenchmarks(const std::string& boardString, int searchDepth, int numMoves) {
        std::cout << "\n====== PERFORMANCE BENCHMARKS ======" << std::endl;
        std::cout << "Search depth: " << searchDepth << std::endl;
        std::cout << "Number of moves: " << numMoves << std::endl;

        std::vector<int> threadCounts = {0, 2, 4, 8}; // 0 means sequential
        std::vector<BenchmarkResult> results;

        for (int threads : threadCounts) {
            results.push_back(runSingleBenchmark(boardString, searchDepth, numMoves, threads));
        }

        // Print comparison
        printComparison(results);
    }

private:
    static BenchmarkResult runSingleBenchmark(const std::string& boardString,
                                              int searchDepth, int numMoves, int numThreads) {
        BenchmarkResult result;
        result.numThreads = numThreads;

        // Use sequential MoveRecommender for 0 threads
        std::unique_ptr<Game> game;
        if (numThreads == 0) {
            std::cout << "\nRunning sequential benchmark..." << std::endl;
            game = std::make_unique<Game>(boardString, 1); // 1 thread = sequential
        } else {
            std::cout << "\nRunning parallel benchmark with " << numThreads << " threads..." << std::endl;
            game = std::make_unique<Game>(boardString, numThreads);
        }

        game->setSearchDepth(searchDepth);

        AutoPlayer autoPlayer(false); // Use best move selection
        Chess chessUI(boardString);

        auto totalStart = std::chrono::high_resolution_clock::now();

        int moveCount = 0;
        int codeResponse = 0;

        while (moveCount < numMoves) {
            auto moveStart = std::chrono::high_resolution_clock::now();

            // Get recommended moves (this is where timing matters)
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
            result.moveTimes.push_back(moveDuration.count());

            // Update UI (without actually displaying)
            chessUI.setCodeResponse(codeResponse);

            if (codeResponse == 41 || codeResponse == 42) {
                moveCount++;
            } else {
                std::cout << "Invalid move generated! Code: " << codeResponse << std::endl;
                break;
            }
        }

        auto totalEnd = std::chrono::high_resolution_clock::now();
        result.totalTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(totalEnd - totalStart).count();

        return result;
    }

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
