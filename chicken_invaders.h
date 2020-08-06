#ifndef __CHICKEN_INVADERS_H
#define __CHICKEN_INVADERS_H

//modificare matrice joc
void set_mat_draw(int r, int c, char ch);
char get_mat_draw(int r, int c);

///
///game
extern score_st score;
extern target_st *target_ptrmat[HMAX][LMAX];

#endif
