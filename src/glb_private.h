/**
 * @internal
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
#include <stdio.h>

#include <GL/gl.h>

#define GLB_ASSERT(x,err,jmp) \
    do { \
        if(!(x)) { \
            errcode = err; \
            goto jmp; \
        } \
    } while(0);

#ifdef DEBUG
#define GLB_SET_ERROR(a) \
    do { \
        if(errcode_ret) {\
            *errcode_ret = (a);\
        }\
        if(a){ \
            printf("Error in function %s: %s\n", __FUNCTION__, glbErrorString(a));\
        }\
    } while(0);

#define GLB_RETURN_ERROR(a) \
        {\
        if(a){ \
            printf("Error in function %s: %s\n", __FUNCTION__, glbErrorString(a));\
        }\
        return (a);\
        }
        
#else
#define GLB_SET_ERROR(a) \
    do { \
        if(errcode_ret) {\
            *errcode_ret = (a);\
        }\
    } while(0);

#define GLB_RETURN_ERROR(a) \
        return (a); 
#endif

/*{{{ Buffer*/

///@private
struct GLBVBufferData
{
    int count;
    struct GLBVertexLayout *layout;
};

///@private
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

    size_t nmemb;                ///< number of members (eg. number of vertices)
    size_t sz;                   ///< size of each member (eg. vertex size)
    struct GLBIBufferData idata; ///< index metadata (if buffer is interpreted as indices)
    struct GLBVBufferData vdata; ///< vertex metadata (if buffer is interpreted as vertices)
};/*}}}*/

/*{{{ Framebuffer*/
struct GLBFramebuffer
{
    int refcount;
    GLuint globj;

    GLenum status; ///< current status as retrieved from glCheckFramebufferStatus
    int ncolors;    ///< number of color textures currently bound
    GLBTexture *depth;
    GLBTexture *stencil;
    GLBTexture **colors;
};/*}}}*/

/*{{{ Shader*/
/**
 * @private
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

    int nuniforms;  ///< number of uniforms defined
    int nopaques;   ///< number of opaque types defined (eg sampler2D)
    int ninputs;    ///< number if 'in' definitions in GLSL
    int noutputs;   ///< number of 'out' definitions in GLSL
    struct GLBShaderIdent *uniforms[16];    ///< uniforms currently bound to the shader
    struct GLBShaderIdent *inputs[16];      ///< identifiers for inputs
    struct GLBShaderIdent *outputs[16];     ///< identifiers for outputs
};/*}}}*/

/*{{{ Sampler*/
struct GLBSampler
{
    int refcount;
    int minfilter;  ///< GL_TEXTURE_MIN_FILTER
    int magfilter;  ///< GL_TEXTURE_MAX_FILTER
    float minlod;   ///< GL_TEXTURE_MIN_LOD
    float maxlod;   ///< GL_TEXTURE_MAX_LOD
    int wrap_s;     ///< GL_TEXTURE_WRAP_S
    int wrap_t;     ///< GL_TEXTURE_WRAP_T
    int wrap_r;     ///< GL_TEXTURE_WRAP_R
    int compare_mode; ///< GL_TEXTURE_COMPARE_MODE
    int compare_func;   ///< GL_TEXTURE_COMPARE_FUNC
};/*}}}*/

/*{{{ Texture*/

struct GLBTexture
{
    int refcount;
    GLuint globj;

    int dim[3]; ///<texture dimensions (width, height, depth (for 3D textures)) 
    uint32_t format; ///< image format
    uint32_t size;  ///< size in bytes
    GLenum target;  ///< texture unit target (eg GL_TEXTURE_2D)
    struct GLBSampler *sampler; ///< curently used sampler
};/*}}}*/

/*{{{ Program*/
///@private
typedef struct GLBProgramOption
{
    int tmp;
    //TODO: options
} GLBProgramOption;

///@private
typedef struct GLBProgramOptions
{
   int noptions;
   int maxoptions;
   struct GLBProgramOptions *options;
} GLBProgramOptions;

///@private
typedef struct GLBProgramIdent
{
    GLenum type;
    int location;
    int size; ///< array size
    int isInt;
    int order; ///< order that GLSL var is defined. For Sampler variables, its also the Texture Unit
} GLBProgramIdent;

struct GLBProgram
{
    int refcount;   ///< reference to this object
    GLuint globj;   ///< reference to GL program object

    int dirty;      ///< requires a relink
    GLBShader *shaders[GLB_NPROGRAM_SHADERS]; ///< reference to attached shader
    int nuniforms;  ///< number of uniforms in all attached shaders
    int ninputs;    ///< number of inputs in all attached shaders
    int noutputs;   ///< number of ouputs in all attached shaders
    struct GLBFramebuffer *framebuffer;
    struct GLBProgramOptions *options;
    struct GLBTexture *textures[GLB_MAX_TEXTURES]; //currently bound texture units
    struct GLBProgramIdent *uniforms[GLB_MAX_UNIFORMS];
    struct GLBProgramIdent *inputs[GLB_MAX_INPUTS];
    struct GLBProgramIdent *outputs[GLB_MAX_OUTPUTS]; //TODO use a linked list instread
};/*}}}*/

#endif
