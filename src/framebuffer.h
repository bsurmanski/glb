/*
 * framebuffer.h
 * GLB
 * March 18, 2013
 * Brandon Surmanski
 */

#ifndef _GLB_FRAMEBUFFER_H
#define _GLB_FRAMEBUFFER_H

#include "glb_types.h"

#define GLB_FRAMEBUFFER_COLORS_MAX 16

GLBFramebuffer *glbCreateFramebuffer(int *errcode_ret);
void            glbDeleteFramebuffer(GLBFramebuffer *framebuffer);
void            glbRetainFramebuffer(GLBFramebuffer *framebuffer);
void            glbReleaseFramebuffer(GLBFramebuffer *framebuffer);
int             glbFramebufferTexture(GLBFramebuffer *framebuffer, GLBTexture *texture);
int             glbFramebufferColor(GLBFramebuffer *framebuffer, int i, GLBTexture *texture);
int             glbFramebufferDepth(GLBFramebuffer *framebuffer, GLBTexture *depth);
int             glbFramebufferStencil(GLBFramebuffer *framebuffer, GLBTexture *stencil);
int             glbFramebufferDepthStencil(GLBFramebuffer *framebuffer, GLBTexture *depth_stencil);

#endif
