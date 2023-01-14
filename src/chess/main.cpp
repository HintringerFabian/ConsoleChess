#include "pieces/ChessPiece.hpp"
#include "gameboard/GameBoard.hpp"
#include "game/Game.hpp"

int main() {

    // Possible options for the game_type are:

    // NORMAL, CHECKMATE, STALEMATE, PROMOTION, CASTLING, EN_PASSANT, CHECK
    // Normal will create a normal game
    // Checkmate will create a game where white can create a checkmate with one move
    // Stalemate will create a game where white can create a stalemate with one move
    // Promotion will create a game where white can promote a pawn with 1 to (and possibly create a checkmate)
    // En passant will create a game where white can capture a pawn with en passant with 2 moves
    // Castling will create a game where white/black can castle with 1 move, also the detection of check when castling is tested
    // Check will create a game where white can create a check with one move
    auto game_type = GameTest::CHECK;

    Game game{game_type};
    game.play();
}
