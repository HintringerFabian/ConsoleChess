#include <ranges>
#include "GameBoard.hpp"

// *****************************************************
// Public Methods
// *****************************************************

GameBoard::GameBoard(GameTest option) {
    // initialize the board with nullptrs
    for (auto& row : board) {
        for (auto& position : row) {
            // init the unique pointer
            position = nullptr;
        }
    }

    switch (option) {
        case GameTest::NORMAL:
            init_normal();
            break;
        case GameTest::CHECK:
            init_check();
            break;
        case GameTest::CHECKMATE:
            init_checkmate();
            break;
        case GameTest::STALEMATE:
            init_stalemate();
            break;
        case GameTest::PROMOTION:
            init_promotion();
            break;
        case GameTest::EN_PASSANT:
            init_en_passant();
            break;
        case GameTest::CASTLING:
            init_castling();
            break;
    }
}

GameBoard::GameBoard(const GameBoard& other) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (other.board[i][j] != nullptr) {
                switch(other.board[i][j]->get_name()) {
                    case 'K':
                        board[i][j] = make_unique<King>(other.board[i][j]->get_color());
                        break;
                    case 'Q':
                        board[i][j] = make_unique<Queen>(other.board[i][j]->get_color());
                        break;
                    case 'R':
                        board[i][j] = make_unique<Rook>(other.board[i][j]->get_color());
                        break;
                    case 'B':
                        board[i][j] = make_unique<Bishop>(other.board[i][j]->get_color());
                        break;
                    case 'N':
                        board[i][j] = make_unique<Knight>(other.board[i][j]->get_color());
                        break;
                    case 'P':
                        board[i][j] = make_unique<Pawn>(other.board[i][j]->get_color());
                        break;
                    default:
                        board[i][j] = nullptr;
                        break;
                }
            }
        }
    }
}

/**
 * Prints the current gameboard.
 */
void GameBoard::print() const {
    clear_screen();
    std::string character_row = "     |  A  |  B  |  C  |  D  |  E  |  F  |  G  |  H  |     ";
    std::string split_row = "-----+-----+-----+-----+-----+-----+-----+-----+-----+-----\n";
    int row_number = (flipped) ? 1 : 8;

    if (flipped) {
        std::reverse(character_row.begin(), character_row.end());
    }
    character_row += "\n";

    std::cout << character_row;

    for (const auto& row : board | std::views::reverse) {

        std::cout << split_row;
        std::cout << "  " << row_number << "  |";

        for (const auto & field : row) {
            std::string symbol = (field ? field->get_symbol() : " ");
            std::cout << "  " << symbol << "  |";
        }

        std::cout << "  " << row_number << std::endl;
        (flipped) ? ++row_number : --row_number;
    }

    std::cout << split_row << character_row << std::endl;
}

/**
 * Prints the current gameboard with the valid moves for the given from.
 * @param valid_moves The valid moves for the given from.
 */
void GameBoard::print(const VecPos& valid_moves) const {
    clear_screen();
    std::string character_row = "     |  A  |  B  |  C  |  D  |  E  |  F  |  G  |  H  |     ";
    std::string split_row = "-----+-----+-----+-----+-----+-----+-----+-----+-----+-----\n";
    int row_number = (flipped) ? 1 : 8;

    if (flipped) {
        std::reverse(character_row.begin(), character_row.end());
    }
    character_row += "\n";

    std::cout << character_row;

    for (auto x = 7; x >= 0; --x) {
        const auto & row = board[x];

        std::cout << split_row;
        std::cout << "  " << row_number << "  |";

        for (auto y = 0; y < 8; ++y) {
            auto field = row[y].get();
            std::string sym_prefix = "  ";
            std::string sym_suffix = "  |";

            for (const auto & move : valid_moves) {
                if (move.get_x() == x && move.get_y() == y) {
                    sym_prefix = " (";
                    sym_suffix = ") |";
                }
            }

            std::string symbol = (field ? field->get_symbol() : " ");
            std::cout << sym_prefix << symbol << sym_suffix;
        }

        std::cout << "  " << row_number << std::endl;
        (flipped) ? ++row_number : --row_number;
    }

    std::cout << split_row << character_row;
}

/**
 * Gets the valid moves for the piece on the given from.
 * If there is no piece on the given from, an empty vector is returned.
 * @param position The from.
 * @return The valid moves.
 */
