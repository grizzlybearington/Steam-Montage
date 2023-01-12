/* See LICENSE file for copyright & license details. */
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

#include "api.h"

#define API_URL_SIZ 139
#define HEADER_URL_SIZ 70
#define HEADER_FILENAME_SIZ 22

#define JPEG_BYTE_1 255
#define JPEG_BYTE_2 216

static CURL *curl_handler;

static size_t
write_mem_callback (void *ptr, size_t size, size_t nmemb, void *data)
{
    size_t realsize = size * nmemb;
    struct res_buffer *api_res = (struct res_buffer *) data;

    api_res->buffer = realloc(api_res->buffer, api_res->size + realsize + 1);

    if (api_res->buffer) {
        memcpy(&(api_res->buffer[api_res->size]), ptr, realsize);
        api_res->size += realsize;
        api_res->buffer[api_res->size] = 0;
    }
    return realsize;
}

int
init_curl()
{
    curl_global_init(CURL_GLOBAL_ALL);
    curl_handler = NULL;
    curl_handler = curl_easy_init();

    if (!curl_handler) {
        fprintf(stderr,
            "Curl init failure\n");
        return -1;
    }
    curl_easy_setopt(curl_handler, CURLOPT_WRITEFUNCTION, write_mem_callback);

    return 1;
}

int
free_curl()
{
    curl_easy_cleanup(curl_handler);
    curl_global_cleanup();
    return 1;
}

int
get_game_header_data(struct res_buffer *api_res, int appid)
{
    CURLcode res;
    char url[HEADER_URL_SIZ];

    snprintf(url, HEADER_URL_SIZ,
            "http://cdn.akamai.steamstatic.com/steam/apps/%d/header.jpg",
            appid);

    curl_easy_setopt(curl_handler, CURLOPT_URL, url);
    curl_easy_setopt(curl_handler, CURLOPT_WRITEDATA, (void *)api_res);

    res = curl_easy_perform(curl_handler);

    if (res != CURLE_OK
        || api_res->buffer[0] != JPEG_BYTE_1
        || api_res->buffer[1] != JPEG_BYTE_2) {
        fprintf(stderr,
                "header could not be found or loaded. "
                "Skipping...\n",
                appid);
        return 0;
    }

    return 1;
}

json_data
get_json_data(struct config *cfg)
{
    struct res_buffer api_res;
    json_data res_data;
    CURLcode res;
    cJSON *full_json_res = NULL;
    cJSON *json_res;
    cJSON *game_count;
    cJSON *games = NULL;
    char url[API_URL_SIZ];

    snprintf(url, API_URL_SIZ,
            "http://api.steampowered.com/IPlayerService/GetOwnedGames/v0001"
            "?key=%s&steamid=%s&format=json",
            cfg->api_key, cfg->steam_id);

    curl_easy_setopt(curl_handler, CURLOPT_URL, url);
    curl_easy_setopt(curl_handler, CURLOPT_WRITEDATA, (void *)&api_res);

    res = curl_easy_perform(curl_handler);

    if (res != CURLE_OK) {
        fprintf(stderr,
                "Error fetching reponse from Steam API. "
                "Are you connected to the internet?\n"
                "curl: %s\n",
                curl_easy_strerror(res));
        goto end;
    }

    full_json_res = cJSON_Parse(api_res.buffer);
    if (full_json_res == NULL) {
        fprintf(stderr, "ERROR: Did not receive a valid JSON response. "
                    "Did you enter your api-key parameter correctly?\n");
        goto end;
    }

    json_res = cJSON_GetObjectItemCaseSensitive
                                (full_json_res, "response");
    res_data.games_array = cJSON_DetachItemFromObjectCaseSensitive
                                (json_res, "games");
    if (res_data.games_array == NULL) {
        fprintf(stderr, "Received an empty response from Steam API. "
                    "Check your steam-id parameter.\n"
                    "Does this profile exist? Is it private? "
                    "Did you hit the 100,000 request limit?\n");
        goto end;
    }

    game_count = cJSON_GetObjectItemCaseSensitive
                            (json_res, "game_count");

    if (!cJSON_IsNumber(game_count)) {
        fprintf(stderr,
                "Failed to retrieve game count from Steam's JSON "
                "response\n");
    } else {
        res_data.games_count = game_count->valueint;
    }

end:
    if (full_json_res) cJSON_Delete(full_json_res);
    if (api_res.buffer) free(api_res.buffer);
    return res_data;
}
