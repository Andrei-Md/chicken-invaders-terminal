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

static int interior_bullet();

static int collision_bullet_tg_check();
static void collision_bullet_tg_update();

bullet_st bullet;

static int targets_left = NR_TARGETS;

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
  //TODO
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
    if (do_sleep(50))
    {
      fprintf(stderr, "bullet: Sleep-ul a esuat\n");
      perror("Cauza este");
    }
  }
}

static void game_on_bullet()
{
  bullet_fire_pos();
  bullet.on_off = kOn;

  while (1)
  {
    update_bullet();

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
      /// curat matricea
      // update_matrix_bullet(0);
      pthread_mutex_unlock(get_bullet_status_mutex());
      return;
    }
    pthread_mutex_unlock(get_bullet_status_mutex());


    if (do_sleep(50))
    {
      fprintf(stderr, "bullet: Sleep-ul a esuat\n");
      perror("Cauza este");
    }
  }
}

static void update_bullet()
{
  //update daca viit poz a bullet este in interior
  if (interior_bullet())
  {
    bullet.pos.y += BULLET_ADV_DIST; //updatez cu BULLET_ADV_DIST

    //blochez target-urile si matr de ptr la target
    pthread_mutex_lock(get_target_upd_mutex());
    // pthread_mutex_lock(get_target_ptrmat_upd_mutex());

    //collision check pt noua pozitie
    if (collision_bullet_tg_check())
    {
      collision_bullet_tg_update();
      //deblochez
      pthread_mutex_unlock(get_target_upd_mutex());
      // pthread_mutex_unlock(get_target_ptrmat_upd_mutex());

      //scad nr de tinte
      targets_left--;
      if (targets_left == 0)
      { //semnalez ca nu mai am tinte si este victorie
        pthread_mutex_lock(get_winner_mutex());
        set_winner_status(1);
        pthread_mutex_unlock(get_winner_mutex());

        //opresc jocul
        pthread_mutex_lock(get_game_on_mutex());
        set_game_on_check(0);
        pthread_mutex_unlock(get_game_on_mutex());
      }
      return;
    }
    else
    {
      //deblochez
      pthread_mutex_unlock(get_target_upd_mutex());
      // pthread_mutex_unlock(get_target_ptrmat_upd_mutex());
      update_matrix_bullet(1);
    }
  }
  else
  {
    // setez bullet-status pe 0-NO bullet
    pthread_mutex_lock(get_bullet_status_mutex());
    set_bullet_status_check(0);
    pthread_mutex_unlock(get_bullet_status_mutex());

    //curat matricea
    update_matrix_bullet(0);
  }
}

static int collision_bullet_tg_check()
{
  //vf
  if (get_target_ptrmat(bullet.pos.y, bullet.pos.x) != NULL)
  {
    return 1;
  }
  return 0;
}

static void collision_bullet_tg_update()
{
  // updatez scorul
  pthread_mutex_lock(get_score_upd_mutex());
  set_score(get_score().points + TARGETSCORE);
  pthread_mutex_unlock(get_score_upd_mutex());

  //setez target off
  get_target_ptrmat(bullet.pos.y, bullet.pos.x)->on_off = kOff;
  ///curat target din matr ptr
  target_st *tmp_tg = get_target_ptrmat(bullet.pos.y, bullet.pos.x);

  for (size_t i = 0; i < TARGET_WIDTH; i++)
  {
    set_target_ptrmat(tmp_tg->pos_y, tmp_tg->pos_x[i], -1);
  }

  ///curat matr afisare
  //bullet
  pthread_mutex_lock(get_mat_upd_mutex());
  //sterg de pe poz veche doar daca este bullet
  if(get_mat_draw(bullet.pos.y - BULLET_ADV_DIST,bullet.pos.x) == shape_bullet)
    set_mat_draw(bullet.pos.y - BULLET_ADV_DIST, bullet.pos.x, ' ');

  //target
  for (int i = 0; i < SPACESHIP_WIDTH; i++)
    set_mat_draw(tmp_tg->pos_y, tmp_tg->pos_x[i], ' ');
  pthread_mutex_unlock(get_mat_upd_mutex());

  // setez bullet-status pe 0-NO bullet
  pthread_mutex_lock(get_bullet_status_mutex());
  set_bullet_status_check(0);
  pthread_mutex_unlock(get_bullet_status_mutex());
}

static void update_matrix_bullet(int clean)
{
  pthread_mutex_lock(get_mat_upd_mutex());
  //mut bullet
  if(get_mat_draw(bullet.pos.y,bullet.pos.x) == ' ')
    set_mat_draw(bullet.pos.y, bullet.pos.x, shape_bullet);
  //sterg de pe poz veche
  if(get_mat_draw(bullet.pos.y + clean,bullet.pos.x) == shape_bullet)
    set_mat_draw(bullet.pos.y + clean, bullet.pos.x, ' ');
  pthread_mutex_unlock(get_mat_upd_mutex());
}

static int interior_bullet()
{
  if (bullet.pos.y + BULLET_ADV_DIST < H_IN_MIN)
    return 0;
  return 1;
}

//bullet fire pos -> player pos + BULLET_ADV_DIST(-1) pe y
static void bullet_fire_pos()
{
  pthread_mutex_lock(get_player_upd_mutex());
  position_st pl_pos = get_player_pos();
  bullet.pos.x = pl_pos.x;
  bullet.pos.y = pl_pos.y;
  pthread_mutex_unlock(get_player_upd_mutex());
}
