// SPDX-License-Identifier: GPL-3.0-or-later

#include <curses.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

const int NAME_LENGHT=8;

static void finish();

void render(int sock) {

    signal(SIGINT, finish);
    WINDOW *main_screen = initscr();
    keypad(stdscr, true);
    nonl();
    cbreak();

    if (has_colors()) {
        start_color();

        init_pair(1, COLOR_GREEN, COLOR_BLACK);
        init_pair(1, COLOR_RED, COLOR_BLACK);
    }

    printw("Enter your name:\n");
    refresh();
    char *name = malloc(sizeof(char)*(NAME_LENGHT+1));
    echo();
    getnstr(name, NAME_LENGHT);
    send(sock, name, sizeof(char)*(NAME_LENGHT+1), 0);

    erase();
    noecho();
    mvprintw(0, 0, "Waiting for all players to join game");
    refresh();
    erase();

    int lobby_size = 0;
    recv(sock, &lobby_size, sizeof(int), 0);
    int index = 0;
    recv(sock, &index, sizeof(int), 0);
    if (lobby_size < 5 || lobby_size > 10) {
        endwin();
        fprintf(stderr,
                "Invalid lobby size given by sever, "
                "expected between 1 and 5, given %d",
                lobby_size);
        exit(1);
    }

    char *names = malloc(lobby_size*(NAME_LENGHT+1)*sizeof(char));
    for (int i = 0; i < lobby_size; i++) {
        if (i == index) {
            strcpy(names+i*(NAME_LENGHT+1), "You\0");
        }
        else {
            recv(sock,
                 names+i*sizeof(char)*(NAME_LENGHT+1),
                 (NAME_LENGHT+1)*sizeof(char),
                 0);
        }
    }
    refresh();
    erase();


//    for (;;) {
//
//        refresh();
//    }
    sleep(50);
    free(names);
    free(name);
    endwin();
}

void finish() {
    endwin();
    exit(0);
}
