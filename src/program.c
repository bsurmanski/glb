/**
 * program.c
 * glb
 * March 01, 2013
 * Brandon Surmanski
 */

#include "glb.h"

#include <alloca.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
} GLBProgramIdent;

/**
 * single identifier in a shader program. An identifier represents some sort of
 * variable that is visible externally to OpenGL. This can be an input, output,
 * uniform, sampler, etc.
 *
 * XXX declared in shader.c also. keep synchronzied
 */
struct GLBShaderIdent
{
    int type; ///< GL type
    int size; ///< size of array
    char *name; ///< name string
};

struct GLBProgram
{
    int refcount;   ///< reference to this object
    GLuint globj;   ///< reference to GL program object
    int dirty;      ///< requires a relink
    GLBShader *shaders[GLB_NPROGRAM_SHADERS]; ///< reference to attached shader
    int nuniforms;
    int ninputs;
    int noutputs;
    struct GLBProgramOptions *options;
    struct GLBProgramIdent *textures[GLB_MAX_TEXTURES];
    struct GLBProgramIdent *uniforms[GLB_MAX_UNIFORMS];
    struct GLBProgramIdent *inputs[GLB_MAX_INPUTS];
    struct GLBProgramIdent *outputs[GLB_MAX_OUTPUTS]; //TODO use a linked list instread
};

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
                /*{{{XXX debug*/ printf("attaching shader %i\n", i);/*}}}*/
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
        if(!vshader || !fshader) goto ERROR;
        struct GLBShaderIdent **s_ident = vshader->inputs;
        struct GLBProgramIdent **p_ident = program->inputs;
        while(*s_ident)
        {
            *p_ident = malloc(sizeof(struct GLBProgramIdent));
            (*p_ident)->type = (*s_ident)->type;
            (*p_ident)->location = glGetAttribLocation(program->globj, (*s_ident)->name);
            (*p_ident)->isInt = glbTypeIsInt((*p_ident)->type);
            (*p_ident)->bind = NULL;
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
            (*p_ident)->bind = NULL;
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
                (*p_ident)->bind = NULL;
                (*p_ident)->size = 1; //TODO, parse
                p_ident++;
                s_ident++;
                program->nuniforms++;
            }
        }

    //XXX debug/*{{{*/
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
   /*}}}*/

        program->dirty = 0;
    }

    return 0;
ERROR: //TODO: proper errors
    return 1;
}

/*{{{ Initialization/Deinitializaion*/
GLBProgram *glbCreateProgram (int *errcode_ret)
{
    int errcode = 0;

    GLBProgram *program = malloc(sizeof(GLBProgram));
    GLB_ASSERT(program, GLB_OUT_OF_MEMORY, ERROR);

    program->refcount = 1;
    program->dirty = 0;
    program->ninputs = 0;
    program->noutputs = 0;
    program->nuniforms = 0;

    program->globj = glCreateProgram();
    GLB_ASSERT(program, GLB_UNKNOWN_ERROR, ERROR_CREATE);

    int i;
    for(i = 0; i < GLB_NPROGRAM_SHADERS; i++)
    {
        program->shaders[i] = NULL; 
    }

    //TODO: max this more flexible
    for(i = 0; i < GLB_MAX_TEXTURES; i++)
    {
        program->textures[i] = NULL;
    }

    for(i = 0; i < GLB_MAX_INPUTS; i++)
    {
        program->inputs[i] = NULL;
    }

    for(i = 0; i < GLB_MAX_OUTPUTS; i++)
    {
        program->outputs[i] = NULL;
    }

    for(i = 0; i < GLB_MAX_UNIFORMS; i++)
    {
        program->uniforms[i] = NULL;
    }

    return program;

ERROR_CREATE:
    free(program);
ERROR:
    if(errcode_ret)
    {
        *errcode_ret = errcode;
    }

#ifdef DEBUG
    printf("GLBCreateProgram Error: %s\n", glbErrorString(errcode));
#endif

    return NULL;
}

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

    glDeleteProgram(program->globj);
}

void glbRetainProgram (GLBProgram *program)
{
    program->refcount++;
}

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
int glbProgramAttachNewShaderSourceFile (GLBProgram *program, 
                                           const char *filenm,
                                           enum GLBShaderStage stage)
{
    int errcode = GLB_NO_ERROR;

    GLBShader *shader = glbCreateShaderWithSourceFile(filenm, stage, &errcode);
    GLB_ASSERT(!errcode, errcode, ERROR_SOURCE);

