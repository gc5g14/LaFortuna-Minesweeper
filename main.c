/* 
    main.c
    functions: - initialising os, timers, lcd
               - providing main menu, settings and instructions
               - reading switches and calling on methods accordingly

MineSweeper game clone created for the COMP2215 final task

Instructions:
1. Run the provided make file and upload hex into LaFortuna memory.
2. Select game size (7x11 selected by default) and press on a difficulty level to start.
3. Use arrows and rotary encoder to navigate.
4. Press center to reveal field.
5. Long press up or down arrow to mark as flagged / uncertain field.
6. Find all the mines!

## Features included:
* 3 map sizes
* 3 levels of difficulty for each map size
* Randomly generated game setups
* Flagging (with counter) and question marks
* Empty field expansion
* Instructions

## Features to implement in the future::
* More map sizes
* Nicer graphics for mines and flags
* Timer and high scores

## FAQ:
* Q: The flag counter went below 0, what does this mean?
  * R: Too many flags have been placed, the number of bombs on the map is the same as the initial flag counter

## LaFortuna hardware:
* avr90usb1286 MCU
* 240x320 screen with ILI9341 driver
* Rotary encoder with 4 directional buttons and a central button

## Some of the code used in this projects belongs to different authors:
* To read switches (routa.c and routa.h) Peter Danneger's code (adapted by Klaus-Peter Zauner) is used
* To drive the display (lcd folder) Steve Gunn's code (adapted by Klaus-Peter Zauner) is used

This code is released under the GPLv3 licence

*  by Gergely Csegzi - gc5g14 */

#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>
#include "buttons.h"
#include <stdlib.h>
#include <stdio.h>

#define width LCDHEIGHT


/*int blink(int);*/
int update_dial(int);
int collect_delta(int);
void draw_blocks();
void game_map_init();
void start_menu();
int check_switches(int);
void highlight_menu_item();
void generate_rand_values(int);


int position = 0;
int16_t height, status_height, rows, columns, bombs, in_menu, in_instr, menu_index;
char val[10][15] = {{0}};
int left_shift = width/2-90;
int top_shift = 30;
int menu_count = 7;
int bomb_multiplier = 2;
int big_size = 1;  // 2- for biggest, 1- for bigger map, 0 -for smaller
 

void main(void) {
    os_init();

    in_menu = 1;
    in_instr = 0;
    menu_index = menu_count;
    rows = 7;
    columns = 11;

    // display_string_xy("in main", 100, 100);
    // _delay_ms(1000);
    // display_string_xy("       ", 100, 100);
    
    os_add_task( collect_delta,   100, 1);
    os_add_task( check_switches,  100, 1);

    //timer to seed the generation of random values
    TCCR2B |= (1 << CS10);
    srand(TCNT2);

    start_menu();
    
    sei();
    for(;;){}
    
}

void start_menu(){

    display_color(BLACK, GRAY);
    clear_screen();
    
    display_string_xy("WELCOME TO MINESWEEPER!", left_shift, top_shift - 10);
    
    display_string_xy("Select difficulty: ", left_shift, top_shift + 20);

    display_string_xy("Easy", left_shift + 50, top_shift + 40);
    display_string_xy("Medium", left_shift + 50, top_shift + 60);
    display_string_xy("Hard", left_shift + 50, top_shift + 80);

    display_string_xy("Set map size: ", left_shift, top_shift + 100);
    switch(big_size){
	case 0:
        display_string_xy("4  x 6        [x]", left_shift + 50, top_shift + 120);
        display_string_xy("7  x 11       [ ]", left_shift + 50, top_shift + 140);
        display_string_xy("10 x 15       [ ]", left_shift + 50, top_shift + 160);
	break;
	case 1: 
        display_string_xy("4  x 6        [ ]", left_shift + 50, top_shift + 120);
        display_string_xy("7  x 11       [x]", left_shift + 50, top_shift + 140);
        display_string_xy("10 x 15       [ ]", left_shift + 50, top_shift + 160);
	break;
	case 2:
        display_string_xy("4  x 6        [ ]", left_shift + 50, top_shift + 120);
        display_string_xy("7  x 11       [ ]", left_shift + 50, top_shift + 140);
        display_string_xy("10 x 15       [x]", left_shift + 50, top_shift + 160);
	break;
    }

    display_string_xy("Instructions ", left_shift, top_shift + 190);
    menu_index = 0;
    highlight_menu_item();

}

