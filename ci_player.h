#ifndef CI_PLAYER_H
#define CI_PLAYER_H

typedef struct Player_st
{
  //player ON sau OFF
  on_off_et on_off;

  //pozitia pe y
  int pos_y;

  //pozitiile pe x
  int pos_x[SPACESHIP_WIDTH];

} player_st;



//init
void *start_player(void *);


//game
void game_player();
void game_on_player();

void update_player(int dir_x, int dir_y);
void update_matrix_pl(int dir_x,int dir_y);

int interior_player(int dir_x, int dir_y);


position_st get_player_pos();

////
extern void *start_bullet(void *thread_id);

//tmp
void debug_player();
void tmp_update_player();

#endif