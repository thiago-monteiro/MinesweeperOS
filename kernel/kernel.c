#include "../cpu/idt.h"
#include "../cpu/isr.h"
#include "../cpu/timer.h"
#include "../drivers/display.h"
#include "../drivers/keyboard.h"

#include "util.h"
#include "mem.h"

void* alloc(int n) {
    int *ptr = (int *) mem_alloc(n * sizeof(int));
    if (ptr == NULL_POINTER) {
        print_string("Memory not allocated.\n");
    } else {
        // Get the elements of the array
        for (int i = 0; i < n; ++i) {
//            ptr[i] = i + 1; // shorthand for *(ptr + i)
        }

        for (int i = 0; i < n; ++i) {
//            char str[256];
//            int_to_string(ptr[i], str);
//            print_string(str);
        }
//        print_nl();
    }
    return ptr;
}

#define ROWS 9
#define COLS 9
#define MINES 10

int mines[ROWS][COLS];
int revealed[ROWS][COLS];
int game_initialized = 0;
int game_over = 0;
int first_move = 1;
int first_move_x = -1;
int first_move_y = -1;

unsigned int seed = 0;

int count_adjacent_mines(int x, int y);

void srand(unsigned int new_seed) {
    seed = new_seed;
}

int rand() {
    seed = seed * 1103515245 + 12345;
    return (unsigned int)(seed / 65536) % 32768;
}

void init_mines(int avoid_x, int avoid_y) {
    int i, x, y;
    for (i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            mines[i][j] = 0;
        }
    }
    
    for (i = 0; i < MINES; i++) {
        do {
            x = rand() % ROWS;
            y = rand() % COLS;
        } while (mines[x][y] == -1 || (x == avoid_x && y == avoid_y));
        mines[x][y] = -1;
    }
    
    for (i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (mines[i][j] != -1) {
                mines[i][j] = count_adjacent_mines(i, j);
            }
        }
    }
}

int count_adjacent_mines(int x, int y) {
    int count = 0;
    int i, j;
    for (i = -1; i <= 1; i++) {
        for (j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            if (x + i >= 0 && x + i < ROWS && y + j >= 0 && y + j < COLS && mines[x + i][y + j] == -1) {
                count++;
            }
        }
    }
    return count;
}

void reveal_cell(int x, int y) {
    if (x < 0 || x >= ROWS || y < 0 || y >= COLS) return;
    if (revealed[x][y]) return;
    if (mines[x][y] == -1) return;
    
    revealed[x][y] = 1;
    
    if (mines[x][y] == 0) {
        int i, j;
        for (i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {
                if (i == 0 && j == 0) continue;
                reveal_cell(x + i, y + j);
            }
        }
    }
}

int check_win() {
    int i, j;
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            if (mines[i][j] != -1 && !revealed[i][j]) {
                return 0;
            }
        }
    }
    return 1;
}

void print_board() {
    int i, j;
    char str[32];
    clear_screen();
    print_string("Minesweeper - ");
    int_to_string(MINES, str);
    print_string(str);
    print_string(" mines\n");
    print_string("  ");
    for (i = 0; i < COLS; i++) {
        int_to_string(i, str);
        print_string(str);
        print_string(" ");
    }
    print_nl();
    for (i = 0; i < ROWS; i++) {
        int_to_string(i, str);
        print_string(str);
        print_string(" ");
        for (j = 0; j < COLS; j++) {
            if (revealed[i][j]) {
                if (mines[i][j] == -1) {
                    print_string("* ");
                } else {
                    int_to_string(mines[i][j], str);
                    print_string(str);
                    print_string(" ");
                }
            } else {
                print_string(". ");
            }
        }
        print_nl();
    }
    print_nl();
}

void init_game() {
    int i, j;
    game_over = 0;
    first_move = 1;
    first_move_x = -1;
    first_move_y = -1;
    
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            mines[i][j] = 0;
            revealed[i][j] = 0;
        }
    }
    
    game_initialized = 1;
}

int main() {
    srand(1);
    return 0;
}

void start_kernel() {
    clear_screen();
    print_string("Installing interrupt service routines (ISRs).\n");
    isr_install();

    print_string("Enabling external interrupts.\n");
    asm volatile("sti");

    print_string("Initializing keyboard (IRQ 1).\n");
    init_keyboard();

    print_string("Initializing dynamic memory.\n");
    init_dynamic_mem();

    clear_screen();

    init_game();
    print_string("Minesweeper Game\n");
    print_string("Type coordinates as 'x y' (e.g., '3 4')\n");
    print_string("Type 'RESTART' to start a new game\n");
    print_string("Type 'EXIT' to quit\n");
    print_nl();
    print_board();
    print_string("Enter x <SPACE> y and press enter: ");
}

void execute_command(char *input) {
    if (compare_string(input, "EXIT") == 0) {
        print_string("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    }
    
    if (compare_string(input, "RESTART") == 0) {
        init_game();
        print_string("New game started!\n");
        print_board();
        print_string("Enter x <SPACE> y and press enter: ");
        return;
    }
    
    if (game_over) {
        print_string("Game over! Type 'RESTART' to play again.\n");
        return;
    }
    
    int x = 0, y = 0;
    int i = 0;
    int has_x = 0, has_y = 0;
    
    while (input[i] == ' ' || input[i] == '\t') i++;
    
    while (input[i] >= '0' && input[i] <= '9') {
        x = x * 10 + (input[i] - '0');
        i++;
        has_x = 1;
    }
    
    while (input[i] == ' ' || input[i] == '\t') i++;
    
    while (input[i] >= '0' && input[i] <= '9') {
        y = y * 10 + (input[i] - '0');
        i++;
        has_y = 1;
    }
    
    if (!has_x || !has_y) {
        print_string("Invalid input! Use format: x y (e.g., 3 4)\n");
        print_board();
        print_string("Enter x <SPACE> y and press enter: ");
        return;
    }
    
    if (x < 0 || x >= ROWS || y < 0 || y >= COLS) {
        print_string("Coordinates out of range! Use 0-");
        char str[32];
        int_to_string(ROWS - 1, str);
        print_string(str);
        print_string(" for x and 0-");
        int_to_string(COLS - 1, str);
        print_string(str);
        print_string(" for y.\n");
        print_board();
        print_string("Enter x <SPACE> y and press enter: ");
        return;
    }
    
    if (revealed[x][y]) {
        print_string("Cell already revealed!\n");
        print_board();
        print_string("Enter x <SPACE> y and press enter: ");
        return;
    }
    
    if (first_move) {
        first_move_x = x;
        first_move_y = y;
        init_mines(x, y);
        first_move = 0;
    }
    
    if (mines[x][y] == -1) {
        game_over = 1;
        revealed[x][y] = 1;
        int i, j;
        for (i = 0; i < ROWS; i++) {
            for (j = 0; j < COLS; j++) {
                if (mines[i][j] == -1) {
                    revealed[i][j] = 1;
                }
            }
        }
        print_board();
        print_string("Game Over! You hit a mine!\n");
        print_string("Type 'RESTART' to play again.\n");
        return;
    }
    
    reveal_cell(x, y);
    
    if (check_win()) {
        game_over = 1;
        print_board();
        print_string("Congratulations! You won!\n");
        print_string("Type 'RESTART' to play again.\n");
        return;
    }
    
    print_board();
    print_string("Enter x <SPACE> y and press enter: ");
}
