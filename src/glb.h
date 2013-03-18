/**
 * glb.h
 * GLB
 * March 01, 2013
 * Brandon Surmanski
 */

#ifndef _GLB_H
#define _GLB_H

#include <stdbool.h>
#include <stdio.h>

#include <GL/gl.h>
#include <GL/glext.h>

#include "buffer.h"
#include "sampler.h"
#include "texture.h"
#include "shader.h"
#include "program.h"

#define GLB_ASSERT(x,err,jmp) \
    do { \
        if(!(x)) { \
            errcode = err; \
            goto jmp; \
        } \
    } while(0);

typedef struct GLBVertexLayout
{
    unsigned int size;
    unsigned int type;
    unsigned int normalized;
    unsigned int stride;
    unsigned int offset;
} GLBVertexLayout;

const char *const glbTypeString(int type);
int glbStringType(int len, const char *const str);
int glbTypeSizeof(int type);
int glbTypeLength(int type);
bool glbTypeIsInt(int type);
bool glbTypeIsFloat(int type);
bool glbTypeIsDouble(int type);
bool glbTypeIsUnsigned(int type);
bool glbTypeIsScalar(int type);
bool glbTypeIsVector(int type);
bool glbTypeIsMatrix(int type);
bool glbTypeIsOpaque(int type);

//specific vector types
bool glbTypeIsUIVec(int type);
bool glbTypeIsVec(int type);
bool glbTypeIsDVec(int type);
bool glbTypeIsIVec(int type);
bool glbTypeIsBVec(int type);

int glbTypeToScalar(int type);
int glbTypeToUnsigned(int type);
int glbTypeToSigned(int type);

const char *const glbErrorString(int error);

enum GLBDrawOptions
{
    GLB_NO_DRAW_OPTIONS = 0,
    GLB_OPTIONS_RESET   = 1,
    GLB_POLYGON_MODE    = 2,
    GLB_CULL_MODE       = 3,
    GLB_PRIMATIVE_MODE  = 4, ///< uses enum GLBPrimativeTypes
    GLB_FRONT_FACE_MODE = 5, // TODO: create enum
    GLB_LINE_WIDTH      = 6,
    GLB_POINT_SIZE      = 7,
    GLB_PROVOKING_VERTEX = 8, // TODO: create enum
};

enum GLBPrimativeTypes
{
    GLB_POINTS                      = GL_POINTS,
    GLB_LINE_STRIP                  = GL_LINE_STRIP,
    GLB_LINE_LOOP                   = GL_LINE_LOOP,
    GLB_LINES                       = GL_LINES,
    GLB_TRIANGLES                   = GL_TRIANGLES,
    GLB_TRIANGLE_STRIP              = GL_TRIANGLE_STRIP,
    GLB_TRIANGLE_FAN                = GL_TRIANGLE_FAN,
    GLB_LINES_ADJACENCY             = GL_LINES_ADJACENCY,
    GLB_LINE_STRIP_ADJACENCY        = GL_LINE_STRIP_ADJACENCY,
    GLB_TRIANGLES_ADJACENCY         = GL_TRIANGLES_ADJACENCY,
    GLB_TRIANGLE_STRIP_ADJACENCY    = GL_TRIANGLE_STRIP_ADJACENCY,
    GLB_PATCHES                     = GL_PATCHES,
};

enum GLBError
{
    GLB_NO_ERROR = 0,
    GLB_FILE_NOT_FOUND,
    GLB_READ_ERROR,
    GLB_WRITE_ERROR,
    GLB_OUT_OF_MEMORY,
    GLB_INVALID_ARGUMENT,
    GLB_COMPILE_ERROR,
    GLB_UNKNOWN_ERROR,
    GLB_UNIMPLEMENTED,
    GLB_GL_TOO_OLD,
};

enum GLBScalar
{
    GLB_BOOL    = GL_BOOL, //TODO: should this be here? messes with buffer layout?
    GLB_BYTE    = GL_BYTE,
    GLB_SHORT   = GL_SHORT,
    GLB_INT     = GL_INT,
    GLB_FLOAT   = GL_FLOAT,
    GLB_DOUBLE  = GL_DOUBLE,
    GLB_HALF_FLOAT      = GL_HALF_FLOAT,
    GLB_UNSIGNED_BYTE   = GL_UNSIGNED_BYTE,
    GLB_UNSIGNED_SHORT  = GL_UNSIGNED_SHORT,
    GLB_UNSIGNED_INT    = GL_UNSIGNED_INT,
    GLB_UBYTE   = GL_UNSIGNED_BYTE,
    GLB_USHORT  = GL_UNSIGNED_SHORT,
    GLB_UINT    = GL_UNSIGNED_INT
};

