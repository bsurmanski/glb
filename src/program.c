/**
 * program.c
 * @file program.h
 * glb
 * @date March 01, 2013
 * @author Brandon Surmanski
 *
 * @brief definition of the GLBProgram object interface
 */

#include "glb_private.h"

#include <alloca.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * gets the array index for a given shader
 */
static int glbProgramShaderIndex(int shader_index)
{
    int ret = 0;
    switch(shader_index)
    {
        case GLB_VERTEX_SHADER:
            ret = 0;
            break;
        case GLB_TESS_CONTROL_SHADER:
            ret = 1;
            break;
        case GLB_TESS_EVALUATION_SHADER:
            ret = 2;
            break;
        case GLB_GEOMETRY_SHADER:
            ret = 3;
            break;
        case GLB_FRAGMENT_SHADER:
            ret = 4;
            break;
    }
    return ret;
}

// forces the program to clean

/**
 * synchronizes the OpenGL program object state with the GLB state.
 */
static int glbProgramClean(GLBProgram *program)
{
    if(program->dirty)
    {
        int i;
        for(i = 0; i < GLB_NPROGRAM_SHADERS; i++)
        {
            if(program->shaders[i])
            {
                glAttachShader(program->globj, program->shaders[i]->globj);
#ifdef DEBUG
                /*{{{XXX debug*/ printf("attaching shader %i\n", i);/*}}}*/
#endif
            }
        }

        // free existing inputs
        for(i = 0; i < program->ninputs; i++)
        {
            if(program->inputs[i])
            {
                free(program->inputs[i]);
            }
        }

        // free existing outputs
        for(i = 0; i < program->noutputs; i++)
        {
            if(program->outputs[i])
            {
                free(program->outputs[i]);
            }
        }

        // free existing uniforms
        for(i = 0; i < program->nuniforms; i++)
        {
            if(program->uniforms[i])
            {
                free(program->uniforms[i]);
            }
        }

        glLinkProgram(program->globj);

        //TODO: assumtion that input is vshader, and output is fshader
        GLBShader *vshader = program->shaders[glbProgramShaderIndex(GLB_VERTEX_SHADER)];
        GLBShader *fshader = program->shaders[glbProgramShaderIndex(GLB_FRAGMENT_SHADER)];
        program->ninputs = 0;
        program->noutputs = 0;
        int nopaques = 0;
        if(!vshader || !fshader) goto ERROR;
        struct GLBShaderIdent **s_ident = vshader->inputs;
        struct GLBProgramIdent **p_ident = program->inputs;
        while(*s_ident)
        {
            *p_ident = malloc(sizeof(struct GLBProgramIdent));
            (*p_ident)->type = (*s_ident)->type;
            (*p_ident)->location = glGetAttribLocation(program->globj, (*s_ident)->name);
            (*p_ident)->isInt = glbTypeIsInt((*p_ident)->type);
            (*p_ident)->order = program->ninputs;
            p_ident++;
            s_ident++;
            program->ninputs++;
        }

        s_ident = fshader->outputs;
        p_ident = program->outputs;
        while(*s_ident)
        {
            *p_ident = malloc(sizeof(struct GLBProgramIdent));
            (*p_ident)->type = (*s_ident)->type;
            (*p_ident)->location = glGetFragDataLocation(program->globj, (*s_ident)->name);
            (*p_ident)->isInt = glbTypeIsInt((*p_ident)->type);
            (*p_ident)->order = program->noutputs;
            p_ident++;
            s_ident++;
            program->noutputs++;
        }

        // get uniforms from ALL shaders
        p_ident = program->uniforms;
        for(i = 0; i < GLB_NPROGRAM_SHADERS; i++)
        {
            GLBShader *cshader = program->shaders[i];
            if(!cshader) continue;
            s_ident = cshader->uniforms;
            while(*s_ident)
            {
                *p_ident = malloc(sizeof(struct GLBProgramIdent));
                (*p_ident)->type = (*s_ident)->type;
                (*p_ident)->location = glGetUniformLocation(program->globj, (*s_ident)->name);
                (*p_ident)->isInt = glbTypeIsInt((*p_ident)->type);
                if(glbTypeIsOpaque((*p_ident)->type))
                {
                    (*p_ident)->order = nopaques;
                    nopaques++;
                } else
                {
                    (*p_ident)->order = program->nuniforms;
                }
                (*p_ident)->size = 1; //TODO, parse
                p_ident++;
                s_ident++;
                program->nuniforms++;
            }
        }

    //XXX debug/*{{{*/
#ifdef DEBUG
    int max_attribs;
    glGetProgramiv(program->globj, GL_ACTIVE_ATTRIBUTES, &max_attribs);
    for(i = 0; i < max_attribs; i++)
    {
        GLchar buf[64];
        GLint size;
        GLenum type;
        glGetActiveAttrib(program->globj, i, 64, NULL, &size, &type, buf);
        printf("attrib %d: %s, %s\n", i, glbTypeString(type), buf);
    }
    printf("\n");
#endif
   /*}}}*/

        program->dirty = 0;
    }

    return 0;
ERROR: //TODO: proper errors
    return 1;
}

