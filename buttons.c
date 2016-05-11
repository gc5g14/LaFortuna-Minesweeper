/*
 * buttons.c
 * functions: - stores game state, which is mostly the button states in the "2d" array of buttons
 *            - deals with the logic of moving selection, revealing fields, expanding empty fields etc.
 *            - deals with flagging, checking for winning configuration and losing
 * by Gergely Csegzi - gc5g14  */

#include "buttons.h"
#include <util/delay.h>
#include <stdlib.h>

uint16_t** Make2DIntArray(int, int);
button** Make2DButtonArray(int, int);

volatile rectangle R = {0,0,0,0};
uint16_t button_col, def_button_col, game_state;    //0 - lost , 1 ongoing
uint16_t** lefts;
uint16_t** tops;
button** button_arr;
int16_t button_posx, button_posy, xcount, ycount, i, j, button_width, rows, columns, flags, maxflags;

void init_buttons(uint16_t grows, uint16_t gcolumns, uint16_t width, uint16_t bombs) {
    
    free(lefts);
    free(tops);
    free(button_arr);

    xcount = 0;
    ycount = -1;
    button_posx = 0;
    button_posy = 0;
    game_state = 1;
    button_col = GRAY;
    def_button_col = GRAY;
    button_width =width;
    rows = grows;
    columns = gcolumns;
    flags = bombs;
    maxflags = bombs;

    lefts = (uint16_t**) Make2DIntArray(rows, columns);
    tops = (uint16_t**) Make2DIntArray(rows, columns);
    button_arr = (button**) Make2DButtonArray(rows, columns);

}

void fill_rec(uint16_t l, uint16_t r, uint16_t t, uint16_t b, uint16_t col) { 
    R.left = l; 
    R.right = r; 
    R.top = t; 
    R.bottom = b; 
    fill_rectangle(R, col); 
}


void draw_shading(uint16_t x, uint16_t y){
   
    int16_t light_width = 2; 
     //light
    fill_rec(lefts[x][y],lefts[x][y]+light_width,tops[x][y],tops[x][y]+button_width,WHITE);
    fill_rec(lefts[x][y],lefts[x][y]+button_width,tops[x][y],tops[x][y]+light_width,WHITE);
    //shadow
    int16_t temp = button_width - light_width;
    fill_rec(lefts[x][y]+temp,lefts[x][y]+button_width,tops[x][y],tops[x][y]+button_width, DARK_GRAY);
    fill_rec(lefts[x][y],lefts[x][y]+button_width,tops[x][y]+temp,tops[x][y]+button_width, DARK_GRAY);
  
}

void draw_button_background(uint16_t x, uint16_t y) {

    fill_rec(lefts[x][y],lefts[x][y]+button_width-1,tops[x][y],tops[x][y]+button_width-1,button_col);
       
}

void draw_grid_lines(uint16_t x, uint16_t y){
         int16_t light_width = 1; 
     //GRID LINES
    fill_rec(lefts[x][y],lefts[x][y]+light_width,tops[x][y],tops[x][y]+button_width,BLACK);
    fill_rec(lefts[x][y],lefts[x][y]+button_width,tops[x][y],tops[x][y]+light_width,BLACK);
    int16_t temp = button_width - light_width;
    fill_rec(lefts[x][y]+temp,lefts[x][y]+button_width,tops[x][y],tops[x][y]+button_width, BLACK);
    fill_rec(lefts[x][y],lefts[x][y]+button_width,tops[x][y]+temp,tops[x][y]+button_width, BLACK);
       
}

void draw_button_content(uint16_t x, uint16_t y){
    
    char disp[20];
    if (button_arr[x][y].value == -1){
        sprintf(disp, "x");  
        display_string_xy(disp, lefts[x][y] + button_width/5*2 + 2, tops[x][y] + button_width/5*2 + 2);
    } else {
         sprintf(disp, "%d", button_arr[x][y].value);  
         display_string_xy(disp, lefts[x][y] + button_width/5*2, tops[x][y] + button_width/5*2);
    }

}