enum GLBVector
{
    GLB_VEC2    = GL_FLOAT_VEC2,
    GLB_VEC3    = GL_FLOAT_VEC3,
    GLB_VEC4    = GL_FLOAT_VEC4,
    GLB_FLOAT_VEC2    = GL_FLOAT_VEC2,
    GLB_FLOAT_VEC3    = GL_FLOAT_VEC3,
    GLB_FLOAT_VEC4    = GL_FLOAT_VEC4,

    GLB_IVEC2    = GL_INT_VEC2,
    GLB_IVEC3    = GL_INT_VEC3,
    GLB_IVEC4    = GL_INT_VEC4,
    GLB_INT_VEC2 = GL_INT_VEC2,
    GLB_INT_VEC3 = GL_INT_VEC3,
    GLB_INT_VEC4 = GL_INT_VEC4,

    GLB_UIVEC2    = GL_UNSIGNED_INT_VEC2,
    GLB_UIVEC3    = GL_UNSIGNED_INT_VEC3,
    GLB_UIVEC4    = GL_UNSIGNED_INT_VEC4,
    GLB_UNSIGNED_INT_VEC2 = GL_UNSIGNED_INT_VEC2,
    GLB_UNSIGNED_INT_VEC3 = GL_UNSIGNED_INT_VEC3,
    GLB_UNSIGNED_INT_VEC4 = GL_UNSIGNED_INT_VEC4,

    GLB_DVEC2   = GL_DOUBLE_VEC2,
    GLB_DVEC3   = GL_DOUBLE_VEC3,
    GLB_DVEC4   = GL_DOUBLE_VEC4,
    GLB_DOUBLE_VEC2   = GL_DOUBLE_VEC2,
    GLB_DOUBLE_VEC3   = GL_DOUBLE_VEC3,
    GLB_DOUBLE_VEC4   = GL_DOUBLE_VEC4,

    GLB_BVEC2 = GL_BOOL_VEC2,
    GLB_BVEC3 = GL_BOOL_VEC3,
    GLB_BVEC4 = GL_BOOL_VEC4,
    GLB_BOOL_VEC2 = GL_BOOL_VEC2,
    GLB_BOOL_VEC3 = GL_BOOL_VEC3,
    GLB_BOOL_VEC4 = GL_BOOL_VEC4,
};

enum GLBMatrix
{
    GLB_MAT2    = GL_FLOAT_MAT2,   
    GLB_MAT3    = GL_FLOAT_MAT3,  
    GLB_MAT4    = GL_FLOAT_MAT4, 
    GLB_FLOAT_MAT2    = GL_FLOAT_MAT2,   
    GLB_FLOAT_MAT3    = GL_FLOAT_MAT3,  
    GLB_FLOAT_MAT4    = GL_FLOAT_MAT4, 
    GLB_MAT2x3  = GL_FLOAT_MAT2x3,
    GLB_MAT2x4  = GL_FLOAT_MAT2x4,
    GLB_MAT3x2  = GL_FLOAT_MAT3x2,
    GLB_MAT3x4  = GL_FLOAT_MAT3x4,
    GLB_MAT4x2  = GL_FLOAT_MAT4x2,
    GLB_MAT4x3  = GL_FLOAT_MAT4x3,
    GLB_FLOAT_MAT2x3  = GL_FLOAT_MAT2x3,
    GLB_FLOAT_MAT2x4  = GL_FLOAT_MAT2x4,
    GLB_FLOAT_MAT3x2  = GL_FLOAT_MAT3x2,
    GLB_FLOAT_MAT3x4  = GL_FLOAT_MAT3x4,
    GLB_FLOAT_MAT4x2  = GL_FLOAT_MAT4x2,
    GLB_FLOAT_MAT4x3  = GL_FLOAT_MAT4x3,
};

//TODO: add all types from GLSL Opaque list
enum GLBOpaque
{
    GLB_SAMPLER_1D = GL_SAMPLER_1D,
    GLB_SAMPLER_2D = GL_SAMPLER_2D,
    GLB_SAMPLER_3D = GL_SAMPLER_3D,
    GLB_SAMPLER_CUBE = GL_SAMPLER_CUBE,
    GLB_SAMPLER_1D_SHADOW = GL_SAMPLER_1D_SHADOW,
    GLB_SAMPLER_2D_SHADOW = GL_SAMPLER_2D_SHADOW,
    GLB_SAMPLER_CUBE_SHADOW = GL_SAMPLER_CUBE_SHADOW,
};

#endif
