#ifndef CI_INIT_H
#define CI_INIT_H

///board
//margini exterioare
#define HMAX 15
#define LMAX 60
//margini interioare
#define H_IN_MIN 1
#define H_IN_MAX (HMAX - 2)
#define L_IN_MIN 1
#define L_IN_MAX (LMAX - 2)

//player
#define SPACESHIP_WIDTH 3

//targets
#define TARGET_WIDTH 3
#define NR_TARGETS 50
#define TARGET_DISTANCE 5

//bullet
#define BULLET_ADV_DIST -1

//scoreboard
#define LIVES 1
#define LIVESWIDTH 2
#define SCOREWIDTH 6
#define MAXSCORE 999999
#define TARGETSCORE 100

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

typedef struct Score_st
{
  size_t points;
  int lives;
} score_st;

typedef struct Target_st
{
  //status: target ON sau OFF
  on_off_et on_off;

  //pozitia pe y
  int pos_y;

  //state: 0,1,2
  int state;

  //pozitiile pe x
  int pos_x[TARGET_WIDTH];

  //move direction -1,0,1
  int move_dir;
  //lane change 0,1
  int lane_ch;

} target_st;

typedef struct Player_st
{
  //player ON sau OFF
  on_off_et on_off;

  //pozitia pe y
  int pos_y;

  //pozitiile pe x
  int pos_x[SPACESHIP_WIDTH];
}player_st;


typedef struct Bullet_st
{
  position_st pos;
  on_off_et on_off;
} bullet_st;

//sleep
int do_sleep(int microseconds);

//modificare matrice joc
pthread_mutex_t *get_mat_upd_mutex();
extern void set_mat_draw(int r, int c, char ch);
extern char get_mat_draw(int r, int c);

//modificare game status
pthread_mutex_t *get_game_on_mutex();
int get_game_on_check();
void set_game_on_check(int val);

//update player
pthread_mutex_t *get_player_upd_mutex();

//modificare bullet status
pthread_mutex_t *get_bullet_status_mutex();
int get_bullet_status_check();
void set_bullet_status_check(int val);

//bullet cond var
pthread_cond_t *get_bullet_cv();


///target
//modificare target ptr matrix
// pthread_mutex_t *get_target_ptrmat_upd_mutex();
//update target
pthread_mutex_t *get_target_upd_mutex();


//score update
pthread_mutex_t *get_score_upd_mutex();
score_st get_score();
void set_score(int val);

//winner
pthread_mutex_t *get_winner_mutex();
int get_winner_status();
void set_winner_status(int val);

///
//target
extern void set_target_ptrmat(int r, int c, int tg_nr);
extern target_st* get_target_ptrmat(int r,int c);

//player
extern position_st get_player_pos();
extern player_st get_player();
///
#endif