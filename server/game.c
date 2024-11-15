// SPDX-License-Identifier: GPL-3.0-or-later

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
    char players;
    char fpolicy;
    char lpolicy;
};

void loop(int sock, int lobby_size) {
    // Initalisation
    // Conect clients
    struct sockaddr *clientSFD = malloc(lobby_size * sizeof(struct sockaddr));
    int *clients = malloc(sizeof(int)*lobby_size);
    char *names = malloc(lobby_size*(NAME_LENGHT+1)*sizeof(char));
    
    int i = 0;
    while (i < lobby_size) {
        socklen_t clientlen = sizeof(clientSFD[i]);
        clients[i] = accept(sock, &clientSFD[i], &clientlen);
        if (clients[i] == -1) {
            fprintf(stderr, "Failed to accept connection: error %d\n", errno);
        } else {
            recv(clients[i], names+i*sizeof(char)*(NAME_LENGHT+1), (NAME_LENGHT+1)*sizeof(char), 0);

            i++;
        }
    }
    
    // Initalise game state

    // Loop
    //for (;;) {
    //}
    
    // Cleanup
    for (int i = 0; i < lobby_size; i++)
        shutdown(clients[i], SHUT_RDWR);
    free(clients);
    free(clientSFD);
    free(names);
}