/*{{{ Initialization/Deinitializaion*/
/**
 * creates a new program object. This also sets the program's reference count to 1,
 * so no call to glbRetainProgram is required.
 * @param errcode_ret optional parameter that returns non-zero on error.
 * @return a reference to a fully initialized program object
 */
GLBProgram *glbCreateProgram (int *errcode_ret)
{
    int errcode;

    GLBProgram *program = malloc(sizeof(GLBProgram));
    GLB_ASSERT(program, GLB_OUT_OF_MEMORY, ERROR);

    program->refcount = 1;
    program->dirty = 0;
    program->ninputs = 0;
    program->noutputs = 0;
    program->nuniforms = 0;

    program->globj = glCreateProgram();

    program->framebuffer = NULL;
    memset(program->shaders, 0, sizeof(void* [GLB_NPROGRAM_SHADERS]));
    memset(program->textures, 0, sizeof(void* [GLB_MAX_TEXTURES]));
    memset(program->inputs, 0, sizeof(void* [GLB_MAX_INPUTS]));
    memset(program->outputs, 0, sizeof(void* [GLB_MAX_OUTPUTS]));
    memset(program->uniforms, 0, sizeof(void* [GLB_MAX_UNIFORMS]));

    GLB_SET_ERROR(GLB_SUCCESS);
    return program;

//ERROR_CREATE:
//    free(program);
ERROR:
    GLB_SET_ERROR(errcode);

#ifdef DEBUG
    printf("GLBCreateProgram Error: %s\n", glbErrorString(errcode));
#endif

    return NULL;
}

/**
 * deletes a program object. Any shaders the program object held will be released.
 * Any allocated memory held will be freed.
 * Any state the program object had will become undefined.
 * If any remaining references to the program are used, results are undefined.
 * Users should always release instead of directly deleting, unless they are certain
 * there are no remaining references in use.
 * @param program the prgram to be deleted
 */
void glbDeleteProgram (GLBProgram *program)
{
    int i;
    for(i = 0; i < GLB_NPROGRAM_SHADERS; i++)
    {
        if(program->shaders[i])
        {
            glbReleaseShader(program->shaders[i]);
        }
    }

    for(i = 0; i < program->nuniforms; i++)
    {
        free(program->uniforms[i]); //TODO: free texture if attached to 'bind'?
    }

    for(i = 0; i < program->ninputs; i++)
    {
        free(program->inputs[i]);
    }

    for(i = 0; i < program->noutputs; i++)
    {
        free(program->outputs[i]);
    }

    glbReleaseFramebuffer(program->framebuffer);
    //TODO: delete Identifiers
    glDeleteProgram(program->globj);
    free(program);
}

/**
 * retains the program for use. Increments the reference count.
 * Any thread that calls 'retain' on a program is responsible for calling
 * a corisponding release, or there may be memory leaks.
 * @param program the program to retain
 */
void glbRetainProgram (GLBProgram *program)
{
    program->refcount++;
}

/**
 * releases the program. This decrements the reference count. If there
 * are no remaining references (reference count is 0), the program is also deleted.
 * To prevent a program object from being deleted, a thread should always retain
 * the object.
 */
void glbReleaseProgram (GLBProgram *program)
{
    program->refcount--;
    if(program->refcount <= 0)
    {
        glbDeleteProgram(program);
    }
}/*}}}*/

/*{{{ Options */
int glbProgramExtendOptionsList(GLBProgram *program, int n)
{

    return 0;
}

int glbProgramOption (GLBProgram *program, int option, int value)
{
    //if(program->options.noptions > program->options.maxoptions)
    {
        //TODO: extend
    }

    //TODO: set program options
    return GLB_UNIMPLEMENTED;
}/*}}}*/