    glbProgramAttachShader(program, shader);
    glbReleaseShader(shader);

    return errcode;

ERROR_SOURCE:
    return errcode;
}

int glbProgramAttachNewShaderSource (GLBProgram *program, 
                                     int len, const char *mem, enum GLBShaderStage stage)
{
    int errcode = 0;
    GLBShader *shader = glbCreateShaderWithSource(len, mem, stage, &errcode);
    glbProgramAttachShader(program, shader);
    glbReleaseShader(shader);

    return 0; //TODO: error handle
}

void glbProgramAttachShader (GLBProgram *program, GLBShader *shader)
{
    glbRetainShader(shader);

    GLint shadertype;
    glGetShaderiv(shader->globj, GL_SHADER_TYPE, &shadertype);

    int index = glbProgramShaderIndex(shadertype);
    if(program->shaders[index])
    {
        glbReleaseShader(shader);
    }
    program->shaders[index] = shader;
    program->dirty = 1;
}

void glbProgramDetachShader (GLBProgram *program, GLBShader *shader)
{
    int i;
    for(i = 0; i < GLB_NPROGRAM_SHADERS; i++)
    {
        if(program->shaders[i] == shader)
        {
            glDetachShader(program->globj, shader->globj);
            glbReleaseShader(shader);
            program->shaders[i] = NULL;
            break;
        }
    }

    program->dirty = 1;
}

void glbProgramDetachShaderStage (GLBProgram *program, enum GLBShaderStage stage)
{
    int index = glbProgramShaderIndex(stage);
    glDetachShader(program->globj, program->shaders[index]->globj);
    glbReleaseShader(program->shaders[index]);
    program->shaders[index] = NULL;
    program->dirty = 1;
}/*}}}*/

/*{{{ Bindables */

