#include "Chess.h"
#include "Game.h"
#include <iostream>

int main()
{
    string board = "RNBQKBNRPPPPPPPP################################pppppppprnbqkbnr";
//	string board = "##########K###############################R#############r#r#####";

    std::cout << "=== Chess Game Initialization ===" << std::endl;
    Chess a(board);
    Game chessGame(board);

    int codeResponse = 0;
    string res = a.getInput();


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

    cout << endl << "Exiting " << endl;
    return 0;
}