VecPos GameBoard::get_valid_moves_for(const Position position) {
    auto row = position.get_x();
    auto column = position.get_y();
    auto piece = board[row][column].get();

    if (!piece) {
        return {};
    }

    // standard check for moves
    auto moves = piece->get_moves_for(position, this->board);

    {
        // check for en passant

        // position is initialized with -1, -1, so if there is no en passant,
        // move_piece is not affected by this check
        Position en_passant_pos;
        if (piece->get_name() == 'P' && last_move.get_name() == 'P') {
            en_passant_pos = Pawn::get_en_passant(position, this->board, last_move);

            if (en_passant_pos.is_valid()) {
                moves.push_back(en_passant_pos);
            }
        }
        en_passant_target = en_passant_pos;
    }

    validate_moves(position, piece, moves);

    // we check for castling after validating the moves, because we validate the moves
    // ourselves with some additional checks
    {
        VecPos left_castle_moves;
        VecPos right_castle_moves;

        if (piece->get_name() == 'K') {
            auto color = piece->get_color();
            bool left{true}, right{false};
            // check for castling on the left side
            left_castle_moves = King::get_castling(position, this->board, left, color, flipped);

            // if validate_moves returns a vector smaller than 2, castling is not possible
            // as the king would be in check
            auto size = left_castle_moves.size();
            validate_moves(position, piece, left_castle_moves);
            if (left_castle_moves.size() != size) {
                left_castle_moves.clear();
            }

            // check for castling on the right side
            right_castle_moves = King::get_castling(position, this->board, right, color, flipped);

            // if validate_moves returns a vector smaller than 2, castling is not possible
            // as the king would be in check
            size = right_castle_moves.size();
            validate_moves(position, piece, right_castle_moves);
            if (right_castle_moves.size() != size) {
                right_castle_moves.clear();
            }

            // insert only the last move of the left and right castling moves
            // as the king is the only piece that can move in castling,
            // and it moves two fields
            // we also clear castling target, as we need to refill it with the correct targets for this turn
            castling_targets.clear();

            if (!left_castle_moves.empty()) {
                moves.push_back(left_castle_moves.back());
                castling_targets.push_back(left_castle_moves.back());
            }
            if (!right_castle_moves.empty()) {
                moves.push_back(right_castle_moves.back());
                castling_targets.push_back(right_castle_moves.back());
            }
        }
    }

    // all moves are validated
    // normal moves are validated
    // and the castling moves are validated manually with some additional checks
    return moves;
}

/**
 * Validates the moves for the given piece.
 * A move is invalid if the king would be in check after the move,
 * or if the move would put the king in check.
 * @param position of the piece.
 * @param piece The piece.
 * @param moves The moves to validate.
 */
void GameBoard::validate_moves(const Position &position, ChessPiece *piece, VecPos &moves) const {
    auto piece_color = piece->get_color();

    auto iter = moves.begin();
    while (iter != moves.end()) {

        // create a copy of the board
        // so we don't have to undo the move and we don't change the real board
        auto tmp_board = GameBoard{*this};
        tmp_board.move_piece(position, *iter, false);

        if (tmp_board.is_king_in_check(piece_color)) {
            iter = moves.erase(iter);
        } else {
            ++iter;
        }
    }
}

/**
 * Moves the piece from the given from to the given to.
 * @param current_player The color of the player that is moving.
 */
bool GameBoard::is_king_in_check(Color current_player) const {
    King* king = nullptr;
    Position king_pos;

    // iterate over the board to find the king
    // if the king was found create the king_pos
    // break the loop
    find_king(current_player, king, king_pos);

    // if the king was not found throw an exception as this should not happen
    if (!king) {
        throw std::runtime_error("King not found");
    }

    auto diagonalAttackers = king->get_diagonal_attackers(this->board, king_pos);
    auto straightAttackers = king->get_straight_attackers(this->board, king_pos);
    auto horseAttackers = king->get_horse_attackers(this->board, king_pos);

    return king_in_check(current_player, king_pos, diagonalAttackers, straightAttackers, horseAttackers);
}

/**
 * Checks the whole board for the given color, searching for the king.
 * Put the king in the king parameter and the position in the king_pos parameter.
 * @param current_player The color of the player that is moving.
 * @param king The king.
 * @param king_pos The position of the king.
 */
