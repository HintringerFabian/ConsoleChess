#include "ChessPiece.hpp"

// *****************************************************
// Static ChessPiece Methods
// *****************************************************

/**
 * Check if the delta from has not changed
 * Used to check if the from is the same as the old one in a loop
 * where the positions are incremented/decremented
 * @param x the x delta from
 * @param y the y delta from
 */
bool ChessPiece::delta_changed(int &x, int &y) {
    return !(x == 0 && y == 0);
}

/**
 * Check if the from is on the board
 * @param x the x from
 * @param y the y from
 */
bool ChessPiece::is_on_board(int &x, int &y) {
    return Position(x, y).is_valid();
}

/**
 * Check if the from is empty or has an enemy piece
 * @param board the gameboard with all the pieces
 * @param x the x from on the board
 * @param y the y from on the board
 */
bool ChessPiece::is_empty_or_enemy(const std::unique_ptr<ChessPiece> (&board)[8][8], int &x, int &y) const {
    return is_empty(board, x, y) || is_enemy(board, x, y);
}

/**
 * Check if the from has an enemy piece
 * @param board the gameboard with all the pieces
 * @param x the x from on the board
 * @param y the y from on the board
 */
bool ChessPiece::is_enemy(const std::unique_ptr<ChessPiece> (&board)[8][8], int &x, int &y) const {
    return !is_empty(board, x, y) && board[x][y]->get_color() != this->get_color();
}

bool ChessPiece::is_empty(const std::unique_ptr<ChessPiece> (&board)[8][8], int &x, int &y) {
    return !board[x][y];
}

// *****************************************************
// King Methods
// *****************************************************

King::King(Color color) : ChessPiece(color) {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    if (color == Color::WHITE) {
        set_symbol("K");
    } else {
        set_symbol("k");
    }
#else
    if (color == Color::WHITE) {
        set_symbol("♚");
    } else {
        set_symbol("♔");
    }
#endif
}

VecPos King::get_moves_for(Position position, const std::unique_ptr<ChessPiece> (&board)[8][8]) {
    VecPos valid_moves;

    auto px = position.get_x();
    auto py = position.get_y();

    // king can only move one step in any direction
    // if king was chosen, he can not stay on the same position
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {

            auto x = px + i;
            auto y = py + j;

            if (
                is_on_board(x, y) &&
                is_empty_or_enemy(board, x, y) &&
                is_valid_delta(i, j)
            ) {
                auto new_position = Position(x, y);
                valid_moves.push_back(new_position);
            }
        }
    }

    return valid_moves;
}

bool King::is_valid_delta(int &x, int &y) {
    return delta_changed(x, y);
}

VecPos King::get_diagonal_attackers(const std::unique_ptr<ChessPiece> (&board)[8][8], const Position position) const{
    auto bishop = std::make_unique<Bishop>(this->get_color());
    return bishop->get_moves_for(position, board);
}

VecPos King::get_straight_attackers(const std::unique_ptr<ChessPiece> (&board)[8][8], const Position position) const {
    auto rook = std::make_unique<Rook>(this->get_color());
    return rook->get_moves_for(position, board);
}

VecPos King::get_horse_attackers(const std::unique_ptr<ChessPiece> (&board)[8][8], const Position position) const {
    auto horse = std::make_unique<Knight>(this->get_color());
    return horse->get_moves_for(position, board);
}

/**
 * Check if the king is able to castle to the left or right.
 * @param position of the king
 * @param pPtr to the board
 * @param left check the left side or right side?
 * @param color color of the current player
 * @param flipped is the board flipped?
 * @return A vector of positions that have possible castling moves
 */
VecPos King::get_castling(const Position position, std::unique_ptr<ChessPiece> pPtr[8][8], bool left, Color color, bool flipped) {
    VecPos valid_moves;

    auto x = position.get_x();
    auto y = position.get_y();

    auto king = dynamic_cast<King*>(pPtr[x][y].get());

    // if king already moved, return empty vector
    if (king->has_moved()) {
        return {};
    }

    // we need to act as if the board was flipped
    // if it is flipped, we need to flip the -1 to plus 1
    // -2 to +2 and so on
    bool check = (color == Color::WHITE && !flipped) || (color == Color::BLACK && !flipped);

    auto one = check ? 1 : -1;
    auto two = check ? 2 : -2;
    auto three = check ? 3 : -3;
    auto four = check ? 4 : -4;

    if (left) {
        // check if left side of the king is empty
        // y -4 would be the rook
        if (!pPtr[x][y - one] && !pPtr[x][y - two] && !pPtr[x][y - three]) {
            // check if there is a rook on the left side
            auto rook = dynamic_cast<Rook*>(pPtr[x][y - four].get());
            if (rook && !rook->has_moved()) {
                auto move_one = Position(x, y - one);
                auto new_position = Position(x, y - two);

                valid_moves.push_back(move_one);
                valid_moves.push_back(new_position);
            }
        }
    } else {
        // check if right side of the king is empty
        // y + 3 would be the rook
        if (!pPtr[x][y + one] && !pPtr[x][y + two]) {
            // check if there is a rook on the right side
            auto rook = dynamic_cast<Rook*>(pPtr[x][y + three].get());
            if (rook && !rook->has_moved()) {
                auto move_one = Position(x, y + one);
                auto new_position = Position(x, y + two);

                valid_moves.push_back(move_one);
                valid_moves.push_back(new_position);
            }
        }
    }

    return valid_moves;
}