void highlight_menu_item(){
     display_color(BLACK, GRAY);

    display_string_xy("Easy", left_shift + 50, top_shift + 40);
    display_string_xy("Medium", left_shift + 50, top_shift + 60);
    display_string_xy("Hard", left_shift + 50, top_shift + 80);

    switch(big_size){
    case 0:
        display_string_xy("4  x 6        [x]", left_shift + 50, top_shift + 120);
        display_string_xy("7  x 11       [ ]", left_shift + 50, top_shift + 140);
        display_string_xy("10 x 15       [ ]", left_shift + 50, top_shift + 160);
    break;
    case 1: 
        display_string_xy("4  x 6        [ ]", left_shift + 50, top_shift + 120);
        display_string_xy("7  x 11       [x]", left_shift + 50, top_shift + 140);
        display_string_xy("10 x 15       [ ]", left_shift + 50, top_shift + 160);
    break;
    case 2:
        display_string_xy("4  x 6        [ ]", left_shift + 50, top_shift + 120);
        display_string_xy("7  x 11       [ ]", left_shift + 50, top_shift + 140);
        display_string_xy("10 x 15       [x]", left_shift + 50, top_shift + 160);
    break;
    }

    display_string_xy("Instructions ", left_shift, top_shift + 190);
    display_color(BLUE, GRAY);
     switch(menu_index){
        case 0 :
            display_string_xy("Easy", left_shift + 50, top_shift + 40);
            break;
        case 1 :
            display_string_xy("Medium", left_shift + 50, top_shift + 60);
            break;
        case 2 :
            display_string_xy("Hard", left_shift + 50, top_shift + 80);
            break;
        case 3 :
            switch(big_size){
                case 0:
                    display_string_xy("4  x 6        [x]", left_shift + 50, top_shift + 120);
                break;
                case 1: 
                    display_string_xy("4  x 6        [ ]", left_shift + 50, top_shift + 120);
                break;
                case 2:
                    display_string_xy("4  x 6        [ ]", left_shift + 50, top_shift + 120);
                break;
            }
            break;
        case 4 :
            switch(big_size){
                case 0:
                    display_string_xy("7  x 11       [ ]", left_shift + 50, top_shift + 140);
                break;
                case 1: 
                    display_string_xy("7  x 11       [x]", left_shift + 50, top_shift + 140);
                break;
                case 2:
                    display_string_xy("7  x 11       [ ]", left_shift + 50, top_shift + 140);
                break;
                }

            break;
        case 5 :
             switch(big_size){
                case 0:
                    display_string_xy("10 x 15       [ ]", left_shift + 50, top_shift + 160);
                break;
                case 1: 
                    display_string_xy("10 x 15       [ ]", left_shift + 50, top_shift + 160);
                break;
                case 2:
                    display_string_xy("10 x 15       [x]", left_shift + 50, top_shift + 160);
                break;
                }
            break;
        case 6 :
            display_string_xy("Instructions ", left_shift, top_shift + 190);
            break; 
     } 
}

void game_map_init(){
    height =  LCDWIDTH;
    status_height = 30;

    clear_screen();
    init_buttons(rows, columns, width / columns, bombs);

    // make status line
    fill_rec(0,width,0,status_height, DIM_GRAY);
    fill_rec(4, width-4, 4, status_height - 4, GRAY);
    display_color(BLACK, GRAY);
    display_string_xy("Flag tokens: ", width/2-85, 13);
    char disp[20];
    sprintf(disp, "== %d ==", bombs);         
    display_string_xy(disp, width/2 + 25, 13);

    generate_rand_values(bombs);
    draw_blocks();
    // clear_switches();
}

