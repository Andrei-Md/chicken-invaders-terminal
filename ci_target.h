#ifndef CI_TARGET_H
#define CI_TARGET_H

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

void *start_targets(void *thread_id);

void init_ptrmat();

//target ptr matrix
void set_target_ptrmat(int r, int c, int tg_nr);
target_st* get_target_ptrmat(int r,int c);
#endif
