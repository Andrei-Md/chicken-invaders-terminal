CC=gcc
CFLAGS=-DDEBUG -g -O3 -Wall
LIBS=-pthread

CFILES=ci_globals.c ci_player.c ci_target.c ci_bullet.c chicken_invaders.c
HFILES=ci_globals.h ci_player.h ci_target.h ci_bullet.h chicken_invaders.h
OFILES=ci_globals.o ci_player.o ci_target.o ci_bullet.o chicken_invaders.o
all: chicken_invaders


chicken_invaders: $(OFILES) $(HFILES)
		$(CC) $(LDFLAGS) -o $@ $(OFILES) $(LIBS)

%.o: %.c $(HFILES)
		$(CC) -c $(CFLAGS) -o $@ $<

clean:
		rm -f chicken_invaders $(OFILES)
