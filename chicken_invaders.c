#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <termios.h> //pentru schimbare optiuni terminal

#include "ci_globals.h"
#include "ci_player.h"
#include "ci_bullet.h"
#include "ci_score.h"
#include "ci_target.h"
#include "chicken_invaders.h"

/* matrice afisare bordata
  x -> 1 : LMAX -2;
  y -> 1 : HMAX -2;
 */
static char mat_draw[HMAX][LMAX];

static pthread_t threads[NUM_THREADS];

//optiuni terminal
static struct termios initial_settings, new_settings;


// //counter for wait
// static int counter = NUM_THREADS;

int main(int argc, char *argv[])
{
  init();

  //threads din main
  ci_start();

  ///tmp
  //start game
  set_game_on_check(1);

  while (1)
  {

    pthread_mutex_lock(get_game_on_mutex());
    if (!get_game_on_check())
    {
      pthread_mutex_unlock(get_game_on_mutex());
      break;
    }
    pthread_mutex_unlock(get_game_on_mutex());
    draw_game();

    if (do_sleep(65))
    {
      fprintf(stderr, "main: Sleep-ul a esuat\n");
      perror("Cauza este");
    }
  }

  ci_final();

  draw_game_over();

  reset_terminal();

  return 0;
}

//start la thread-uri pentru fiecare elem
void ci_start()
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

  ///start threads
  //player
  if (pthread_create(&threads[TH_PLAYER], &attr, start_player, (void *)TH_PLAYER))
  {
    fprintf(stderr, "main: Nu s-a creat thread-ul cu nr %d\n", TH_PLAYER);
    perror("Cauza este");
  }

  //targets
  if (pthread_create(&threads[TH_TARGET], &attr, start_targets, (void *)TH_TARGET))
  {
    fprintf(stderr, "main: Nu s-a creat thread-ul cu nr %d\n", TH_TARGET);
    perror("Cauza este");
  }

  start_score();

  ///

  //destroy attribute
  if (pthread_attr_destroy(&attr))
  {
    fprintf(stderr, "main: Nu am putut distruge pthread_attr\n");
    perror("Cauza este");
  }
}

//initializare inainte de a porni thread-uri
static void init()
{
  init_terminal();
  init_draw();
  init_ptrmat();
}

//initializare mat de afisare playing ground cu spatii
void init_draw()
{
  ///fill
  for (size_t r = 0; r < HMAX; r++)
    for (size_t c = 0; c < LMAX; c++)
      mat_draw[r][c] = ' ';

  ///border
  for (size_t r = 1; r < HMAX; r++)
  {
    mat_draw[r][0] = 'X';
    mat_draw[r][LMAX - 1] = 'X';
  }

  for (size_t c = 1; c < LMAX - 1; c++)
  {
    mat_draw[0][c] = '_';
    mat_draw[HMAX - 1][c] = '_';
  }
}

//initializare optiuni terminal
void init_terminal()
{

  tcgetattr(0, &initial_settings);
  /* save the original state so I can reset at the end */
  new_settings = initial_settings;

  /* enable canonical input processing mode
  characters are not grouped in lines,
   and ERASE and KILL processing is not performed.
   the grnularity with which bytes are read in
   noncanonical input mode is controled by the *MIN* and *TIME* settings.
   */
  new_settings.c_lflag &= ~ICANON;

  /* echo of input character back to terminal is unabled */
  new_settings.c_lflag &= ~ECHO;

  /* INTR, QUIR and SUSP characters are not recognized */
  // new_settings.c_lflag &= ~ISIG;

  //options meaningful only in NONCANONICAL input mode
  /* specify minimum number of bytes that must be available in the
   input queue in order for read to return */
  new_settings.c_cc[VMIN] &= 0;

  /* how long to wait for input before returning
  - units of 0.1 seconds */
  new_settings.c_cc[VTIME] &= 0;

  //set attributes
  tcsetattr(0, TCSANOW, &new_settings);
}

void ci_final()
{
  //wait threads to complete
  int t;
  void *status;
  for (t = 0; t < NUM_THREADS; t++)
  {
    if (pthread_join(threads[t], &status))
    {
      fprintf(stderr, "main: Nu s-a facut join la thread-ul cu nr %d avand un status %ld\n", t, (long)status);
      perror("Cauza este");
    }
    printf("Main: completed joined with thread %d having a status of %ld\n", t, (long)status);
  }
}

void reset_terminal()
{
  //reset terminal to initial state
  tcsetattr(0, TCSANOW, &initial_settings);
}

void draw_game()
{
  // printf("\e[1;1H\e[2J");
  printf("\e[2J");
  // system("clear");

  //draw Score board
  printf("Points - %.*ld\t", SCOREWIDTH, score.points);
  printf("Lives - %.*d\n", LIVESWIDTH, score.lives);

  //afisare playing ground
  for (size_t r = 0; r < HMAX; r++)
  {
    for (size_t c = 0; c < LMAX; c++)
      printf("%c", mat_draw[r][c]);
    printf("\n");
  }
  return;
}

void draw_game_over()
{
  printf("\e[2J");
  printf("  #####   ####  ##   ## ###### \n");
  printf(" ##      ##  ## ####### ##     \n");
  printf(" ## ###  ###### ## # ## #####  \n");
  printf(" ##  ##  ##  ## ##   ## ##     \n");
  printf("  #####  ##  ## ##   ## ###### \n");
  printf("                               \n");
  printf("  ####  ##   ## ###### ######  \n");
  printf(" ##  ## ##   ## ##     ##   ## \n");
  printf(" ##  ##  ## ##  #####  ######  \n");
  printf(" ##  ##  ## ##  ##     ##  ##  \n");
  printf("  ####    ###   ###### ##   ## \n");
  printf("                               \n");
}

//set matricea de afisare
void set_mat_draw(int r, int c, char ch)
{
  mat_draw[r][c] = ch;
}

// /////////////////////////////////////////////////////////////////
// //tmp
// void tmp_update_mat_draw()
// {
//   //targets
//   for (size_t tg = 0; tg < NR_TARGETS; tg++)
//   {
//     for (size_t i = 0; i < TARGET_WIDTH; i++)
//       if (target[tg].on_off == kOn)
//       {
//         mat_draw[target[tg].pos_y][target[tg].pos_x[i]] = shape_target[target[tg].state][i];
//       }
//   }
// }