/**
 * glb_private.h
 * GLB
 * March 18, 2013
 * Brandon Surmanski
 *
 * This contains definitions shared throughout the GLB implementation
 * that should not be visible in the external interface. Definitions 
 * in this header are subject to change without notice.
 */

#ifndef _GLB_PRIVATE_H
#define _GLB_PRIVATE_H

#include "glb_types.h"
#include "glb.h"

#include <GL/gl.h>

#define GLB_ASSERT(x,err,jmp) \
    do { \
        if(!(x)) { \
            errcode = err; \
            goto jmp; \
        } \
    } while(0);

/*{{{ Buffer*/

struct GLBVBufferData
{
    int count; 
    struct GLBVertexLayout *layout;
};

struct GLBIBufferData
{
    int count;  ///< number of vertex indices
    int offset; ///< offset from the first
    int type;   ///< type of vertex indices (must be unsigned byte, short or int)
};

struct GLBBuffer
{
    int refcount;
    GLuint globj;
    size_t nmemb;
    size_t sz;
    struct GLBIBufferData idata;
    struct GLBVBufferData vdata;
};/*}}}*/

/*{{{ Shader*/
/**
 * single identifier in a shader program. An identifier represents some sort of
 * variable that is visible externally to OpenGL. This can be an input, output,
 * uniform, sampler, etc.
 */
struct GLBShaderIdent
{
    int type; ///< GL type
    int size; ///< size of array
    char *name; ///< name string
};

struct GLBShader
{
    int refcount;
    GLuint globj;
    int nuniforms;
    int ninputs;
    int noutputs;
    int ntextures;
    struct GLBShaderIdent *textures;  ///< textures currently bound to the shader
    struct GLBShaderIdent *uniforms[16];  ///< uniforms currently bound to the shader
    struct GLBShaderIdent *inputs[16];
    struct GLBShaderIdent *outputs[16];
};/*}}}*/

/*{{{ Sampler*/
struct GLBSampler
{
    int refcount;
    int minfilter;
    int magfilter;
    float minlod;
    float maxlod;
    int wrap_s;
    int wrap_t;
    int wrap_r;
    int compare_mode;
    int compare_func;
};/*}}}*/

/*{{{ Texture*/

struct GLBTexture
{
    int refcount;
    GLuint globj;
    uint32_t x;
    uint32_t y;
    uint32_t z;
    uint32_t format;
    uint32_t size;
    GLenum target;
    struct GLBSampler *sampler;
};/*}}}*/

/*{{{ Program*/
typedef struct GLBProgramOption
{
    int tmp;
    //TODO: options
} GLBProgramOption;

typedef struct GLBProgramOptions
{
   int noptions;
   int maxoptions;
   struct GLBProgramOptions *options;  
} GLBProgramOptions; 

typedef struct GLBProgramIdent
{
    int type;
    int location;
    int size; // array size
    int isInt;
    void *bind; //used for textures
} GLBProgramIdent;/*}}}*/

#endif
