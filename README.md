# Chess Game with Parallel Move Recommendation

This project implements a complete chess game with AI move recommendation using parallel processing, featuring full chess rules including checkmate detection and castling.

## How to Build and Run

### Compilation Instructions:
```bash
mkdir build
cd build
cmake ..
make
./ChessGame
```

### Running the Game:
```bash
./ChessGame
```

**Example Run Session:**
```
=== Chess Game with Parallel Move Calculation ===
Available CPU cores: 8

Enter search depth for move recommendations (1-3): 2

Select game mode:
1. User vs User
2. User vs Computer
3. Computer vs Computer (Auto play)
4. Run performance benchmarks
Enter choice (1-4): 2

=== USER vs COMPUTER MODE ===
You are playing as White. Computer is Black.
Using default 8 threads for move calculation.

===== RECOMMENDED MOVES =====
1. a7c6 (score: 15)
2. b3c3 (score: 12)
3. b1fc (score: 10)

Player 1 (White - Capital letters) >> a7c6
Valid move
Computer is thinking...
Computer plays: g1f1
Computer move valid
Computer has moved. Your turn!
```

## Project Structure

```
Chess/
├── include/                       # Header files
│   ├── AutoPlayer.h              # Simple AI player for demonstrations
│   ├── Benchmark.h               # Performance testing framework
│   ├── Board.h                   # Chess board with castling and promotion
│   ├── Chess.h                   # ASCII UI and input handling
│   ├── CMakeLists.txt            # CMake configuration for include
│   ├── ComputerPlayer.h          # Advanced AI player with debugging
│   ├── DebugUtility.h            # Debugging and validation utilities
│   ├── Game.h                    # Game logic with endgame detection
│   ├── Move.h                    # Chess move representation
│   ├── MoveRecommender.h         # Base move evaluation algorithm
│   ├── ParallelMoveRecommender.h # Multithreaded move calculation
│   ├── Piece.h                   # Chess pieces with castling support
│   ├── PriorityQueue.h           # Template priority queue
│   ├── ThreadPool.h              # Thread pool implementation
│   └── ThreadSafePriorityQueue.h # Thread-safe priority queue
├── src/                           # Implementation files
│   ├── Board.cpp                 # Board implementation
│   ├── Chess.cpp                 # UI implementation
│   ├── CMakeLists.txt            # CMake configuration for src
│   ├── Game.cpp                  # Game logic implementation
│   ├── main.cpp                  # Program entry point and game mode selection
│   └── Piece.cpp                 # Piece implementation
├── cmake-build-debug/            # CMake build directory (ignored)
├── img/                          # Image resources
├── .gitignore                    # Git ignore configuration
├── CMakeLists.txt                # Main CMake configuration
├── CONTRIBUTING.md               # Contribution guidelines
├── PRACTICES.md                  # Development practices
└── README.md                     # This file
```

## Implemented Features

### Chess Rules Implementation (2 of 3 required):

#### ✅ **1. Checkmate Detection**
- **Complete Implementation**: The game automatically detects when a player has no legal moves to escape check
- **Algorithm**: 
  - Uses `isKingInCheck()` to verify the king is under attack
  - Generates all possible moves with `getAllValidMoves()`
  - Confirms no move can remove the check condition
- **Game Termination**: Automatically ends game and declares winner
- **Integration**: Works across all game modes (User vs User, User vs Computer, Computer vs Computer)

#### ✅ **2. Castling (Both Kingside and Queenside)**
- **Complete Implementation**: Full castling mechanics for both sides
- **Validation Conditions**:
  - King and rook must not have moved (`hasKingMoved()`, `hasRookMoved()`)
  - No pieces between king and rook
  - King not currently in check
  - King doesn't pass through or end in check squares
- **Move Notation**: King moves 2 squares (e.g., "e1g1" for white kingside)
- **Execution**: `performCastle()` automatically moves both king and rook
- **State Tracking**: Maintains movement history for all pieces

#### ❌ **3. Draw Conditions (Not Implemented)**
- **Note**: Only 2 of 3 rules were required
- **Implemented Alternative**: Stalemate detection (king not in check but no legal moves)

### Additional Features Implemented:

#### **🎯 Computer Player Algorithm**
- **Strategy**: Uses existing `ParallelMoveRecommender` with minimax-style evaluation
- **Selection**: Always chooses highest-scoring move from AI recommendations
- **Pawn Promotion**: Automatic queen promotion for computer players
- **Performance**: Runs with 8 threads by default for optimal speed

#### **🚀 Parallel Processing**
- **ThreadPool Architecture**: Distributes move calculations across multiple threads
- **Thread Safety**: `ThreadSafePriorityQueue` with mutex protection
- **Performance Monitoring**: Tracks calculation time and thread utilization
- **Scalability**: Supports 1-8 threads with performance benchmarks

#### **📊 Performance Benchmarking**
- **Multi-threaded Testing**: Compares performance across different thread counts
- **Metrics**: Total time, per-move time, speedup ratios
- **Validation**: Ensures parallel correctness matches sequential results

