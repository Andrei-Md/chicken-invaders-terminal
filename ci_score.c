#include <stdio.h>
#include <stdlib.h>

#include "ci_globals.h"
#include "ci_score.h"

score_st score;


void start_score()
{
  init_score();
}

void init_score(){
  score.points = 0;
  score.lives = LIVES;
}

