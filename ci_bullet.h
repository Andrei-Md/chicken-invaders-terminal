#ifndef CI_BULLET_H
#define CI_BULLET_H

typedef struct Bullet_st
{
  position_st pos;
  on_off_et on_off;
} bullet_st;

void *start_bullet(void *thread_id);

#endif
