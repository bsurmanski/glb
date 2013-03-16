/**
 * program.h
 * GLB
 * March 01, 2013
 * Brandon Surmanski
 */

#ifndef _GLB_PROGRAM_H
#define _GLB_PROGRAM_H

#define GLB_NPROGRAM_SHADERS 5

struct GLBShader;
struct GLBTexture;
struct GLBVertexLayout;
struct GLBShaderIdent;

struct GLBProgramOptions;
struct GLBProgramIdent;

#define GLB_MAX_TEXTURES    16
#define GLB_MAX_UNIFORMS    16
#define GLB_MAX_INPUTS      16
#define GLB_MAX_OUTPUTS     16

typedef struct GLBProgram GLBProgram;

// Initialization/Deinitialization

GLBProgram *glbCreateProgram              (int *errcode_ret); //TODO: really need errcode?
void        glbDeleteProgram              (GLBProgram *program);
void        glbRetainProgram              (GLBProgram *program);
void        glbReleaseProgram             (GLBProgram *program);

// Options

int         glbProgramOption              (GLBProgram *program, int option, int value);

// Shaders

int         glbProgramAttachNewShaderSourceFile (GLBProgram *program, 
                                               const char *filenm,
                                               enum GLBShaderStage stage);

int         glbProgramAttachNewShaderSource (GLBProgram *program, 
                                         int len, const char *mem,
                                         enum GLBShaderStage stage); 

void        glbProgramAttachShader        (GLBProgram *program, GLBShader *shader);
void        glbProgramDetachShader        (GLBProgram *program, GLBShader *shader);
void        glbProgramDetachShaderStage   (GLBProgram *program, enum GLBShaderStage stage);

// Bindables

int         glbProgramUniform             (GLBProgram *program, int shader, int i, int sz, void *val);
int         glbProgramTexture             (GLBProgram *program, int shader, int i, GLBTexture *texture);
int         glbProgramNamedUniform        (GLBProgram *program, char *name, int sz, void *val);
int         glbProgramNamedTexture        (GLBProgram *program, char *name, GLBTexture *texture);
int         glbProgramUniformBuffer       (GLBProgram *program, char *blocknm, GLBBuffer *buffer);
int         glbProgramUniformBufferRange  (GLBProgram *program, char *blocknm, 
                                           int offset, int size, GLBBuffer *buffer);

// Layouts

int         glbProgramUseVertexLayout     (GLBProgram *program, int n, struct GLBVertexLayout *layout);

int         glbProgramLayout              (GLBProgram *program, 
                                           int noutputs, char **outputs);

int         glbProgramInputLayout         (GLBProgram *program, int n, char **inputs);
int         glbProgramOutputLayout        (GLBProgram *program, int n, char **outputs);

//Draw

int         glbProgramDraw                (GLBProgram *program, 
                                           GLBBuffer *array);

int         glbProgramDrawIndexed         (GLBProgram *program, 
                                           GLBBuffer *array,
                                           GLBBuffer *index);

int         glbProgramDrawRange           (GLBProgram *program, 
                                           GLBBuffer *array, 
                                           int offset, int count);

int         glbProgramDrawIndexedRange    (GLBProgram *program, 
                                           GLBBuffer *array, 
                                           GLBBuffer *index,
                                           int offset, int count);

//TODO: allow non-triangles, wireframe, other options

#endif
