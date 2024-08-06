#ifndef _VIDEO_H
#define _VIDEO_H

#include <stddef.h>
#include <stdint.h>

struct video_info {
    uint64_t framebuffer_addr;
    uint32_t width;
    uint32_t height;
    uint8_t bpp;
};

void video_init();
void video_put_pixel();
void video_flush_buffer();

#endif
