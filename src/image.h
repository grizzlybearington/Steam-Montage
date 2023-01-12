/* See LICENSE file for copyright & license details. */
#ifndef SM_IMG_H
#define SM_IMG_H

#define HDR_WIDTH 460
#define HDR_HEIGHT 215

typedef struct {
    unsigned int num_components;
    unsigned char raw_data[(sizeof(unsigned char)) *
            HDR_WIDTH * HDR_HEIGHT * 3];
} img_data;

int create_montage(struct config *cfg, runningdir currdir);

#endif
