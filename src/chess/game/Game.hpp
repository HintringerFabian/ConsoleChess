#pragma once
#include "../gameboard/GameBoard.hpp"
#include "../color/Color.hpp"
#include "../position/Position.hpp"
#include <unistd.h>

class Game {
    GameBoard gameboard;
    Color current_player;
    bool player_vs_player;

    Position player_choose_piece(Color color);
    static void print_player_action(Color color, const std::string& action);
    Position player_choose_move(Color color, const VecPos& valid_moves);

    Position ai_choose_piece(Color color);
    static Position ai_choose_move(const VecPos& valid_moves);

    static std::string get_input();
    [[nodiscard]] Position get_position(std::string input);

    void print_winner(Color winner);

    void switch_player();

public:
    Game(GameTest option): gameboard(option), current_player(Color::WHITE) {
        std::cout << "Welcome to Chess!" << std::endl;
        std::cout << "Do you want to play against a friend? (y/n): ";
        std::string input;
        std::cin >> input;
        player_vs_player = (input == "y");

        srand((unsigned int)time(nullptr));
    }
    ~Game() = default;

    void play();
};
