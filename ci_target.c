#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "ci_globals.h"
#include "ci_target.h"

target_st target[NR_TARGETS];
static target_st *target_ptrmat[HMAX][LMAX];

//init
static void init();
static void init_target();

//game
static void game_target();
static void game_on_target();

///insert target
static void insert_target(size_t, position_st);
static void update_insert_target(size_t tg_nr, position_st in_pos);

//move target
static void move_target(size_t tg_nr);
static void update_target(size_t tg_nr);
static void update_target_changelane(size_t tg_nr);
static void collision_target_pl(size_t tg_nr);

//matrix update
static void update_matrix_tg();

//target ptr matrix
static void update_target_ptrmat(size_t tg_nr);

//aux
static int interior_target(int pos_x, int pos_y);

//new state
static void new_state_tg();

//insert positions
position_st insert_pos[] = {{1, 1}};
int insert_dir = 1; //insert from left to right

int check;

char shape_target[3][3] = {{'\\', 'O', '/'},
                           {'-', 'O', '-'},
                           {'/', 'O', '\\'}};

void *start_targets(void *thread_id)
{
  init();

  game_target();

  pthread_exit(thread_id);
}

static void init()
{
  init_target();
  init_ptrmat();
}

static void init_target()
{
  for (size_t i = 0; i < NR_TARGETS; i++)
  {
    for (size_t w = 0; w < TARGET_WIDTH; w++)
    {
      target[i].pos_x[w] = 0;
    }
    target[i].pos_y = 0;
    target[i].on_off = kOff;
    target[i].state = 0;
    target[i].move_dir = insert_dir;
    target[i].lane_ch = 0;
  }
}

void init_ptrmat()
{
  // pthread_mutex_lock(get_target_ptrmat_upd_mutex());
  pthread_mutex_lock(get_target_upd_mutex());
  for (size_t r = 0; r < HMAX; r++)
    for (size_t c = 0; c < LMAX; c++)
      target_ptrmat[r][c] = NULL;
  // pthread_mutex_unlock(get_target_ptrmat_upd_mutex());
  pthread_mutex_unlock(get_target_upd_mutex());
}

static void game_target()
{
  game_on_target();
}

static void game_on_target()
{

  int target_nr = 0;
  while (1)
  {
    //vf state
    pthread_mutex_lock(get_game_on_mutex());
    if (!get_game_on_check()) //daca este game off
    {                         //(pun toate target-urile pe off)
      for (size_t tg = 0; tg < NR_TARGETS; tg++)
      {
        target[tg].on_off = kOff;
      }
      pthread_mutex_unlock(get_game_on_mutex());
      return;
    }
    pthread_mutex_unlock(get_game_on_mutex());

    //update targets
    pthread_mutex_lock(get_target_upd_mutex());
    // pthread_mutex_lock(get_target_ptrmat_upd_mutex());
    //move target and change lane
    for (size_t tg = 0; tg < target_nr; tg++)
    {
      move_target(tg);
    }
    //insert and wait for other targets to move
    if (target_nr < NR_TARGETS && check == 0)
    {
      insert_target(target_nr, insert_pos[0]);
      target_nr++;
    }
    pthread_mutex_unlock(get_target_upd_mutex());
    // pthread_mutex_unlock(get_target_ptrmat_upd_mutex());
    //

    //update matrice de afisare
    update_matrix_tg();

    //schimbare stare target
    new_state_tg();

    //update check
    check++;
    check %= 4;

    if (do_sleep(80))
    {
      fprintf(stderr, "target: Sleep-ul a esuat\n");
      perror("Cauza este");
    }
  }
}

static void insert_target(size_t tg_nr, position_st in_pos)
{
  update_insert_target(tg_nr, in_pos);
  update_target_ptrmat(tg_nr);
  return;
}

static void update_insert_target(size_t tg_nr, position_st in_pos)
{
  //y
  target[tg_nr].pos_y = in_pos.y;
  //x
  target[tg_nr].pos_x[2] = in_pos.x;
  target[tg_nr].pos_x[1] = target[tg_nr].pos_x[2] - target[tg_nr].move_dir;
  target[tg_nr].pos_x[0] = target[tg_nr].pos_x[1] - target[tg_nr].move_dir;
  //status
  target[tg_nr].on_off = kOn;
}

//update matrice afisare
static void update_matrix_tg()
{
  //lock matrix mutex
  pthread_mutex_lock(get_mat_upd_mutex());

  for (size_t tg_nr = 0; tg_nr < NR_TARGETS; tg_nr++)
  {
    if (target[tg_nr].on_off == kOn)
    {
      //noua pozitie
      for (size_t i = 0; i < TARGET_WIDTH; i++)
      {
        if (interior_target(target[tg_nr].pos_x[i], target[tg_nr].pos_y))
          set_mat_draw(target[tg_nr].pos_y, target[tg_nr].pos_x[i], shape_target[target[tg_nr].state][i]);
      }

      //sterg urma veche
      if (target[tg_nr].lane_ch == 0) //daca nu am schimbat linia
        set_mat_draw(target[tg_nr].pos_y, target[tg_nr].pos_x[1] - 2 * target[tg_nr].move_dir, ' ');
      else
      { //daca am schimbat linia
        for (size_t i = 0; i < TARGET_WIDTH; i++)
          set_mat_draw(target[tg_nr].pos_y - 1, target[tg_nr].pos_x[i], ' ');
        //reset lane change
        target[tg_nr].lane_ch = 0;
      }
    }
  }

  //unlock matrix mutex
  pthread_mutex_unlock(get_mat_upd_mutex());
}

