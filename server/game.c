// SPDX-License-Identifier: CC-BY-NC-SA-4.0

// Connection
#include <sys/socket.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

const int NAME_LENGHT=8;

struct GameState {
    // Deck contains 17 cards so uint16_t has too few bits
    uint32_t pdeck;
    char dfullness;
    char hitler;
    char facist[4];
    char fpolicy;
    char lpolicy;
};

uint32_t populatedeck() {
    uint32_t deck = 0;
    int deckseed[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    // Fisher-Yates shuffle
    for (int i = 0; i < 15; i++) {
        int j = rand()%(17-i)+i;
        int jval = deckseed[j];
        deckseed[j] = deckseed[i];
        deckseed[i] = jval;
    }
    for (int i = 0; i < 11; i++) {
        printf("%d\n", deckseed[i]);
        deck = deck | 1<<(deckseed[i]);
    }
    printf("%17b\n", deck);

    return deck;
}

void loop(int *clients, int lobby_size) {
    struct GameState state;
    state.pdeck = populatedeck();
    state.dfullness = 17;

    char *players = malloc(lobby_size * sizeof(char));
    for (int i = 0; i < lobby_size; i++)
        players[i] = i;
    // Fisher-Yates shuffle
    for (int i = 0; i < lobby_size - 2; i++) {
        int j = rand()%(lobby_size-i)+i;
        char jval = players[j];
        players[j] = players[i];
        players[i] = jval;
    }
    // Number of facists including Hitler from game size
    char facists = (lobby_size - 1)/ 2;
    for (int i = 0; i < lobby_size; i++) {
        char sendval = 0;
        if (i < facists) {
            state.facist[i] = players[i];
            if (i == 0) {
                state.hitler = state.facist[0];
                sendval++;
            }
            sendval++;
        }
        send(clients[players[i]], &sendval, sizeof(char), 0);
    }
    free(players);

    for (int i = (facists == 2 ? 0 : 1); i < facists; i++) {
        send(clients[state.facist[i]], state.facist, facists*sizeof(char), 0);
        send(clients[state.facist[i]], &state.hitler, sizeof(char), 0);
    }

    state.fpolicy = 0;
    state.lpolicy = 0;


}