void GameBoard::find_king(const Color &current_player, King *&king, Position &king_pos) const {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (board[i][j] != nullptr && board[i][j]->get_name() == 'K' && board[i][j]->get_color() == current_player) {
                king = dynamic_cast<King*>(board[i][j].get());
                king_pos = Position{i, j};
                return;
            }
        }
    }
}

/**
 * Checks if the piece at the given from is of the players player
 * @param position The from of the piece.
 * @param color The player of the player.
 * @return Returns a false Position if the piece is not of the players player.
 *        Returns the from if the piece is of the players player.
 */
Position GameBoard::check_piece(Position position, Color color) {
    auto x = position.get_x();
    auto y = position.get_y();

    auto piece = board[x][y].get();

    if (piece && piece->get_color() == color) {
        return position;
    }

    return {-1, -1};
}

/** Player interaction to choose the piece to promote to.
 * @return The char of the piece to promote to.
*/
char GameBoard::get_promotion_piece() {
    char promotion_piece = ' ';

    // while promotion piece is not Q or R or B or N
    while (promotion_piece != 'Q' && promotion_piece != 'R' && promotion_piece != 'B' && promotion_piece != 'N') {
        std::cout << "Promote to Q, R, B or N: ";
        std::cin >> promotion_piece;
    }

    return promotion_piece;
}

/**
 * Moves the piece from the old from to the new from.
 * Clears the enemys piece if there is one.
 * @param old_pos The old from.
 * @param new_pos The new from.
 */
void GameBoard::move_piece(Position old_pos, Position new_pos, bool not_tmp) {
    auto old_x = old_pos.get_x();
    auto old_y = old_pos.get_y();
    auto new_x = new_pos.get_x();
    auto new_y = new_pos.get_y();

    // check if there is a piece on the new from
    // if there is a piece on the new from, delete it
    if (board[new_x][new_y]) {
        board[new_x][new_y].reset();
    }

    // some pieces have special moves, which occur when they do their first move,
    // so we need to track the first move of the piece

    // if the piece to is a pawn, set the value that the pawn has moved
    if (board[old_x][old_y]->get_name() == 'P') {
        dynamic_cast<Pawn*>(board[old_x][old_y].get())->set_moved(true);
    }

        // if the piece to is a king, set the value that the king has moved
    else if (board[old_x][old_y]->get_name() == 'K') {
        dynamic_cast<King*>(board[old_x][old_y].get())->set_moved(true);
    }

        // if the piece to is a rook, set the value that the rook has moved
    else if (board[old_x][old_y]->get_name() == 'R') {
        dynamic_cast<Rook*>(board[old_x][old_y].get())->set_moved(true);
    }

    // if en passant target is valid and matches the new_pos, we need to delete the pawn which was captured
    if (en_passant_target.is_valid() && en_passant_target == new_pos) {
        // old_x is the row of the pawn which was captured
        // new_y is the column of the pawn which was captured
        board[old_x][new_y].reset();
    }

    // extra check for castling
    // if the piece to is a king and the new from is two fields away from the old from
    // we need to move the rook as well
    if (board[old_x][old_y]->get_name() == 'K' && abs(new_y - old_y) == 2) {
        // if the new from is two fields to the right of the old from
        // we need to move the rook on the right side
        if (new_y - old_y == 2) {
            // move the rook to the right side
            board[new_x][new_y - 1] = std::move(board[new_x][7]);
            board[new_x][7].reset();
        }

            // if the new from is two fields to the left of the old from
            // we need to move the rook on the left side
        else {
            // move the rook to the left side
            board[new_x][new_y + 1] = std::move(board[new_x][0]);
            board[new_x][0].reset();
        }
    }

    // to the piece
    board[new_x][new_y] = std::move(board[old_x][old_y]);

    // bool not_tmp is manually set to true or false at 2 positions
    // it has to be manually set because we do not want to check for a promotion
    // when we are checking if the king is in check, there we check all possible moves
    // and also to the piece, but we do not want to check for a promotion
    if (not_tmp) {
        // if this is not the tmp_board which is used to check if the king is in check
        // then we need to

        // promotion check
        auto player_color = board[new_x][new_y]->get_color();
        do_possible_promotion(new_x, new_y, player_color);

        // set the last move to the piece which was moved
        auto current_move = LastMove{board[new_x][new_y]->get_name(), old_pos, new_pos};
        last_move = current_move;
    }
}

