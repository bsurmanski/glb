/**
 * @internal
 * tga.c
 * @file    tga.h
 * obj
 * @date    May 29, 2012
 * @author  Brandon Surmanski
 *
 * @brief used internally for TGA loading
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tga.h"

#define CMAP_TRUE 1
#define CMAP_FALSE 0

#define ITYPE_NONE 0
#define ITYPE_UCCM 1
#define ITYPE_UCTC 2
#define ITYPE_UCBW 3
#define ITYPE_RLECM 9
#define ITYPE_RLETC 10
#define ITYPE_RLEBW 11

#define DISCR_ALPHA_MASK 0x0F
#define DISCR_DIREC_MASK 0x40

#define RLE_REPEAT_MASK 0x7F
#define RLE_FLAG_MASK 0x80

#define BUF_SZ 1024

#define IS_FULLCOLOR(x) ((x).img_type == 2)
#define HAS_CMAP(x) ((x).cmap_type == 1)
#define HAS_RLE(x) ((x).img_type >= 9 && (x).img_type <= 11)
#define NALPHA_BITS(x) ((x).img_discriptor & DISCR_ALPHA_MASK)

#define OFFSET(x,n) (((char*)(x)) + (int)(n))

/**
 * returns the size of a pixel in bytes
 */
size_t glbTGA_pxl_sz(glbTGA_header *h)
{
    return (h->img.depth /8); //+ NALPHA_BITS(*h)) / 8;
}

/**
 * gets the size of the tga colormap in bytes
 */
size_t glbTGA_colormap_sz(glbTGA_header *h)
{
   return h->cmap.len * glbTGA_pxl_sz(h);
}

/**
 * gets the size of the tga image in bytes
 */
size_t glbTGA_image_sz(glbTGA_header *h)
{
    return h->img.w * h->img.h * glbTGA_pxl_sz(h);
}

/**
 * determines whether a header contains valid values
 */
int glbTGA_header_is_valid(glbTGA_header *h)
{
    //validate cmap_type
    if(h->cmap_type != CMAP_TRUE && h->cmap_type != CMAP_FALSE)
        return false;

    //validate img_type
    if(h->img_type != ITYPE_NONE && h->img_type != ITYPE_UCCM && h->img_type != ITYPE_UCTC
            && h->img_type != ITYPE_UCBW && h->img_type != ITYPE_RLECM
            && h->img_type != ITYPE_RLETC && h->img_type != ITYPE_RLEBW)
        return false;
    return true;
}

/**
 * loads a header of the tga file discribed by 'fd' into the header *h
 */
int glbTGA_header_read(FILE *fd, glbTGA_header *h)
{
    if(!fd) {
        return -1;
    }

    fread(&h->id_len, sizeof(uint8_t), 1, fd);
    fread(&h->cmap_type, sizeof(uint8_t), 1, fd);
    fread(&h->img_type, sizeof(uint8_t), 1, fd);
    fread(&h->cmap, 5, 1, fd);
    fread(&h->img, 10, 1, fd);
    return !glbTGA_header_is_valid(h);
}

///@private
struct buffer_t {
    uint32_t current_index;
    uint32_t max;
    uint8_t *buffer;
};

/*
static void buffer_set(struct buffer_t *b, void *buffer, int sz)
{
    b->current_index = sz;
    b->max = sz;
    b->buffer = (uint8_t *) buffer;
}*/

// Note: this will not work if the TGA is embedded in another file
static void *buffer_read(struct buffer_t *b, FILE *fd, int n, void *dst)
{
    uint8_t *buf = b->buffer;
    uint32_t i = b->current_index;
    if(i + n >= b->max)
    {
        memmove(buf, &buf[i], b->max - i);
        fread(&buf[b->max - i], sizeof(uint8_t), i, fd);
        b->current_index = 0;
    }
    i = b->current_index;
    b->current_index += n;
    memcpy(dst, &b->buffer[i], n);
    return &b->buffer[i];
}

/**
 * reads the image data of a tga file into a buffer
 */
int glbTGA_image_read(FILE *fd, glbTGA_header *h, void *data)
{
    fseek(fd, h->id_len, SEEK_CUR); //ignore optional ID field

    if(!glbTGA_header_is_valid(h)){
        return -1; // invalid tga file
    }

    //unsigned int cm_sz = glbTGA_colormap_sz(h);
    //unsigned int img_sz = glbTGA_image_sz(h);
    if(HAS_CMAP(*h)){
        //assert(false && "TGA Color map not yet supported");
        return -1; ///< TODO: proper color map for tga
            //cmap = malloc(cm_sz);
            //read(fd, cmap, cm_sz);
    }

    uint8_t nrepeat = 0;
    uint8_t rle_flag = 0;
    uint8_t Bpp = glbTGA_pxl_sz(h);

    uint8_t buf[BUF_SZ];
    struct buffer_t buffer = {BUF_SZ, BUF_SZ, buf};

    int i, j;
    for(j = 0; j < h->img.h; j++){
        for(i = 0; i < h->img.w; i++, data = (void*)OFFSET(data,Bpp)){

            if(nrepeat == 0 && HAS_RLE(*h)){
                uint8_t rle_pkt;
                buffer_read(&buffer, fd, 1, &rle_pkt);
                rle_flag = rle_pkt & RLE_FLAG_MASK;
                nrepeat  = rle_pkt & RLE_REPEAT_MASK;
                ///<@TODO: include cmap indexing
                buffer_read(&buffer, fd, Bpp, data);
                continue;
            }
            if(rle_flag){
                memcpy(data, OFFSET(data, -Bpp), Bpp);
            } else {
                buffer_read(&buffer, fd, Bpp, data);
            }
            nrepeat--;
        }
    }
    return 0;
}
