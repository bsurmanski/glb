/*
 * texture.h
 * GLB
 * March 01, 2013
 * Brandon Surmanski
 */

#ifndef _GLB_TEXTURE_H
#define _GLB_TEXTURE_H

#include "glb_types.h"

/*
enum GLBAccess
{
    GLB_READ_ONLY = GL_READ_ONLY,
    GLB_WRITE_ONLY = GL_WRITE_ONLY,
    GLB_READ_WRITE = GL_READ_WRITE,
};*/

enum GLBImageFormat
{
    GLB_RGBA            = 0,
    GLB_RGB             = 1,
    GLB_DEPTH           = 2,
    GLB_STENCIL         = 3,
    GLB_DEPTH_STENCIL   = 4,
    GLB_INT8            = 5,
    GLB_INT16           = 6,
    GLB_INT32           = 7,
    GLB_2INT16          = 8,
};

enum GLBTextureFlags
{
    GLB_READ_ONLY     = 1,
    GLB_WRITE_ONLY    = 2,
    GLB_READ_WRITE    = 3,
    GLB_TEXTURE_ARRAY = 4,  
};

GLBTexture*  glbCreateTexture  (int flags,
                                enum GLBImageFormat format,
                                int x,
                                int y,
                                int z,
                                void *ptr,
                                int *errcode_ret);

GLBTexture*  glbCreateTextureWithTGA (enum GLBTextureFlags flags,
                                      const char *filenm,
                                      int *errcode_ret);

int          glbDeleteTexture  (GLBTexture *texture);
int          glbRetainTexture  (GLBTexture *texture);
int          glbReleaseTexture (GLBTexture *texture);
int          glbTextureGenerateMipmap(GLBTexture *texture);
int          glbTextureSampler (GLBTexture *texture, GLBSampler *sampler);

int          glbFillTexture    (GLBTexture *texture, int level, int *origin, int *region, 
                                enum GLBImageFormat fillfmt, void *fill_color);

int          glbWriteTexture   (GLBTexture *texture, int level, int *origin, int *region, 
                                enum GLBImageFormat writefmt, int size, void *ptr);

int          glbWriteTextureWithTGA(GLBTexture *texture, int level, int *origin, int *region,
                                const char *filenm);

int          glbReadTexture    (GLBTexture *texture, int level, const int * const origin, 
                                const int *const region, 
                                enum GLBImageFormat readfmt, int size, void *ptr);

int          glbCopyTexture    (GLBTexture *src, GLBTexture *dst, 
                                int srclvl, int dstlvl,
                                int *srcorigin, int *dstorigin,
                                int *region);
                                

const int *const glbTextureSize (GLBTexture *texture);

#endif
