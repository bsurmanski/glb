/**
 * glb_types.h
 * GLB
 * March 18, 2013
 * Brandon Surmanski
 *
 * Forward declarations of all public types and typedefs.
 * To see definitions of opaque types, look in glb_private.h
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
struct GLBFramebuffer;
struct GLBProgram;
struct GLBSampler;
struct GLBShader;
struct GLBTexture;

typedef struct GLBBuffer GLBBuffer;
typedef struct GLBFramebuffer GLBFramebuffer;
typedef struct GLBProgram GLBProgram;
typedef struct GLBSampler GLBSampler;
typedef struct GLBShader GLBShader;
typedef struct GLBTexture GLBTexture;
typedef struct GLBVertexLayout GLBVertexLayout;

#endif
