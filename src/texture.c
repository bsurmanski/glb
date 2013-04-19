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
    //TODO: for some reason, I require to specify BGRA instread of RGBA (endianess?)
    {4, GL_RGBA8,               GL_BGRA,            GL_UNSIGNED_BYTE},  // RGBA
    {3, GL_RGB8,                GL_BGR,             GL_UNSIGNED_BYTE},  // RGB
    {4, GL_DEPTH_COMPONENT32,   GL_DEPTH_COMPONENT, GL_FLOAT},          // DEPTH
    {1, GL_R8,                  GL_RED,             GL_FLOAT},          // STENCIL
    {4, GL_DEPTH24_STENCIL8,    GL_DEPTH_STENCIL,   GL_FLOAT},          // DEPTH-STENCIL
    {1, GL_R8UI,                GL_RED,             GL_UNSIGNED_BYTE},  // BYTE
    {2, GL_RG16UI,              GL_RG_INTEGER,      GL_UNSIGNED_SHORT}, // SHORT
    {4, GL_R32UI,               GL_RED_INTEGER,     GL_UNSIGNED_INT},   // INT
    {4, GL_RG16UI,              GL_RG_INTEGER,      GL_UNSIGNED_SHORT},   // INT-INT
};

static int glbTextureDimensions(GLBTexture *texture)
{
    if(texture->dim[2] > 1)
    {
        return 3;
    }
    if(texture->dim[1] > 1)
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
GLBTexture* glbCreateTexture (int flags,
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
    texture->dim[0] = x;
    texture->dim[1] = y;
    texture->dim[2] = z;
    texture->format = format;
    texture->sampler = NULL;
    texture->size = x * y * z * FORMAT[format].depth; //TODO: assert format is correct

    switch(glbTextureDimensions(texture))
    {

        case 3:
            if(flags & GLB_TEXTURE_ARRAY)
            {
                texture->target = GL_TEXTURE_2D_ARRAY;
            } else
            {
                texture->target = GL_TEXTURE_3D;
            }

            glBindTexture(texture->target, texture->globj);
            glTexImage3D(texture->target, 0, FORMAT[format].internalFormat,
                         x, y, z, 0, FORMAT[format].format, FORMAT[format].type, ptr);
            break;
        case 2:
            if(flags & GLB_TEXTURE_ARRAY)
            {
                texture->target = GL_TEXTURE_1D_ARRAY;
            } else
            {
                texture->target = GL_TEXTURE_2D;
            }

            glBindTexture(texture->target, texture->globj);
            glTexImage2D(texture->target, 0, FORMAT[format].internalFormat,
                         x, y, 0, FORMAT[format].format, FORMAT[format].type, ptr);
            break;
        case 1:
            texture->target = GL_TEXTURE_1D;
            glBindTexture(texture->target, texture->globj);
            glTexImage1D(texture->target, 0, FORMAT[format].internalFormat,
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

GLBTexture* glbCreateTextureWithTGA (enum GLBTextureFlags flags,
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
    fclose(file);

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
    GLB_ASSERT(!errcode, errcode, ERROR);

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
    if(!texture) GLB_RETURN_ERROR(GLB_INVALID_ARGUMENT);

    glDeleteTextures(1, &texture->globj);
    return 0;
}

int glbRetainTexture(GLBTexture *texture)
{
    if(!texture) GLB_RETURN_ERROR(GLB_INVALID_ARGUMENT);

    texture->refcount++;
    return 0;
}

int glbReleaseTexture(GLBTexture *texture)
{
    if(!texture) GLB_RETURN_ERROR(GLB_SUCCESS);

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
    if(!texture) GLB_RETURN_ERROR(GLB_INVALID_ARGUMENT);

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

int glbFillTexture (GLBTexture *texture, int level, int *origin, int *region, 
                                enum GLBImageFormat fillfmt, void *fill_color)
{
    int errcode = 0;
    int x = 1;
    int y = 1;
    int z = 1;
    size_t bufsz = texture->dim[0] * texture->dim[1] * texture->dim[2] * 
                   FORMAT[fillfmt].depth;
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
                memcpy(&buf[((k + origin[2]) * texture->dim[1] * texture->dim[0]) +
                            ((j + origin[1]) * texture->dim[0]) +
                            ((i + origin[0]))],
                        fill_color,
                        sizeof(uint32_t)); //TODO: non-rgba
            }
        }
    }
    errcode = glbWriteTexture(texture, level, origin, region, fillfmt, bufsz, buf);
    free(buf);
    GLB_RETURN_ERROR(errcode);
}

//TODO: is size required?
int glbWriteTexture (GLBTexture *texture, int level, int *origin, int *region, 
                            enum GLBImageFormat writefmt, int size, void *ptr)
{
    if(!texture || !ptr) GLB_RETURN_ERROR(GLB_INVALID_ARGUMENT);

    glBindTexture(texture->target, texture->globj);

    struct GLBTextureFormat *format = &FORMAT[writefmt];
    if(format->depth * region[0] * region[1] > size) GLB_RETURN_ERROR(GLB_INVALID_ARGUMENT);

    switch(texture->target)
    {
        case GL_TEXTURE_1D:
            glTexSubImage1D(texture->target, level, origin[0],
                            region[0], format->format, format->type, ptr);
            return 0;
        case GL_TEXTURE_2D:
        case GL_TEXTURE_1D_ARRAY:
            glTexSubImage2D(texture->target, level, origin[0], origin[1],
                            region[0], region[1], format->format, format->type, ptr);
            return 0;

        case GL_TEXTURE_3D:
        case GL_TEXTURE_2D_ARRAY:
            glTexSubImage3D(texture->target, level, origin[0], origin[1], origin[2],
                             region[0], region[1], region[2], 
                             format->format, format->type, ptr);
            return 0;
        default:
            GLB_RETURN_ERROR(GLB_UNIMPLEMENTED);
    }
}

int glbWriteTextureWithTGA(GLBTexture *texture, int level, int *origin, int *region,
                           const char *filenm)
{
    int errcode;
    FILE *file = fopen(filenm, "rb");
    GLB_ASSERT(file, GLB_FILE_NOT_FOUND, ERROR);

    struct glbTGA_header header;
    errcode = glbTGA_header_read(file, &header);
    GLB_ASSERT(!errcode, GLB_READ_ERROR, ERROR_READ);

    size_t bufsz = glbTGA_image_sz(&header);
    void *buf = malloc(bufsz);
    errcode = glbTGA_image_read(file, &header, buf);
    fclose(file);

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

    errcode = glbWriteTexture(texture, level, origin, region, format, bufsz, buf);
    free(buf);
    GLB_RETURN_ERROR(errcode);

ERROR_IMG:
    free(buf);
ERROR_READ:
    fclose(file);
ERROR:
    GLB_RETURN_ERROR(errcode);
}

int glbReadTexture (GLBTexture *texture, int level, const int *const origin, 
                            int const *const region, 
                            enum GLBImageFormat readfmt, int size, void *ptr)
{
    if(!texture || !ptr) GLB_RETURN_ERROR(GLB_INVALID_ARGUMENT);

    glBindTexture(texture->target, texture->globj);

    int x = 0;
    int y = 0;
    int z = 0;
    int rx = 1;
    int ry = 1;
    int rz = 1;
    int levelw = 1, levelh = 1, leveld = 1;
    switch(glbTextureDimensions(texture))
    {
        case 3:
            rz = region[2];
            z = origin[2];
            glGetTexLevelParameteriv(texture->target, level, GL_TEXTURE_DEPTH, &leveld);
        case 2:
            ry = region[1];
            y = origin[1];
            glGetTexLevelParameteriv(texture->target, level, GL_TEXTURE_HEIGHT, &levelh);
        case 1:
            rx = region[0];
            x = origin[0];
            glGetTexLevelParameteriv(texture->target, level, GL_TEXTURE_WIDTH, &levelw);
    }

    struct GLBTextureFormat *format = &FORMAT[readfmt];
    if(size && size < rx * ry * rz * format->depth) GLB_RETURN_ERROR(GLB_INVALID_ARGUMENT);

    uint8_t *readbuf = ptr;

    // region is not whole image

    if(x != 0 || y != 0 || z != 0 ||
       rx != levelw || ry != levelh || rz != leveld)
    {
        readbuf = malloc(format->depth * levelw * levelh * leveld);
    }

    // kind of silly how they dont have glGetTexSubImage
    // and how it is inconsistant with glTexSubImage2D in that there is no
    // glGetTexImage{1D,2D,3D}
    glGetTexImage(texture->target, level, format->format, format->type, readbuf);

    if(readbuf != ptr)
    {
        int i,j,k;

        for(k = 0; k < rz; k++)
        {
            for(j = 0; j < ry; j++)
            {
                for(i = 0; i < rx; i++)
                {
                    memcpy(&(((char*)ptr)[((k + z) * ry * rx) +
                            ((j + y) * rx) +
                            ((i + x))]), 
                            &readbuf[((k + z) * texture->dim[1] * texture->dim[0]) +
                            ((j + y) * texture->dim[0]) +
                            ((i + x))], 
                            format->depth);
                }
            }
        }

        /*
        int i;
        for(i = 0; i < ry; i++) //copy each row to ptr
        {
            memcpy(((char*)ptr) + (i * region[0]) * format->depth,
                   ((char*)readbuf) + (origin[0] + (origin[1] + i) * region[0]) * format->depth,
                    region[0] * format->depth);
        }*/
        free(readbuf);
    }
    return 0;
}

int glbCopyTexture (GLBTexture *src, GLBTexture *dst, 
                    int srclvl, int dstlvl,
                    int *srcorigin, int *dstorigin,
                    int *region)
{
    if(!src || !dst) return GLB_INVALID_ARGUMENT;
    int errcode = 0;
    //struct GLBTextureFormat *srcfmt = &FORMAT[src->format];
    struct GLBTextureFormat *dstfmt = &FORMAT[dst->format];
    size_t sz = dstfmt->depth;
    switch(glbTextureDimensions(src))
    {
        default:
            sz = 0;
            break;
        case 3:
            sz *= region[2];
        case 2:
            sz *= region[1];
        case 1:
            sz *= region[0];
        
    }
    if(!sz) GLB_RETURN_ERROR(GLB_INVALID_ARGUMENT);

    void *data = malloc(sz);
    

    errcode = glbReadTexture(src, srclvl, srcorigin, region, dst->format, sz, data);
    GLB_ASSERT(!errcode, GLB_READ_ERROR, ERROR_READ);
    errcode = glbWriteTexture(dst, dstlvl, dstorigin, region, dst->format, sz, data);
    GLB_ASSERT(!errcode, GLB_WRITE_ERROR, ERROR_READ);
    //TODO deal with different formats between src and dst

ERROR_READ:
    free(data);
    GLB_RETURN_ERROR(errcode);
}

const int *const glbTextureSize (GLBTexture *texture)
{
    return texture->dim;
}
