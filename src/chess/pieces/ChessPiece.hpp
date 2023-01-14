#pragma once
class ChessPiece;
#include <utility>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include "../color/Color.hpp"
#include "../position/Position.hpp"
#include "../position/LastMove.hpp"

class ChessPiece {
    Color color;
    std::string symbol;

protected:
    static bool delta_changed(int& x, int& y) ;
    static bool is_on_board(int& x, int& y) ;
    bool is_empty_or_enemy(const std::unique_ptr<ChessPiece> (&board)[8][8], int& x, int& y) const;
    bool is_enemy(const std::unique_ptr<ChessPiece> (&board)[8][8], int& x, int& y) const;
    static bool is_empty(const std::unique_ptr<ChessPiece> (&board)[8][8], int& x, int& y);

public:
    ChessPiece(Color c) : color(c) {}
    virtual ~ChessPiece() = default;

    [[nodiscard]] Color get_color() const {
        return color;
    }

    void set_symbol(std::string s) {
        this->symbol = std::move(s);
    }

    [[nodiscard]] std::string get_symbol() const {
        return symbol;
    }

    virtual VecPos get_moves_for(Position position, const std::unique_ptr<ChessPiece> (&board)[8][8]) = 0;
    virtual char get_name() = 0;
};

class King : public ChessPiece {
    bool first_move{true};
    static bool is_valid_delta(int& x, int& y);
public:
    King(Color color);
    ~King() = default;

    char get_name() override {
        return 'K';
    }

    VecPos get_moves_for(Position position, const std::unique_ptr<ChessPiece> (&board)[8][8]) override;
    [[nodiscard]] VecPos get_diagonal_attackers(const std::unique_ptr<ChessPiece> (&board)[8][8], Position position) const;
    [[nodiscard]] VecPos get_straight_attackers(const std::unique_ptr<ChessPiece> (&board)[8][8], Position position) const;
    [[nodiscard]] VecPos get_horse_attackers(const std::unique_ptr<ChessPiece> (&board)[8][8], Position position) const;

    static VecPos get_castling(Position position, std::unique_ptr<ChessPiece> pPtr[8][8], bool left, Color color, bool flipped);

    void set_moved(bool b) {
        first_move = !b;
    }

    [[nodiscard]] bool has_moved() const {
        return !first_move;
    }
};

class Queen : public ChessPiece {
public:
    Queen(Color color);
    ~Queen() = default;

    char get_name() override {
        return 'Q';
    }

    VecPos get_moves_for(Position position, const std::unique_ptr<ChessPiece> (&board)[8][8]) override;
};

class Bishop : public ChessPiece {
public:
    Bishop(Color color);
    ~Bishop() = default;

    char get_name() override {
        return 'B';
    }

    VecPos get_moves_for(Position position, const std::unique_ptr<ChessPiece> (&board)[8][8]) override;
};

class Knight : public ChessPiece {
    static bool is_valid_delta(int& x, int& y);
public:
    Knight(Color color);
    ~Knight() = default;

    char get_name() override {
        return 'N';
    }

    VecPos get_moves_for(Position position, const std::unique_ptr<ChessPiece> (&board)[8][8]) override;
};

class Rook : public ChessPiece {
    bool first_move{true};
    static bool is_valid_delta(int& x, int& y);
public:
    Rook(Color color);
    ~Rook() = default;

    char get_name() override {
        return 'R';
    }

    VecPos get_moves_for(Position position, const std::unique_ptr<ChessPiece> (&board)[8][8]) override;
    [[nodiscard]] bool has_moved() const {
        return !first_move;
    }
    void set_moved(bool b) {
        first_move = !b;
    }
};

class Pawn : public ChessPiece {
    bool first_move = true;
public:
    Pawn(Color color);
    ~Pawn() = default;

    char get_name() override {
        return 'P';
    }

    void set_moved(bool b) {
        first_move = !b;
    }

    VecPos get_moves_for(Position position, const std::unique_ptr<ChessPiece> (&board)[8][8]) override;
    static Position get_en_passant(Position position, const std::unique_ptr<ChessPiece> (&board)[8][8], const LastMove& last_move);
};