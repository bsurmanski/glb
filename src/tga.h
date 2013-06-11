/**
 * @internal
 * tga.h
 * obj
 * May 29, 2012
 * Brandon Surmanski
 * 
 * Private TGA loading header used by GLBTexture
 */

#ifndef _glbTGA_H
#define _glbTGA_H

#include <stddef.h>
#include <stdint.h>

///@private
typedef struct glbTGA_header {
    uint8_t id_len;
    uint8_t cmap_type;
    uint8_t img_type;

    //Colormap Specification
    struct glbTGA_header_cmap {
        uint16_t offset;
        uint16_t len;
        uint8_t entry_sz;
    } cmap;

    //Image specification
    struct glbTGA_header_img {
        uint16_t xorg;
        uint16_t yorg; //TODO: properly read bottom left origined image
        uint16_t w;
        uint16_t h;
        uint8_t depth;
        uint8_t discriptor;
    } img;
} glbTGA_header;

size_t glbTGA_pxl_sz(glbTGA_header *h);
size_t glbTGA_colormap_sz(glbTGA_header *h);
size_t glbTGA_image_sz(glbTGA_header *h);

int glbTGA_header_is_valid(glbTGA_header *h);

int glbTGA_header_read(FILE *fd, glbTGA_header *h);
int glbTGA_image_read(FILE *fd, glbTGA_header *h, void *buf);


#endif
