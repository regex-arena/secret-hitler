// SPDX-License-Identifier: CC-BY-NC-SA-4.0

#include <curses.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

const int NAME_LENGHT=8;

static void finish();
void drawh(char *names,
           char *facists,
           char players,
           char hitler,
           char presedent,
           char chancelor,
           WINDOW *window);


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

    char role;
    recv(sock, &role, sizeof(char), 0);

    char facists[4] = { 11, 11, 11, 11 };
    char nfacists = (lobby_size - 1) / 2;
    char hitler = 11;
    if (role == 1 || (role == 2 && lobby_size < 7)) {
        recv(sock, facists, nfacists * sizeof(char), 0);
        recv(sock, &hitler, sizeof(char), 0);
    }

    char president = 11;
    char chanselor = 11;

    drawh(names, facists, index, hitler, president, chanselor, main_screen);


    //    for (;;) {
    //
    //        refresh();
    //    }
    sleep(5);
    free(names);
    free(name);
    endwin();
}

int inarray(char *array, char length, char value) {
    for (int i = 0; i < length; i++) {
        if (array[i] == value) {
            return 1;
        }
    }
    return 0;
}

void drawh(char *names,
           char *facists,
           char players,
           char hitler,
           char presedent,
           char chancelor,
           WINDOW *window) {
    move(1, 0);
    for (int i = 0; i < getmaxx(window); i++) {
        printw("-");
    }

    move(0, 0);
    for (int i = 0; i < players; i++) {
        printw(" ");
        if (i == hitler) {
            printw("(H) ");
        } else if (inarray(facists, 4, i)) {
            printw("(F) ");
        }

        if (i == presedent) {
            printw("(P) ");
        } else if (i == chancelor) {
            printw("(C) ");
        }

        printw("%s ", names+i*(NAME_LENGHT+1));
        if (i != players - 1) {
            printw("|");

        }
    }
    refresh();
}

void finish() {
    endwin();
    exit(0);
}
