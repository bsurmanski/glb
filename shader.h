/**
 * shader.h
 * GLB
 * March 01, 2013
 * Brandon Surmanski
 */

#ifndef _GLB_SHADER_H
#define _GLB_SHADER_H

struct GLBShaderIdent;

typedef struct GLBShader
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
} GLBShader;

enum GLBShaderStage
{
    GLB_VERTEX_SHADER          = GL_VERTEX_SHADER,
    GLB_TESS_CONTROL_SHADER    = GL_TESS_CONTROL_SHADER,
    GLB_TESS_EVALUATION_SHADER = GL_TESS_EVALUATION_SHADER,
    GLB_GEOMETRY_SHADER        = GL_GEOMETRY_SHADER,
    GLB_FRAGMENT_SHADER        = GL_FRAGMENT_SHADER
};

GLBShader* glbCreateShaderWithSourceFile (  const char *filenm,
                                            enum GLBShaderStage stage, 
                                            int *errcode_ret);

GLBShader* glbCreateShaderWithSource (  int len,
                                        const char *mem,
                                        enum GLBShaderStage stage, 
                                        int *errcode_ret); 

void        glbDeleteShader       (GLBShader *shader);
void        glbRetainShader       (GLBShader *shader);
void        glbReleaseShader      (GLBShader *shader);

#endif