/*{{{ Shaders*/

/**
 * initialized and attaches a new shader to a given shader stage. The shader is
 * created using a GLSL shader stored in a text file. The functionality is equivilent
 * to loading the entire file referenced by 'filenm' into a string, and then passing it
 * into glbProgramAttachNewShaderSource.
 * @param program the program to attach to
 * @param filenm the filename of a text file containing the entire source of a GLSL shader
 * @param stage the shader stage to attach the new shader to
 * @returns 0 on success, or any error that glbCreateShaderWithSourceFile would return
 */
int glbProgramAttachNewShaderSourceFile (GLBProgram *program,
                                           const char *filenm,
                                           enum GLBShaderStage stage)
{
    int errcode = GLB_SUCCESS;

    GLBShader *shader = glbCreateShaderWithSourceFile(filenm, stage, &errcode);
    GLB_ASSERT(!errcode, errcode, ERROR_SOURCE);

    glbProgramAttachShader(program, shader);
    glbReleaseShader(shader);

    return errcode;

ERROR_SOURCE:
    return errcode;
}

/**
 * initialized and attaches a new shader to a given shader stage. The shader is
 * created using a reference to pointer to a string containing the entire source of
 * a GLSL shader.
 * @param program the program to attach to
 * @param len the length of the shader source string
 * @param mem a pointer to a string containing a GLSL shader
 * @param stage the shader stage to attach the new shader to
 * @returns 0 //TODO
 */
int glbProgramAttachNewShaderSource (GLBProgram *program,
                                     int len, const char *mem, enum GLBShaderStage stage)
{
    int errcode = 0;
    GLBShader *shader = glbCreateShaderWithSource(len, mem, stage, &errcode);
    glbProgramAttachShader(program, shader);
    glbReleaseShader(shader);

    return errcode; //TODO: error handle
}
/**
 * Attaches a shader to be used by program. The program will also retain the
 * shader in the process (though glbRetainShader). Any shader currently attached
 * to the shader stage in which 'shader' replaces will be detached
 * (as through a call to glbProgramDetachShaderStage)
 * @param program the program to attach to
 * @param shader a pointer to the shader to attach.
 * @returns 0 on success, GLB_INVALID_ARGUMENT if 'shader' is not attached
 */
void glbProgramAttachShader (GLBProgram *program, GLBShader *shader)
{
    glbRetainShader(shader);

    GLint shadertype;
    glGetShaderiv(shader->globj, GL_SHADER_TYPE, &shadertype);

    int index = glbProgramShaderIndex(shadertype);
    if(program->shaders[index])
    {
        glDetachShader(program->globj, program->shaders[index]->globj);
        glbReleaseShader(program->shaders[index]);
    }
    program->shaders[index] = shader;
    program->dirty = 1;
}

/**
 * find and remove a shader from the given program. The program will
 * also release the shader in the process (though glbReleaseShader).
 * @param program the program to detach from
 * @param shader a pointer to the shader to detach
 * @returns 0 on success, GLB_INVALID_ARGUMENT if 'shader' is not attached
 */
int glbProgramDetachShader (GLBProgram *program, GLBShader *shader)
{
    int errcode = GLB_INVALID_ARGUMENT;
    int i;
    for(i = 0; i < GLB_NPROGRAM_SHADERS; i++)
    {
        if(program->shaders[i] == shader)
        {
            glDetachShader(program->globj, shader->globj);
            glbReleaseShader(shader);
            program->shaders[i] = NULL;
            errcode = 0;
            program->dirty = 1;
            break;
        }
    }

    return errcode;
}

/**
 * removes the shader attached to 'stage' from the given program. The program will
 * also release the shader in the process (though glbReleaseShader).
 * @param program the program to detach from
 * @param stage the shader stage to detach, can be GLB_{VERTEX,TESS_CONTROL,TESS_EVALUATION,
 * GEOMETRY,FRAGMENT}_SHADER.
 * @returns 0 on success, GLB_INVALID_ARGUMENT if 'stage' is not attached
 */
int glbProgramDetachShaderStage (GLBProgram *program, enum GLBShaderStage stage)
{
    int errcode = GLB_INVALID_ARGUMENT;
    int index = glbProgramShaderIndex(stage);
    if(program->shaders[index])
    {
        glDetachShader(program->globj, program->shaders[index]->globj);
        glbReleaseShader(program->shaders[index]);
        program->shaders[index] = NULL;
        errcode = 0;
        program->dirty = 1;
    }
    return errcode;
}/*}}}*/

