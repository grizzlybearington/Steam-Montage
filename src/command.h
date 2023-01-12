/* See LICENSE file for copyright & license details. */
#ifndef SM_COMMAND_H
#define SM_COMMAND_H

#include "config.h"

struct options {
    struct config cfg;
    int help;
    int version;
};

void print_cli_failure(const char *arg0);

void print_help(const char *arg0);

int parse_args(struct options *opts, int argc, char *argv[]);

#endif
