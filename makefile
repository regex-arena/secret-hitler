# SPDX-License-Identifier: GPL-3.0-or-later

CC=gcc

SERVER_LINK=

CLIENT_LINK=-lncurses -lmenu

server: server/main.c server/game.h server/game.c
	$(CC) $(SERVER_LINK) -o bin/server server/main.c

client: client/main.c client/game.h client/game.c
	$(CC) $(CLIENT_LINK) -o bin/client client/main.c

clean: ./bin/
	rm ./bin/*