/*{{{ Bindables */

static int glbProgramUniformIdent(GLBProgram *program, GLBProgramIdent *ident,
                                       int transposed, int sz, void *val)
{
    int errcode = 0;
    int n=1;

    if(ident->location < 0)
    {
        return GLB_INVALID_ARGUMENT;
    }

    if(glbTypeSizeof(ident->type)) //TODO: workaround to avoid FPEXCEPT for Opaque types without size
    n = sz / glbTypeSizeof(ident->type);

    if(n > ident->size)
    {
        //TODO: enable this once array length parsing is finished
        //return GLB_INVALID_ARGUMENT;
    }

    glUseProgram(program->globj);
    if(glbTypeIsMatrix(ident->type))
    {
        switch(ident->type)
        {
            case GLB_MAT2:
                glUniformMatrix2fv(ident->location, n, transposed, val);
                break;
            case GLB_MAT3:
                glUniformMatrix3fv(ident->location, n, transposed, val);
                break;
            case GLB_MAT4:
                glUniformMatrix4fv(ident->location, n, transposed, val);
                break;
            case GLB_MAT2x3:
                glUniformMatrix2x3fv(ident->location, n, transposed, val);
                break;
            case GLB_MAT2x4:
                glUniformMatrix2x4fv(ident->location, n, transposed, val);
                break;
            case GLB_MAT3x2:
                glUniformMatrix3x2fv(ident->location, n, transposed, val);
                break;
            case GLB_MAT3x4:
                glUniformMatrix3x4fv(ident->location, n, transposed, val);
                break;
            case GLB_MAT4x2:
                glUniformMatrix4x2fv(ident->location, n, transposed, val);
                break;
            case GLB_MAT4x3:
                glUniformMatrix4x3fv(ident->location, n, transposed, val);
                break;
            default:
                goto ERROR;
        }
    } else if (glbTypeIsScalar(ident->type))
    {
        switch(glbTypeLength(ident->type))
        {
            case 1: // scalar
                if(glbTypeIsFloat(ident->type))
                {
                    glUniform1fv(ident->location, n, val);
                } else if(glbTypeIsUnsigned(ident->type)) // unsigned int
                {
                    glUniform1uiv(ident->location, n, val);
                } else if(glbTypeIsInt(ident->type))// signed int
                {
                    glUniform1iv(ident->location, n, val);
                } else if(glbTypeIsDouble(ident->type))
                {
                    //glUniform1dv(ident->location, n, val);
                    return GLB_GL_TOO_OLD;
                }
                break;
            case 2:
                if(glbTypeIsFloat(ident->type))
                {
                    glUniform2fv(ident->location, n, val);
                } else if(glbTypeIsUnsigned(ident->type)) // unsigned int
                {
                    glUniform2uiv(ident->location, n, val);
                } else if(glbTypeIsInt(ident->type)) // signed int
                {
                    glUniform2iv(ident->location, n, val);
                } else if(glbTypeIsDouble(ident->type))
                {
                    //glUniform2dv(ident->location, n, val);
                    return GLB_GL_TOO_OLD;
                }
                break;
            case 3:
                if(glbTypeIsFloat(ident->type))
                {
                    glUniform3fv(ident->location, n, val);
                } else if(glbTypeIsUnsigned(ident->type)) // unsigned int
                {
                    glUniform3uiv(ident->location, n, val);
                } else if(glbTypeIsInt(ident->type)) // signed int
                {
                    glUniform3iv(ident->location, n, val);
                } else if(glbTypeIsDouble(ident->type))
                {
                    //glUniform3dv(ident->location, n, val);
                    return GLB_GL_TOO_OLD;
                }
                break;
            case 4:
                if(glbTypeIsFloat(ident->type))
                {
                    glUniform4fv(ident->location, n, val);
                } else if(glbTypeIsUnsigned(ident->type)) // unsigned int
                {
                    glUniform4uiv(ident->location, n, val);
                } else if(glbTypeIsInt(ident->type)) // signed int
                {
                    glUniform4iv(ident->location, n, val);
                } else if(glbTypeIsDouble(ident->type))
                {
                    //glUniform4dv(ident->location, n, val);
                    return GLB_GL_TOO_OLD;
                }
                break;
            default:
                goto ERROR;
        }
    } else if(glbTypeIsOpaque(ident->type))
    {
        GLB_ASSERT(sz == sizeof(GLBTexture), GLB_INVALID_ARGUMENT, ERROR);
        switch(ident->type)
        {
            case GLB_SAMPLER_1D:
            case GLB_SAMPLER_2D:
                //TODO: bind textures to shader
                glActiveTexture(GL_TEXTURE0 + ident->order);
                glBindTexture(((GLBTexture*)val)->target, ((GLBTexture*)val)->globj);
                glUniform1i(ident->location, ident->order);
                break;
            default:
                return GLB_UNIMPLEMENTED;
        }
    } else // unknown type
    {
        errcode = GLB_INVALID_ARGUMENT;
        goto ERROR;
    }
    glUseProgram(0);
    return errcode;

ERROR:
    glUseProgram(0);

    return errcode;
}

