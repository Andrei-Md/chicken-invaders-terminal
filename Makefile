CC=gcc
CFLAGS=-DDEBUG -g -O3 -Wall
LIBS=-pthread

CFILES=ci_globals.c ci_player.c ci_target.c ci_bullet.c ci_score.c chicken_invaders.c ci_test.c
HFILES=ci_globals.h ci_player.h ci_target.h ci_bullet.h ci_score.h chicken_invaders.h ci_test.h
OFILES=ci_globals.o ci_player.o ci_target.o ci_bullet.o ci_score.o chicken_invaders.o ci_test.o
all: chicken_invaders


chicken_invaders: $(OFILES) $(HFILES)
		$(CC) $(LDFLAGS) -o$@ $(OFILES) $(LIBS)

%.o: %.c $(HFILES)
		$(CC) -c $(CFLAGS) -o $@ $<

clean:
		rm -f chicken_invaders $(OFILES)
