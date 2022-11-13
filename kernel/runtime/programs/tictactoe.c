#include "tictactoe.h"

#include <drivers/serial.h>
#include <input/input_event.h>
#include <memory/pmm.h>
#include <multitasking/task.h>
#include <printf.h>
#include <runtime/drawutils.h>
#include <runtime/windowmanager.h>

#define WIDTH 350
#define HEIGHT 350

typedef struct {
    int board[9];
    int turn;
    int winner;
} tictactoe_state_t;

void tictactoe_draw(window_t* window, tictactoe_state_t* state) {
    drawutils_draw_rect_filled(window->buffer, 0, 0, WIDTH, HEIGHT, RGB(255, 255, 255));

    // horizontal lines
    drawutils_draw_line(window->buffer, 0, HEIGHT / 3, WIDTH, HEIGHT / 3, RGB(0, 0, 0));
    drawutils_draw_line(window->buffer, 0, HEIGHT / 3 * 2, WIDTH, HEIGHT / 3 * 2,
                        RGB(0, 0, 0));

    // vertical lines
    drawutils_draw_line(window->buffer, WIDTH / 3, 0, WIDTH / 3, HEIGHT, RGB(0, 0, 0));
    drawutils_draw_line(window->buffer, WIDTH / 3 * 2, 0, WIDTH / 3 * 2, HEIGHT,
                        RGB(0, 0, 0));

    for (int i = 0; i < 9; i++) {
        if (state->board[i] == 1) {
            drawutils_draw_line(window->buffer, (i % 3) * WIDTH / 3, (i / 3) * HEIGHT / 3,
                                (i % 3 + 1) * WIDTH / 3, (i / 3 + 1) * HEIGHT / 3,
                                RGB(0, 0, 0));
            drawutils_draw_line(window->buffer, (i % 3 + 1) * WIDTH / 3,
                                (i / 3) * HEIGHT / 3, (i % 3) * WIDTH / 3,
                                (i / 3 + 1) * HEIGHT / 3, RGB(0, 0, 0));
        } else if (state->board[i] == 2) {
            drawutils_draw_circle(window->buffer, (i % 3 + 0.5) * WIDTH / 3,
                                  (i / 3 + 0.5) * HEIGHT / 3, WIDTH / 3 / 2,
                                  RGB(0, 0, 0));
        }
    }

    // no winner, don't draw text
    if (state->winner == 0) {
        return;
    }

    char* text;

    if (state->winner == 1) {
        text = "Player 1 wins!";
    } else if (state->winner == 2) {
        text = "Player 2 wins!";
    } else {
        text = "Draw!";
    }

    int center_y = HEIGHT / 2 - 16 / 2;
    drawutils_draw_string(window->buffer, WIDTH / 2 - strlen(text) * 16 / 2, center_y - 8,
                          text, 2, RGB(255, 0, 0));

    char* text2 = "Click to restart";
    drawutils_draw_string(window->buffer, WIDTH / 2 - strlen(text2) * 16 / 2,
                          center_y + 8, text2, 2, RGB(255, 0, 0));
}

int tictactoe_check_winner(tictactoe_state_t* state) {
    for (int i = 0; i < 3; i++) {
        if (state->board[i] == state->board[i + 3] &&
            state->board[i] == state->board[i + 6] && state->board[i] != 0) {
            return state->board[i];
        }
        if (state->board[i * 3] == state->board[i * 3 + 1] &&
            state->board[i * 3] == state->board[i * 3 + 2] && state->board[i * 3] != 0) {
            return state->board[i * 3];
        }
    }

    if (state->board[0] == state->board[4] && state->board[0] == state->board[8] &&
        state->board[0] != 0) {
        return state->board[0];
    }

    if (state->board[2] == state->board[4] && state->board[2] == state->board[6] &&
        state->board[2] != 0) {
        return state->board[2];
    }

    for (int i = 0; i < 9; i++) {
        if (state->board[i] == 0) {
            // no winner yet
            return 0;
        }
    }

    // draw
    return -1;
}

void tictactoe_on_event(window_t* window, input_event_t event) {
    tictactoe_state_t* state = (tictactoe_state_t*)window->data;

    if (event.kind == EVENT_MOUSE_BUTTON) {
        if (event.mouse_button.button == MOUSE_BUTTON_LEFT &&
            event.mouse_button.s_kind == MOUSE_BUTTON_DOWN) {

            // clicked for restart
            if (state->winner != 0) {
                // reset board
                for (int i = 0; i < 9; i++) {
                    state->board[i] = 0;
                }
                state->turn = 1;
                state->winner = 0;
                tictactoe_draw(window, state);
                return;
            }

            // normal click during game

            // get the cell
            size_t cursor_x, cursor_y;
            windowmanager_get_relative_cursor_pos(window, &cursor_x, &cursor_y);
            int cell_x = cursor_x / (WIDTH / 3);
            int cell_y = cursor_y / (HEIGHT / 3);

            // set the cell
            if (state->board[cell_x + cell_y * 3] == 0) {
                state->board[cell_x + cell_y * 3] = state->turn;
                state->turn = state->turn == 1 ? 2 : 1;
            }

            state->winner = tictactoe_check_winner(state);

            tictactoe_draw(window, state);
        }
    }
}

void tictactoe_on_close(window_t* window) {
    free(window->data);
    windowmanager_destroy_window(window);
}

void tictactoe_main() {
    window_t* window = windowmanager_create_window(WIDTH, HEIGHT, "TicTacToe");
    malloc(sizeof(tictactoe_state_t));
    tictactoe_state_t* state = malloc(sizeof(tictactoe_state_t));
    memset(state, 0, sizeof(tictactoe_state_t));
    window->data = state;

    window->on_event = &tictactoe_on_event;
    window->on_close = &tictactoe_on_close;

    // player 1 starts
    state->turn = 1;

    // intitial draw
    tictactoe_draw(window, state);
}
