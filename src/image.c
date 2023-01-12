/* See LICENSE file for copyright & license details. */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <jpeglib.h>
#include <jerror.h>

#include "api.h"
#include "config.h"
#include "image.h"
#include "steammontage.h"

#define FIL_NAM "montage.jpg"
#define FIL_NAM_LEN 12

int
cmyk_into_rgb (unsigned char *cmyk_data, unsigned char *rgb_data, long pixel_count)
{
    unsigned char *src = cmyk_data;
    unsigned char *dest = rgb_data;

    while (pixel_count--) {
        unsigned char c = src[0];
        unsigned char m = src[1];
        unsigned char y = src[2];
        unsigned char k = src[3];

        dest[0] = (c * k) / 255;
        dest[1] = (m * k) / 255;
        dest[2] = (y * k) / 255;

        src += 4;
        dest += 3;
    }
    return 1;
}

int get_raw_img_data
(img_data *cur_hdr_img, struct res_buffer *api_res)
{
    struct jpeg_decompress_struct hdr_info;
    struct jpeg_error_mgr err;
    int retval = 0;
    unsigned char* raw_row_buffer[1];

    hdr_info.err = jpeg_std_error(&err);
    jpeg_create_decompress(&hdr_info);

    jpeg_mem_src(&hdr_info, api_res->buffer, api_res->size);
    jpeg_read_header(&hdr_info, TRUE);

    if (hdr_info.image_width != HDR_WIDTH
            || hdr_info.image_height != HDR_HEIGHT) {
        fprintf(stderr, "header image not the correct dimensions. "
                        "(width: %d, height: %d) skipping...\n",
                        hdr_info.image_width, hdr_info.image_height);
        goto end;
    }

    cur_hdr_img->num_components = hdr_info.num_components;

    jpeg_start_decompress(&hdr_info);

    if (hdr_info.out_color_space == JCS_CMYK) {
        long pixel_count = (hdr_info.output_width*
                            hdr_info.output_height);
        unsigned char cmyk_buffer [HDR_WIDTH * HDR_HEIGHT * 4];

        while (hdr_info.output_scanline < hdr_info.output_height) {
                raw_row_buffer[0] = (unsigned char*)
                    (&cmyk_buffer[4 *hdr_info.output_width*
                        hdr_info.output_scanline]);
            jpeg_read_scanlines(&hdr_info, raw_row_buffer, 1);
        }
        cmyk_into_rgb(cmyk_buffer, cur_hdr_img->raw_data, pixel_count);
    } else {
        while (hdr_info.output_scanline < hdr_info.output_height) {
            raw_row_buffer[0] = (unsigned char*)
                        (&cur_hdr_img->raw_data[3 *hdr_info.output_width
                                            *hdr_info.output_scanline]);
            jpeg_read_scanlines(&hdr_info, raw_row_buffer, 1);
        }
    }

    jpeg_finish_decompress(&hdr_info);

    retval = 1;

end:
    jpeg_destroy_decompress(&hdr_info);
    return retval;
}

