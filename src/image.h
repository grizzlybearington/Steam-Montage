#ifndef SM_IMG_H
#define SM_IMG_H

#define HDR_WIDTH 460
#define HDR_HEIGHT 215

struct img_data
{
    unsigned int num_components;
    unsigned char raw_data[(sizeof(unsigned char)) *
            HDR_WIDTH * HDR_HEIGHT * 3];
};

int
create_montage(struct config *cfg, struct runningdir runningdir);

#endif
