#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "ci_globals.h"
#include "ci_bullet.h"

static void init_bullet();

static void game_bullet();
static void game_on_bullet();

//fct
static void bullet_fire_pos();
static void update_bullet();
static void update_matrix_bullet(int clean);

static int margine_bullet();
static int interior_bullet();


bullet_st bullet;

char shape_bullet = '!';

void *start_bullet(void *thread_id)
{
  init_bullet();

  game_bullet();

  pthread_exit(thread_id);
}

static void init_bullet()
{
  //
  bullet_fire_pos();
  bullet.on_off = kOff;
}

static void game_bullet()
{
  //TO DO
  //signal wait implementation

  while (1)
  {
    pthread_mutex_lock(get_bullet_status_mutex());
    switch (get_bullet_status_check())
    {
    case 0:
      //nothing (no bullet)
      pthread_mutex_unlock(get_bullet_status_mutex());
      break;
    case 1:
      //bullet fired
      pthread_mutex_unlock(get_bullet_status_mutex());
      game_on_bullet();
      break;
    case 2:
      //exit
      pthread_mutex_unlock(get_bullet_status_mutex());
      return;
    default:
      pthread_mutex_unlock(get_bullet_status_mutex());
      break;
    }
    do_sleep(50);
  }
}

static void game_on_bullet()
{
  bullet_fire_pos();
  bullet.on_off = kOn;

  //daca este direct in margine
  if (margine_bullet())
  {
    //nu trag nici un bullet si setez pe 0-NO bullet
    pthread_mutex_lock(get_bullet_status_mutex());
    set_bullet_status_check(0);
    pthread_mutex_unlock(get_bullet_status_mutex());
    bullet.on_off = kOff;
    return;
  }
  //TO DO
  //collision check

  //altfel
  update_matrix_bullet(0);

  do_sleep(50);
  while (1)
  {
    pthread_mutex_lock(get_bullet_status_mutex());
    if (get_bullet_status_check() == 2)
    {
      bullet.on_off = kOff;
      //curat matricea
      update_matrix_bullet(0);
      pthread_mutex_unlock(get_bullet_status_mutex());
      break;
    }
    else if (get_bullet_status_check() == 0)
    {
      bullet.on_off = kOff;
      //curat matricea
      update_matrix_bullet(0);
      pthread_mutex_unlock(get_bullet_status_mutex());
      return;
    }
    pthread_mutex_unlock(get_bullet_status_mutex());
    update_bullet();
    do_sleep(50);
  }
}

static void update_bullet()
{
  //update daca viit poz a bullet este in interior
  if (interior_bullet())
  {
    bullet.pos.y -= 1; //updatez cu 1
    update_matrix_bullet(1);
  }
  else
  {
    pthread_mutex_lock(get_bullet_status_mutex());
    set_bullet_status_check(0);
    pthread_mutex_unlock(get_bullet_status_mutex());

    //curat matricea
    update_matrix_bullet(0);
  }
}

static void update_matrix_bullet(int clean)
{
  pthread_mutex_lock(get_mat_upd_mutex());
  //mut bullet
  set_mat_draw(bullet.pos.y, bullet.pos.x, shape_bullet);
  //sterg de pe poz veche
  set_mat_draw(bullet.pos.y + clean, bullet.pos.x, ' ');
  pthread_mutex_unlock(get_mat_upd_mutex());
}

static int margine_bullet()
{
  if (bullet.pos.y == H_IN_MIN - 1)
    return 1;
  return 0;
}

static int interior_bullet()
{
  if (bullet.pos.y - 1 < H_IN_MIN)
    return 0;
  return 1;
}

//bullet fire pos - player pos -1 y
static void bullet_fire_pos()
{
  position_st pl_pos = get_player_pos();
  bullet.pos.x = pl_pos.x;
  bullet.pos.y = pl_pos.y - 1;
}
