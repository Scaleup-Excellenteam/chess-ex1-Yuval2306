#include "Chess.h"
#include "Game.h"
#include <iostream>
#include <string>

int main()
{
    std::string board = "RNBQKBNRPPPPPPPP################################pppppppprnbqkbnr";
//  std::string board = "##########K###############################R#############r#r#####";

    std::cout << "=== Chess Game Initialization ===" << std::endl;
    Chess a(board);
    Game chessGame(board);

    // Let the user choose search depth for move recommendations
    int searchDepth;
    std::cout << "Enter search depth for move recommendations (1-3): ";
    std::cin >> searchDepth;
    chessGame.setSearchDepth(searchDepth);

    // Clear the input buffer
    std::cin.clear();
    std::cin.ignore(10000, '\n');

    int codeResponse = 0;
    std::string res = a.getInput();

    while (res != "exit")
    {
        /*
        codeResponse value :
        Illegal movements :
        11 - there is not piece at the source
        12 - the piece in the source is piece of your opponent
        13 - there one of your pieces at the destination
        21 - illegal movement of that piece
        31 - this movement will cause you checkmate

        legal movements :
        41 - the last movement was legal and cause check
        42 - the last movement was legal, next turn
        */

        codeResponse = chessGame.validateMove(res);
        a.setCodeResponse(codeResponse);
        res = a.getInput();
    }

    std::cout << std::endl << "Exiting " << std::endl;
    return 0;
}