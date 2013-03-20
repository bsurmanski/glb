/**
 * shader.c
 * @file shader.h
 * GLB
 * @date March 01, 2013
 * @author Brandon Surmanski
 * @brief definition of the GLBShader object interface
 */

#include "glb_private.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MIN(a,b) ((a) > (b) ? (b) : (a))

/**
 * linked list of shader identifiers.
 */
struct GLBShaderIdentList
{
    struct GLBShaderIdent *ident[6];
    struct GLBShaderIdentList *prev;
    struct GLBShaderIdentList *next;
};

/*{{{ Metadata Parsing*/
/**
 * finds the last occurance of any character in accept, searching from the end of the string
 * at str[max]. if the character is not found, return NULL
 * ignores all intermediate NULL bytes between str[0] and str[max]
 */
static const char *rstrpbrk(const char *str, int max, char *accept)
{
    while(max)
    {
        if(strchr(accept, str[max]))
        {
            return &str[max];
        }
        max--;
    }
    return NULL;
}

/**
 * finds the last occurance of any character that is a graphical character. searches
 * backwards from str[max] to str[0]. if character is not found, return NULL
 */
static const char *rstralnum(const char *str, int max)
{
    while(max)
    {
        if(isalnum(str[max]))
        {
            return &str[max];
        }
        max--;
    }
    return NULL;
}

/**
 * reverse strchr. Searches backwards from str[max] to str[0], finding *last*
 * occurance of c
 */
static const char *rstrchr(const char *str, int max, char c)
{
    while(max)
    {
        if(str[max] == c)
        {
            return &str[max];
        }
        max--;
    }
    return NULL;
}

/**
 * strndup is not ANSI, need to reimpliment...
 */
static char *strndup(const char *s, size_t n)
{
    int end = 0;
    while(s[end] && end < n) end++; //find null byte or end of string
    if(!end) return NULL;
    char *ret = malloc(end+1);
    memcpy(ret, s, end);
    ret[end] = '\0';
    return ret;
}

/*
static int getArraySize(char *arrayStart, int max)
{

}*/

//TODO: allow for default values
//TODO: parse comma seperated initialization (eg uniform bool a,b,c;)
static struct GLBShaderIdent *get_ident(int len, const char *str)
{
    char *rbrk = " \f\n\r\t\v/"; //space characters and comment delimeter
    const char *end = strchr(str, ';'); // find end of line
    const char *isArray = rstrchr(str, end - str, '[');
    if(isArray) end = &isArray[-1]; //TODO: work around for arrays
    const char *end_name = rstralnum(str, end - str) + 1;
    const char *tmp;
    while((tmp = rstrchr(str, end_name - str, '=')))
    {
        end_name = rstralnum(str, tmp - str) + 1;
    }
    const char *name = rstrpbrk(str, end_name - str - 1, rbrk) + 1; //start of identifier string
    const char *end_type = rstralnum(str, name - str - 1) + 1;
    const char *type = rstrpbrk(str, (end_type - str - 1), rbrk) + 1; // find start of type id
    //(!name || !type); //XXX error invalid glsl? invalid parser?

    struct GLBShaderIdent *ident = malloc(sizeof(struct GLBShaderIdent));
    ident->type = glbStringType(end_type - type, type);
    ident->size = 1;
    ident->name = strndup(name, end_name - name); //TODO: reimpliment i guess...
    return ident;
}

void glbShaderMetadata(GLBShader *shader, int max, const char *str)
{
    const char *start = str;

    while(max && *str)
    {
        //parse inputs
        if(!strncmp(str, "in", 2) && isspace(str[2]) && (str == start || isspace(str[-1])))
        {
            shader->inputs[shader->ninputs] = get_ident(max, str);
            shader->ninputs++;
#ifdef DEBUG/*{{{*/
            printf("input found: %s : %s\n",
                    shader->inputs[shader->ninputs-1]->name,
                    glbTypeString(shader->inputs[shader->ninputs-1]->type));
#endif/*}}}*/
        }

        //parse outputs
        if(!strncmp(str, "out", 3) && isspace(str[3]) && (str == start || isspace(str[-1])))
        {
            shader->outputs[shader->noutputs] = get_ident(max, str);
            shader->noutputs++;
#ifdef DEBUG/*{{{*/
            printf("output found: %s : %s\n",
                    shader->outputs[shader->noutputs-1]->name,
                    glbTypeString(shader->outputs[shader->noutputs-1]->type));
#endif/*}}}*/
        }

        //parse uniforms
        //TODO: parse arrays
        if(!strncmp(str, "uniform", 7) && isspace(str[7]) && (str == start || isspace(str[-1])))
        {
            shader->uniforms[shader->nuniforms] = get_ident(max, str);
            if(glbTypeIsOpaque(shader->uniforms[shader->nuniforms]->type))
            {
                shader->nopaques++;
            }
            shader->nuniforms++;

#ifdef DEBUG/*{{{*/
            printf("uniform found: %s : %s\n",
                    shader->uniforms[shader->nuniforms-1]->name,
                    glbTypeString(shader->uniforms[shader->nuniforms-1]->type));
#endif/*}}}*/
        }

        max--;
        str++;
    }
}/*}}}*/

