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

#define GLB_SET_ERROR(a) \
    do { \
        if(errcode_ret) {\
            *errcode_ret = (a);\
        }\
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

    int ncolors;
    GLBTexture *depth;
    GLBTexture *stencil;
    GLBTexture **colors;
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

    int nuniforms;  ///< number of uniforms defined
    int nopaques;   ///< number of opaque types defined (eg sampler2D)
    int ninputs;    ///< number if 'in' definitions in GLSL
    int noutputs;   ///< number of 'out' definitions in GLSL
    int ntextures;  ///<XXX unused?
    struct GLBShaderIdent *textures;  ///< XXX (unused?) textures currently bound to the shader
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

    uint32_t x;     ///< width
    uint32_t y;     ///< height
    uint32_t z;     ///< depth (of a 3D image)
    uint32_t format; ///< image format
    uint32_t size;  ///< size in bytes
    GLenum target;  ///< texture unit target (eg GL_TEXTURE_2D)
    struct GLBSampler *sampler; ///< curently used sampler
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
    GLenum type;
    int location;
    int size; ///< array size
    int isInt;
    int order; ///< order that GLSL var is defined. For Sampler variables, its also the Texture Unit
    void *bind; ///< XXX (curently unused(ish?)) used for textures
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
    struct GLBProgramOptions *options;
    struct GLBProgramIdent *textures[GLB_MAX_TEXTURES];
    struct GLBProgramIdent *uniforms[GLB_MAX_UNIFORMS];
    struct GLBProgramIdent *inputs[GLB_MAX_INPUTS];
    struct GLBProgramIdent *outputs[GLB_MAX_OUTPUTS]; //TODO use a linked list instread
};/*}}}*/

#endif
