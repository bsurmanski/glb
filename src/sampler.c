/**
 * sampler.c
 * @file sampler.h
 * GLB
 * @date March 02, 2013
 * @author Brandon Surmanski
 *
 * @brief definition of the GLBSampler object interface
 */

#include "glb_private.h"

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

    GLB_SET_ERROR(GLB_SUCCESS);
    return sampler;

ERROR:
    GLB_SET_ERROR(errcode);
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

void glbSamplerFilter (GLBSampler *sampler, enum GLBSamplerFilter min, enum GLBSamplerFilter mag)
{
    sampler->minfilter = min;
    sampler->magfilter = mag;
}

void glbSamplerLOD    (GLBSampler *sampler, float minlod, float maxlod)
{
    sampler->minlod = minlod;
    sampler->maxlod = maxlod;
}

void glbSamplerWrap   (GLBSampler *sampler,
                          enum GLBSamplerWrap s,
                          enum GLBSamplerWrap t,
                          enum GLBSamplerWrap r)
{
    sampler->wrap_s = s;
    sampler->wrap_t = t;
    sampler->wrap_r = r;
}

void glbSamplerCompare(GLBSampler *sampler,
                          enum GLBSamplerCompareMode m,
                          enum GLBSamplerCompareFunc f)
{
    sampler->compare_mode = m;
    sampler->compare_func = f;
}
