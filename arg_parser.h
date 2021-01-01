//
// Created by boris on 23.12.2020.
//

#ifndef HTTP10SERVER_ARG_PARSER_H
#define HTTP10SERVER_ARG_PARSER_H

#include <unistd.h>

void parse_args(int argc, char *const *argv, const char *&port, const char *&ip, const char *&directory) {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnusedValue"
    int opchar = 0; // без 0 не проходило тесты
#pragma clang diagnostic pop
    while (-1 != (opchar = getopt(argc, argv, "h:p:d:"))) {
        switch (opchar) {
            case 'h': {
                ip = optarg;
                break;
            }
            case 'p': {
                port = optarg;
                break;
            }
            case 'd': {
                directory = optarg;
                break;
            }
            default:
                break;
        }
    }
}

#endif //HTTP10SERVER_ARG_PARSER_H
