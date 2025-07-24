#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h> 

// --- Constants ---
#define BOARD_SIZE 3
#define HUMAN_PLAYER 'X'
#define COMPUTER_PLAYER 'O'
#define EMPTY_CELL ' '

// --- Game State Structure ---
typedef struct {
    char board[BOARD_SIZE][BOARD_SIZE];
    GtkButton* buttons[BOARD_SIZE][BOARD_SIZE]; // Pointers to the actual GTK buttons
    GtkLabel* status_label;
    bool game_over;
    char current_player; // 'X' for human, 'O' for computer
} GameState;

// --- Function Prototypes ---
void init_board(GameState* game);
bool check_win(GameState* game, char player);
bool check_draw(GameState* game);
void update_status(GameState* game, const char* message);
void handle_human_move(GameState* game, int r, int c);
gboolean handle_computer_move(GameState* game);
void on_button_clicked(GtkButton* button, gpointer user_data);
void on_reset_clicked(GtkButton* button, gpointer user_data);
void activate(GtkApplication* app, gpointer user_data);

// --- Game Logic Functions ---

// Initializes the game board and GUI elements
void init_board(GameState* game) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            game->board[i][j] = EMPTY_CELL;
            gtk_button_set_label(game->buttons[i][j], " ");
            gtk_widget_set_sensitive(GTK_WIDGET(game->buttons[i][j]), TRUE); // Enable buttons
        }
    }
    game->game_over = false;
    game->current_player = HUMAN_PLAYER; // Human starts
    update_status(game, "Your turn (X)");
}

// Checks if a player has won
bool check_win(GameState* game, char player) {
    // Check rows
    for (int i = 0; i < BOARD_SIZE; ++i) {
        if (game->board[i][0] == player && game->board[i][1] == player && game->board[i][2] == player) return true;
    }
    // Check columns
    for (int j = 0; j < BOARD_SIZE; ++j) {
        if (game->board[0][j] == player && game->board[1][j] == player && game->board[2][j] == player) return true;
    }
    // Check diagonals
    if (game->board[0][0] == player && game->board[1][1] == player && game->board[2][2] == player) return true;
    if (game->board[0][2] == player && game->board[1][1] == player && game->board[2][0] == player) return true;

    return false;
}

// Checks if the game is a draw (board full, no winner)
bool check_draw(GameState* game) {
    if (check_win(game, HUMAN_PLAYER) || check_win(game, COMPUTER_PLAYER)) {
        return false; // Not a draw if someone won
    }
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (game->board[i][j] == EMPTY_CELL) {
                return false; // Board is not full
            }
        }
    }
    return true; // Board is full and no winner
}

// Updates the text in the status label
void update_status(GameState* game, const char* message) {
    gtk_label_set_text(game->status_label, message);
}

// Disables all board buttons
void disable_all_buttons(GameState* game) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            gtk_widget_set_sensitive(GTK_WIDGET(game->buttons[i][j]), FALSE);
        }
    }
}

// Handles a human player's move
void handle_human_move(GameState* game, int r, int c) {
    if (game->game_over || game->board[r][c] != EMPTY_CELL || game->current_player != HUMAN_PLAYER) {
        return; // Invalid move or not human's turn
    }

    game->board[r][c] = HUMAN_PLAYER;
    gtk_button_set_label(game->buttons[r][c], "X");
    gtk_widget_set_sensitive(GTK_WIDGET(game->buttons[r][c]), FALSE); // Disable clicked button

    if (check_win(game, HUMAN_PLAYER)) {
        update_status(game, "Human (X) wins!");
        game->game_over = true;
        disable_all_buttons(game);
    } else if (check_draw(game)) {
        update_status(game, "It's a draw!");
        game->game_over = true;
        disable_all_buttons(game);
    } else {
        game->current_player = COMPUTER_PLAYER;
        update_status(game, "Computer's turn (O)");
        g_timeout_add(500, (GSourceFunc)handle_computer_move, game); // Add a small delay for computer move
    }
}

