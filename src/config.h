#ifndef SM_CONFIG_H
#define SM_CONFIG_H

#include <stdint.h>

#define OPT_API_KEY "api-key"
#define OPT_HELP "help"
#define OPT_STEAM_ID "steam-id"
#define OPT_VERSION "version"
#define OPT_WIDTH "width"

#define MAX_CFG_LINE_SIZE 256

#define CFG_DEFAULT_WIDTH 10

#define STEAM_API_LEN 32
#define STEAM_ID_LEN 17

struct runningdir {
    char *dirpath;
    int dirlen;
};

struct config {
    char api_key[STEAM_API_LEN + 1];
    char steam_id[STEAM_ID_LEN + 1];
    uint16_t width;
};

#define CONFIG_DEFAULTS { \
    .api_key[0] = '\0', \
    .steam_id[0] = '\0', \
    .width = CFG_DEFAULT_WIDTH, \
}

int
validate_input(struct config *cfg);

int
parse_api_key(const char *arg, char *api_key);

int
parse_steam_id(const char *arg, char *steam_id);

int
parse_width(const char *arg, uint16_t *width);

int
get_running_dir(struct runningdir *runningdir);

int
parse_config(char *cfgpath, struct config *cfg);

#endif
