/**
 * framebuffer.c
 * @file framebuffer.h
 * GLB
 * @date March 18, 2013
 * @author Brandon Surmanski
 */

#include "glb_private.h"

#include <stdlib.h>

/**
 * creates a new GLBFramebuffer. All attachement points are created empty.
 *
 */
GLBFramebuffer *glbCreateFramebuffer(int *errcode_ret)
{
    int errcode;
    GLBFramebuffer *framebuffer = malloc(sizeof(GLBFramebuffer));
    GLB_ASSERT(framebuffer, GLB_OUT_OF_MEMORY, ERROR);

    framebuffer->ncolors = 0;
    framebuffer->refcount = 1;
    framebuffer->depth = NULL;
    framebuffer->stencil = NULL;
    framebuffer->colors = calloc(GLB_FRAMEBUFFER_COLORS_MAX, sizeof(GLBTexture*));
    glGenFramebuffers(1, &framebuffer->globj);

    GLB_SET_ERROR(GLB_SUCCESS);
    return framebuffer;

ERROR:
    GLB_SET_ERROR(errcode);
    return NULL;
}

void glbDeleteFramebuffer(GLBFramebuffer *framebuffer)
{
    if(!framebuffer) return;

    int i;
    for(i = 0; i < framebuffer->ncolors; i++)
    {
        glbReleaseTexture(framebuffer->colors[i]);
    }

    glbReleaseTexture(framebuffer->depth);

    if(framebuffer->depth != framebuffer->stencil)
    {
        glbReleaseTexture(framebuffer->stencil);
    }

    free(framebuffer);
}

void glbRetainFramebuffer(GLBFramebuffer *framebuffer)
{
    if(!framebuffer) return; //error
    framebuffer->refcount++;
}

void glbReleaseFramebuffer(GLBFramebuffer *framebuffer)
{
    if(!framebuffer) return; //error
    framebuffer->refcount--;
    if(framebuffer->refcount <= 0)
    {
        glbDeleteFramebuffer(framebuffer);
    }
}

/**
 * attaches a texture to this framebuffer. If the texture is a color texture,
 * it will be bound to the next availible texture unit. If the texture is a
 * depth texture, it will be bound to the depth unit. if the texture is stencil
 * formatted, it will be bound to the stencil unit. If the texture is depth-stencil
 * formatted, it will be bound to both the depth and stencil units.
 * If the texture supplied is a color texture and is already bound, this function
 * will have no effect.
 */
int glbFramebufferTexture(GLBFramebuffer *framebuffer, GLBTexture *texture)
{
    int errcode;
    GLB_ASSERT(framebuffer && texture, GLB_INVALID_ARGUMENT, ERROR);

    int i;
    switch(texture->format)
    {
        case GLB_RGBA:
        case GLB_RGB:
        case GLB_INT8:
        case GLB_INT16:
        case GLB_INT32:
            for(i = 0; i < framebuffer->ncolors; i++)
            {
                if(framebuffer->colors[i] == texture)
                {
                    return GLB_SUCCESS; // color already attached
                }
            }

            return glbFramebufferColor(framebuffer, framebuffer->ncolors, texture);
        case GLB_DEPTH:
            return glbFramebufferDepth(framebuffer, texture);
        case GLB_STENCIL:
            return glbFramebufferStencil(framebuffer, texture);
        case GLB_DEPTH_STENCIL:
            return glbFramebufferDepthStencil(framebuffer, texture);
        default:
            return GLB_INVALID_ARGUMENT;
    }

ERROR:
    return errcode;
}

static int glbFramebufferAttachment(GLBFramebuffer *framebuffer,
                                        GLenum attachment, GLBTexture *texture)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->globj);
    switch(texture->target)
    {
        case GL_TEXTURE_1D:
            glFramebufferTexture1D(GL_FRAMEBUFFER, attachment,
                                   GL_TEXTURE_1D, texture->globj, 0);
            break;
        case GL_TEXTURE_2D:
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment,
                                   GL_TEXTURE_2D, texture->globj, 0);
            break;
        case GL_TEXTURE_3D:
            //TODO: allow user to select the layer to use
            glFramebufferTexture3D(GL_FRAMEBUFFER, attachment,
                                   GL_TEXTURE_3D, texture->globj, 0, 0);
            break;
    }
    framebuffer->status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return GLB_SUCCESS;
}

