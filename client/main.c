// SPDX-License-Identifier: GPL-3.0-or-later

// Connect
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

// Standard
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

// Output
#include <stdio.h>
#include "game.h"

int validateIP(char *ip) {
    struct sockaddr_in ad;
    return
        inet_pton(AF_INET, ip, &(ad.sin_addr)) == 1 ||
        inet_pton(AF_INET6, ip, &(ad.sin_addr)) == 1;
}

int main(int argc, char** argv) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    struct addrinfo *result, *rp;
    int sock;

    // Validate input
    if (argc != 3) {
        fprintf(stderr, "Invalid argument - Expected form 'command ip port'");
        exit(EXIT_FAILURE);
    } else if (!validateIP(argv[1])) {
        fprintf(stderr, "Invalid IP - please provide valid IPv4 or IPv6 address");
        exit(EXIT_FAILURE);
    } else if (argv[2]) {
    }
    
    hints.ai_family = AF_UNSPEC; // Can use both IPv4 and IPv6
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    if (getaddrinfo(argv[1], argv[2], &hints, &result) != 0) {
        fprintf(stderr, "Unable to get address info: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock == -1)
            continue;
        if (connect(sock, rp->ai_addr, rp->ai_addrlen) != -1)
            break;
        close(sock);
    }
    freeaddrinfo(result);

    if (rp == NULL) {
        fprintf(stderr, "Unable to bind to socket");
        exit(EXIT_FAILURE);
    }

    render(sock);

    return 0;
}
