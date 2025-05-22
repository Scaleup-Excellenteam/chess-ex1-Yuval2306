#pragma once
#include <string>
#include <ostream>

class Move {
private:
    int srcRow;
    int srcCol;
    int dstRow;
    int dstCol;
    int score; // Score for this move based on our algorithm
    char promotionPiece; // For pawn promotion, default is 0

public:
    Move(int src_row, int src_col, int dst_row, int dst_col, int move_score = 0, char promotion = 0)
            : srcRow(src_row), srcCol(src_col), dstRow(dst_row), dstCol(dst_col),
              score(move_score), promotionPiece(promotion) {}

    // Convert to string in chess notation
    std::string toString() const {
        std::string result;
        result += static_cast<char>('a' + srcCol);
        result += static_cast<char>('8' - srcRow);
        result += static_cast<char>('a' + dstCol);
        result += static_cast<char>('8' - dstRow);
        if (promotionPiece) {
            result += promotionPiece;
        }
        return result;
    }

    int getSrcRow() const { return srcRow; }
    int getSrcCol() const { return srcCol; }
    int getDstRow() const { return dstRow; }
    int getDstCol() const { return dstCol; }
    int getScore() const { return score; }

    // Setters
    void setScore(int move_score) { score = move_score; }

    // Operator overloads for comparing moves based on score
    bool operator<(const Move& other) const {
        return score < other.score;
    }

    bool operator>(const Move& other) const {
        return score > other.score;
    }

    int operator-(const Move& other) const {
        return score - other.score;
    }

    // Friend function for stream output
    friend std::ostream& operator<<(std::ostream& os, const Move& move) {
        os << move.toString() << " (score: " << move.score << ")";
        return os;
    }
};

// Comparator for the Move class to use with PriorityQueue
struct MoveComparator {
    int operator()(const Move& a, const Move& b) const {
        // Higher scores should have higher priority (reverse order)
        return b.getScore() - a.getScore();
    }
};