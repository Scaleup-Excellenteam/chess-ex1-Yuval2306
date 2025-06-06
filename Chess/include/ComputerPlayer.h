#pragma once
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include "Move.h"
#include <iostream>

class ComputerPlayer {
private:
    std::mt19937 rng;
    bool useRandomSelection;

public:
    ComputerPlayer(bool randomSelection = false) :
            rng(std::chrono::steady_clock::now().time_since_epoch().count()),
            useRandomSelection(randomSelection) {}

    // Select a move from the recommended moves
    std::string selectMove(const std::vector<Move>& recommendedMoves) {
        if (recommendedMoves.empty()) {
            std::cout << "No moves available - exiting" << std::endl;
            return "exit";
        }

        // Debug: Print all available moves
        std::cout << "Available moves: ";
        for (size_t i = 0; i < std::min(recommendedMoves.size(), size_t(5)); i++) {
            std::cout << recommendedMoves[i].toString() << "(" << recommendedMoves[i].getScore() << ") ";
        }
        std::cout << std::endl;

        Move selectedMove(0, 0, 0, 0);

        if (useRandomSelection && recommendedMoves.size() > 1) {
            // Select random move from top 3
            int maxIndex = std::min(3, static_cast<int>(recommendedMoves.size()));
            std::uniform_int_distribution<int> dist(0, maxIndex - 1);
            selectedMove = recommendedMoves[dist(rng)];
            std::cout << "Computer selected random move from top " << maxIndex << std::endl;
        } else {
            // Select the best move
            selectedMove = recommendedMoves[0];
            std::cout << "Computer selected best move" << std::endl;
        }

        std::string moveStr = selectedMove.toString();

        // Check if this might be a pawn promotion move
        // If score is very high and it's a pawn move to the end rank, add promotion piece
        if (selectedMove.getScore() >= 1000) {
            // Check if move string is only 4 characters
            if (moveStr.length() == 4) {
                // Check if destination row is 0 or 7
                char dstRow = moveStr[2];
                if (dstRow == 'a' || dstRow == 'h') {  // Promotion ranks in the coordinate system
                    moveStr += "q";  // Always promote to queen
                    std::cout << "Added queen promotion to move: " << moveStr << std::endl;
                }
            }
        }

        std::cout << "Computer move: " << moveStr << " (score: " << selectedMove.getScore() << ")" << std::endl;
        return moveStr;
    }
};

