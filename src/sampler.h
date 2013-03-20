/**
 * sampler.h
 * GLB
 * March 02, 2013
 * Brandon Surmanski
 *
 * definition of the GLBSampler object interface
 */

#ifndef _GLB_SAMPLER_H
#define _GLB_SAMPLER_H

#include "glb_types.h"

enum GLBSamplerFilter
{
    GLB_NEAREST = GL_NEAREST,
    GLB_LINEAR  = GL_LINEAR,
    GLB_NEAREST_MIPMAP_NEAREST  = GL_NEAREST_MIPMAP_NEAREST,
    GLB_LINEAR_MIPMAP_NEAREST   = GL_LINEAR_MIPMAP_NEAREST,
    GLB_NEAREST_MIPMAP_LINEAR   = GL_NEAREST_MIPMAP_LINEAR,
    GLB_LINEAR_MIPMAP_LINEAR    = GL_LINEAR_MIPMAP_LINEAR
};

enum GLBSamplerWrap
{
    GLB_CLAMP_TO_EDGE   = GL_CLAMP_TO_EDGE,
    GLB_MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
    GLB_REPEAT          = GL_REPEAT,
    GLB_CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER
};

enum GLBSamplerCompareMode
{
    GLB_COMPARE_REF_TO_TEXTURE = GL_COMPARE_REF_TO_TEXTURE,
    GLB_NONE                   = GL_NONE
};

enum GLBSamplerCompareFunc
{
    GLB_LEQUAL   = GL_LEQUAL,
    GLB_GEQUAL   = GL_GEQUAL,
    GLB_LESS     = GL_LESS,
    GLB_GREATER  = GL_GREATER,
    GLB_EQUAL    = GL_EQUAL,
    GLB_NOTEQUAL = GL_NOTEQUAL,
    GLB_ALWAYS   = GL_ALWAYS,
    GLB_NEVER    = GL_NEVER,
};

GLBSampler* glbCreateSampler (int *errcode_ret);
void        glbDeleteSampler (GLBSampler *sampler);
void        glbRetainSampler (GLBSampler *sampler);
void        glbReleaseSampler(GLBSampler *sampler);
void glbSetSamplerFilter (GLBSampler *sampler, enum GLBSamplerFilter min, enum GLBSamplerFilter mag);
void glbSetSamplerLOD    (GLBSampler *sampler, float minlod, float maxlod);
void glbSetSamplerWrap   (GLBSampler *sampler,
                          enum GLBSamplerWrap s,
                          enum GLBSamplerWrap t,
                          enum GLBSamplerWrap r);
void glbSetSamplerCompare(GLBSampler *sampler,
                          enum GLBSamplerCompareMode m,
                          enum GLBSamplerCompareFunc f);

#endif
