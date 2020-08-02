#ifndef __CHICKEN_INVADERS_H
#define __CHICKEN_INVADERS_H

//afisare
void draw_game();
void draw_game_over();

///init
static void init();
void init_draw();
void init_terminal(); //terminal

///start
void ci_start();

//exit
void ci_final();
void reset_terminal();

//modificare matrice joc
void set_mat_draw(int r, int c, char ch);

//tmp
void tmp_update_mat_draw();

///
///game
// extern player_st player;
extern target_st target[NRMAX_TARGETS];
// extern bullet_st bullet;
extern score_st score;

//shapes
extern char shape_bullet;
extern char shape_target[3][3];


#endif