static char *glbFileToString(const char *filenm, int *out_sz, int *out_err)
{
    char *filestr = NULL;
    int err;
    int errcode = 0;

    FILE *file = fopen(filenm, "rb");
    GLB_ASSERT(file, GLB_FILE_NOT_FOUND, ERROR_FOPEN);

    err = fseek(file, 0L, SEEK_END);
    GLB_ASSERT(!err, GLB_READ_ERROR, ERROR_FILE);

    size_t fsz = ftell(file);
    filestr = malloc(fsz + 1);
    GLB_ASSERT(filestr, GLB_OUT_OF_MEMORY, ERROR_ALLOC);

    fseek(file, 0L, SEEK_SET);
    int nread = fread(filestr, 1, fsz, file);
    GLB_ASSERT(fsz == nread, GLB_READ_ERROR, ERROR_READ);

    filestr[fsz] = '\0';

    if(out_sz)
    {
        *out_sz = fsz;
    }

    return filestr;

ERROR_READ:
    free(filestr);
    filestr = NULL;
ERROR_ALLOC:
ERROR_FILE:
    fclose(file);
ERROR_FOPEN:
    if(out_err)
    {
        *out_err = errcode;
    }
    return NULL;
}

GLBShader *glbCreateShaderWithSourceFile(const char *filenm,
                                         enum GLBShaderStage stage,
                                         int *errcode_ret)
{
    int errcode;
    GLBShader *shader = NULL;

    //TODO: assert stage is allowed in current GL version

    GLB_ASSERT(filenm, GLB_INVALID_ARGUMENT, ERROR);

    char *filestr = glbFileToString(filenm, NULL, &errcode);
    GLB_ASSERT(filestr,errcode,ERROR_READ);

    shader = glbCreateShaderWithSource(-1, filestr, stage, errcode_ret);
    free(filestr);

    GLB_SET_ERROR(GLB_SUCCESS);
    return shader;

ERROR_READ:
ERROR:
    GLB_SET_ERROR(errcode);

#ifdef DEBUG
    printf("GLBCreateShader Error: %s\n", glbErrorString(errcode));
#endif

    return NULL;
}


/**
 * if 'len' is less than zero, string is NULL terminated
 */
GLBShader *glbCreateShaderWithSource(int len,
                                     const char *mem,
                                     enum GLBShaderStage stage,
                                     int *errcode_ret)
{
    int errcode = 0;
    GLBShader *shader = NULL;

    int major;
    int minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    GLB_ASSERT(stage != GLB_VERTEX_SHADER ||
              (major > 2 || (major == 2 && minor >= 1)), GLB_GL_TOO_OLD, ERROR);
    GLB_ASSERT(stage != GLB_TESS_CONTROL_SHADER || (major >= 4), GLB_GL_TOO_OLD, ERROR);
    GLB_ASSERT(stage != GLB_TESS_EVALUATION_SHADER || (major >= 4), GLB_GL_TOO_OLD, ERROR);
    GLB_ASSERT(stage != GLB_GEOMETRY_SHADER ||
              (major > 3 || (major == 3 && minor >= 1)), GLB_GL_TOO_OLD, ERROR);
    GLB_ASSERT(stage != GLB_FRAGMENT_SHADER ||
              (major > 2 || (major == 2 && minor >= 1)), GLB_GL_TOO_OLD, ERROR);
    GLB_ASSERT(stage == GLB_VERTEX_SHADER || stage == GLB_FRAGMENT_SHADER ||
               stage == GLB_GEOMETRY_SHADER || stage == GLB_TESS_CONTROL_SHADER ||
               stage == GLB_TESS_EVALUATION_SHADER, GLB_INVALID_ARGUMENT, ERROR);

    GLB_ASSERT(mem, GLB_INVALID_ARGUMENT, ERROR);

    shader = malloc(sizeof(GLBShader));
    GLB_ASSERT(shader, GLB_OUT_OF_MEMORY, ERROR_ALLOC);

    shader->globj = glCreateShader((GLenum) stage);
    GLB_ASSERT(shader->globj, GLB_INVALID_ARGUMENT, ERROR_CREATE);
    shader->refcount = 1;

    glShaderSource(shader->globj, 1, (const GLchar**)&mem, &len);

    glCompileShader(shader->globj);

    GLint cstatus;
    glGetShaderiv(shader->globj, GL_COMPILE_STATUS, &cstatus);

    // Error compiling
#ifdef DEBUG
    if(cstatus != GL_TRUE)
    {
        int bufsz = 512;
        GLchar *log = malloc(bufsz);
        glGetShaderInfoLog(shader->globj, bufsz, 0, log);
        printf("GLBShader compile error: %s\n\n", log);
        free(log);
    }
#endif
    GLB_ASSERT(cstatus == GL_TRUE, GLB_COMPILE_ERROR, ERROR_COMPILE);

    int i;
    for(i = 0; i < 16; i++) //TODO: variable 16
    {
        shader->inputs[i] = NULL;
        shader->outputs[i] = NULL;
        shader->uniforms[i] = NULL;
    }
    shader->ninputs = 0;
    shader->noutputs = 0;
    shader->nuniforms = 0;
    shader->nopaques = 0;

    glbShaderMetadata(shader, len, mem);

    GLB_SET_ERROR(GLB_SUCCESS);

    return shader;

ERROR_COMPILE:
    glDeleteShader(shader->globj);
    shader->globj = 0;
ERROR_CREATE:
    free(shader);
ERROR_ALLOC:
ERROR:
    GLB_SET_ERROR(errcode);

#ifdef DEBUG
    printf("GLBCreateShader Error: %s\n", glbErrorString(errcode));
#endif

    return NULL;
}

void glbDeleteShader (GLBShader *shader)
{
    glDeleteShader(shader->globj);
}

void glbRetainShader (GLBShader *shader)
{
    shader->refcount++;
}

void glbReleaseShader (GLBShader *shader)
{
    shader->refcount--;
    if(shader->refcount <= 0)
    {
        glbDeleteShader(shader);
    }
}