void draw_flag(uint16_t x, uint16_t y){

    if(button_arr[x][y].flag == 1){
         display_string_xy("!", lefts[x][y] + button_width/5*2 + 3, tops[x][y] + button_width/5*2);
    } else if (button_arr[x][y].flag == 2){
         display_string_xy("?", lefts[x][y] + button_width/5*2, tops[x][y] + button_width/5*2);
    }
}

/*
buttons have to be squares
white shadow left, top
dark shadow right, bottom
*/
void draw_button(uint16_t x, uint16_t y){
    draw_button_background(x, y);
    draw_shading(x, y);
}


void make_button(uint16_t l, uint16_t t, char text) {

    ++ycount;
    if (ycount == columns){
        ycount = 0;
        ++xcount;
    }

    lefts[xcount][ycount] = l;
    tops[xcount][ycount] = t;
    // texts[last] = text;
    button b = button_arr[xcount][ycount];
    b.state = 0;
    b.flag = 0;
    b.value = text;
    b.posx = xcount;
    b.posy = ycount;

    button_arr[xcount][ycount] = b;

    button_col = def_button_col;

    draw_button(xcount, ycount);
}

void select_button(uint16_t x, uint16_t y) {
    button_col = BLUE;
    button_posx = x;
    button_posy = y;
    draw_button(x, y);
    display_color(BLACK,button_col);
    draw_flag(x,y);
}

void check_neighbours(int16_t x, int16_t y){

    //up
   if ( x > 0 && button_arr[x-1][y].state == 0 
        && button_arr[x-1][y].flag == 0 && button_arr[x-1][y].value == 0){
        

        button_posx = x - 1;
        button_posy = y;
        reveal_button();
        
        button_posx = x;
        button_posy = y;
   } 

   //down
    if ( x < rows -1 && button_arr[x+1][y].state == 0 
        && button_arr[x+1][y].flag == 0 && button_arr[x+1][y].value == 0){
    
        button_posx = x + 1;
        button_posy = y;
        reveal_button();
        button_posx = x;
        button_posy = y;

   }
   //left
   if ( y > 0 && button_arr[x][y-1].state == 0 
        && button_arr[x][y-1].flag == 0 && button_arr[x][y-1].value == 0){
        
        button_posy = y - 1;
        button_posx = x;
        reveal_button();
        button_posx = x;
        button_posy = y;
   }
   //right
   if ( y < columns -1 && button_arr[x][y+1].state == 0 
        && button_arr[x][y+1].flag == 0 && button_arr[x][y+1].value == 0){

        button_posy = y + 1;                
        button_posx = x;
        reveal_button();
        button_posx = x;
        button_posy = y;
   }

   if (button_arr[x][y].value == 0){

        if ( x > 0 && button_arr[x-1][y].state == 0 
            && button_arr[x-1][y].flag == 0 && button_arr[x-1][y].value != -1){
            
            button_posx = x - 1;
            button_posy = y;
            reveal_button();
            
            button_posx = x;
            button_posy = y;
       } 

        if ( x < rows -1 && button_arr[x+1][y].state == 0 
            && button_arr[x+1][y].flag == 0 && button_arr[x+1][y].value != -1){
            
            button_posx = x + 1;
            button_posy = y;
            reveal_button();
            button_posx = x;
            button_posy = y;

       }
       //left
       if ( y > 0 && button_arr[x][y-1].state == 0 
            && button_arr[x][y-1].flag == 0 && button_arr[x][y-1].value != -1){

            button_posy = y - 1;
            button_posx = x;
            reveal_button();
            button_posx = x;
            button_posy = y;
       }
       //right
       if ( y < columns -1 && button_arr[x][y+1].state == 0 
            && button_arr[x][y+1].flag == 0 && button_arr[x][y+1].value != -1){
            
           button_posy = y + 1;                
            button_posx = x;
            reveal_button();
            button_posx = x;
            button_posy = y;
       }
       //up-left
        if ( x > 0 && y > 0 && button_arr[x-1][y-1].state == 0 
            && button_arr[x-1][y-1].flag == 0 && button_arr[x-1][y-1].value != -1){
            
            button_posx = x - 1;
            button_posy = y - 1;
            reveal_button();
            
            button_posx = x;
            button_posy = y;
       } 
       //down-right
        if ( x < rows -1 && y < columns - 1 && button_arr[x+1][y+1].state == 0 
            && button_arr[x+1][y+1].flag == 0 && button_arr[x+1][y+1].value != -1){
            
            button_posx = x + 1;
            button_posy = y + 1;
            reveal_button();
            button_posx = x;
            button_posy = y;

       }
       //down-left
       if ( x < rows - 1 && y > 0 && button_arr[x+1][y-1].state == 0 
            && button_arr[x+1][y-1].flag == 0 && button_arr[x+1][y-1].value != -1){

            button_posy = y - 1;
            button_posx = x + 1;
            reveal_button();
            button_posx = x;
            button_posy = y;
       }
       //up - right
       if ( x > 0 && y < columns -1 && button_arr[x-1][y+1].state == 0 
            && button_arr[x-1][y+1].flag == 0 && button_arr[x-1][y+1].value != -1){
            
            button_posy = y + 1;                
            button_posx = x - 1;
            reveal_button();
            button_posx = x;
            button_posy = y;
       }
   }
}

