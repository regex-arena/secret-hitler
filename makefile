# SPDX-License-Identifier: GPL-3.0-or-later

CC=gcc

server: server/main.c server/game.h server/game.c
	$(CC) -o bin/server server/main.c

client: client/main.c
	$(CC) -o bin/client client/main.c

clean: ./bin/
	rm ./bin/*

