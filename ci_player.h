#ifndef CI_PLAYER_H
#define CI_PLAYER_H

//init
void *start_player(void *);

//
extern void *start_bullet(void *thread_id);

player_st get_player();

#endif