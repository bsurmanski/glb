/**
 * framebuffer.c
 * GLB
 * March 18, 2013
 * Brandon Surmanski
 */

#include "glb_private.h"

#include <stdlib.h>

GLBFramebuffer *glbCreateFramebuffer(int *errcode_ret)
{
    int errcode;
    GLBFramebuffer *framebuffer = malloc(sizeof(GLBFramebuffer));
    GLB_ASSERT(framebuffer, GLB_OUT_OF_MEMORY, ERROR);

    framebuffer->ncolors = 0;
    framebuffer->refcount = 1;
    framebuffer->depth = NULL;
    framebuffer->stencil = NULL;
    framebuffer->colors = calloc(16, sizeof(GLBTexture*));
    glGenFramebuffers(1, &framebuffer->globj);

    GLB_SET_ERROR(GLB_SUCCESS);
    return framebuffer;

ERROR:
    GLB_SET_ERROR(errcode);
    return NULL;
}

void glbDeleteFramebuffer(GLBFramebuffer *framebuffer)
{
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

int glbFramebufferTexture(GLBFramebuffer *framebuffer, GLBTexture *texture)
{
    return 0;
}

int glbFramebufferColor(GLBFramebuffer *framebuffer, int i, GLBTexture *texture)
{
    return 0;
}

int glbFramebufferDepth(GLBFramebuffer *framebuffer, GLBTexture *depth)
{
    return 0;
}

int glbFramebufferStencil(GLBFramebuffer *framebuffer, GLBTexture *stencil)
{
    return 0;
}

int glbFramebufferDepthStencil(GLBFramebuffer *framebuffer, GLBTexture *depth_stencil)
{
    return 0;
}
