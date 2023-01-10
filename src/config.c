#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "steammontage.h"
#include "whereami/whereami.h"

#define CH_BACKSLASH '\\'
#define CH_EQUAL '='
#define CH_FORSLASH '/'
#define CH_NEWL '\n'
#define CH_NUMSIGN '#'
#define CH_NULL '\0'
#define CH_SPACE ' '

#define CONFIGTXT_LEN 10

int
validate_input(struct config *cfg)
{
    /* To be greatly expanded upon as more features are added */
    if (cfg->api_key[0] == '\0' || cfg->steam_id[0] == '\0') {
        fprintf(stderr,
                "Steam ID and/or API key not set.\n");
        return -1;
    }
    return 1;
}

int
parse_api_key(const char *val, char *api_key)
{
    int c = 0;
    for (int i = 0; val[i] != '\0'; i++) {
        if (!isalnum(val[i])) {
            return 0;
        }
        c++;
    }

    if (c != STEAM_API_LEN) {
        return 0;
    }

    strncpy(api_key, val, STEAM_API_LEN);
    return 1;
}

int
parse_steam_id(const char *val, char *steam_id)
{
    int c = 0;
    for (int i = 0; val[i] != '\0'; i++) {
        if (!isdigit(val[i])) {
            return 0;
        }
        c++;
    }

    if (c != STEAM_ID_LEN) {
        return 0;
    }

    strncpy(steam_id, val, STEAM_ID_LEN);
    return 1;
}

int
parse_width(const char *arg, uint16_t *width)
{
    char *end;
    static int set = 0;
    if (set == 1) {
        return -1;
    }
    long value = strtoul(arg, &end, 10);
    if (end == arg || *end != '\0' || value == 0 || value > UINT16_MAX) {
        return 0;
    }
    *width = (uint16_t) value;
    set = 1;
    return 1;
}

int
get_running_dir(struct runningdir *runningdir)
{
    int dirname_length;
    int length = wai_getExecutablePath(NULL, 0, NULL);
    runningdir->dirpath = (char*)malloc(length);
    if (runningdir->dirpath == NULL) {
        print_oom();
        return 0;
    }

    wai_getExecutablePath(runningdir->dirpath, length, &dirname_length);
    runningdir->dirlen = dirname_length;
    runningdir->dirpath = realloc(runningdir->dirpath, dirname_length + 2);
    if (runningdir->dirpath == NULL) {
        print_oom();
        return 0;
    }
    runningdir->dirpath[dirname_length + 1] = '\0';

    return 1;
}

int
parse_config(char *dirpath, struct config *cfg)
{
    FILE* cfgfile;

    int cfgpathlength = strlen(dirpath) + CONFIGTXT_LEN + 1;
    char cfgpath[cfgpathlength];
    snprintf(cfgpath, cfgpathlength, "%sconfig.txt\0", dirpath);

    if ((cfgfile = fopen(cfgpath, "r")) == NULL) {
        fprintf(stderr,
                "Unable to open cfg file.\n");
        return 0;
    }

    char linebuf[MAX_CFG_LINE_SIZE];
    char scanbuf[MAX_CFG_LINE_SIZE];
    int linenum = 0;
    int errcnt = 0;

    int api_key_set = (cfg->api_key[0] != '\0' ? 1 : 0);
    int steam_id_set = (cfg->steam_id[0] != '\0' ? 1 : 0);

    while (fgets(linebuf, sizeof(linebuf), cfgfile) != NULL) {
        linenum++;
        if (sscanf(linebuf, " %s", scanbuf) == EOF) {
            continue;
        }
        if (sscanf(linebuf, " %[#]", scanbuf) == 1) {
            continue;
        }

        if (sscanf(linebuf, " " OPT_API_KEY " = %s", scanbuf) == 1) {
            if (api_key_set) {
                continue;
            }
            if(parse_api_key(scanbuf, &cfg->api_key[0]) == 0){
                fprintf(stderr,
                        "Error in %s\n"
                        "Line %d: invalid " OPT_API_KEY
                        " provided in config\n", cfgpath, linenum);
                return -1;
            }
            continue;
        }
        if (sscanf(linebuf, " " OPT_STEAM_ID " = %s", scanbuf) == 1) {
            if (steam_id_set) {
                continue;
            }
            if(parse_steam_id(scanbuf, &cfg->steam_id[0]) == 0){
                fprintf(stderr,
                        "Error in %s\n"
                        "Line %d: invalid " OPT_STEAM_ID
                        " provided in config\n", cfgpath, linenum);
                return -1;
            }
            continue;
        }
        if (sscanf(linebuf, " " OPT_WIDTH " = %s", scanbuf) == 1) {
            if(parse_width(scanbuf, &cfg->width) == 0){
                fprintf(stderr,
                        "Error in %s\n"
                        "Line %d: invalid " OPT_WIDTH
                        " provided in config\n", cfgpath, linenum);
                return -1;
            }
            continue;
        }
        fprintf(stderr,
                "Invalid line in config file: %d\n",
                linenum);
        return -1;
    }

    fclose(cfgfile);

    return 1;
}
