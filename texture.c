/**
 * texture.c
 * GLB
 * March 01, 2013
 * Brandon Surmanski
 */

#include "glb.h"

#include <stdlib.h>
#include <string.h>

struct GLBTextureFormat
{
    uint8_t depth;
    uint16_t internalFormat;
    uint16_t format;
    uint16_t type;
};

static struct GLBTextureFormat FORMAT[] = 
{
    {4, GL_RGBA8,               GL_RGBA,            GL_UNSIGNED_BYTE},  // RGBA
    {3, GL_RGB8,                GL_RGB,             GL_UNSIGNED_BYTE},  // RGB
    {4, GL_DEPTH_COMPONENT32,   GL_DEPTH_COMPONENT, GL_FLOAT},          // DEPTH
    {1, GL_R8,                  GL_RED,             GL_FLOAT},          // STENCIL
    {4, GL_DEPTH24_STENCIL8,    GL_DEPTH_STENCIL,   GL_FLOAT},          // DEPTH-STENCIL
    {1, GL_R8UI,                GL_RED,             GL_UNSIGNED_BYTE},  // BYTE 
    {2, GL_RG16UI,              GL_RG_INTEGER,      GL_UNSIGNED_SHORT}, // SHORT
    {4, GL_R32UI,               GL_RED_INTEGER,     GL_UNSIGNED_INT},   // INT
};

GLBTexture* glbCreateTexture (enum GLBAccess flags, 
                              enum GLBImageFormat format,
                              int x,
                              int y,
                              int z,
                              void *ptr,
                              int *errcode_ret)
{
    int errcode = 0;

    GLBTexture *texture = malloc(sizeof(GLBTexture));
    GLB_ASSERT(texture, GLB_OUT_OF_MEMORY,  ERROR); 
    texture->refcount = 1;
    texture->globj = 0;
    return texture;

ERROR:
    if(errcode_ret)
    {
        *errcode_ret = errcode;
    }
    return NULL;
}

int glbDeleteTexture (GLBTexture *texture)
{
    if(!texture) return GLB_INVALID_ARGUMENT;

    glDeleteTextures(1, &texture->globj);
    return 0;
}

int glbRetainTexture(GLBTexture *texture)
{
    if(!texture) return GLB_INVALID_ARGUMENT;

    texture->refcount++;
    return 0;
}

int glbReleaseTexture(GLBTexture *texture)
{
    if(!texture) return GLB_INVALID_ARGUMENT;

    texture->refcount--;
    if(texture->refcount <= 0)
    {
        glbDeleteTexture(texture);
    }
    return 0;
}

int glbTextureSampler (GLBTexture *texture, struct GLBSampler *sampler)
{
    if(!texture) return GLB_INVALID_ARGUMENT; 

    if(texture->sampler)
    {
        glbReleaseSampler(texture->sampler);
    }
        glbRetainSampler(sampler);
        texture->sampler = sampler;

        glBindTexture(texture->target, texture->globj);
        glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, sampler->minfilter);
        glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, sampler->magfilter);
        glTexParameteri(texture->target, GL_TEXTURE_WRAP_S, sampler->wrap_s);
        glTexParameteri(texture->target, GL_TEXTURE_WRAP_T, sampler->wrap_t);
        glTexParameteri(texture->target, GL_TEXTURE_WRAP_R, sampler->wrap_r);
        glTexParameterf(texture->target, GL_TEXTURE_MIN_LOD, sampler->minlod);
        glTexParameterf(texture->target, GL_TEXTURE_MAX_LOD, sampler->maxlod);
        //TODO: other params
        glBindTexture(texture->target, 0);

    return 0;
}

int glbFillTexture (GLBTexture *texture, void *fill_color,
                    int *origin, int *region)
{
    return GLB_UNIMPLEMENTED;
}

//TODO: is size required?
int glbWriteTexture(GLBTexture *texture, 
                    int level, int *origin, 
                    int *region, int size, void *ptr)
{
    if(!texture || !ptr) return GLB_INVALID_ARGUMENT;

    glBindTexture(texture->target, texture->globj);

    struct GLBTextureFormat *format = &FORMAT[texture->format];
    if(format->depth * region[0] * region[1] < size) return GLB_INVALID_ARGUMENT;

    switch(texture->target)
    {
        case GL_TEXTURE_2D:
            glTexSubImage2D(GL_TEXTURE_2D, level, origin[0], origin[1],
                            region[0], region[1], format->format, format->type, ptr);
            return 0;

        case GL_TEXTURE_1D:
        case GL_TEXTURE_3D:
        default:
            return GLB_UNIMPLEMENTED;
    }
}

int glbReadTexture (GLBTexture *texture, int level, int *origin, int *region, 
                    int size, void *ptr)
{
    if(!texture || !ptr) return GLB_INVALID_ARGUMENT;

    struct GLBTextureFormat *format = &FORMAT[texture->format];
    if(size < region[0] * region[1] * format->depth) return GLB_INVALID_ARGUMENT;

    void *readbuf = ptr;
    int levelw, levelh;
    glBindTexture(texture->target, texture->globj);
    glGetTexLevelParameteriv(texture->target, level, GL_TEXTURE_WIDTH, &levelw);
    glGetTexLevelParameteriv(texture->target, level, GL_TEXTURE_WIDTH, &levelh);

    // region is not whole image
    if(origin[0] != 0 || origin[1] != 0 ||
       region[0] != levelw || region[1] != levelh)
    {
        readbuf = malloc(format->depth * levelw * levelh); 
    }

    // kind of silly how they dont have glGetTexSubImage
    // and how it is inconsistant with glTexSubImage2D in that there is no
    // glGetTexImage{1D,2D,3D}
    glGetTexImage(texture->target, level, format->format, format->type, readbuf);

    if(readbuf != ptr)
    {
        int i;
        for(i = 0; i < region[1]; i++) //copy each row to ptr
        {
            memcpy(((char*)ptr) + (i * region[0]) * format->depth,
                   ((char*)readbuf) + (origin[0] + (origin[1] + i) * region[0]) * format->depth,
                    region[0] * format->depth);
        }
        free(readbuf);
    }
    return 0;
}