int collect_delta(int state) {
    position += os_enc_delta();
    if (get_game_state()){
        if (position > 0){
            move_button_right();
        } else if (position < 0) {
            move_button_left();
        }
    }
    if (in_menu){
        if (position > 0){
            menu_index = (menu_index + 1) % menu_count;
            highlight_menu_item();
        } else if (position < 0) {
            menu_index = (menu_index + menu_count - 1) % menu_count;
            highlight_menu_item();
        }
    }
    position = 0;
    return state;
}

void choose_menu(){
     in_menu = 0;
     switch(menu_index){
        case 0 :
            bombs = 2 * bomb_multiplier;
            game_map_init();
            break;
        case 1 :
            bombs = 4 * bomb_multiplier;
            game_map_init();
            break;
        case 2 :
            bombs = 6 * bomb_multiplier;
            game_map_init();
            break;
        case 3 :
            bomb_multiplier = 1;
            big_size = 0;
            display_color(BLUE, GRAY);
            display_string_xy("4  x 6        [x]", left_shift + 50, top_shift + 120);
            display_color(BLACK, GRAY);
            display_string_xy("7  x 11       [ ]", left_shift + 50, top_shift + 140);
            display_string_xy("10 x 15       [ ]", left_shift + 50, top_shift + 160);
            rows = 4;
            columns = 6;
            in_menu = 1;
            break;
        case 4 :
            bomb_multiplier = 2;
            big_size = 1;
            display_color(BLACK, GRAY);
            display_string_xy("4  x 6        [ ]", left_shift + 50, top_shift + 120);
            display_string_xy("10 x 15       [ ]", left_shift + 50, top_shift + 160);
            display_color(BLUE, GRAY);
            display_string_xy("7  x 11       [x]", left_shift + 50, top_shift + 140);           
            rows = 7;
            columns = 11;
            in_menu = 1;
            break;
        case 5 :
            bomb_multiplier = 5;
            big_size = 2;
            display_color(BLACK, GRAY);
            display_string_xy("4  x 6        [ ]", left_shift + 50, top_shift + 120);
            display_string_xy("7  x 11       [ ]", left_shift + 50, top_shift + 140);
            display_color(BLUE, GRAY);
            display_string_xy("10 x 15       [x]", left_shift + 50, top_shift + 160);
            rows = 10;
            columns = 15;
            in_menu = 1;
            break;
        case 6 :
            display_color(BLACK, GRAY);
            clear_screen();
            int temp = top_shift;
            int temp2 = left_shift;
            left_shift = 5;
            top_shift = 10;
            
            small_display_string_xy("INSTRUCTIONS FOR MINESWEEPER", left_shift, top_shift);
            
            small_display_string_xy("To navigate through menu items use the up", left_shift, top_shift + 20);
            small_display_string_xy("and down buttons or the rotary controller", left_shift, top_shift + 35);

            small_display_string_xy("Inside the game: ", left_shift, top_shift + 55);
            small_display_string_xy("Use arrow buttons or the rotary controller", left_shift + 20, top_shift + 70);
            small_display_string_xy("to navigate (select) through active fields", left_shift + 20, top_shift + 85);
            small_display_string_xy("Long press up or down buttons to cycle through", left_shift + 20, top_shift + 100);
            small_display_string_xy(" flags        - marked by !", left_shift + 40, top_shift + 115);
            small_display_string_xy("'uncertains'  - marked by ?", left_shift + 40, top_shift + 130);
            small_display_string_xy("The initial flag token number is the number of mines", left_shift, top_shift + 145);
            small_display_string_xy("You win by revealing all safe fields or by", left_shift, top_shift + 160);
            small_display_string_xy("flagging all the mines (and only the mines)", left_shift, top_shift + 175);

            small_display_string_xy("Long press left < arrow to return", left_shift, top_shift + 200);
            small_display_string_xy("to the main  menu from any screen", left_shift, top_shift + 215);
           
            in_instr = 1;
            top_shift = temp;
            left_shift = temp2;
            break; 
     } 
}