int create_montage(struct config *cfg, runningdir currdir)
{
    int retval = -1;
    int fullpathlen = currdir.dirlen + FIL_NAM_LEN;
    int mon_x, mon_y;
    int curcol = 0, currow = 0;
    cJSON *game = NULL;
    struct jpeg_compress_struct outinfo;
    struct jpeg_error_mgr err;
    struct res_buffer api_res;
    json_data json_info = get_json_data(cfg);
    img_data cur_hdr_img;
    FILE *outfile;
    char outfilepath[fullpathlen];
    unsigned char *row_buffer[1];
    unsigned char *montage_rgb = NULL;

    snprintf(outfilepath, fullpathlen + 1, "%s%s", currdir.dirpath,
                FIL_NAM);

    outfile = fopen(outfilepath, "wb");
    if (outfile == NULL) {
        fprintf(stderr,
                "ERROR: Unable to open output file\n");
        return retval;
    }

    if (json_info.games_array == NULL) {
        retval = -1;
        goto end;
    }

    outinfo.err = jpeg_std_error(&err);
    jpeg_create_compress(&outinfo);

    jpeg_stdio_dest(&outinfo, outfile);

    if (json_info.games_count > 0
            && cfg->width > json_info.games_count) {
        fprintf(stderr, "Width greater than amount of games!\n"
                "Setting width to %d\n", json_info.games_count);
        cfg->width = (uint16_t) json_info.games_count;
    }

    outinfo.image_width = HDR_WIDTH * cfg->width;
    outinfo.image_height = HDR_HEIGHT *
                    (json_info.games_count/cfg->width +
                            (json_info.games_count % cfg->width != 0));

    outinfo.input_components = 3;
    outinfo.in_color_space = JCS_RGB;

    montage_rgb = malloc(sizeof(unsigned char)* (outinfo.image_width *
            outinfo.image_height * 3));
    if (montage_rgb == NULL) {
        print_oom();
        goto end;
    }

    api_res.buffer = malloc(1);
    if (api_res.buffer == NULL) {
        print_oom();
        goto end;
    }
    cur_hdr_img.num_components = 0;

    fprintf(stderr,
            "Found %d games\n", json_info.games_count);

    cJSON_ArrayForEach(game, json_info.games_array) {
        cJSON* appid = cJSON_GetObjectItemCaseSensitive(game, "appid");
        int hdr_x, hdr_y;

        if (!cJSON_IsNumber(appid)) {
            fprintf(stderr,
                    "appid isn't a number? Skipping...\n");
            continue;
        }

        fprintf(stderr,
                "Adding app %d... ",
                appid->valueint);

        api_res.size = 0;

        if (get_game_header_data(&api_res, appid->valueint) == 0) {
            continue;
        }

        if (get_raw_img_data(&cur_hdr_img, &api_res) == 0) {
            continue;
        }

        for (hdr_y = 0, mon_y = HDR_HEIGHT * currow; hdr_y < HDR_HEIGHT;
                                                hdr_y++, mon_y++) {
            for (hdr_x = 0, mon_x = HDR_WIDTH * curcol; hdr_x < HDR_WIDTH;
                                                    hdr_x++, mon_x++) {

                montage_rgb[(mon_y*outinfo.image_width*3)+(mon_x*3)+0]
                    = cur_hdr_img.raw_data[(hdr_y*HDR_WIDTH*3)+(hdr_x*3)+0];

                montage_rgb[(mon_y*outinfo.image_width*3)+(mon_x*3)+1]
                    = cur_hdr_img.raw_data[(hdr_y*HDR_WIDTH*3)+(hdr_x*3)+1];

                montage_rgb[(mon_y*outinfo.image_width*3)+(mon_x*3)+2]
                    = cur_hdr_img.raw_data[(hdr_y*HDR_WIDTH*3)+(hdr_x*3)+2];
            }
        }

        fprintf(stderr,
                "success!\n");

        if (++curcol > cfg->width - 1) {
            curcol = 0, currow++;
        }
    }

    jpeg_set_defaults(&outinfo);
    jpeg_set_quality(&outinfo, 100, TRUE);

    jpeg_start_compress(&outinfo, TRUE);

    while (outinfo.next_scanline < outinfo.image_height) {
        row_buffer[0] = &(montage_rgb[outinfo.next_scanline
                            * (outinfo.image_width * 3)]);
        jpeg_write_scanlines(&outinfo, row_buffer, 1);
    }

    jpeg_finish_compress(&outinfo);

    retval = 1;

end:
    jpeg_destroy_compress(&outinfo);
    fclose(outfile);
    cJSON_Delete(json_info.games_array);
    free(api_res.buffer);
    free(json_info.games_array);
    free(montage_rgb);

    return retval;
}
