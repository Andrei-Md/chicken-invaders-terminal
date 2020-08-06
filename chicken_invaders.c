#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <termios.h> //pentru schimbare optiuni terminal

#include "ci_globals.h"
#include "ci_player.h"
#include "ci_bullet.h"
#include "ci_target.h"
#include "chicken_invaders.h"

 /*matrice afisare bordata
  x -> 1 : LMAX -2;
  y -> 1 : HMAX -2;
 */
static char mat_draw[HMAX][LMAX];

static pthread_t threads[NUM_THREADS];

//optiuni terminal
static struct termios initial_settings, new_settings;

///init
static void init_draw();
static void init_terminal(); //terminal

static void init();
static void init_score();
///start
static void ci_start();

//exit
static void ci_final();
static void reset_terminal();

//afisare
static void draw_game();
static void draw_game_over();
static void draw_winner();

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

  //draw final
  pthread_mutex_lock(get_winner_mutex());

  if (get_winner_status() == 0)
    draw_game_over();
  else
    draw_winner();
  pthread_mutex_unlock(get_winner_mutex());

  reset_terminal();

  return 0;
}

//start la thread-uri pentru fiecare elem
static void ci_start()
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
  init_score();
}

//initializare score
static void init_score()
{
  score.points = 0;
  score.lives = LIVES;
}

//initializare mat de afisare playing ground cu spatii
static void init_draw()
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
static void init_terminal()
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

static void ci_final()
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

static void reset_terminal()
{
  //reset terminal to initial state
  tcsetattr(0, TCSANOW, &initial_settings);
}

static void draw_game()
{
  // printf("\e[1;1H\e[2J");
  printf("\e[2J");
  // system("clear");

  pthread_mutex_lock(get_score_upd_mutex());

  //draw Score board
  printf("Points - %.*ld\t", SCOREWIDTH, score.points);
  printf("Lives - %.*d\n", LIVESWIDTH, score.lives);

  pthread_mutex_unlock(get_score_upd_mutex());

  pthread_mutex_lock(get_mat_upd_mutex());

  //afisare playing ground
  for (size_t r = 0; r < HMAX; r++)
  {
    for (size_t c = 0; c < LMAX; c++)
      printf("%c", mat_draw[r][c]);
    printf("\n");
  }

  pthread_mutex_unlock(get_mat_upd_mutex());

  return;
}

static void draw_game_over()
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

static void draw_winner()
{
  printf("\e[2J");
  printf("##                       ##\n");
  printf(" ##         ###         ## \n");
  printf("  ##       ## ##       ##  \n");
  printf("   ##     ##   ##     ##   \n");
  printf("    ##   ##     ##   ##    \n");
  printf("     ## ##       ## ##     \n");
  printf("      ###         ###      \n");

  pthread_mutex_lock(get_score_upd_mutex());

  //draw Score board
  printf("Points - %.*ld\n", SCOREWIDTH, score.points);
  printf("Lives Left- %.*d\n", LIVESWIDTH, score.lives);

  pthread_mutex_unlock(get_score_upd_mutex());

}

//set matricea de afisare
void set_mat_draw(int r, int c, char ch)
{
  mat_draw[r][c] = ch;
}

//get matricea de afisare
char get_mat_draw(int r, int c)
{
  return mat_draw[r][c];
}