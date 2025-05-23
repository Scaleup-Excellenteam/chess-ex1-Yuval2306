#pragma once
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include "Move.h"

class AutoPlayer {
private:
    std::mt19937 rng;
    bool useRandomMove;

public:
    AutoPlayer(bool randomSelection = false) :
            rng(std::chrono::steady_clock::now().time_since_epoch().count()),
            useRandomMove(randomSelection) {}

    // Get the next move from recommended moves
    std::string getNextMove(const std::vector<Move>& recommendedMoves) {
        if (recommendedMoves.empty()) {
            return "exit";
        }

        Move selectedMove(0, 0, 0, 0);

        if (useRandomMove && recommendedMoves.size() > 1) {
            // Select random move from top 3
            int maxIndex = std::min(3, static_cast<int>(recommendedMoves.size()));
            std::uniform_int_distribution<int> dist(0, maxIndex - 1);
            selectedMove = recommendedMoves[dist(rng)];
            std::cout << "AutoPlayer selected random move from top " << maxIndex << std::endl;
        } else {
            // Select the best move
            selectedMove = recommendedMoves[0];
            std::cout << "AutoPlayer selected best move" << std::endl;
        }

        std::cout << "AutoPlayer move: " << selectedMove << std::endl;
        return selectedMove.toString();
    }
};