#pragma once
#include "Position.hpp"
#include "../color/Color.hpp"

// class that saves the last moved piece
// the old and new position
class LastMove {
    char piece;
    Position from;
    Position to;
public:
    LastMove(char name, Position position, Position move) : piece(name), from(position), to(move) {}
    LastMove(const LastMove& other) = default;
    ~LastMove() = default;

    [[nodiscard]] char get_name() const {
        return piece;
    }

    [[nodiscard]] Position get_from() const {
        return from;
    }

    [[nodiscard]] Position get_to() const {
        return to;
    }

    void set_from(Position position) {
        this->from = position;
    }

    void set_to(Position move) {
        this->to = move;
    }
};