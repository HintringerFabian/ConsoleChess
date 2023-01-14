#include "Game.hpp"

// *****************************************************
// Public Methods
// *****************************************************

/**
 * Starts the game and runs it until it is over.
 */
void Game::play() {
   VecPos valid_moves;

    // game loop
    while (valid_moves.empty()) {
        // print board
        gameboard.print();

        // get player input if it is a game of 2 players
        auto piece = (player_vs_player) ?
                player_choose_piece(current_player) : ai_choose_piece(current_player);

        // the gameboard function has to return at least one valid move
        // for the chosen piece, else the player has to choose another piece
        valid_moves = gameboard.get_valid_moves_for(piece);

        if (!valid_moves.empty()) {
            gameboard.print(valid_moves);
            // it has already been checked, that the moves
            // here are valid, so let the player or ai choose one move
            // and move the piece
            auto move = (player_vs_player) ?
                    player_choose_move(current_player, valid_moves) : ai_choose_move(valid_moves);

            // call the function
            gameboard.move_piece(piece, move, true);

            // piece has been moved, so clear the vector
            valid_moves.clear();

            // check if the current player has won
            if (gameboard.is_game_over(current_player)) {
                print_winner(current_player);
                return;
            }

            if (gameboard.is_stalemate()) {
                print_winner(Color::DRAW);
                return;
            }

            // game is not over, so switch the player
            // and flip the gameboard
            switch_player();
        }

        if (!player_vs_player) sleep(1);
    }
}

// *****************************************************
// Private Methods
// *****************************************************

/**
 * Asks the player to choose a piece. And checks if the input is valid.
 * If the input is not valid, the player has to choose another piece.
 * @param color The player of the player.
 * @return The from of the chosen piece.
 */
Position Game::player_choose_piece(Color color) {
    print_player_action(color, "choose a piece");

    auto input = get_input();
    auto position = get_position(input);
    auto result = gameboard.check_piece(position, color);

    while (!result.is_valid()) {
        std::cout << "Invalid input! Try again: ";
        input = get_input();
        position = get_position(input);
        result = gameboard.check_piece(position, color);
    }

    return result;
}

/**
 * Prints the action of the player.
 * @param color The player of the player.
 * @param action The action of the player.
 */
void Game::print_player_action(Color color, const std::string& action) {
    auto player_color =
            (color == Color::WHITE) ? "White" : "Black";

    std::cout << "Player " << player_color << " " << action << std::endl;
}

/**
 * Asks the player to choose a move. And checks if the input is valid.
 * If the input is not valid, the player has to choose another move.
 * The user input has to be a valid move for the chosen piece.
 * @param color The player of the player.
 * @param valid_moves The vector of valid moves.
 * @return The result of the chosen move.
 */
Position Game::player_choose_move(Color color, const VecPos& valid_moves) {
    print_player_action(color, "choose a move");

    auto input = get_input();
    auto result = get_position(input);

    while (
            !result.is_valid() ||
            !result.exists_in(valid_moves)
    ) {
        std::cout << "Invalid input! Try again: ";
        input = get_input();
        result = get_position(input);
    }

    return result;
}

/**
 * Asks the ai to choose a piece. And checks if the input is valid.
 * If the input is not valid, the ai has to choose another piece.
 * @param color The player of the ai.
 * @return The from of the chosen piece.
 */
Position Game::ai_choose_piece(Color color) {
    auto position = Position(-1, -1);

    while (!position.is_valid()) {
        // get two random numbers between 0 and 7
        auto x = rand() % 8;
        auto y = rand() % 8;

        // convert the numbers to a position
        position = Position(x, y);
        position = gameboard.check_piece(position, color);
    }

    return position;
}

/**
 * Asks the ai to choose a one of the valid moves.
 * @param valid_moves The vector of valid moves.
 * @return The from of the chosen to.
 */
Position Game::ai_choose_move(const VecPos &valid_moves) {
    Position result;
    auto vector_size = valid_moves.size();

    // get a random number between 0 and the size of the vector
    auto random_number = (unsigned long)rand() % vector_size;
    result = valid_moves[random_number];

    return result;
}

/**
 * Gets the user input.
 * @return The user input.
 */
std::string Game::get_input() {
    std::string input;
    std::cin >> input;

    if (input.size() != 2) {
        return "";
    }

    const std::string characters = "ABCDEFGH";
    const std::string numbers = "12345678";

    if (characters.find(input[0]) == std::string::npos) {
        return "";
    }

    if (numbers.find(input[1]) == std::string::npos) {
        return "";
    }

    return input;
}

/**
 * Gets the from from the user input.
 * Therefore the string is converted to integers and
 * the representative from on the gameboard is returned.
 * @param input The user input.
 * @return The from.
 */
Position Game::get_position(std::string field) {
    int row, column;

    if (gameboard.is_flipped()) {
        row = 7 - (field[1] - '1');
        column = 7 - (field[0] - 'A');
    } else {
        row = field[1] - '1';
        column = field[0] - 'A';
    }

    //int row = field[1] - '1';
    //int column = field[0] - 'A';

    return {row, column};
}

/**
 * Prints the winner of the game.
 * @param color The player of the winner.
 */
void Game::print_winner(Color winner) {
    gameboard.print();
    switch (winner) {
        case Color::WHITE:
            std::cout << "White wins!" << std::endl;
            break;
        case Color::BLACK:
            std::cout << "Black wins!" << std::endl;
            break;
        case Color::DRAW:
        case Color::NONE:
        default:
            std::cout << "Draw!" << std::endl;
            break;
    }
}

/**
 * Switches the current player.
 * And flips the gameboard.
 * So the player can see the board from the other side.
 */
void Game::switch_player() {
    current_player = (current_player == Color::WHITE) ? Color::BLACK : Color::WHITE;
    gameboard.flip();
}
