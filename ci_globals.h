#ifndef CI_INIT_H
#define CI_INIT_H

///board
//margini exterioare
#define HMAX 14
#define LMAX 20
//margini interioare
#define H_IN_MIN 1
#define H_IN_MAX (HMAX-2)
#define L_IN_MIN 1
#define L_IN_MAX (LMAX-2)

//player
#define SPACESHIP_WIDTH 3

//targets
#define TARGET_WIDTH 3
#define NR_TARGETS 20

#define LIVES 10
#define LIVESWIDTH 2
#define SCOREWIDTH 6
#define MAXSCORE 999999

//threads
#define NUM_THREADS 2
#define TH_PLAYER 0
#define TH_TARGET 1

#define TH_BULLET 10

typedef struct Position_st
{
  int x;
  int y;
} position_st;

typedef struct Position3x_st
{
  int left;
  int mid;
  int right;
} position3x_st;

typedef enum On_off_et
{
  kOff = 0,
  kOn = 1
} on_off_et;


//sleep
int do_sleep(int microseconds);

//modificare matrice joc
pthread_mutex_t *get_mat_upd_mutex();
extern void set_mat_draw(int r, int c, char ch);


//modificare game status
pthread_mutex_t *get_game_on_mutex();
int get_game_on_check();
void set_game_on_check(int val);

//modificare bullet status
pthread_mutex_t *get_bullet_status_mutex();
int get_bullet_status_check();
void set_bullet_status_check(int val);

//target
//modificare target ptr matrix
pthread_mutex_t *get_target_ptrmat_upd_mutex();
//update target
pthread_mutex_t *get_target_upd_mutex();



#endif