/**
 * Check if the game is over
 * @return private variable game_over
 */
bool GameBoard::is_game_over(const Color current_player) {
    auto enemy_color = (current_player == Color::WHITE) ? Color::BLACK : Color::WHITE;

    // check if the king is in check
    if (!is_king_in_check(enemy_color)) {
        return false;
    }

    return !has_moves_left(enemy_color);
}

bool GameBoard::is_stalemate() {
    // check if the king is in check
    if (is_king_in_check(Color::WHITE) || is_king_in_check(Color::BLACK)) {
        return false;
    }

    auto is_stalemate = !has_moves_left(Color::WHITE) || !has_moves_left(Color::BLACK);

    return is_stalemate;
}

/**
 * Flips the gameboard.
 * This is used to print the gameboard from the perspective of the other player.
 */
void GameBoard::flip() {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 8; ++j) {
            std::swap(board[i][j], board[7 - i][7 - j]);
        }
    }

    // flip the last move
    last_move.set_to({abs(7 - last_move.get_to().get_x()), abs(7-last_move.get_to().get_y())});
    last_move.set_from({abs(7 - last_move.get_from().get_x()), abs(7-last_move.get_from().get_y())});

    // flip the en passant target
    //if (en_passant_target.is_valid()) {
    //    en_passant_target = Position{abs(7 - en_passant_target.get_x()), abs(7 - en_passant_target.get_y())};
    //}

    flipped = !flipped;
}

// *****************************************************
// Private Methods
// *****************************************************

void GameBoard::init_normal() {
    // create standard chess board
    board[7][0] = make_unique<Rook>(Color::BLACK);
    board[7][1] = make_unique<Knight>(Color::BLACK);
    board[7][2] = make_unique<Bishop>(Color::BLACK);
    board[7][3] = make_unique<Queen>(Color::BLACK);
    board[7][4] = make_unique<King>(Color::BLACK);
    board[7][5] = make_unique<Bishop>(Color::BLACK);
    board[7][6] = make_unique<Knight>(Color::BLACK);
    board[7][7] = make_unique<Rook>(Color::BLACK);

    for (int i = 0; i < 8; ++i) {
        board[6][i] = make_unique<Pawn>(Color::BLACK);
        board[1][i] = make_unique<Pawn>(Color::WHITE);
    }

    board[0][0] = make_unique<Rook>(Color::WHITE);
    board[0][1] = make_unique<Knight>(Color::WHITE);
    board[0][2] = make_unique<Bishop>(Color::WHITE);
    board[0][3] = make_unique<Queen>(Color::WHITE);
    board[0][4] = make_unique<King>(Color::WHITE);
    board[0][5] = make_unique<Bishop>(Color::WHITE);
    board[0][6] = make_unique<Knight>(Color::WHITE);
    board[0][7] = make_unique<Rook>(Color::WHITE);
}

void GameBoard::init_check() {
    board[7][0] = make_unique<Rook>(Color::WHITE);
    board[7][3] = make_unique<Queen>(Color::WHITE);
    board[7][1] = make_unique<King>(Color::WHITE);
    board[0][1] = make_unique<King>(Color::BLACK);
}


void GameBoard::init_checkmate() {
    board[2][1] = make_unique<Rook>(Color::WHITE);
    board[3][2] = make_unique<Rook>(Color::WHITE);
    board[7][7] = make_unique<King>(Color::WHITE);
    board[0][0] = make_unique<King>(Color::BLACK);
}

void GameBoard::init_stalemate() {
    board[1][0] = make_unique<King>(Color::WHITE);
    board[2][1] = make_unique<Queen>(Color::BLACK);
    board[2][7] = make_unique<Queen>(Color::BLACK);
    board[7][7] = make_unique<King>(Color::BLACK);
}

void GameBoard::init_promotion() {
    board[6][0] = make_unique<Pawn>(Color::WHITE);
    board[7][6] = make_unique<Queen>(Color::WHITE);
    board[0][0] = make_unique<King>(Color::WHITE);
    board[7][7] = make_unique<King>(Color::BLACK);
}

