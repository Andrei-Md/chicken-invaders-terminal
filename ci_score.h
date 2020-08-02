#ifndef CI_SCORE_H
#define CI_SCORE_H

typedef struct Score_st
{
  size_t points;
  int lives;
} score_st;

void start_score();
void init_score();

#endif