/**
 * attaches a color texture to the framebuffer at a given texture unit.
 * The texture format must be one of the following:
 * GLB_RGBA, GLB_RGB, GLB_R_INT, GLB_RG_INT, GLB_RGBA_INT.
 * If a texture is currently bound to the given texture unit, it will be unbound
 * and released.
 */
int glbFramebufferColor(GLBFramebuffer *framebuffer, int i, GLBTexture *texture)
{
    if(i > GLB_FRAMEBUFFER_COLORS_MAX)
    {
        return GLB_INVALID_ARGUMENT;
    }

    glbRetainTexture(texture);
    glbReleaseTexture(framebuffer->colors[i]);
    framebuffer->colors[i] = texture;

    return glbFramebufferAttachment(framebuffer, GL_COLOR_ATTACHMENT0 + i, texture);
}

/**
 * attaches a depth texture to the framebuffer. The texture format *must* be GLB_DEPTH
 * or GLB_DEPTH_STENCIL. This action will unbind and release the currently bound depth
 * texture.
 */
int glbFramebufferDepth(GLBFramebuffer *framebuffer, GLBTexture *depth)
{
    if(depth != framebuffer->stencil)
    {
        glbRetainTexture(depth);
    }

    glbReleaseTexture(framebuffer->depth);
    if(framebuffer->depth == framebuffer->stencil)
    {
        framebuffer->stencil = NULL;
    }
    framebuffer->depth = depth;

    return glbFramebufferAttachment(framebuffer, GL_DEPTH_ATTACHMENT, depth);
}

/**
 * attaches a stencil texture to the framebuffer. The texture format *must* be GLB_STENCIL
 * or GLB_DEPTH_STENCIL. This action will unbind and release the currently bound stencil
 * texture.
 */
int glbFramebufferStencil(GLBFramebuffer *framebuffer, GLBTexture *stencil)
{
    if(stencil != framebuffer->depth)
    {
        glbRetainTexture(stencil);
    }

    glbReleaseTexture(framebuffer->stencil);
    if(framebuffer->depth == framebuffer->stencil)
    {
        framebuffer->depth = NULL;
    }
    framebuffer->stencil = stencil;

    return glbFramebufferAttachment(framebuffer, GL_STENCIL_ATTACHMENT, stencil);
}

/**
 * attaches a depth-stencil texture to the framebuffer. The texture format *must* be
 * GLB_DEPTH_STENCIL. This action will unbind both the depth and stencil textures currently
 * bound, and release both of them.
 */
int glbFramebufferDepthStencil(GLBFramebuffer *framebuffer, GLBTexture *depth_stencil)
{
    glbRetainTexture(depth_stencil);
    glbReleaseTexture(framebuffer->depth);
    if(framebuffer->stencil != framebuffer->depth)
    {
        glbReleaseTexture(framebuffer->stencil);
    }
    framebuffer->depth = depth_stencil;
    framebuffer->stencil = depth_stencil;

    return glbFramebufferAttachment(framebuffer, GL_DEPTH_STENCIL_ATTACHMENT, depth_stencil);
}

void glbFramebufferClear(GLBFramebuffer *framebuffer)
{
    if(framebuffer) 
    { 
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->globj);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void glbFramebufferReadColor(GLBFramebuffer *framebuffer, int i, 
                             int *origin, int *region, void *dst)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->globj);
    glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
    glReadPixels(origin[0], origin[1], region[0], region[1], 
                 GL_RG_INTEGER, GL_UNSIGNED_SHORT, dst); //TODO: flexible format
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLBTexture *glbGetFramebufferColor(GLBFramebuffer *framebuffer, int i)
{
    return framebuffer->colors[i];
}

GLBTexture *glbGetFramebufferDepth(GLBFramebuffer *framebuffer)
{
    return framebuffer->depth;
}

GLBTexture *glbGetFramebufferStencil(GLBFramebuffer *framebuffer)
{
    return framebuffer->stencil;
}
