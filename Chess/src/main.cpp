#include "Chess.h"
#include "Game.h"
#include "AutoPlayer.h"
#include "Benchmark.h"
#include <iostream>
#include <string>
#include <thread>

int main()
{
    std::string board = "RNBQKBNRPPPPPPPP################################pppppppprnbqkbnr";

    std::cout << "=== Chess Game with Parallel Move Calculation ===" << std::endl;
    std::cout << "Available CPU cores: " << std::thread::hardware_concurrency() << std::endl;

    int searchDepth;
    std::cout << "\nEnter search depth for move recommendations (1-3): ";
    std::cin >> searchDepth;

    //play mode
    std::cout << "\nSelect play mode:" << std::endl;
    std::cout << "1. Manual play (user input)" << std::endl;
    std::cout << "2. Automatic play (AI vs AI)" << std::endl;
    std::cout << "3. Run performance benchmarks" << std::endl;
    std::cout << "Enter choice (1-3): ";

    int playMode;
    std::cin >> playMode;

    std::cin.clear();
    std::cin.ignore(10000, '\n');

    if (playMode == 3) {
        std::cout << "\nRunning benchmarks with 8 moves..." << std::endl;
        Benchmark::runBenchmarks(board, searchDepth, 8);
        return 0;
    }

    int numThreads = 4; // Default
    if (playMode == 1 || playMode == 2) {
        std::cout << "\nEnter number of threads for move calculation (1-8): ";
        std::cin >> numThreads;

        if (numThreads < 1) numThreads = 1;
        if (numThreads > 8) numThreads = 8;

        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }

    // Create game
    Chess chessUI(board);
    Game chessGame(board, numThreads);
    chessGame.setSearchDepth(searchDepth);

    if (playMode == 2) {
        // Automatic play mode
        std::cout << "\n=== AUTOMATIC PLAY MODE ===" << std::endl;
        std::cout << "The AI will play against itself." << std::endl;

        AutoPlayer whitePlayer(false);
        AutoPlayer blackPlayer(true);

        int moveCount = 0;
        int maxMoves = 50;

        while (moveCount < maxMoves) {
            //current player
            bool isWhiteTurn = chessGame.isWhiteTurn();
            AutoPlayer& currentPlayer = isWhiteTurn ? whitePlayer : blackPlayer;

            // Display board
            chessUI.displayBoard();

            // Get recommendations
            auto recommendedMoves = chessGame.recommendMoves();

            if (recommendedMoves.empty()) {
                std::cout << "Game Over - No valid moves available!" << std::endl;
                break;
            }

            // Get move from auto player
            std::string move = currentPlayer.getNextMove(recommendedMoves);

            if (move == "exit") {
                break;
            }

            // Validate and execute move
            int codeResponse = chessGame.validateMove(move);
            chessUI.setCodeResponse(codeResponse);

            if (codeResponse == 41 || codeResponse == 42) {
                moveCount++;
                std::cout << "Move " << moveCount << " completed." << std::endl;

                // Add delay for visualization
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            } else {
                std::cout << "Error: Invalid move generated! Code: " << codeResponse << std::endl;
                std::cout << "Attempted move: " << move << std::endl;
                break;
            }
        }

        std::cout << "\nAutomatic game completed after " << moveCount << " moves." << std::endl;

    } else {
        std::cout << "\n=== MANUAL PLAY MODE ===" << std::endl;
        std::cout << "Using " << numThreads << " threads for move calculation." << std::endl;

        int codeResponse = 0;
        std::string res = chessUI.getInput();

        while (res != "exit")
        {
            codeResponse = chessGame.validateMove(res);
            chessUI.setCodeResponse(codeResponse);
            res = chessUI.getInput();
        }
    }

    std::cout << std::endl << "Exiting..." << std::endl;
    return 0;
}