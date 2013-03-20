/**
 * shader.h
 * GLB
 * March 01, 2013
 * Brandon Surmanski
 *
 * definition of the GLBShader object interface
 */

#ifndef _GLB_SHADER_H
#define _GLB_SHADER_H

#include "glb_types.h"

struct GLBShaderIdent;

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
