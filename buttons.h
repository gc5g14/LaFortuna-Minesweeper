/*
 * buttons.h
 * functions: - header file for buttons.c
 			  - provides structure of buttons
 *            
 * by Gergely Csegzi - gc5g14  */
#include "os.h"

typedef struct button{
    uint8_t state;  // 0 - hidden, 1 - revealed
    uint8_t flag; // 0 - not flagged, 1 - flagged, 2 - questioned
    char *value; // 0 - 8 nr of bombs, * - bomb
    uint16_t posx;
    uint16_t posy;
} button;

void init_buttons(uint16_t rows, uint16_t columns, uint16_t width, uint16_t bombs);
void fill_rec(uint16_t l, uint16_t r, uint16_t t, uint16_t b, uint16_t col);
void make_button(uint16_t l, uint16_t t, char text);
void select_button(uint16_t x, uint16_t y);
void reveal_button();
void switch_button_flag();
void bck_switch_button_flag();
void move_button_left();
void move_button_right();
void move_button_up();
void move_button_down();
void reset_selections();
int get_selected_button(button *b);
uint16_t** Make2DIntArray(int, int);
uint16_t get_game_state();
void finish_game();