// Computer logic for the computer's move
gboolean handle_computer_move(GameState* game) {
    if (game->game_over || game->current_player != COMPUTER_PLAYER) {
        return G_SOURCE_REMOVE; // Not computer's turn or game over
    }

    int best_r = -1, best_c = -1;

    // 1. Check if computer can win on this turn
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (game->board[i][j] == EMPTY_CELL) {
                game->board[i][j] = COMPUTER_PLAYER;
                if (check_win(game, COMPUTER_PLAYER)) {
                    best_r = i;
                    best_c = j;
                    game->board[i][j] = EMPTY_CELL; // Undo move
                    goto make_computer_move; // Found winning move
                }
                game->board[i][j] = EMPTY_CELL; // Undo move
            }
        }
    }

    // 2. Check if human can win on their next turn and block them
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (game->board[i][j] == EMPTY_CELL) {
                game->board[i][j] = HUMAN_PLAYER; // Temporarily make human move
                if (check_win(game, HUMAN_PLAYER)) {
                    best_r = i;
                    best_c = j;
                    game->board[i][j] = EMPTY_CELL; // Undo human move
                    goto make_computer_move; // Found blocking move
                }
                game->board[i][j] = EMPTY_CELL; // Undo human move
            }
        }
    }

    // 3. Take the center if available
    if (game->board[1][1] == EMPTY_CELL) {
        best_r = 1;
        best_c = 1;
        goto make_computer_move;
    }

    // 4. Take a corner if available
    int corners[4][2] = {{0,0}, {0,2}, {2,0}, {2,2}};
    for (int i = 0; i < 4; ++i) {
        int r = corners[i][0];
        int c = corners[i][1];
        if (game->board[r][c] == EMPTY_CELL) {
            best_r = r;
            best_c = c;
            goto make_computer_move;
        }
    }

    // 5. Take any empty side if available (fallback)
    int sides[4][2] = {{0,1}, {1,0}, {1,2}, {2,1}};
    for (int i = 0; i < 4; ++i) {
        int r = sides[i][0];
        int c = sides[i][1];
        if (game->board[r][c] == EMPTY_CELL) {
            best_r = r;
            best_c = c;
            goto make_computer_move;
        }
    }
    
    // If somehow no move was found (shouldn't happen in Tic-Tac-Toe unless board is full)
    // Find the first empty cell as a last resort
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (game->board[i][j] == EMPTY_CELL) {
                best_r = i;
                best_c = j;
                goto make_computer_move;
            }
        }
    }


make_computer_move:
    if (best_r != -1 && best_c != -1) {
        game->board[best_r][best_c] = COMPUTER_PLAYER;
        gtk_button_set_label(game->buttons[best_r][best_c], "O");
        gtk_widget_set_sensitive(GTK_WIDGET(game->buttons[best_r][best_c]), FALSE); // Disable clicked button

        if (check_win(game, COMPUTER_PLAYER)) {
            update_status(game, "Computer (O) wins!");
            game->game_over = true;
            disable_all_buttons(game);
        } else if (check_draw(game)) {
            update_status(game, "It's a draw!");
            game->game_over = true;
            disable_all_buttons(game);
        } else {
            game->current_player = HUMAN_PLAYER;
            update_status(game, "Your turn (X)");
        }
    }
    // Return false for g_timeout_add to remove the source
    return G_SOURCE_REMOVE;
}

// Callback for board button clicks
void on_button_clicked(GtkButton* button, gpointer user_data) {
    GameState* game = (GameState*)user_data;

    // Find which button was clicked
    int r = -1, c = -1;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (game->buttons[i][j] == button) {
                r = i;
                c = j;
                break;
            }
        }
        if (r != -1) break;
    }

    if (r != -1) {
        handle_human_move(game, r, c);
    }
}

// Callback for the reset button
void on_reset_clicked(GtkButton* button, gpointer user_data) {
    GameState* game = (GameState*)user_data;
    init_board(game);
}

// Main activation function for the GTK application
void activate(GtkApplication* app, gpointer user_data) {
    GameState* game = (GameState*)user_data;

    GtkWidget* window;
    GtkWidget* grid;
    GtkWidget* status_label;
    GtkWidget* reset_button;
    GtkWidget* vbox;

    // Create the main window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Tic-Tac-Toe (Human vs. AI)");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 350);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    // Create a vertical box to arrange grid, status, and reset button
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create the 3x3 grid for the game board
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0);

    // Create buttons and add them to the grid
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            game->buttons[i][j] = GTK_BUTTON(gtk_button_new_with_label(" "));
            gtk_widget_set_size_request(GTK_WIDGET(game->buttons[i][j]), 80, 80);
            gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(game->buttons[i][j]), j, i, 1, 1);
            g_signal_connect(game->buttons[i][j], "clicked", G_CALLBACK(on_button_clicked), game);
        }
    }

    // Create status label
    status_label = gtk_label_new("Welcome to Tic-Tac-Toe!");
    game->status_label = GTK_LABEL(status_label);
    gtk_box_pack_start(GTK_BOX(vbox), status_label, FALSE, FALSE, 0);

    // Create reset button
    reset_button = gtk_button_new_with_label("Reset Game");
    g_signal_connect(reset_button, "clicked", G_CALLBACK(on_reset_clicked), game);
    gtk_box_pack_end(GTK_BOX(vbox), reset_button, FALSE, FALSE, 0);

    // Initialize the game state
    init_board(game);

    // Show all widgets
    gtk_widget_show_all(window);
}

// --- Main Function ---
int main(int argc, char** argv) {
    GtkApplication* app;
    int status;

    // Allocate and initialize GameState
    GameState* game = g_new(GameState, 1);
    
    // Create a new GTK application
    app = gtk_application_new("org.gtk.tictactoe", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), game);

    // Run the application
    status = g_application_run(G_APPLICATION(app), argc, argv);

    // Clean up
    g_object_unref(app);
    g_free(game); // Free the allocated GameState
    
    return status;
}