// *****************************************************
// Queen Methods
// *****************************************************

Queen::Queen(Color color) : ChessPiece(color) {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    if (color == Color::WHITE) {
        set_symbol("Q");
    } else {
        set_symbol("q");
    }
#else
    if (color == Color::WHITE) {
        set_symbol("♛");
    } else {
        set_symbol("♕");
    }
#endif
}

VecPos Queen::get_moves_for(Position position, const std::unique_ptr<ChessPiece> (&board)[8][8]) {
    // We take advantage of the fact that the queen can to like a rook OR a bishop
    auto bishop = std::make_unique<Bishop>(this->get_color());
    auto rook = std::make_unique<Rook>(this->get_color());

    // We get the valid moves of the bishop and the rook.
    auto bishop_moves = bishop->get_moves_for(position, board);
    auto rook_moves = rook->get_moves_for(position, board);

    // We merge the two vectors.
    bishop_moves.insert(bishop_moves.end(), rook_moves.begin(), rook_moves.end());

    // We return the merged vector.
    // This is a copy, so we don't have to worry about the memory getting lost.
    return bishop_moves;
}

// *****************************************************
// Bishop Methods
// *****************************************************

Bishop::Bishop(Color color) : ChessPiece(color) {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    if (color == Color::WHITE) {
        set_symbol("B");
    } else {
        set_symbol("b");
    }
#else
    if (color == Color::WHITE) {
        set_symbol("♝");
    } else {
        set_symbol("♗");
    }
#endif
}

VecPos Bishop::get_moves_for(Position position, const std::unique_ptr<ChessPiece> (&board)[8][8]) {
    VecPos valid_moves;

    auto px = position.get_x();
    auto py = position.get_y();

    // First two loops are for the positive x and y
    // Note that 0 is not included, because the bishop can't to
    // in a straight line, only diagonally
    for (int i = -1; i <= 1; i += 2) {
        for (int j = -1; j <= 1; j += 2) {

            // Third is used to increment the x and y negative or positive
            // depending on the first two loops
            for (int k = 1; k <= 7; ++k) {
                auto x = px + k * i;
                auto y = py + k * j;

                if (is_on_board(x, y)) {
                    if (is_empty_or_enemy(board, x, y)) {
                        auto new_position = Position(x, y);
                        valid_moves.push_back(new_position);
                    }
                }

                // if the from in any direction is not empty,
                // we can't to any further in that direction
                if (!is_on_board(x, y) || !is_empty(board, x, y)) {
                    break;
                }
            }
        }
    }

    return valid_moves;
}

// *****************************************************
// Knight Methods
// *****************************************************

Knight::Knight(Color color) : ChessPiece(color) {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    if (color == Color::WHITE) {
        set_symbol("N");
    } else {
        set_symbol("n");
    }
#else
    if (color == Color::WHITE) {
        set_symbol("♞");
    } else {
        set_symbol("♘");
    }
#endif
}

VecPos Knight::get_moves_for(Position position, const std::unique_ptr<ChessPiece> (&board)[8][8]) {
    VecPos valid_moves;

    auto px = position.get_x();
    auto py = position.get_y();

    // Knight can move in with an L shape in any direction
    // Assuming the position is not filled with our own piece
    for (int i = -2; i <= 2; ++i) {
        for (int j = -2; j <= 2; ++j) {
            auto x = px + i;
            auto y = py + j;

            if (
                is_valid_delta(i, j) &&
                is_on_board(x, y) &&
                is_empty_or_enemy(board, x, y)
            ) {
                auto new_position = Position(x, y);
                valid_moves.push_back(new_position);
            }
        }
    }

    return valid_moves;
}

bool Knight::is_valid_delta(int &x, int &y) {
    return (abs(x) == 2 && abs(y) == 1) || (abs(x) == 1 && abs(y) == 2);
}

// *****************************************************
// Rook Methods
// *****************************************************

Rook::Rook(Color color) : ChessPiece(color) {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    if (color == Color::WHITE) {
        set_symbol("R");
    } else {
        set_symbol("r");
    }
#else
    if (color == Color::WHITE) {
        set_symbol("♜");
    } else {
        set_symbol("♖");
    }
#endif
}

