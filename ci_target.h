#ifndef CI_TARGET_H
#define CI_TARGET_H

typedef struct Target_st
{
  //target ON sau OFF
  on_off_et on_off;

  //pozitia pe y
  int pos_y;

  //stare 0,1,2
  int state;

  //pozitiile pe x
  int pos_x[TARGET_WIDTH];

} target_st;

void start_targets();
void init_target();
void debug_targets();
#endif
