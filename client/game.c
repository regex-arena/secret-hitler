// SPDX-License-Identifier: CC-BY-NC-SA-4.0

#include <curses.h>
#include <menu.h>
#include <menu.h>
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

    char *candidates[9];

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

    ITEM **voteitms = (ITEM **)calloc(3, sizeof(ITEM *));
    voteitms[0] = new_item("Ja", "Ja");
    voteitms[1] = new_item("Nein", "Nein");
    voteitms[3] = (ITEM *)NULL;
    MENU *vote = new_menu(voteitms);
    menu_opts_off(vote, O_SHOWDESC);
    for (;;) {
        drawh(names, facists, lobby_size, hitler, president, chanselor, main_screen);
        if (index == president) {
            move(2, 0);
            printw("Select a chanselor from valid candidates:\n");
            refresh();
            char ppres, pchance;
            int valid = (1<<11) - 1;

            printw("before pchance\n");
            refresh();
            recv(sock, &pchance, sizeof(char), 0);
            printw("before ppres\n");
            refresh();
            recv(sock, &ppres, sizeof(char), 0);
            printw("after ppres\n");
            refresh();
            exit(1);

            valid &= ~(1<<pchance);
            valid &= ~(1<<ppres);
            valid &= ~(1<<index);

            ITEM **candidates = (ITEM **)calloc(lobby_size, sizeof(ITEM *));

            int j = 0;
            int map [10];
            for (int i = 0; i < lobby_size; i++) {
                if ((1<<i) & valid) {
                    candidates[j] = new_item(names+((i)*(NAME_LENGHT+1)*sizeof(char)),
                                             names+((i)*(NAME_LENGHT+1)*sizeof(char)));
                    map[j] = i;
                    j++;
                }
            }
            candidates[j] = (ITEM *)NULL;
            WINDOW *selwin = newwin(10, 15, 2, 0);
            MENU *selcandidate = new_menu(candidates);
            set_menu_win(selcandidate, selwin);
            set_menu_opts(selcandidate, O_ONEVALUE | O_SELECTABLE);
            menu_opts_off(selcandidate, O_SHOWDESC);
            post_menu(selcandidate);
            refresh();
            int c;
            while ((c = getch()) != KEY_ENTER) {
                switch(c) {
                    case KEY_UP:
                        menu_driver(selcandidate, REQ_PREV_ITEM);
                    break;
                    case KEY_DOWN:
                        menu_driver(selcandidate, REQ_NEXT_ITEM);
                    break;
                }
            }

            send(sock, &map[item_index(current_item(selcandidate))], sizeof(int), 0);
            for (int i = 0; i < lobby_size; i++) {
                free_item(candidates[i]);
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

        post_menu(vote);
        refresh();
        int c;
        while ((c = getch()) != KEY_ENTER) {
            switch (c) {
                case KEY_UP:
                    menu_driver(vote, REQ_PREV_ITEM);
                break;
                case KEY_DOWN:
                    menu_driver(vote, REQ_PREV_ITEM);
                break;
            }
        }

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
