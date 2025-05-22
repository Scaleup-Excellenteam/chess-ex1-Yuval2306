# Chess Game with Move Recommendation

This project implements a chess game with a move recommendation system that uses a template priority queue to rank and suggest the best moves to the player.

## Algorithm Description

The move recommendation algorithm evaluates potential moves and suggests the best options to the player. Implementation details:

1. **Structure Creation**: A template `PriorityQueue` holds moves sorted by score, keeping the top 5 best moves.

2. **Move Generation and Evaluation Process**:
   - The algorithm iterates through all pieces of the current player
   - For each piece, all valid moves are calculated (checking board boundaries, piece movement rules, etc.)
   - Each valid move is evaluated with a sophisticated scoring system
   - Moves are inserted into the priority queue according to their scores

3. **Recursive Depth Analysis**:
   - The algorithm is recursive with a configurable depth (1-3)
   - For each candidate move at the current depth:
     - A temporary board is created by copying the current board
     - The move is applied to the temporary board
     - The evaluation considers the current move and opponent's best response
     - At depth 0: Calculate the immediate score of player's moves
     - At depth 1: Calculate opponent's best response and subtract its score
     - At depth 2: Calculate player's counter-responses and add the best score
     - This minimax-like approach helps predict several moves ahead

4. **Detailed Scoring Criteria**:
   - **Piece Capture**: If a move captures an opponent's piece, add `CAPTURE_BONUS_MULTIPLIER (10) × captured piece value`
   - **Central Control**: If a move places a piece in the central 4×4 squares, add `CENTRAL_SQUARES_BONUS (10)`
   - **Danger Assessment**: Check if the moved piece would be threatened by a weaker piece, apply `PIECE_IN_DANGER_PENALTY (-15)`
   - **Threat Evaluation**: If the moved piece threatens a stronger opponent piece, add `THREAT_BONUS (5)`
   - **Recursive Evaluation**: Subtract the score of the opponent's best response

5. **Piece Values**:
   - Pawn: 100
   - Knight: 300
   - Bishop: 300
   - Rook: 500
   - Queen: 900
   - King: 10000

6. **Move Ranking and Display**:
   - The priority queue's `push` method inserts each evaluated move in the correct position
   - The internal comparator (`MoveComparator`) sorts moves by their scores in descending order
   - The top 3 moves with highest scores are displayed to the player

## Algorithm Complexity

1. **Time Complexity**:
   - For a single position evaluation: O(N²) where N is the number of squares on the board (64)
   - For the complete algorithm with depth d: O(M^d × N²) where M is the average number of legal moves per position

2. **Space Complexity**:
   - O(d × N) where d is the depth and N is the board size
   - Each recursive call creates a new board copy

## Custom Exceptions

The project implements two custom exceptions:

1. **QueueEmptyException**: Thrown when trying to poll from an empty priority queue.
2. **QueueFullException**: Thrown when trying to push to a full priority queue.
3. **PawnPromotionException**: Thrown when a pawn reaches the opposite end of the board, allowing the player to choose a promotion piece.

## Project Structure

```
Chess Game
├── include/         # Header files
│   ├── Board.h      # Chess board representation
│   ├── Chess.h      # UI and input handling
│   ├── Game.h       # Game logic and move validation
│   ├── Move.h       # Chess move representation
│   ├── MoveRecommender.h  # Move evaluation algorithm
│   ├── Piece.h      # Chess piece classes
│   └── PriorityQueue.h  # Template priority queue
└── src/             # Implementation files
    ├── Board.cpp    # Board implementation
    ├── Chess.cpp    # UI implementation
    ├── Game.cpp     # Game logic implementation
    ├── main.cpp     # Program entry point
    └── Piece.cpp    # Piece implementation
```

## File Descriptions

- **main.cpp**: Entry point that initializes the game and handles the main game loop.
- **PriorityQueue.h**: Template class for the priority queue with custom comparator.
- **Move.h**: Represents a chess move with source, destination, and score.
- **MoveRecommender.h**: Implements the move recommendation algorithm.
- **Game.h/cpp**: Manages game state and validates moves.
- **Board.h/cpp**: Represents the chess board and handles piece movement.
- **Piece.h/cpp**: Defines the chess pieces and their movement rules.
- **Chess.h/cpp**: Handles user interface and input processing.

## How to Build and Run

```bash
mkdir build
cd build
cmake ..
make
./ChessGame
```

When the game starts, you'll be prompted to enter a search depth (1-3) for move recommendations.
 The game will display the three best moves before each turn. 
 Input your move in the format "b2c4" (source square to destination square).