VecPos Rook::get_moves_for(Position position, const std::unique_ptr<ChessPiece> (&board)[8][8]) {
    VecPos valid_moves;

    auto px = position.get_x();
    auto py = position.get_y();

    // First two loops are for the positive x and y
    // Note that 0 is included in the loop, this allows us to check the
    // horizontal and vertical lines, because the rook can only to in
    // a straight line.
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            for (int k = 1; k <= 7; ++k) {

                auto delta_x = k*i;
                auto delta_y = k*j;

                auto x = px + delta_x;
                auto y = py + delta_y;

                auto on_board = is_on_board(x, y);
                auto empty_or_enemy = on_board && is_empty_or_enemy(board, x, y);
                auto valid_delta = on_board && is_valid_delta(delta_x, delta_y);

                if (
                        on_board &&
                        empty_or_enemy &&
                        valid_delta
                ) {
                    auto new_position = Position(x, y);
                    valid_moves.push_back(new_position);
                }

                // if the from in any direction is not empty,
                // we can't to any further in that direction
                if (board[x][y] != nullptr) {
                    break;
                }
            }
        }
    }

    return valid_moves;
}

bool Rook::is_valid_delta(int &x, int &y) {
    return (abs(x) == 0 && abs(y) > 0) || (abs(x) > 0 && abs(y) == 0);
}

// *****************************************************
// Pawn Methods
// *****************************************************

Pawn::Pawn(Color color) : ChessPiece(color) {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    if (color == Color::WHITE) {
        set_symbol("P");
    } else {
        set_symbol("p");
    }
#else
    if (color == Color::WHITE) {
        set_symbol("♟");
    } else {
        set_symbol("♙");
    }
#endif
}

VecPos Pawn::get_moves_for(Position position, const std::unique_ptr<ChessPiece> (&board)[8][8]) {
    VecPos valid_moves;

    // check if the pawn can to 1 up
    auto px = position.get_x();
    auto py = position.get_y();

    bool enemy_ahead = false;

    // block for checking if the pawn can to 1 up
    // this block also checks if there is an enemy 1 up
    {
        auto x = px + 1;
        auto y = py;

        // check if new from is on board and empty or enemy
        if (
            is_on_board(x, y) &&
            is_empty_or_enemy(board, x, y)
        ) {
            auto new_position = Position(x, y);
            valid_moves.push_back(new_position);

            if (is_enemy(board, x, y)) {
                enemy_ahead = true;
            }
        }
    }

    // block for checking if the pawn can to 2 up
    {
        auto x = px + 2;
        auto y = py;

        // check if new from is on board and empty or enemy
        if (
            first_move &&
            !enemy_ahead &&
            is_on_board(x, y) &&
            is_empty(board, x, y)
        ) {
            auto new_position = Position(x, y);
            valid_moves.push_back(new_position);
        }
    }

    // block for checking if the pawn can to 1 up and 1 left
    // aka take enemy piece on the left
    {
        auto x = px + 1;
        auto y = py - 1;

        // check if new from is on board and empty or enemy
        if (
            is_on_board(x, y) &&
            is_enemy(board, x, y)
        ) {
            auto new_position = Position(x, y);
            valid_moves.push_back(new_position);
        }
    }

    // block for checking if the pawn can to 1 up and 1 right
    // aka take enemy piece on the right
    {
        auto x = px + 1;
        auto y = py + 1;

        // check if new from is on board and empty or enemy
        if (
            is_on_board(x, y) &&
            is_enemy(board, x, y)
        ) {
            auto new_position = Position(x, y);
            valid_moves.push_back(new_position);
        }
    }

    return valid_moves;
}

Position Pawn::get_en_passant(Position position, const std::unique_ptr<ChessPiece> (&board)[8][8], const LastMove& last_move) {
    // get the delta x and y from the last move
    auto delta_x = last_move.get_to().get_x() - last_move.get_from().get_x();
    auto delta_y = last_move.get_to().get_y() - last_move.get_from().get_y();

    // if delta y is not 2, the last move is not a move that allows en passant
    if (abs(delta_x) != 2) {
        return {-1, -1};
    }

    // if the delta x is not 0, the last move is not a move that allows en passant
    if (delta_y != 0) {
        return {-1, -1};
    }

    // get the current position of the enemy pawn / the pawn that was moved last turn
    auto enemy_pawn_x = last_move.get_to().get_x();
    auto enemy_pawn_y = last_move.get_to().get_y();

    // if the enemy is not at array position 3 (4 on the board), it is not a move that allows en passant
    if (enemy_pawn_x != 4) {
        return {-1, -1};
    }

    // check if the enemy pawn is standing directly next to the pawn
    // if not the last move is not a move that allows en passant
    if (abs(enemy_pawn_y - position.get_y()) != 1) {
        return {-1, -1};
    }

    // if there is another piece directly behind the enemy pawn that moved last turn
    // the last move is not a move that allows en passant
    if (board[enemy_pawn_x - 1][enemy_pawn_y] != nullptr) {
        return {-1, -1};
    }

    // if all those checks pass, the last move is a move that allows en passant
    // return the position of the enemy pawn with enemy pawn x - 1
    return {enemy_pawn_x + 1, enemy_pawn_y};
}