void reveal_button(){

     button b;
     int e = get_selected_button(&b);
     if (e != 0){
        char disp[20];

        uint16_t x = b.posx;
        uint16_t y = b.posy;
        button_arr[x][y].state = 1;
        if (button_arr[x][y].flag == 1){
            if (flags < maxflags) {
                button_arr[x][y].flag = 0;
                ++flags;
                char disp[20];
                display_color(BLACK, def_button_col);
                sprintf(disp, "== %d ==", flags);         
                display_string_xy(disp, LCDHEIGHT/2 + 25, 13);
            }
        }

        if (b.value == -1){
            cli();
            button_col = RED;
            draw_button_background(x, y);
            display_color(BLACK,button_col);
            draw_button_content(x, y);
            draw_grid_lines(x,y);

            _delay_ms(1000);
            clear_screen();
            display_string_xy("GAME OVER :(\n", 110, 100);
            display_string_xy("Press center to restart", 60, 130);
            display_string_xy("Hold < to return to menu", 56, 150);
            finish_game();
            sei();
            return;
        } else {
            button_col = DIM_GRAY;
            draw_button_background(x, y);
            char n = b.value;
            if (n > 0){
                switch(n) {
                    case 1 :
                        display_color(BLUE,button_col);
                    break;
                    case 2 :
                        display_color(DARK_GREEN,button_col);
                    break;
                    case 3 :
                        display_color(RED, button_col);
                    break;
                    case 4:
                        display_color(PURPLE, button_col);
                    break;
                    default :
                        display_color(PURPLE,button_col);  
                }
                draw_button_content(x, y);
            }
            draw_grid_lines(x,y);
            check_neighbours(x,y);
            check_win();
        }
        
     }
     
}

void switch_button_flag(){
    button b;
     int e = get_selected_button(&b);
     if (e != 0){
        uint16_t x = b.posx;
        uint16_t y = b.posy;
        
        if (!button_arr[x][y].state) {
            int before = button_arr[x][y].flag;
            button_arr[x][y].flag = (button_arr[x][y].flag + 1) % 3;
            button_col = BLUE;
            draw_button_background(x,y);
            display_color(BLACK,button_col);  
            draw_flag(x,y);
            
            if (button_arr[x][y].flag == 1){
                
                    --flags;
                    char disp[20];
                    display_color(BLACK, def_button_col);
                    sprintf(disp, "== %d ==", flags);         
                    display_string_xy(disp, LCDHEIGHT/2 + 25, 13);
                
            } else {
                if (flags < maxflags && before == 1) {
                    ++flags;
                    char disp[20];
                    display_color(BLACK, def_button_col);
                    sprintf(disp, "== %d ==", flags);         
                    display_string_xy(disp, LCDHEIGHT/2 + 25, 13);
                }
                
            }
            check_win();

        }
     }
}