int glbProgramUniform (GLBProgram *program, int shader, int i, int sz, void *val)
{
    glbProgramClean(program);

    // increments i to be relative to the selected shader
    int j;
    int shaderid = glbProgramShaderIndex(shader);
    for(j = 0; j < shaderid; j++)
    {
        if(program->shaders[j])
        {
            i += program->shaders[j]->nuniforms;
        }
    }

    return glbProgramUniformIdent(program, program->uniforms[i], true, sz, val);
}

int glbProgramTexture (GLBProgram *program, int shader, int i, GLBTexture *texture)
{
    int errcode;
    glbProgramClean(program);

    int shaderid = glbProgramShaderIndex(shader);

    /*
     * adjusts the suplied index 'i' from the Opaque ordering to the normal 
     * uniform ordering. eg. if sampler2D tex is the first sampler2D defined, but 
     * the third uniform defined (in the GLSL shader), this will transform i from 1 -> 3
     */
    int j;
    int trans = i;
    for(j = 0; j < i; j++)
    {
        if(!glbTypeIsOpaque(program->shaders[shaderid]->uniforms[j]->type))
        {
            trans++;
        }
    }

    // i now refers to the correct index in the uniform array

    for(j = 0; j < shaderid; j++)
    {
        if(program->shaders[j])
        {
            trans += program->shaders[j]->nuniforms; 
        }
    }

    //TODO: index i by Opaque types index instead of Uniform indices
    errcode = glbProgramUniformIdent(
                                    program, 
                                    program->uniforms[trans], 
                                    true, 
                                    sizeof(GLBTexture), 
                                    texture
                                    );
    if(!errcode)
    {
        glbRetainTexture(texture);
        glbReleaseTexture(program->textures[i]);
        program->textures[i] = texture;
        return GLB_SUCCESS;
    }
    return errcode;
}

int glbProgramNamedUniform (GLBProgram *program, char *name, int sz, void *val)
{
    GLuint index;
    GLBProgramIdent ident;

    glbProgramClean(program);

    //get location
    ident.location = glGetUniformLocation(program->globj, name);

    //get index
    glGetUniformIndices(program->globj, 1, (const GLchar* const*)&name, &index);

    //get type
    glGetActiveUniform(program->globj, index, 0, NULL, &ident.size, &ident.type, NULL);

    //TODO: column major switch?
    glbProgramUniformIdent(program, &ident, true, sz, val);

    return 0; //TODO: error detection
}

int glbProgramNamedTexture (GLBProgram *program, char *name, GLBTexture *texture)
{
    glbProgramClean(program);
    //int location;
    //location = glGetUniformLocation(program->globj, name);
    //TODO: find order of texture

    int i;
    for(i = 0; i < GLB_MAX_TEXTURES; i++)
    {
        if(program->textures[i] == NULL)
        {
            //program->textures[i]->location = location;
            //program->textures[i]->texture = texture;
        }
    }

    return GLB_UNIMPLEMENTED;
}

int glbProgramUniformBuffer (GLBProgram *program, char *blocknm, GLBBuffer *buffer)
{
    glbProgramClean(program);
    return GLB_UNIMPLEMENTED;
}

int glbProgramUniformBufferRange (GLBProgram *program, char *blocknm,
                                  int offset, int size, GLBBuffer *buffer)
{
    glbProgramClean(program);
    return GLB_UNIMPLEMENTED;
}/*}}}*/

