#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <termio.h>

#include "ci_globals.h"
#include "ci_player.h"

//init
static void init();
static void init_player();
static void start_bullet_pl();

//final
static void final();

//game
static void game_player();
static void game_on_player();

static void update_player(int dir_x, int dir_y);
static void update_matrix_pl(int dir_x, int dir_y);

static int interior_player(int dir_x, int dir_y);
static int collision_player_tg(int dir_x, int dir_y);

///

player_st player;

static char shape_player[3] = {'/', '^', '\\'};

static int key;

static pthread_t thread_bullet;

void *start_player(void *thread_id)
{
  init();

  game_player();

  final();

  pthread_exit(thread_id);
}

static void init()
{
  init_player();
  start_bullet_pl();
}

static void init_player()
{
  pthread_mutex_lock(get_player_upd_mutex());
  player.pos_x[1] = ((L_IN_MAX - L_IN_MIN) / 2) + L_IN_MIN;
  player.pos_x[0] = player.pos_x[1] - 1;
  player.pos_x[2] = player.pos_x[1] + 1;
  player.pos_y = H_IN_MAX;
  player.on_off = kOff;
  pthread_mutex_unlock(get_player_upd_mutex());
}

static void start_bullet_pl()
{
  /*Initialize and set:
  - thread detached attribute (Portability)*/
  pthread_attr_t attr;
  if (pthread_attr_init(&attr))
  {
    fprintf(stderr, "main: Nu am putut initializa pthread_attr\n");
    perror("Cauza este");
  }
  if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE))
  {
    fprintf(stderr, "main: Nu am putut set detach state for thread attribute\n");
    perror("Cauza este");
  }

  //start threads
  if (pthread_create(&thread_bullet, &attr, start_bullet, (void *)TH_BULLET))
  {
    fprintf(stderr, "main: Nu s-a creat thread-ul cu nr %d\n", TH_BULLET);
    perror("Cauza este");
  }

  //destroy attribute
  if (pthread_attr_destroy(&attr))
  {
    fprintf(stderr, "main: Nu am putut distruge pthread_attr\n");
    perror("Cauza este");
  }
}

static void game_player()
{

  //TODO
  //signal wait implementation

  player.on_off = kOn;

  game_on_player();

  return;
}

static void game_on_player()
{
  //initial state
  update_player(0, 0);

  while (1)
  {
    //vf state
    pthread_mutex_lock(get_game_on_mutex());
    if (!get_game_on_check()) //daca este game_off modific starea player-ului
    {
      player.on_off = kOff;
      pthread_mutex_unlock(get_game_on_mutex());

      //opresc bullet
      pthread_mutex_lock(get_bullet_status_mutex());
      set_bullet_status_check(2);
      pthread_mutex_unlock(get_bullet_status_mutex());

      return;
    }
    pthread_mutex_unlock(get_game_on_mutex());

    //input pt player
    key = getchar();
    if (key == 27)
    {
      //ARROWS
      key = getchar();
      if (key == 91)
      {
        key = getchar();
        switch (key)
        {
        case 65: //UP arrow
          update_player(0, -1);
          break;
        case 66: //DOWN arrow
          update_player(0, 1);
          break;
        case 67: //RIGHT arrow
          update_player(1, 0);
          break;
        case 68: //LEFT arrow
          update_player(-1, 0);
          break;
        default:
          break;
        }
      }
      else
      {
        //ESC a fost apasat si opresc jocul
        //opresc bullet
        pthread_mutex_lock(get_bullet_status_mutex());
        set_bullet_status_check(2);
        pthread_mutex_unlock(get_bullet_status_mutex());

        //opresc jocul
        pthread_mutex_lock(get_game_on_mutex());
        set_game_on_check(0);
        pthread_mutex_unlock(get_game_on_mutex());
      }
    }
    //SPACE
    if (key == 32)
    {
      pthread_mutex_lock(get_bullet_status_mutex());
      if (!get_bullet_status_check())
        set_bullet_status_check(1);
      pthread_mutex_unlock(get_bullet_status_mutex());
    }
  }

  return;
}

