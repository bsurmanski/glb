/*
 * texture.h
 * GLB
 * March 01, 2013
 * Brandon Surmanski
 */

#ifndef _GLB_TEXTURE_H
#define _GLB_TEXTURE_H

#include "glb_types.h"

enum GLBAccess
{
    GLB_READ_ONLY = GL_READ_ONLY,
    GLB_WRITE_ONLY = GL_WRITE_ONLY,
    GLB_READ_WRITE = GL_READ_WRITE,
};

enum GLBImageFormat
{
    GLB_RGBA            = 0,
    GLB_RGB             = 1,
    GLB_DEPTH           = 2,
    GLB_STENCIL         = 3,
    GLB_DEPTH_STENCIL   = 4,
    GLB_R_INT           = 5,
    GLB_RG_INT          = 6,
    GLB_RGBA_INT        = 7,
};

GLBTexture*  glbCreateTexture  (enum GLBAccess flags,
                                enum GLBImageFormat format,
                                int x,
                                int y,
                                int z,
                                void *ptr,
                                int *errcode_ret);

GLBTexture*  glbCreateTextureWithTGA (enum GLBAccess flags,
                                      const char *filenm,
                                      int *errcode_ret);

int          glbDeleteTexture  (GLBTexture *texture);
int          glbRetainTexture  (GLBTexture *texture);
int          glbReleaseTexture (GLBTexture *texture);
int          glbTextureGenerateMipmap(GLBTexture *texture);
int          glbTextureSampler (GLBTexture *texture, GLBSampler *sampler);

int          glbFillTexture    (GLBTexture *texture, int level,
                                int *origin, int *region, void *fill_color);

int          glbWriteTexture   (GLBTexture *texture,
                                int level, int *origin,
                                int *region, int size, void *ptr);

int          glbReadTexture    (GLBTexture *texture, int level, int *origin,
                                int *region, int size, void *ptr);

#endif
