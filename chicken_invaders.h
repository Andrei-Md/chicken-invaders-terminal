#ifndef __CHICKEN_INVADERS_H
#define __CHICKEN_INVADERS_H

//afisare
void draw_game();
void draw_game_over();

///init

void init_draw();
void init_terminal(); //terminal

///start
void ci_start();

//exit
void ci_final();
void reset_terminal();

//modificare matrice joc
void set_mat_draw(int r, int c, char ch);
char get_mat_draw(int r, int c);

///
///game
extern score_st score;
extern target_st *target_ptrmat[HMAX][LMAX];



#endif
