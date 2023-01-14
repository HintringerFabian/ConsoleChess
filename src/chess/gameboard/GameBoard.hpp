#pragma once
class Gameboard;
#include "../pieces/ChessPiece.hpp"
#include "../color/Color.hpp"
#include "../position/LastMove.hpp"
#include <memory>
#include <string>
using std::make_unique;

enum class GameTest {
    NORMAL,
    CHECKMATE,
    STALEMATE,
    PROMOTION,
    EN_PASSANT,
    CASTLING,
    CHECK
};

class GameBoard {
    std::unique_ptr<ChessPiece> board[8][8];
    bool flipped{false};
    LastMove last_move{' ', Position(-1, -1), Position(-1, -1)};
    Position en_passant_target;
    VecPos castling_targets;

    void init_normal();
    void init_check();
    void init_checkmate();
    void init_stalemate();
    void init_promotion();
    void init_en_passant();
    void init_castling();

    static void clear_screen();

    [[nodiscard]] bool king_in_check(Color current_player, const Position& king_pos, const VecPos& diagonal_moves, const VecPos& straight_moves, const VecPos& knight_moves) const;
    void do_possible_promotion(int& x, int& y, Color player_color);
    bool has_moves_left(Color player_color);
public:
    GameBoard(GameTest option);
    GameBoard(const GameBoard& other);
    ~GameBoard() = default;

    void print() const;
    void print(const VecPos& valid_moves) const;

    [[nodiscard]] VecPos get_valid_moves_for(Position position);
    void validate_moves(const Position &position, ChessPiece *piece, VecPos &moves) const;
    [[nodiscard]] bool is_king_in_check(Color current_player) const;
    void find_king(const Color &current_player, King *&king, Position &king_pos) const;
    Position check_piece(Position position, Color color);

    static char get_promotion_piece();

    void move_piece(Position old_pos, Position new_pos, bool not_tmp);

    [[nodiscard]] bool is_game_over(Color current_player);
    bool is_stalemate();

    [[nodiscard]] bool is_flipped() const {
        return flipped;
    }
    void flip();
};