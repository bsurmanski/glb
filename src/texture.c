/**
 * texture.c
 * @file texture.h
 * GLB
 * @date March 01, 2013
 * @author Brandon Surmanski
 *
 * @brief definition of the GLBTexture object interface
 */

#include "glb_private.h"

#include "tga.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @private
 * Represents the formatting of a GL texture object
 */
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

static int glbTextureDimensions(GLBTexture *texture)
{
    if(texture->z > 1)
    {
        return 3;
    }
    if(texture->y > 1)
    {
        return 2;
    }
    return 1; //image has to be at least 1D
}

/**
 * creates a new texture object.
 *
 * @param flags TODO
 * @param format the texture format of the newly created image. The passed pointer 
 * should also be in a compatible format. 
 * @param x the width of the new texture
 * @param y the height of the new texture. Must be 1 for 1D textures, and must be
 * greater than one for 2D textures.
 * @param z the depth of the new texture. This value is 1 for non-3D textures and
 * greater than one for 3D textures.
 * @param 'ptr' may be a null pointer. In this case, texture memory is allocated 
 * to accommodate a texture of correct size. The image is undefined if
 * the user tries to apply an uninitialized portion of the texture to a primative.
 * @param errcode_ret an optional pointer used to return any error codes. errcode_ret
 * will be set to GLB_SUCCESS (0) if the operation was successful
 */
GLBTexture* glbCreateTexture (enum GLBAccess flags,
                              enum GLBImageFormat format,
                              int x,
                              int y,
                              int z,
                              void *ptr,
                              int *errcode_ret)
{
    int errcode;

    GLBTexture *texture = malloc(sizeof(GLBTexture));
    GLB_ASSERT(texture, GLB_OUT_OF_MEMORY,  ERROR);
    if(x < 1) x = 1;
    if(y < 1) y = 1;
    if(z < 1) z = 1;
    glGenTextures(1, &texture->globj);
    texture->refcount = 1;
    texture->x = x;
    texture->y = y;
    texture->z = z;
    texture->format = format;
    texture->sampler = NULL;
    texture->size = x * y * z * FORMAT[format].depth; //TODO: assert format is correct

    switch(glbTextureDimensions(texture))
    {

        case 3:
            texture->target = GL_TEXTURE_3D;
            glBindTexture(GL_TEXTURE_3D, texture->globj);
            glTexImage3D(GL_TEXTURE_3D, 0, FORMAT[format].internalFormat,
                         x, y, z, 0, FORMAT[format].format, FORMAT[format].type, ptr);
            break;
        case 2:
            texture->target = GL_TEXTURE_2D;
            glBindTexture(GL_TEXTURE_2D, texture->globj);
            glTexImage2D(GL_TEXTURE_2D, 0, FORMAT[format].internalFormat,
                         x, y, 0, FORMAT[format].format, FORMAT[format].type, ptr);
            break;
        case 1:
            texture->target = GL_TEXTURE_1D;
            glBindTexture(GL_TEXTURE_1D, texture->globj);
            glTexImage1D(GL_TEXTURE_1D, 0, FORMAT[format].internalFormat,
                         x, 0, FORMAT[format].format, FORMAT[format].type, ptr);
            break;
        default: //error, should never reach here
            errcode = GLB_UNIMPLEMENTED;
            goto UNKNOWN_ERROR;
    }

    glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLB_SET_ERROR(GLB_SUCCESS);
    return texture;

UNKNOWN_ERROR:
    glDeleteTextures(1, &texture->globj);
    free(texture);
ERROR:
    GLB_SET_ERROR(errcode);
    return NULL;
}

GLBTexture* glbCreateTextureWithTGA (enum GLBAccess flags,
                                     const char *filenm,
                                     int *errcode_ret)
{
    int errcode;
    FILE *file = fopen(filenm, "rb");
    GLB_ASSERT(file, GLB_FILE_NOT_FOUND, ERROR);

    struct glbTGA_header header;
    errcode = glbTGA_header_read(file, &header);
    GLB_ASSERT(!errcode, GLB_READ_ERROR, ERROR_READ);

    void *buf = malloc(glbTGA_image_sz(&header));
    errcode = glbTGA_image_read(file, &header, buf);

    int depth = glbTGA_pxl_sz(&header);
    int format; 
    switch(depth)
    {
        case 4:
            format = GLB_RGBA;
            break;
        case 3:
            format = GLB_RGB;
            break;
        default:
            errcode = GLB_UNIMPLEMENTED;
            goto ERROR_IMG;
    }

    GLB_ASSERT(!errcode, GLB_UNIMPLEMENTED, ERROR_IMG); ///<TODO: non-rgb/rgba texture formats
    GLBTexture *texture = glbCreateTexture(0, format,
                          header.img.w, header.img.h, 1, buf, &errcode);
    free(buf);
    GLB_ASSERT(!errcode, errcode, ERROR_READ);
    fclose(file);

    GLB_SET_ERROR(GLB_SUCCESS);
    return texture;

ERROR_IMG:
    free(buf);
ERROR_READ:
    fclose(file);
ERROR:
    GLB_SET_ERROR(errcode);
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

int glbTextureGenerateMipmap(GLBTexture *texture)
{
    glBindTexture(texture->target, texture->globj);
    glGenerateMipmap(texture->target);
    if(!texture->sampler)
    {
        glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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

int glbFillTexture (GLBTexture *texture, int level,
                    int *origin, int *region, void *fill_color)
{
    int errcode = 0;
    int x = 1;
    int y = 1;
    int z = 1;
    size_t bufsz = texture->x * texture->y * texture->z * FORMAT[texture->format].depth;
    uint32_t *buf = malloc(bufsz);
    switch(glbTextureDimensions(texture))
    {
        case 3:
            z = region[2];
        case 2:
            y = region[1];
        case 1:
            x = region[0];
    }
    int i,j,k;
    for(k = 0; k < z; k++)
    {
        for(j = 0; j < y; j++)
        {
            for(i = 0; i < x; i++)
            {
                memcpy(&buf[((k + origin[2]) * texture->z) *
                            ((j + origin[1]) * texture->y) *
                            ((i + origin[0]) * texture->x)],
                        fill_color,
                        sizeof(uint32_t)); //TODO: non-rgba
            }
        }
    }
    errcode = glbWriteTexture(texture, level, origin, region, bufsz, buf);
    free(buf);
    return errcode;
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