static int glbProgramUniformByLocation(GLBProgram *program, int loc, int size, int type, 
                                       int transposed, int sz, void *val)
{
    int errcode = 0;
    int n=1;
    if(glbTypeSizeof(type)) //TODO: workaround to avoid FPEXCEPT for Opaque types without size
    n = sz / glbTypeSizeof(type);

    if(n > size)
    {
        return GLB_INVALID_ARGUMENT;
    }

    glUseProgram(program->globj);
    if(glbTypeIsMatrix(type))
    {
        switch(type)
        {
            case GLB_MAT2:
                glUniformMatrix2fv(loc, n, transposed, val);
                break;
            case GLB_MAT3:
                glUniformMatrix3fv(loc, n, transposed, val);
                break;
            case GLB_MAT4:
                glUniformMatrix4fv(loc, n, transposed, val);
                break;
            case GLB_MAT2x3:
                glUniformMatrix2x3fv(loc, n, transposed, val);
                break;
            case GLB_MAT2x4:
                glUniformMatrix2x4fv(loc, n, transposed, val);
                break;
            case GLB_MAT3x2:
                glUniformMatrix3x2fv(loc, n, transposed, val);
                break;
            case GLB_MAT3x4:
                glUniformMatrix3x4fv(loc, n, transposed, val);
                break;
            case GLB_MAT4x2:
                glUniformMatrix4x2fv(loc, n, transposed, val);
                break;
            case GLB_MAT4x3:
                glUniformMatrix4x3fv(loc, n, transposed, val);
                break;
            default:
                goto ERROR;
        }
    } else if (glbTypeIsScalar(type))
    {
        switch(glbTypeLength(type))
        {
            case 1: // scalar
                if(glbTypeIsFloat(type))
                {
                    glUniform1fv(loc, n, val);
                } else if(glbTypeIsUnsigned(type)) // unsigned int
                {
                    glUniform1uiv(loc, n, val);
                } else if(glbTypeIsInt(type))// signed int
                {
                    glUniform1iv(loc, n, val);
                } else if(glbTypeIsDouble(type))
                {
                    //glUniform1dv(loc, n, val);
                    return GLB_GL_TOO_OLD;
                }
                break;
            case 2:
                if(glbTypeIsFloat(type))
                {
                    glUniform2fv(loc, n, val);
                } else if(glbTypeIsUnsigned(type)) // unsigned int
                {
                    glUniform2uiv(loc, n, val);
                } else if(glbTypeIsInt(type)) // signed int
                {
                    glUniform2iv(loc, n, val);
                } else if(glbTypeIsDouble(type))
                {
                    //glUniform2dv(loc, n, val);
                    return GLB_GL_TOO_OLD;
                }
                break;
            case 3:
                if(glbTypeIsFloat(type))
                {
                    glUniform3fv(loc, n, val);
                } else if(glbTypeIsUnsigned(type)) // unsigned int
                {
                    glUniform3uiv(loc, n, val);
                } else if(glbTypeIsInt(type)) // signed int
                {
                    glUniform3iv(loc, n, val);
                } else if(glbTypeIsDouble(type))
                {
                    //glUniform3dv(loc, n, val);
                    return GLB_GL_TOO_OLD;
                }
                break;
            case 4:
                if(glbTypeIsFloat(type))
                {
                    glUniform4fv(loc, n, val);
                } else if(glbTypeIsUnsigned(type)) // unsigned int
                {
                    glUniform4uiv(loc, n, val);
                } else if(glbTypeIsInt(type)) // signed int
                {
                    glUniform4iv(loc, n, val);
                } else if(glbTypeIsDouble(type))
                {
                    //glUniform4dv(loc, n, val);
                    return GLB_GL_TOO_OLD;
                }
                break;
            default:
                goto ERROR;
        }
    } else if(glbTypeIsOpaque(type))
    {
        GLB_ASSERT(sz == sizeof(GLBTexture), GLB_INVALID_ARGUMENT, ERROR);
        switch(type)
        {
            case GLB_SAMPLER_1D:
            case GLB_SAMPLER_2D:
                //TODO: bind textures to shader
                glActiveTexture(GL_TEXTURE0); //TODO: texture unit allocation
                glBindTexture(((GLBTexture*)val)->target, ((GLBTexture*)val)->globj);
                glUniform1i(loc, 0); //TODO: texture unit alloc
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

    if(program->uniforms[i]->location < 0)
    {
        return GLB_INVALID_ARGUMENT;
    }

    return glbProgramUniformByLocation(program, program->uniforms[i]->location,
                                program->uniforms[i]->size, 
                                program->uniforms[i]->type, true, sz, val);
}

int glbProgramTexture (GLBProgram *program, int shader, int i, GLBTexture *texture)
{
    //TODO: treat i as index of texture instead of uniform??
    return glbProgramUniform(program, shader, i, sizeof(GLBTexture), texture);
}

int glbProgramNamedUniform (GLBProgram *program, char *name, int sz, void *val)
{
    GLuint index;
    int size;
    GLenum type;
    int location;

    glbProgramClean(program);

    //get location
    location = glGetUniformLocation(program->globj, name);

    //get index
    glGetUniformIndices(program->globj, 1, (const GLchar* const*)&name, &index);

    //get type
    glGetActiveUniform(program->globj, index, 0, NULL, &size, &type, NULL);

    //TODO: column major switch?
    glbProgramUniformByLocation(program, location, size, type, true, sz, val);

    return 0; //TODO: error detection
}

int glbProgramNamedTexture (GLBProgram *program, char *name, GLBTexture *texture)
{
    glbProgramClean(program);
    //int location;
    //location = glGetUniformLocation(program->globj, name);

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

/*{{{ Layouts*/
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
}/*}}}*/

/*{{{ Draw */
int glbProgramDraw (GLBProgram *program, GLBBuffer *array)
{
    return glbProgramDrawRange (program, array, array->vdata.count, 0);
}

int glbProgramDrawIndexed (GLBProgram *program, GLBBuffer *array, GLBBuffer *index)
{
    return glbProgramDrawIndexedRange(program, array, index, index->idata.count, 0);
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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index->globj);

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
            GLBTexture *tex = program->textures[i]->bind;
            glActiveTexture(GL_TEXTURE0 + i); 
            glBindTexture(tex->target, tex->globj);
        }
    }

    // bind correct vertex data locations
    // if a layout is given, use the layout, else guess from the program
    if(&array->vdata.layout)
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
            int attrib_sz = glbTypeSizeof(attrib_type);
            glEnableVertexAttribArray(i); 
            //TODO: change float
            if(program->inputs[i]->isInt)
            {
                glVertexAttribIPointer(program->inputs[i]->location, 
                                       attrib_sz, GL_FLOAT,
                                       array->sz / array->nmemb, 
                                       (void*) attrib_offset);
            } else //expected
            {
                glVertexAttribPointer(program->inputs[i]->location, 
                                      attrib_sz, GL_FLOAT, GL_FALSE, 
                                      array->sz / array->nmemb, 
                                      (void*) attrib_offset);
            }
            attrib_offset += attrib_sz;
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
