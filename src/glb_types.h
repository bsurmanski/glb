/**
 * glb_types.h
 * GLB
 * March 18, 2013
 * Brandon Surmanski
 *
 * Forward Definitions of all public types and typedefs
 */

#ifndef _GLB_TYPES_H
#define _GLB_TYPES_H

struct GLBVertexLayout
{
    unsigned int size;
    unsigned int type;
    unsigned int normalized;
    unsigned int stride;
    unsigned int offset;
};

struct GLBBuffer;
struct GLBProgram;
struct GLBSampler;
struct GLBShader;
struct GLBTexture;

typedef struct GLBBuffer GLBBuffer;
typedef struct GLBProgram GLBProgram;
typedef struct GLBSampler GLBSampler;
typedef struct GLBShader GLBShader;
typedef struct GLBTexture GLBTexture;
typedef struct GLBVertexLayout GLBVertexLayout;

#endif
