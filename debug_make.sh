gcc -DDEBUG -g -O3 -Wall -c -o chicken_invaders.o chicken_invaders.c 
gcc -DDEBUG -g -O3 -Wall -c -o ci_bullet.o ci_bullet.c 
gcc -DDEBUG -g -O3 -Wall -c -o ci_globals.o ci_globals.c 
gcc -DDEBUG -g -O3 -Wall -c -o ci_player.o ci_player.c 
gcc -DDEBUG -g -O3 -Wall -c -o ci_score.o ci_score.c 
gcc -DDEBUG -g -O3 -Wall -c -o ci_target.o ci_target.c 
gcc -o game_ci chicken_invaders.o ci_bullet.o ci_globals.o ci_player.o ci_score.o ci_target.o -pthread