#### **🎮 Multiple Game Modes**
- **User vs User**: Traditional two-player with full board display
- **User vs Computer**: Human vs AI with move recommendations
- **Computer vs Computer**: AI demonstration mode
- **Benchmark Mode**: Performance analysis tool

## Computer Player Algorithm

### Move Selection Strategy:
The computer player uses a sophisticated **minimax-style evaluation** system:

1. **Move Generation**: Evaluates all legal moves using `ParallelMoveRecommender`
2. **Scoring System**: 
   - **Piece Values**: Pawn(100), Knight/Bishop(300), Rook(500), Queen(900), King(10000)
   - **Tactical Bonuses**: Captures (+10×piece_value), central control (+10), threats (+5)
   - **Safety Penalties**: Pieces in danger from weaker opponents (-15)
3. **Depth Analysis**: Recursive evaluation 1-3 moves ahead considering opponent responses
4. **Selection**: Always chooses the highest-scoring move from recommendations
5. **Special Handling**: Automatic queen promotion for pawn promotion moves

### Algorithm Formula:
```
Move Score = Base Position Value 
           + Capture Bonus 
           + Central Control Bonus 
           + Threat Bonus 
           - Danger Penalty 
           - Opponent's Best Response Score
```

## Testing and Validation

### Testing Methods Used:

#### **1. Manual Testing**
- **Complete Games**: Played full User vs Computer games to verify all rules
- **Chess Rules**: Tested checkmate, stalemate, castling, pawn promotion
- **Move Validation**: Verified all piece movement rules and restrictions
- **User Interface**: Tested input validation and error handling

#### **2. Computer Player Validation**
- **Algorithm Consistency**: Computer always selects from algorithmically validated moves
- **Move Legality**: All computer moves pass through the same validation as human moves
- **Error Detection**: Game terminates gracefully if invalid moves are generated
- **Performance Monitoring**: Tracks move calculation time and quality

### Edge Cases Tested:

#### ✅ **Checkmate Scenarios**
- **Fool's Mate**: Fastest possible checkmate (4 moves)
- **Scholar's Mate**: Common beginner trap (4 moves)
- **Back Rank Mate**: King trapped by own pieces
- **Various Endgames**: Queen vs King, Rook vs King scenarios

#### ✅ **Castling Edge Cases**
- **Castling Through Check**: Prevented correctly
- **Castling After King Movement**: Properly blocked
- **Castling After Rook Movement**: Correctly invalidated
- **Castling with Blocked Path**: Pieces between king and rook

#### ✅ **Stalemate Detection**
- **King with No Legal Moves**: But not in check
- **All Pieces Blocked**: No legal moves available for any piece

#### ✅ **Pawn Promotion**
- **Manual Promotion**: User prompted for piece choice
- **Computer Promotion**: Automatic queen selection
- **Invalid Promotion**: Proper error handling

#### ✅ **Check Detection and Response**
- **Check Escape**: Only legal moves allowed when in check
- **Invalid Check Moves**: Moves that would leave king in check rejected
- **Check Announcements**: Proper notification when check occurs

## Challenges and Solutions

### **Biggest Challenge Encountered:**
**Castling Implementation Complexity**

The most challenging aspect was implementing castling correctly because it involves:
- **Multiple Piece Coordination**: King and rook must move simultaneously
- **Complex Validation**: Five different conditions must all be met
- **Check Path Verification**: King cannot pass through squares under attack
- **State Tracking**: Must track movement history for king and both rooks
- **Integration Complexity**: Required changes across Board, Piece, and Game classes

### **Other Technical Challenges:**
- **Thread Safety**: Implementing parallel move calculation without race conditions
- **Turn Management**: Coordinating User vs Computer mode properly
- **Pawn Promotion**: Handling automatic vs manual promotion consistently

## Future Improvements

Given more time, I would implement:

### **1. Enhanced User Experience**
- **Save/Load Games**: Serialize complete game state to files
- **Undo Move Functionality**: Allow players to take back moves
- **Move History Display**: Show complete game notation
- **Chess Timer System**: Add time controls for competitive play

### **2. Advanced AI Features**
- **Opening Book**: Database of strong opening move sequences
- **Endgame Tablebase**: Perfect play in simple endgame positions
- **Variable Difficulty Levels**: Adjustable AI strength
- **Position Evaluation**: More sophisticated evaluation functions

### **3. Technical Enhancements**
- **Comprehensive Unit Tests**: Automated test suite for all components
- **Graphical User Interface**: Modern GUI instead of ASCII board
- **Network Multiplayer**: Online play capability
- **Chess Engine Protocol**: UCI/XBoard compatibility for external tools

## Dependencies

- **C++17 or later**: Modern C++ features used throughout
- **CMake 3.10+**: Build system
- **Threading Support**: pthread/std::thread
- **Standard Library Only**: No external dependencies required

---