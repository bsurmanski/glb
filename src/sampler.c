/**
 * sampler.c
 * GLB
 * March 02, 2013
 * Brandon Surmanski
 */

#include "glb.h"

#include <stdlib.h>

GLBSampler* glbCreateSampler (int *errcode_ret)
{
    int errcode;
    GLBSampler *sampler = malloc(sizeof(GLBSampler));
    GLB_ASSERT(sampler, GLB_OUT_OF_MEMORY, ERROR);

    sampler->refcount = 1;
    sampler->magfilter = GLB_LINEAR;
    sampler->minfilter = GLB_LINEAR;
    sampler->minlod = -1000.0f;
    sampler->maxlod = 1000.0f;
    sampler->wrap_s = GLB_REPEAT;
    sampler->wrap_t = GLB_REPEAT;
    sampler->wrap_r = GLB_REPEAT;
    sampler->compare_mode = GLB_NONE;
    sampler->compare_func = GLB_LEQUAL;
    return sampler;
    
ERROR:
    if(errcode_ret)
    {
        *errcode_ret = errcode;
    }
    return NULL;
}

void glbDeleteSampler (GLBSampler *sampler)
{
    free(sampler);
}

void glbRetainSampler (GLBSampler *sampler)
{
    sampler->refcount++; 
}

void glbReleaseSampler(GLBSampler *sampler)
{
    sampler->refcount--;
    if(sampler->refcount <= 0)
    {
        glbDeleteSampler(sampler);
    }
}

void glbSetSamplerFilter (GLBSampler *sampler, enum GLBSamplerFilter min, enum GLBSamplerFilter mag)
{
    sampler->minfilter = min;
    sampler->magfilter = mag;
}

void glbSetSamplerLOD    (GLBSampler *sampler, float minlod, float maxlod)
{
    sampler->minlod = minlod;
    sampler->maxlod = maxlod;
}

void glbSetSamplerWrap   (GLBSampler *sampler, 
                          enum GLBSamplerWrap s, 
                          enum GLBSamplerWrap t, 
                          enum GLBSamplerWrap r)
{
    sampler->wrap_s = s;
    sampler->wrap_t = t;
    sampler->wrap_r = r;
}

void glbSetSamplerCompare(GLBSampler *sampler, 
                          enum GLBSamplerCompareMode m, 
                          enum GLBSamplerCompareFunc f)
{
    sampler->compare_mode = m;
    sampler->compare_func = f;
}
