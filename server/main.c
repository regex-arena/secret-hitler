// SPDX-License-Identifier: GPL-3.0-or-later

// Connection
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <netdb.h>

#include <sys/types.h>
#include <stdlib.h>

// IO
#include <stdio.h>
#include <string.h>
#include <errno.h>

// Game loop
#include "game.h"

int main(int argc, char** argv) {
    // Initilises hints to 0
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    struct addrinfo *result, *rp;
    int sock;

    if (argc != 3) {
        fprintf(stderr, "Invalid argument - Expected form 'command port lobbySize'");
        exit(EXIT_FAILURE);
    }
    int loby_size = atoi(argv[2]);
    if (loby_size > 10 || loby_size < 5) {
        fprintf(stderr, "Invalid loby size - expected between 5 and 10");
        exit(EXIT_FAILURE);
    }

    hints.ai_family = AF_UNSPEC; // Can use both IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM; // Stream prefered over DGRAM due to reliability
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    if (getaddrinfo(NULL, argv[1], &hints, &result) != 0) {
        fprintf(stderr, "Unable to get address info: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1)
            continue;
        if (bind(sock, rp->ai_addr, rp->ai_addrlen) == 0)
            break;
        close(sock);
    }
    freeaddrinfo(result);
    if (rp == NULL) {
        fprintf(stderr, "Unable to bind to socket");
        exit(EXIT_FAILURE);
    }

    loop(sock, loby_size);
    return 0;
}