static void move_target(size_t tg_nr)
{
  if (target[tg_nr].on_off == kOff)
    return;
  if (interior_target((target[tg_nr].pos_x[1] + 2 * target[tg_nr].move_dir), target[tg_nr].pos_y))
  {
    update_target(tg_nr);
  }
  else
  {
    update_target_changelane(tg_nr);
  }
  update_target_ptrmat(tg_nr);

  //check collision with player
  collision_target_pl(tg_nr);
}

//check collision of target with player
static void collision_target_pl(size_t tg_nr)
{
  pthread_mutex_lock(get_player_upd_mutex());

  for (size_t tg_part = 0; tg_part < TARGET_WIDTH; tg_part++)
  {
    for (size_t pl_part = 0; pl_part < SPACESHIP_WIDTH; pl_part++)
    {
      if (target[tg_nr].pos_y == get_player().pos_y && target[tg_nr].pos_x[tg_part] == get_player().pos_x[pl_part])
      {
        //collision
        //semnalez oprirea jocului
        printf("aici\n");
        pthread_mutex_lock(get_game_on_mutex());
        set_game_on_check(0);
        pthread_mutex_unlock(get_game_on_mutex());

        pthread_mutex_unlock(get_player_upd_mutex());
        return;
      }
    }
  }
  pthread_mutex_unlock(get_player_upd_mutex());
}

static void update_target(size_t tg_nr)
{
  //only x
  target[tg_nr].pos_x[2] += target[tg_nr].move_dir;
  target[tg_nr].pos_x[1] += target[tg_nr].move_dir;
  target[tg_nr].pos_x[0] += target[tg_nr].move_dir;
}

static void update_target_changelane(size_t tg_nr)
{
  //only y
  target[tg_nr].pos_y += 1;  //doar in sens crescator
  target[tg_nr].lane_ch = 1; //marchez schimbarea de linie
  target[tg_nr].move_dir = -target[tg_nr].move_dir;

  if (target[tg_nr].pos_y > H_IN_MAX)
  {
    //semnalez oprirea jocului
    pthread_mutex_lock(get_game_on_mutex());
    set_game_on_check(0);
    pthread_mutex_unlock(get_game_on_mutex());
  }
}

static void update_target_ptrmat(size_t tg_nr)
{
  //update target matrix
  if (target[tg_nr].lane_ch == 0) //nu schimb linia
  {
    for (size_t i = 0; i < TARGET_WIDTH; i++)
    {
      if (interior_target(target[tg_nr].pos_x[i], target[tg_nr].pos_y))
        //upd target matrix
        set_target_ptrmat(target[tg_nr].pos_y, target[tg_nr].pos_x[i], tg_nr);
    }
    //sterg urma
    if (interior_target(target[tg_nr].pos_x[1] - 2 * target[tg_nr].move_dir, target[tg_nr].pos_y))
      set_target_ptrmat(target[tg_nr].pos_y, target[tg_nr].pos_x[1] - 2 * target[tg_nr].move_dir, -1);
  }
  else //am schimbat linia
  {
    if (target[tg_nr].pos_y > H_IN_MAX) //daca ajung la capat
    {
      for (size_t i = 0; i < TARGET_WIDTH; i++)
      {
        //setez si cele curente cu NULL
        set_target_ptrmat(target[tg_nr].pos_y, target[tg_nr].pos_x[i], -1);
      }
      return;
    }
    else //nu am ajuns in capat si setez noile pozitii
    {
      for (size_t i = 0; i < TARGET_WIDTH; i++)
      {
        //setez si cele curente cu NULL
        set_target_ptrmat(target[tg_nr].pos_y, target[tg_nr].pos_x[i], tg_nr);
      }
    }

    for (size_t i = 0; i < TARGET_WIDTH; i++)
    {
      //setez poz vechi cu NULL
      set_target_ptrmat(target[tg_nr].pos_y - 1, target[tg_nr].pos_x[i], -1);
    }
  }
}

static int interior_target(int pos_x, int pos_y)
{
  if (pos_x < L_IN_MIN || pos_x > L_IN_MAX || pos_y < H_IN_MIN || pos_y > H_IN_MAX)
    return 0;
  return 1;
}

static void new_state_tg()
{
  for (size_t tg = 0; tg < NR_TARGETS; tg++)
  {
    target[tg].state += 1;
    target[tg].state %= 3;
  }
}

//set matricea de ptr la target
void set_target_ptrmat(int r, int c, int tg_nr)
{
  if (tg_nr == -1)
    target_ptrmat[r][c] = NULL;
  else
    target_ptrmat[r][c] = &target[tg_nr];
}

target_st *get_target_ptrmat(int r, int c)
{
  return target_ptrmat[r][c];
}