void GameBoard::init_en_passant() {
    board[6][0] = make_unique<Pawn>(Color::BLACK);
    board[0][0] = make_unique<King>(Color::BLACK);
    board[7][7] = make_unique<King>(Color::WHITE);
    board[3][1] = make_unique<Pawn>(Color::WHITE);

    dynamic_cast<Pawn*>(board[3][1].get())->set_moved(true);
}

void GameBoard::init_castling() {
    board[0][0] = make_unique<Rook>(Color::WHITE);
    board[0][4] = make_unique<King>(Color::WHITE);
    board[0][7] = make_unique<Rook>(Color::WHITE);
    board[7][4] = make_unique<King>(Color::BLACK);
    board[7][7] = make_unique<Rook>(Color::BLACK);
    board[7][0] = make_unique<Rook>(Color::BLACK);
}

/**
 * Clear the screen, implemented for Windows and Linux.
 */
void GameBoard::clear_screen() {
#ifdef WINDOWS
    // command for windows to clear screen
    std::cout << "\033[2J\033[1;1H";
#else
    // command for linux to clear screen
    // does not work in CLion terminal but works in linux terminal
    std::cout << "\x1B[2J\x1B[H";
#endif
}

/**
 * Runs checks to figure out if the king is in check.
 * @param current_player
 * @param king_pos
 * @param diagonal_moves
 * @param straight_moves
 * @param knight_moves
 * @return true if the king is in check, false otherwise
 */
bool GameBoard::king_in_check(Color current_player,
                              const Position &king_pos,
                              const VecPos& diagonal_moves,
                              const VecPos& straight_moves,
                              const VecPos& knight_moves) const
{
    if (diagonal_moves.empty() && straight_moves.empty() && knight_moves.empty()) {
        return false;
    }

    // check if the king is in check by a diagonal attacker
    for (auto & diagonal_move : diagonal_moves) {
        auto piece = board[diagonal_move.get_x()][diagonal_move.get_y()].get();

        if (
                piece &&
                piece->get_color() != current_player &&
                (piece->get_name() == 'B' || piece->get_name() == 'Q' || piece->get_name() == 'P')
                ) {
            if (piece->get_name() == 'P') {
                auto delta_x = diagonal_move.get_x() - king_pos.get_x();
                auto delta_y = diagonal_move.get_y() - king_pos.get_y();

                if (delta_x == 1 && abs(delta_y) == 1) {
                    return true;
                }

            } else {
                return true;
            }
        }
    }

    // check if the king is in check by a straight attacker
    for (auto & straight_move : straight_moves) {
        auto piece = board[straight_move.get_x()][straight_move.get_y()].get();

        if (
                piece &&
                piece->get_color() != current_player &&
                (piece->get_name() == 'R' || piece->get_name() == 'Q')
                ) {
            return true;
        }
    }

    // check if the king is in check by a knight attacker
    for (auto & knight_move : knight_moves) {
        auto piece = board[knight_move.get_x()][knight_move.get_y()].get();

        if (
                piece &&
                piece->get_color() != current_player &&
                piece->get_name() == 'N'
                ) {
            return true;
        }
    }

    return false;
}

/**
 * If the piece is a pawn, ask the player to which piece they want to promote it.
 * @param x
 * @param y
 * @param player_color
 */
// I know all other user interactions are in the Game class, but promotion is easier to do here
void GameBoard::do_possible_promotion(int& x, int& y, Color player_color) {
    if (x == 7 && board[x][y]->get_name() == 'P') {
        board[x][y].reset();

        auto promotion_piece = get_promotion_piece();

        switch (promotion_piece) {
            case 'Q':
                board[x][y] = make_unique<Queen>(player_color);
                break;
            case 'R':
                board[x][y] = make_unique<Rook>(player_color);
                break;
            case 'B':
                board[x][y] = make_unique<Bishop>(player_color);
                break;
            case 'N':
                board[x][y] = make_unique<Knight>(player_color);
                break;
            default:
                break;
        }
    }
}

/**
 * Check if there are any pieces of the given color that have valid moves.
 * @param player_color
 * @return
 */
bool GameBoard::has_moves_left(Color player_color) {
    flip();
    // check if there are any possible moves for the enemy
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (board[i][j] && board[i][j]->get_color() == player_color) {
                auto pos = Position{i, j};
                auto moves = get_valid_moves_for(pos);

                if (!moves.empty()) {
                    flip();
                    return true;
                }
            }
        }
    }

    flip();
    return false;
}