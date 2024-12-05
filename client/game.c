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
    char president = 11;
    char chanselor = 11;
    recv(sock, &role, sizeof(char), 0);
    recv(sock, &president, sizeof(char), 0);

    char facists[4] = { 11, 11, 11, 11 };
    char nfacists = (lobby_size - 1) / 2;
    char hitler = 11;
    if (role == 1 || (role == 2 && lobby_size < 7)) {
        recv(sock, facists, nfacists * sizeof(char), 0);
        recv(sock, &hitler, sizeof(char), 0);
    }

    for (;;) {
        drawh(names, facists, lobby_size, hitler, president, chanselor, main_screen);
        if (index == president) {
            move(2, 0);
            printw("Select a chanselor from valid candidates:\n");
            refresh();
            char ppres, pchance;
            int valid = (1<<11) - 1;

            recv(sock, &pchance, sizeof(char), 0);
            recv(sock, &ppres, sizeof(char), 0);

            valid &= ~(1<<pchance);
            valid &= ~(1<<ppres);
            valid &= ~(1<<index);
            printw("%b\n", valid);
            printw("%b\n", ((1<<index) & valid));
            
            int j = 1;
            int map [10];
            for (int i = 0; i < lobby_size; i++) {
                if ((1<<i) & valid) {
                    printw("%d: %s\n", j, names+((i)*(NAME_LENGHT+1)*sizeof(char)));
                    j++;
                    map[j-1] = i;
                }
            }

            char candidate[2] = {'\0', '\0'};
            echo();
            for (;;) {
                printw("Elect candidate number: ");
                refresh();
                getnstr(candidate, 1);
                char icandidate = atoi(candidate);
                if (icandidate < 1 || icandidate >= j) {
                    move(j+2, 0);
                    continue;
                }

                icandidate = map[icandidate-1];
                send(sock, &icandidate, sizeof(char), 0);
                break;
            }
            noecho();
        } else {
            move(2, 0);
            printw("Waiting for a candidate to be nominated\n");
            refresh();
        }

        char candidate = 0;
        recv(sock, &candidate, sizeof(char), 0);
        drawh(names, facists, lobby_size, hitler, president, chanselor, main_screen);
        move(2, 0);
        printw("Candidate = %s\n", names+(candidate*(NAME_LENGHT+1)*sizeof(char)));
        refresh();

        echo();
        noecho();

        sleep(50);
        break;
    }

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
