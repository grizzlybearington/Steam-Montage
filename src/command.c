#include <getopt.h>
#include <stdio.h>
#include <string.h>

#include "command.h"

#define CFG_API_KEY 20000
#define CFG_STEAM_ID 20001

void
print_cli_failure(const char *arg0)
{
    fprintf(stderr,
    "Error when parsing command-line arguments.\n"
    "An illegal value may have been passed.\n"
    "For more help, enter: '%s -h'\n\n",
    arg0);
}

void
print_help(const char *arg0)
{
    fprintf(stderr,
    "%s [options]\n"
        "\n"
        "Command-line options:\n"
        "\n"
        "    --api-key [value]\n"
        "        Set the Steam Web API key used to [value].\n"
        "        For more information, visit\n"
        "        https://www.steamcommunity.com/dev/\n"
        "\n"
        "    -h --help\n"
        "        Displays this help message and exits the program.\n"
        "\n"
        "    --steam-id [value]\n"
        "        Set the ID of the Steam profile for which the Montage will\n"
        "        be created to [value].\n"
        "\n"
        "    -v --version\n"
        "        Displays the currently running version and exits the\n"
        "        program.\n"
        "\n"
        "    -w, --width [value]\n"
        "        Set the width of the montage to [value] (in tiles).\n"
        "\n"
        "config.txt options\n"
        "!Note!: All config.txt options are overridden by command line\n"
        "options!:\n"
        "\n"
        "    api-key=[value]\n"
        "        Set the Steam Web API key used to [value].\n"
        "        For more information, visit\n"
        "        https://www.steamcommunity.com/dev/\n"
        "\n"
        "    steam-id=[value]\n"
        "        Set the ID of the Steam profile for which the Montage will\n"
        "        be created to [value].\n"
        "\n"
        "    width=[value]\n"
        "        Set the width of the montage to [value] (in tiles).\n"
        "\n"
        "\n", arg0);
}

int
parse_args(struct options *opts, int argc, char *argv[])
{
    int c;
    int option_index = 0;
    struct config *cfg = &opts->cfg;

    static struct option long_options[] = {
        {OPT_API_KEY,  required_argument, 0, CFG_API_KEY},
        {OPT_HELP,     no_argument,       0, 'h'},
        {OPT_STEAM_ID, required_argument, 0, CFG_STEAM_ID},
        {OPT_VERSION,  no_argument,       0, 'v'},
        {OPT_WIDTH,    required_argument, 0, 'w'},
        {0,            0,                 0, 0}
    };

    while ((c = getopt_long (argc, argv, "hvw:",
            long_options, &option_index)) != EOF) {
        switch(c) {
            case CFG_API_KEY:
                if (!parse_api_key(optarg, &cfg->api_key[0])) {
                    return 0;
                }
                break;

            case 'h':
                opts->help = 1;
                break;

            case CFG_STEAM_ID:
                if (!parse_steam_id(optarg, &cfg->steam_id[0])) {
                    return 0;
                }
                break;

            case 'v':
                opts->version = 1;
                break;

            case 'w':
                if (!parse_width(optarg, &cfg->width)) {
                    return 0;
                }
                break;

            default:
                return 0;
        };
    }

    return 1;
}
