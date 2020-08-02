#include <stdio.h>
#include <stdlib.h>

#include "ci_globals.h"
#include "ci_target.h"

target_st target[NRMAX_TARGETS];

char shape_target[3][3] = {{'\\', 'O', '/'},
                           {'-', 'O', '-'},
                           {'/', 'O', '\\'}};

void start_targets()
{
  init_target();

#ifdef DEBUG
  debug_targets();
#endif
}

void init_target()
{
  for (size_t i = 0; i < NRMAX_TARGETS; i++)
  {
    for (size_t w = 0; w < TARGET_WIDTH; w++)
    {
      target[i].pos_x[w] = 0;
    }
    target[i].pos_y = 0;
    target[i].on_off = kOff;
    target[i].state = 0;
  }
}

void debug_targets()
{
  target[0].pos_y = H_IN_MIN;
  target[0].pos_x[0] = L_IN_MIN;
  target[0].pos_x[1] = target[0].pos_x[0]+1;
  target[0].pos_x[2] = target[0].pos_x[1]+1;
  target[0].on_off = kOn;
  
  target[1].pos_y = H_IN_MIN;
  target[1].pos_x[0] = L_IN_MIN+5;
  target[1].pos_x[1] = target[1].pos_x[0]+1;
  target[1].pos_x[2] = target[1].pos_x[1]+1;
  target[1].state = 1;
  target[1].on_off = kOn;

  target[2].pos_y = H_IN_MIN;
  target[2].pos_x[0] = L_IN_MIN+10;
  target[2].pos_x[1] = target[2].pos_x[0]+1;
  target[2].pos_x[2] = target[2].pos_x[1]+1;
  target[2].state = 2;
  target[2].on_off = kOn;

}