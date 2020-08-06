#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>

#include "ci_globals.h"

///> static mutex

//game status
static pthread_mutex_t game_status_m = PTHREAD_MUTEX_INITIALIZER;

//update matrix
static pthread_mutex_t matrix_upd_m = PTHREAD_MUTEX_INITIALIZER;

//player update
static pthread_mutex_t player_upd_m = PTHREAD_MUTEX_INITIALIZER;

//bullet status
static pthread_mutex_t bullet_status_m = PTHREAD_MUTEX_INITIALIZER;

//update target matrix
// static pthread_mutex_t target_ptrmat_upd_m = PTHREAD_MUTEX_INITIALIZER;

//update target
static pthread_mutex_t target_upd_m = PTHREAD_MUTEX_INITIALIZER;

//update score
static pthread_mutex_t score_upd_m = PTHREAD_MUTEX_INITIALIZER;

//winner
static pthread_mutex_t winner_m = PTHREAD_MUTEX_INITIALIZER;

//bullet cond variable
pthread_cond_t bullet_cv = PTHREAD_COND_INITIALIZER;
///<

///status var
static int game_status_check = 0;
static int bullet_status_check = 0;
static int winner_status_check = 0;
//score
score_st score;

/*sleep for specified time in miliseconds*/
int do_sleep(int miliseconds)
{
  // size_t nanoseconds = miliseconds *1000000;
  struct timespec time;
  int res;
  if (miliseconds < 0)
  {
    errno = EINVAL;
    return -1;
  }
  time.tv_sec = miliseconds / 1000;
  time.tv_nsec = (miliseconds % 1000) * 1000000;
  do
  {
    res = nanosleep(&time, &time);
  } while (res && errno == EINTR);
  return res;
}

///functii mutex
pthread_mutex_t *get_game_on_mutex()
{
  return &game_status_m;
}

pthread_mutex_t *get_mat_upd_mutex()
{
  return &matrix_upd_m;
}

pthread_mutex_t *get_player_upd_mutex()
{
  return &player_upd_m;
}

pthread_mutex_t *get_bullet_status_mutex()
{
  return &bullet_status_m;
}

pthread_cond_t *get_bullet_cv()
{
  return &bullet_cv;
}


// pthread_mutex_t *get_target_ptrmat_upd_mutex()
// {
//   return &target_ptrmat_upd_m;
// }

pthread_mutex_t *get_target_upd_mutex()
{
  return &target_upd_m;
}

pthread_mutex_t *get_score_upd_mutex()
{
  return &score_upd_m;
}
pthread_mutex_t *get_winner_mutex()
{
  return &winner_m;
}

///functii status var
//game status
//1 - ON
//0 - OFF
int get_game_on_check()
{
  return game_status_check;
}

void set_game_on_check(int val)
{
  game_status_check = val;
}

//bullet status
//2 - EXIT
//1 - ON
//0 - OFF
int get_bullet_status_check()
{
  return bullet_status_check;
}

void set_bullet_status_check(int val)
{
  bullet_status_check = val;
}

score_st get_score()
{
  return score;
}

void set_score(int val)
{
  score.points = val;
}


//winner status
//0 - Lose
//1 - Win
int get_winner_status()
{
  return winner_status_check;
}
//winner status
//0 - Lose
//1 - Win
void set_winner_status(int val)
{
  winner_status_check = val;
}