/*{{{ Layouts, Inputs, Outputs*/
int glbProgramUseVertexLayout (GLBProgram *program, int n, struct GLBVertexLayout *layout)
{
    glbProgramClean(program);
    return GLB_UNIMPLEMENTED;
}

int glbProgramLayout (GLBProgram *program, int noutputs, char **outputs)
{
    glbProgramClean(program);
    return GLB_UNIMPLEMENTED;
}

int glbProgramInputLayout (GLBProgram *program, int n, char **inputs)
{
    glbProgramClean(program);
    return GLB_UNIMPLEMENTED;
}

int glbProgramOutputLayout (GLBProgram *program, int n, char **outputs)
{
    glbProgramClean(program);
    return GLB_UNIMPLEMENTED;
}

int glbProgramOutput (GLBProgram *program, GLBFramebuffer *output)
{
    glbRetainFramebuffer(output);
    glbReleaseFramebuffer(program->framebuffer);
    program->framebuffer = output;
    return GLB_SUCCESS;
}

/*}}}*/

/*{{{ Draw */
int glbProgramDraw (GLBProgram *program, GLBBuffer *array)
{
    return glbProgramDrawRange (program, array, 0, array->nmemb);
}

int glbProgramDrawIndexed (GLBProgram *program, GLBBuffer *array, GLBBuffer *index)
{
    return glbProgramDrawIndexedRange(program, array, index, 0, index->idata.count);
}

int glbProgramDrawRange (GLBProgram *program, GLBBuffer *array, int offset, int count)
{
    return glbProgramDrawIndexedRange(program, array, NULL, offset, count);
}

int glbProgramDrawIndexedRange (GLBProgram *program, GLBBuffer *array,
                                GLBBuffer *index, int offset, int count)
{
    int i;
    int mode = GL_TRIANGLES;

    glbProgramClean(program);

    if(!program->shaders[0]) // cannot draw if theres no VERTEX SHADER
    {
        return GLB_INVALID_ARGUMENT;
    }

    glUseProgram(program->globj);
    glBindBuffer(GL_ARRAY_BUFFER, array->globj);

    if(index)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index->globj);
    }
    /*
    if(program->framebuffer)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, program->framebuffer->globj);
    } else 
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }*/ //TODO re-enable once framebuffers work

    // set correct draw buffers
    GLenum *drawbufs = alloca(sizeof(GLenum) * program->noutputs);
    for (i = 0; i < program->noutputs; i++)
    {
        drawbufs[i] = GL_COLOR_ATTACHMENT0 + program->outputs[i]->location;
    }
    glDrawBuffers(program->noutputs, drawbufs);

    // bind all textures
    
    for(i = 0; i < GLB_MAX_TEXTURES; i++)
    {
        if(program->textures[i])
        {
            GLBTexture *tex = program->textures[i];
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(tex->target, tex->globj);
        }
    }

    // bind correct vertex data locations
    // if a layout is given, use the layout, else guess from the program
    if(array->vdata.layout)
    {
        for(i = 0; i < array->vdata.count; i++)
        {
            GLBVertexLayout *layout = &array->vdata.layout[i];
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, layout->size, layout->type, layout->normalized,
                                  layout->stride, (void*) layout->offset);
        }
    } else // this assumes each attrib in the shader is sequential and (usually) float type
    {
        int attrib_offset = 0;
        for(i = 0; i < program->ninputs; i++)
        {
            int attrib_type = program->inputs[i]->type;
            int attrib_len = glbTypeLength(attrib_type);
            int attrib_size = glbTypeSizeof(attrib_type);
            glEnableVertexAttribArray(i);
            //TODO: change GL_FLOAT 
            if(program->inputs[i]->isInt)
            {
                glVertexAttribIPointer(program->inputs[i]->location,
                                       attrib_len, GL_FLOAT,
                                       attrib_size, //TODO handle arrays
                                       (void*) attrib_offset);
            } else //expected
            {
                glVertexAttribPointer(program->inputs[i]->location,
                                      attrib_len, GL_FLOAT, GL_FALSE,
                                      attrib_size, //TODO: handle arrays
                                      (void*) attrib_offset);
            }
            attrib_offset += attrib_size;
        }
    }

    if(index)
    {
        glDrawElements(mode, index->idata.count, index->idata.type, 0);
    } else
    {
        glDrawArrays(mode, offset, count);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);
    return 0;
}/*}}}*/
