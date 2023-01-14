#pragma once

#include <algorithm>

class Position {
    int x;
    int y;
public:
    Position(int x, int y) : x(x), y(y) {}
    Position() : x(-1), y(-1) {}
    Position(const Position& position) = default;
    ~Position() = default;

    [[nodiscard]] int get_x() const {
        return x;
    }

    [[nodiscard]] int get_y() const {
        return y;
    }

    [[nodiscard]] bool is_valid() const {
        return x >= 0 && x < 8 && y >= 0 && y < 8;
    }

    friend
    bool operator==(const Position& lhs, const Position& rhs) {
        return lhs.x == rhs.x &&
               lhs.y == rhs.y;
    }

    [[nodiscard]] bool exists_in(const std::vector<Position> &vector) const {
        return std::find(vector.begin(), vector.end(), *this) != vector.end();
    }
};

typedef std::vector<Position> VecPos;