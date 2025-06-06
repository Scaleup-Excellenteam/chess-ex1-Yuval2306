#include "Chess.h"
#include "Game.h"
#include "ComputerPlayer.h"
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

    // Game mode selection
    std::cout << "\nSelect game mode:" << std::endl;
    std::cout << "1. User vs User" << std::endl;
    std::cout << "2. User vs Computer" << std::endl;
    std::cout << "3. Computer vs Computer (Auto play)" << std::endl;
    std::cout << "4. Run performance benchmarks" << std::endl;
    std::cout << "Enter choice (1-4): ";

    int gameMode;
    std::cin >> gameMode;

    std::cin.clear();
    std::cin.ignore(10000, '\n');

    if (gameMode == 4) {
        std::cout << "\nRunning benchmarks with 8 moves..." << std::endl;
        Benchmark::runBenchmarks(board, searchDepth, 8);
        return 0;
    }

    // Default to 8 threads as required
    int numThreads = 8;
    std::cout << "\nUsing default 8 threads for move calculation." << std::endl;

    // Create game
    Chess chessUI(board);
    Game chessGame(board, numThreads);
    chessGame.setSearchDepth(searchDepth);

    if (gameMode == 3) {
        // Computer vs Computer mode
        std::cout << "\n=== COMPUTER vs COMPUTER MODE ===" << std::endl;
        std::cout << "Both players will be controlled by AI." << std::endl;

        ComputerPlayer whitePlayer(false);  // Best move strategy
        ComputerPlayer blackPlayer(false);  // Best move strategy

        int moveCount = 0;
        int maxMoves = 100;

        while (moveCount < maxMoves) {
            bool isWhiteTurn = chessGame.isWhiteTurn();
            ComputerPlayer& currentPlayer = isWhiteTurn ? whitePlayer : blackPlayer;

            // Display board
            chessUI.displayBoard();

            // Check for game end conditions
            if (chessGame.isCheckmate(!isWhiteTurn)) {
                std::cout << "\n*** CHECKMATE! ***" << std::endl;
                std::cout << (isWhiteTurn ? "White" : "Black") << " wins!" << std::endl;
                break;
            }

            if (chessGame.isStalemate(isWhiteTurn)) {
                std::cout << "\n*** STALEMATE! ***" << std::endl;
                std::cout << "Game ends in a draw." << std::endl;
                break;
            }

            // Get recommendations
            auto recommendedMoves = chessGame.recommendMoves();

            if (recommendedMoves.empty()) {
                std::cout << "Game Over - No valid moves available!" << std::endl;
                break;
            }

            // Get move from computer player
            std::string move = currentPlayer.selectMove(recommendedMoves);

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
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            } else {
                std::cout << "Error: Invalid move generated! Code: " << codeResponse << std::endl;
                std::cout << "Attempted move: " << move << std::endl;
                break;
            }
        }

        std::cout << "\nGame completed after " << moveCount << " moves." << std::endl;

    } else if (gameMode == 2) {
        // User vs Computer mode
        std::cout << "\n=== USER vs COMPUTER MODE ===" << std::endl;
        std::cout << "You are playing as White. Computer is Black." << std::endl;

        ComputerPlayer computerPlayer(false);  // Best move strategy
        int codeResponse = 0;

        while (true) {
            bool isWhiteTurn = chessGame.isWhiteTurn();

            // Check for game end conditions before each turn
            if (chessGame.isCheckmate(!isWhiteTurn)) {
                std::cout << "\n*** CHECKMATE! ***" << std::endl;
                std::cout << (isWhiteTurn ? "White" : "Black") << " wins!" << std::endl;
                break;
            }

            if (chessGame.isStalemate(isWhiteTurn)) {
                std::cout << "\n*** STALEMATE! ***" << std::endl;
                std::cout << "Game ends in a draw." << std::endl;
                break;
            }

            if (isWhiteTurn) {
                // Human player's turn (White)
                // Get recommendations for human player
                auto recommendedMoves = chessGame.recommendMoves();

                std::cout << "Player 1 (White - Capital letters) >> ";
                std::string userMove;
                std::cin >> userMove;

                if (userMove == "exit" || userMove == "quit") {
                    break;
                }

                codeResponse = chessGame.validateMove(userMove);

                if (codeResponse == 41) {
                    std::cout << "Valid move - Check!" << std::endl;
                } else if (codeResponse == 42) {
                    std::cout << "Valid move" << std::endl;
                } else {
                    // Invalid move - show error and continue
                    switch (codeResponse) {
                        case 11: std::cout << "No piece at source position" << std::endl; break;
                        case 12: std::cout << "That piece belongs to your opponent" << std::endl; break;
                        case 13: std::cout << "Can't capture your own piece" << std::endl; break;
                        case 21: std::cout << "Invalid move for that piece" << std::endl; break;
                        case 31: std::cout << "Move would put you in check" << std::endl; break;
                        default: std::cout << "Invalid move" << std::endl; break;
                    }
                    continue; // Ask for another move
                }

            } else {
                // Computer player's turn (Black)
                std::cout << "\nComputer is thinking..." << std::endl;

                auto recommendedMoves = chessGame.recommendMoves();

                if (recommendedMoves.empty()) {
                    std::cout << "Computer has no valid moves!" << std::endl;
                    break;
                }

                std::string computerMove = computerPlayer.selectMove(recommendedMoves);
                std::cout << "Computer plays: " << computerMove << std::endl;

                codeResponse = chessGame.validateMove(computerMove);

                if (codeResponse == 41) {
                    std::cout << "Computer move valid - Check!" << std::endl;
                } else if (codeResponse == 42) {
                    std::cout << "Computer move valid" << std::endl;
                } else {
                    std::cout << "Error: Computer generated invalid move! Code: " << codeResponse << std::endl;
                    break;
                }

                std::cout << "Computer has moved. Your turn!" << std::endl;

                // Add small delay for better user experience
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }

    } else {
        // User vs User mode (original implementation)
        std::cout << "\n=== USER vs USER MODE ===" << std::endl;
        std::cout << "Using " << numThreads << " threads for move calculation." << std::endl;

        int codeResponse = 0;
        std::string res = chessUI.getInput();

        while (res != "exit") {
            // Check for game end conditions before move
            bool isWhiteTurn = chessGame.isWhiteTurn();

            if (chessGame.isCheckmate(!isWhiteTurn)) {
                chessUI.displayBoard();
                std::cout << "\n*** CHECKMATE! ***" << std::endl;
                std::cout << (isWhiteTurn ? "White" : "Black") << " wins!" << std::endl;
                break;
            }

            if (chessGame.isStalemate(isWhiteTurn)) {
                chessUI.displayBoard();
                std::cout << "\n*** STALEMATE! ***" << std::endl;
                std::cout << "Game ends in a draw." << std::endl;
                break;
            }

            codeResponse = chessGame.validateMove(res);
            chessUI.setCodeResponse(codeResponse);
            res = chessUI.getInput();
        }
    }

    std::cout << std::endl << "Exiting..." << std::endl;
    return 0;
}
