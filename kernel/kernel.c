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

unsigned int seed = 0;

void srand(unsigned int new_seed) {
    seed = new_seed;
}

int rand() {
    seed = seed * 1103515245 + 12345;
    return (unsigned int)(seed / 65536) % 32768;
}

void init_mines() {
    int i, x, y;
    for (i = 0; i < MINES; i++) {
        do {
            x = rand() % ROWS;
            y = rand() % COLS;
        } while (mines[x][y] == -1);
        mines[x][y] = -1;
    }
}

int count_adjacent_mines(int x, int y) {
    int count = 0;
    int i, j;
    for (i = -1; i <= 1; i++) {
        for (j = -1; j <= 1; j++) {
            if (x + i >= 0 && x + i < ROWS && y + j >= 0 && y + j < COLS && mines[x + i][y + j] == -1) {
                count++;
            }
        }
    }
    return count;
}

void print_board() {
    int i, j;
    print_string("  ");
    for (i = 0; i < COLS; i++) {
        print_string(i);
    }
    print_nl();
    for (i = 0; i < ROWS; i++) {
        print_string(i);
        for (j = 0; j < COLS; j++) {
            if (revealed[i][j]) {
                if (mines[i][j] == -1) {
                    print_string("* ");
                } else {
                    print_string(mines[i][j]);
                }
            } else {
                print_string(". ");
            }
        }
        print_nl();
    }
}

int main() {
    srand(1);

    int i, j;
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            mines[i][j] = 0;
            revealed[i][j] = 0;
        }
    }
    init_mines();

    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            if (mines[i][j] != -1) {
                mines[i][j] = count_adjacent_mines(i, j);
            }
        }
    }

    int x, y;

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

    /*print_string("init_dynamic_mem()\n");
    print_dynamic_node_size();
    print_dynamic_mem();
    print_nl();

    int *ptr1 = alloc(5);
    print_string("int *ptr1 = alloc(5)\n");
    print_dynamic_mem();
    print_nl();

    int *ptr2 = alloc(10);
    print_string("int *ptr2 = alloc(10)\n");
    print_dynamic_mem();
    print_nl();

    mem_free(ptr1);
    print_string("mem_free(ptr1)\n");
    print_dynamic_mem();
    print_nl();

    int *ptr3 = alloc(2);
    print_string("int *ptr3 = alloc(2)\n");
    print_dynamic_mem();
    print_nl();

    mem_free(ptr2);
    print_string("mem_free(ptr2)\n");
    print_dynamic_mem();
    print_nl();

    mem_free(ptr3);
    print_string("mem_free(ptr3)\n");
    print_dynamic_mem();
    print_nl();*/

    print_string("Press enter to start game: ");
}

void execute_command(char *input) {
    if (compare_string(input, "EXIT") == 0) {
        print_string("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    }
    
    print_board();
    print_string("Enter x <SPACE> y and press enter: ");
    int x = input[0];
    int y = input[2];
    if (mines[x][y] == -1) {
        print_string("Game over!\n");
    } else {
        revealed[x][y] = 1;
    }
    
}