void generate_rand_values(int bombs){
    int i,j,n;
    for (i = 0; i < rows; i++){
        for (j = 0; j < columns; j++){  
            val[i][j] = '0';
        }
    }
    int x,y;
    while (bombs > 0){
        x = rand() % rows;
        y = rand() % columns;

        if (val[x][y] != -1){
            --bombs;
            val[x][y] = -1;
        }
    }

    
    for (i = 0; i < rows; i++){
        for (j = 0; j < columns; j++){
            if (val[i][j] != -1){
                n = 0;
                //up - left
                if (i > 0 && j > 0 && val[i-1][j-1] == -1) ++n;
                //up
                if (i > 0 && val[i-1][j] == -1) ++n;
                //up - right
                if (i > 0 && j < columns - 1 && val[i-1][j+1] == -1) ++n;
                //down - left
                if (i < rows -1 && j > 0 && val[i+1][j-1] == -1) ++n;
                //down - right
                if (i < rows -1 && j < columns - 1 && val[i+1][j+1] == -1) ++n;
                //down
                if (i < rows-1 && val[i+1][j] == -1) ++n;
                //left
                if (j > 0 && val[i][j-1] == -1) ++n;
                //right
                if (j < columns - 1 && val[i][j+1] == -1) ++n;

                val[i][j] = (char) n;
            }
        }
    }

}

void draw_blocks(){

   
    int i,j;
    for (i = 0; i < rows; i++){
        for (j = 0; j < columns; j++){
            make_button(j * width / columns, status_height + i * (height - status_height) / rows, val[i][j]);
        }
    }
    select_button(0,0);

    
}

int check_switches(int state) {
    
   if (in_menu){
        if (get_switch_press(_BV(SWN))) {
            menu_index = (menu_index + menu_count - 1) % menu_count;
            highlight_menu_item();
        }
         
        if (get_switch_press(_BV(SWS))) {
            menu_index = (menu_index + 1) % menu_count;           
            highlight_menu_item();
        }

        if (get_switch_short(_BV(SWC))) {
            choose_menu();
        }
   } else
   if (in_instr){
        if (get_switch_long(_BV(SWW))) {
            start_menu();
            in_menu = 1;
            in_instr = 0;
            menu_index = 0;
            _delay_ms(500);
            clear_switches();
        }

   } else
   if (get_game_state()){
        if (get_switch_short(_BV(SWN))) {
            move_button_up();
        }
        if (get_switch_long(_BV(SWN))) {
            switch_button_flag();
        }
            
        if (get_switch_short(_BV(SWE))) {
            move_button_right();
        }
            
        if (get_switch_short(_BV(SWS))) {
             move_button_down();
        }

         if (get_switch_long(_BV(SWS))) {
            bck_switch_button_flag();
        }
            
        if (get_switch_short(_BV(SWW))) {
            move_button_left();
        }
       

        if (get_switch_short(_BV(SWC))) {
            reveal_button();
        }

        if (get_switch_long(_BV(SWW))) {
            in_menu = 1;
            menu_index = 0;
            finish_game();
            start_menu();
            clear_switches();

        }

   
    } else {
        if (get_switch_short(_BV(SWC))) {
            cli();
            game_map_init();
            _delay_ms(100);
            sei();
        }
       
        if (get_switch_long(_BV(SWW))) {
            start_menu();
            in_menu = 1;
            menu_index = 0;
            _delay_ms(100);
            clear_switches();
        }
    }
    return state;   
}