void bck_switch_button_flag(){
    button b;
     int e = get_selected_button(&b);
     if (e != 0){
        uint16_t x = b.posx;
        uint16_t y = b.posy;
        
        if (!button_arr[x][y].state) {
        int before = button_arr[x][y].flag;
        button_arr[x][y].flag = (button_arr[x][y].flag + 2) % 3;
        button_col = BLUE;
        draw_button_background(x,y);
        display_color(BLACK,button_col);  
        draw_flag(x,y);

        if (button_arr[x][y].flag == 1){
            if (flags > 0) {
                --flags;
                char disp[20];
                display_color(BLACK, def_button_col);
                sprintf(disp, "== %d ==", flags);         
                display_string_xy(disp, LCDHEIGHT/2 + 25, 13);
            }
        } else {
            if (flags < maxflags && before == 1) {
                ++flags;
                char disp[20];
                display_color(BLACK, def_button_col);
                sprintf(disp, "== %d ==", flags);         
                display_string_xy(disp, LCDHEIGHT/2 + 25, 13);
            }
        }
        check_win();

     }
 }
}

void check_win(){

        if (flags == 0){
            for(i = 0; i < rows; i++){
                for (j = 0; j < columns; j++){
                    //all flags have bombs
                    if (button_arr[i][j].flag == 1 && button_arr[i][j].value != -1) return;
                    //all bombs have flags
                    if (button_arr[i][j].flag != 1 && button_arr[i][j].value == -1) return;
                }
            }
        } else {
            for(i = 0; i < rows; i++){
            for (j = 0; j < columns; j++){
                //all the hidden ones are bombs
                if (button_arr[i][j].state == 0 && button_arr[i][j].value != -1) return;
            }
        }
        }
      
        cli();
        _delay_ms(500);
        display_color(BLACK, GREEN);
        clear_screen();
        display_string_xy("CONGRATULATIONS! YOU WON! :D\n", 40, 100);
        display_string_xy("Press center to restart", 60, 130);
        display_string_xy("Hold < to return to menu", 56, 150);
        finish_game();
        sei();
}


void deselect_button(int16_t x, int16_t y) {
   
    if (x >= 0 && y >= 0 && button_arr[x][y].state == 0){
        button_col = def_button_col;
        button_posx = x;
        button_posy = y;
        draw_button(x, y);
        display_color(BLACK, def_button_col);
        draw_flag(x,y);
    }
}

void deselect_all() {
    button_posx = -1;
    button_posy = 0;
    for(i=0; i<= xcount; ++i) {
        for (j = 0; j <= ycount; j++){
            deselect_button(i,j);  
        }     
    }
}

int get_selected_button(button *b) {
    
    if (button_posx >= 0 && button_posy >= 0) {
        *b = button_arr[button_posx][button_posy];

        return 1;
    } 

    return 0;
}

void move_button_up() {
    uint16_t x, y;
    deselect_button(button_posx, button_posy);
    int all_revealed = 1;
   
    i = 0;
    while (i < rows && all_revealed){
        if (button_arr[i][button_posy].state == 0) all_revealed = 0;
        i++;
    }

    if (all_revealed){
        if (button_posy > 0){
            x = button_posx;
            y = button_posy - 1;

            if (button_arr[x][y].state == 0){
                select_button(x,y);
            } else {
                button_posx = x;
                button_posy = y;
                move_button_up();
            }
        } else {
            x = rows - 1;
            y = columns- 1;

            if (button_arr[x][y].state == 0){
                select_button(x,y);
            } else {
                button_posx = x;
                button_posy = y;
                move_button_up();
            }
        }
    }else 
    if(button_posx > 0) {
        x = button_posx-1;
        y =  button_posy;

        if (button_arr[x][y].state == 0){
            select_button(x,y);
        } else {
            button_posx = x;
            button_posy = y;
            move_button_up();
        }
    } else {
        x = rows - 1;
        y = button_posy;

        if (button_arr[x][y].state == 0){
            select_button(x,y);
        } else {
            button_posx = x;
            button_posy = y;
            move_button_up();
        }
    }
}

