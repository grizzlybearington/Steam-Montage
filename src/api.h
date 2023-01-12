/* See LICENSE file for copyright & license details. */
#ifndef SM_API_H
#define SM_API_H

#include <stdlib.h>

#include "command.h"
#include "cJSON/cJSON.h"

struct res_buffer {
    unsigned char* buffer;
    size_t size;
};

typedef struct {
    int games_count;
    cJSON* games_array;
} json_data;

int init_curl();

int free_curl();

int get_game_header_data(struct res_buffer *api_res, int appid);

json_data get_json_data(struct config *cfg);

#endif
