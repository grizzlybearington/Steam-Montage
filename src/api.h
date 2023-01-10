#ifndef SM_API_H
#define SM_API_H

#include <stdlib.h>

#include "command.h"
#include "cJSON/cJSON.h"

struct res_buffer {
    unsigned char* buffer;
    size_t size;
};

struct json_data {
    int games_count;
    cJSON* games_array;
};

int
init_curl();

int
free_curl();

int
get_game_header_data(struct res_buffer *api_res, int appid);

struct json_data
get_json_data(struct config *cfg);

#endif