void move_button_down(){
    uint16_t x, y;
    deselect_button(button_posx, button_posy);
    int all_revealed = 1;
   
    i = 0;
    while (i < rows && all_revealed){
        if (button_arr[i][button_posy].state == 0) all_revealed = 0;
        i++;
    }

    if (all_revealed){
        if (button_posy < columns - 1){
            x = button_posx;
            y = button_posy + 1;

            if (button_arr[x][y].state == 0){
                select_button(x,y);
            } else {
                button_posx = x;
                button_posy = y;
                move_button_down();
            }
        } else {
            x = 0;
            y = 0;

            if (button_arr[x][y].state == 0){
                select_button(x,y);
            } else {
                button_posx = x;
                button_posy = y;
                move_button_down();
            }
        }
    }else 
    if (button_posx < rows - 1){
        x = button_posx+1;
        y = button_posy;

        if (button_arr[x][y].state == 0){
            select_button(x,y);
        } else {
            button_posx = x;
            button_posy = y;
            move_button_down();
        }
    } else {
        x = 0;
        y = button_posy;

        if (button_arr[x][y].state == 0){
            select_button(x,y);
        } else {
            button_posx = x;
            button_posy = y;
            move_button_down();
        }
    }
}

void move_button_left(){
    uint16_t x, y;
    deselect_button(button_posx, button_posy);

    if (button_posy > 0){
        x = button_posx;
        y = button_posy - 1;
        if (button_arr[x][y].state == 0){
            select_button(x,y);
        } else {
            button_posx = x;
            button_posy = y;
            move_button_left();
        }
    } else if (button_posx > 0){
        x = button_posx - 1;
        y = columns - 1;
        
        if (button_arr[x][y].state == 0){
            select_button(x,y);
        } else {
            button_posx = x;
            button_posy = y;
            move_button_left();
        }    
    } else {
        x = rows - 1;
        y = columns - 1;
        
        if (button_arr[x][y].state == 0){
            select_button(x,y);
        } else {
            button_posx = x;
            button_posy = y;
            move_button_left();
        }    
    }
}

void move_button_right() {
    uint16_t x, y;
    deselect_button(button_posx, button_posy);

    if(button_posy < columns - 1) {
        x = button_posx;
        y = button_posy + 1;
        if (button_arr[x][y].state == 0){
            select_button(x,y);
        } else {
            button_posx = x;
            button_posy = y;
            move_button_right();
        }    
    } else if (button_posx < rows - 1){
        x = button_posx + 1;
        y = 0;
        if (button_arr[x][y].state == 0){
            select_button(x,y);
        } else {
            button_posx = x;
            button_posy = y;
            move_button_right();
        }    
    } else {
        x = 0;
        y = 0;
        if (button_arr[x][y].state == 0){
            select_button(x,y);
        } else {
            button_posx = x;
            button_posy = y;
            move_button_right();
        }    
    }
}

void reset_selections() {
    deselect_all();
    button_posx = 0;
    button_posy = 0;
}

uint16_t** Make2DIntArray(int arraySizeX, int arraySizeY) {
    uint16_t** theArray;
    theArray = (uint16_t**) malloc(arraySizeX*sizeof(uint16_t*));
    for (i = 0; i < arraySizeX; i++)
       theArray[i] = (uint16_t*) malloc(arraySizeY*sizeof(uint16_t));
    return theArray;
} 

button** Make2DButtonArray(int arraySizeX, int arraySizeY) {
    button** theArray;
    theArray = (button**) malloc(arraySizeX*sizeof(button*));
    for (i = 0; i < arraySizeX; i++)
       theArray[i] = (button*) malloc(arraySizeY*sizeof(button));
    return theArray;
} 

void free2DIntArray(uint16_t** to_free){
    for (i = 0; i < rows; i++){
        free(to_free[i]);
    }
    free(to_free);
}
void free2DButtonArray(button** to_free){
    for (i = 0; i < rows; i++){
        free(to_free[i]);
    }
    free(to_free);
}

uint16_t get_game_state(){
    return game_state;
}

void finish_game(){
    game_state = 0;
    for (i = 0; i < rows; i++){
        free(tops[i]);
        free(lefts[i]);
        free(button_arr[i]);
    }
    // free2DIntArray(lefts);
    // free2DIntArray(tops);
    // free2DButtonArray(button_arr);
}