static void update_player(int dir_x, int dir_y)
{
  //check daca noua pozitie este in interior
  if (interior_player(dir_x, dir_y))
  {
    //vf daca am coliziune cu target
    if (collision_player_tg(dir_x, dir_y))
    {
      //opresc bullet
      pthread_mutex_lock(get_bullet_status_mutex());
      set_bullet_status_check(2);
      pthread_mutex_unlock(get_bullet_status_mutex());

      //opresc jocul
      pthread_mutex_lock(get_game_on_mutex());
      set_game_on_check(0);
      pthread_mutex_unlock(get_game_on_mutex());

      return;
    }

    pthread_mutex_lock(get_player_upd_mutex());

    player.pos_x[0] = player.pos_x[0] + dir_x;
    player.pos_x[1] = player.pos_x[1] + dir_x;
    player.pos_x[2] = player.pos_x[2] + dir_x;
    player.pos_y = player.pos_y + dir_y;

    pthread_mutex_unlock(get_player_upd_mutex());

    update_matrix_pl(dir_x, dir_y);
  }
  return;
}

static int collision_player_tg(int dir_x, int dir_y)
{
  pthread_mutex_lock(get_target_upd_mutex());

  int pl_x;
  int pl_y;

  for (size_t i = 0; i < SPACESHIP_WIDTH; i++)
  {
    pl_x = player.pos_x[i] + dir_x;
    pl_y = player.pos_y + dir_y;
    if (get_target_ptrmat(pl_y, pl_x) != NULL)
    {
      pthread_mutex_unlock(get_target_upd_mutex());
      return 1;
    }
  }

  pthread_mutex_unlock(get_target_upd_mutex());
  return 0;
}

static int interior_player(int dir_x, int dir_y)
{
  if (dir_x == 0 && dir_y != 0) //depl pe y
  {
    if (player.pos_y + dir_y > H_IN_MAX || player.pos_y + dir_y < H_IN_MIN)
      return 0;
    return 1;
  }
  else if (dir_y == 0 && dir_x != 0) //depl pe x
  {
    if (player.pos_x[0] + dir_x < L_IN_MIN || player.pos_x[2] + dir_x > L_IN_MAX)
      return 0;
    return 1;
  }
  else if (dir_y == 0 && dir_x == 0) //ct
    return 1;
  else
    return 0;
}

static void update_matrix_pl(int dir_x, int dir_y)
{
  pthread_mutex_lock(get_mat_upd_mutex());
  ///update matrix
  //noua pozitie
  size_t i;
  if (dir_y == 0 && dir_x != 0) //mut pe x
  {
    //mut player-ul
    for (i = 0; i < SPACESHIP_WIDTH; i++)
      set_mat_draw(player.pos_y, player.pos_x[i], shape_player[i]);
    //sterg urma veche
    set_mat_draw(player.pos_y, player.pos_x[1] - 2 * dir_x, ' ');
  }
  else if (dir_x == 0 && dir_y != 0) //mut pe y
  {
    //mut player-ul
    for (i = 0; i < SPACESHIP_WIDTH; i++)
      set_mat_draw(player.pos_y, player.pos_x[i], shape_player[i]);
    //sterg urma veche
    for (i = 0; i < SPACESHIP_WIDTH; i++)
      set_mat_draw(player.pos_y - dir_y, player.pos_x[i], ' ');
  }
  else
  {
    //pun la poz curenta
    for (i = 0; i < SPACESHIP_WIDTH; i++)
      set_mat_draw(player.pos_y, player.pos_x[i], shape_player[i]);
  }
  pthread_mutex_unlock(get_mat_upd_mutex());
}

static void final()
{

  //wait threads to complete
  void *status;
  if (pthread_join(thread_bullet, &status))
  {
    fprintf(stderr, "player: Nu s-a facut join la thread-ul pt bullet avand un status %ld\n", (long)status);
    perror("Cauza este");
  }
  printf("player: completed joined with bullet thread having a status of %ld\n", (long)status);
}

position_st get_player_pos()
{
  position_st pos;
  pos.x = player.pos_x[(int)SPACESHIP_WIDTH / 2];
  pos.y = player.pos_y;
  return pos;
}

player_st get_player()
{
  return player;

}
