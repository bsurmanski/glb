/**
 * glb.h
 * GLB
 * March 06, 2013
 * Brandon Surmanski
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "glb.h"

#define MIN(a,b) ((a) > (b) ? (b) : (a))

struct TypeAssociation
{
    const char *str;
    int len;
    int type;
};

static struct TypeAssociation types[] = 
{
    // scalar
    {"bool", 4, GLB_BOOL},
    {"byte", 4, GLB_BYTE},
    {"short", 5, GLB_SHORT},
    {"int", 3, GLB_INT},
    {"float", 5, GLB_FLOAT},
    {"double", 6, GLB_DOUBLE},
    {"half", 4, GLB_HALF_FLOAT}, // NOTE: not an actual GLSL type
    {"ubyte", 5, GLB_UBYTE},
    {"ushort", 6, GLB_USHORT},
    {"uint", 4, GLB_UINT},

    //vector
    {"vec2", 4, GLB_VEC2},
    {"vec3", 4, GLB_VEC3},
    {"vec4", 4, GLB_VEC4},
    {"ivec2", 5, GLB_IVEC2},
    {"ivec3", 5, GLB_IVEC3},
    {"ivec4", 5, GLB_IVEC4},
    {"uivec2", 6, GLB_UIVEC2},
    {"uivec3", 6, GLB_UIVEC3},
    {"uivec4", 6, GLB_UIVEC4},
    {"bvec2", 5, GLB_BVEC2},
    {"bvec3", 5, GLB_BVEC3},
    {"bvec4", 5, GLB_BVEC4},
    {"dvec2", 5, GLB_DVEC2},
    {"dvec3", 5, GLB_DVEC3},
    {"dvec4", 5, GLB_DVEC4},

    //matrices
    {"mat2x3", 6, GLB_MAT2x3},
    {"mat2x4", 6, GLB_MAT2x4},
    {"mat3x2", 6, GLB_MAT3x2},
    {"mat3x4", 6, GLB_MAT3x4},
    {"mat4x2", 6, GLB_MAT4x2},
    {"mat4x3", 6, GLB_MAT4x3},
    {"mat2", 4, GLB_MAT2},
    {"mat3", 4, GLB_MAT3},
    {"mat4", 4, GLB_MAT4},

    //Opaque
    {"sampler1D", 9, GLB_SAMPLER_1D},
    {"sampler2D", 9, GLB_SAMPLER_2D},
    {"sampler3D", 9, GLB_SAMPLER_3D},
    {"samplerCube", 9, GLB_SAMPLER_CUBE},
    {"samplerCubeShadow", 9, GLB_SAMPLER_CUBE_SHADOW},
    {NULL, 0, 0},
};

/*{{{ Type info*/
const char *const glbTypeString(int type)
{
    switch(type)
    {
        //scalar types
    case GLB_BOOL:
        return "bool";
    case GLB_BYTE:
        return "byte";
    case GLB_SHORT:
        return "short";
    case GLB_INT:
        return "int";
    case GLB_FLOAT:
        return "float";
    case GLB_DOUBLE:
        return "double";
    case GLB_HALF_FLOAT:
        return "half";
    case GLB_UNSIGNED_BYTE:
        return "ubyte";
    case GLB_UNSIGNED_SHORT:
        return "ushort";
    case GLB_UNSIGNED_INT:
        return "uint";

        //vector types
    case GLB_FLOAT_VEC2:
        return "vec2";
    case GLB_FLOAT_VEC3:
        return "vec3";
    case GLB_FLOAT_VEC4:
        return "vec4";
    case GLB_IVEC2:
        return "ivec2";
    case GLB_UIVEC2:
        return "uivec2";
    case GLB_IVEC3:
        return "ivec3";
    case GLB_UIVEC3:
        return "uivec3";
    case GLB_IVEC4:
        return "ivec4";
    case GLB_UIVEC4:
        return "uivec4";
    case GLB_BOOL_VEC2:
        return "bvec2";
    case GLB_BOOL_VEC3:
        return "bvec3";
    case GLB_BOOL_VEC4:
        return "bvec4";
    case GLB_DVEC2:
        return "dvec2";
    case GLB_DVEC3:
        return "dvec3";
    case GLB_DVEC4:
        return "dvec4";

        //matrix types
    case GLB_MAT2:
        return "mat2";
    case GLB_MAT3:
        return "mat3";
    case GLB_MAT4:
        return "mat4";
    case GLB_MAT2x3:
        return "mat2x3";
    case GLB_MAT2x4:
        return "mat2x4";
    case GLB_MAT3x2:
        return "mat3x2";
    case GLB_MAT3x4:
        return "mat3x4";
    case GLB_MAT4x2:
        return "mat4x2";
    case GLB_MAT4x3:
        return "mat4x3";

        //opaque types

    case GLB_SAMPLER_2D:
        return "sampler2D";

    default:
        return NULL;
    }
}

int glbStringType(int len, const char *const str)
{
    int min;
    struct TypeAssociation *type_ptr = types;
    while(type_ptr->len)
    {
        min = MIN(len, type_ptr->len);
        //TODO: allow parsing without identical lengths
        if(len == type_ptr->len && !strncmp(type_ptr->str, str, min))
        {
            return type_ptr->type;
        }
        type_ptr++;
    }
    return 0;
}

int glbTypeSizeof(int type)
{
    switch(type)
    {
        //scalar types
    case GLB_BOOL:
        return sizeof(bool);
    case GLB_BYTE:
        return sizeof(int8_t);
    case GLB_SHORT:
        return sizeof(int16_t);
    case GLB_INT:
        return sizeof(int32_t);
    case GLB_FLOAT:
        return sizeof(float);
    case GLB_DOUBLE:
        return sizeof(double);
    case GLB_HALF_FLOAT:
        return (sizeof(float) / 2);
    case GLB_UNSIGNED_BYTE:
        return sizeof(uint8_t);
    case GLB_UNSIGNED_SHORT:
        return sizeof(uint16_t);
    case GLB_UNSIGNED_INT:
        return sizeof(uint32_t);

        //vector types
    case GLB_FLOAT_VEC2:
        return sizeof(float) * 2;
    case GLB_FLOAT_VEC3:
        return sizeof(float) * 3;
    case GLB_FLOAT_VEC4:
        return sizeof(float) * 4;
    case GLB_IVEC2:
    case GLB_UIVEC2:
        return sizeof(int) * 2;
    case GLB_IVEC3:
    case GLB_UIVEC3:
        return sizeof(int) * 3;
    case GLB_IVEC4:
    case GLB_UIVEC4:
        return sizeof(int) * 4;
    case GLB_BOOL_VEC2: //TODO: check actual size
        return sizeof(uint8_t) * 2;
    case GLB_BOOL_VEC3:
        return sizeof(uint8_t) * 2;
    case GLB_BOOL_VEC4:
        return sizeof(uint8_t) * 2;
    case GLB_DVEC2:
        return sizeof(double) * 2;
    case GLB_DVEC3:
        return sizeof(double) * 3;
    case GLB_DVEC4:
        return sizeof(double) * 4;

        //matrix types
    case GLB_MAT2:
        return sizeof(float) * 2 * 2;
    case GLB_MAT3:
        return sizeof(float) * 3 * 3;
    case GLB_MAT4:
        return sizeof(float) * 4 * 4;
    case GLB_MAT2x3:
        return sizeof(float) * 2 * 3;
    case GLB_MAT2x4:
        return sizeof(float) * 2 * 4;
    case GLB_MAT3x2:
        return sizeof(float) * 3 * 2;
    case GLB_MAT3x4:
        return sizeof(float) * 3 * 4;
    case GLB_MAT4x2:
        return sizeof(float) * 4 * 2;
    case GLB_MAT4x3:
        return sizeof(float) * 4 * 3;

    default:
        return 0;
    }
}

int glbTypeLength(int type)
{
    switch(type)
    {
        case GLB_VEC2:
        case GLB_IVEC2:
        case GLB_UIVEC2:
        case GLB_DVEC2:
        case GLB_BVEC2:
            return 2;
        case GLB_VEC3:
        case GLB_IVEC3:
        case GLB_UIVEC3:
        case GLB_DVEC3:
        case GLB_BVEC3:
            return 3;
        case GLB_VEC4:
        case GLB_IVEC4:
        case GLB_UIVEC4:
        case GLB_DVEC4:
        case GLB_BVEC4:
            return 4;
    }
    return 1;
}

bool glbTypeIsInt(int type)
{
    return (type == GLB_BYTE || type == GLB_UNSIGNED_BYTE ||
            type == GLB_SHORT || type == GLB_UNSIGNED_SHORT ||
            type == GLB_INT || type == GLB_UNSIGNED_INT ||
            type == GLB_IVEC2 || type == GLB_IVEC3 || type == GLB_IVEC4 ||
            type == GLB_UIVEC2 || type == GLB_UIVEC3 || type == GLB_UIVEC4);
}

bool glbTypeIsFloat(int type)
{
    return (type == GLB_FLOAT || glbTypeIsVec(type) ||
            type == GLB_MAT2 || type == GLB_MAT3 || type == GLB_MAT4 || 
            type == GLB_MAT2x3 || type == GLB_MAT2x4 || type == GLB_MAT3x2 ||
            type == GLB_MAT3x4 || type == GLB_MAT4x2 || type == GLB_MAT4x3);
}

bool glbTypeIsDouble(int type)
{
    return type == GLB_DOUBLE || glbTypeIsDVec(type);
}

bool glbTypeIsUnsigned(int type)
{
    return (type == GLB_UNSIGNED_BYTE || type == GLB_UNSIGNED_SHORT ||
            type == GLB_UNSIGNED_INT || glbTypeIsUIVec(type));
}

bool glbTypeIsScalar(int type)
{
    return type == GLB_BOOL || type == GLB_BYTE || type == GLB_UNSIGNED_BYTE ||
           type == GLB_SHORT || type == GLB_UNSIGNED_SHORT || type == GLB_INT ||
           type == GLB_UNSIGNED_INT || type == GLB_FLOAT || type == GLB_DOUBLE ||
           type == GLB_HALF_FLOAT;
}

bool glbTypeIsVector(int type)
{
    return glbTypeIsVec(type) || glbTypeIsIVec(type) || glbTypeIsUIVec(type) ||
           glbTypeIsDVec(type) || glbTypeIsBVec(type);
}

bool glbTypeIsMatrix(int type)
{
    return (type == GLB_MAT2 || type == GLB_MAT3 || type == GLB_MAT4 ||
            type == GLB_MAT2x3 || type == GLB_MAT2x4 || type == GLB_MAT3x2 ||
            type == GLB_MAT3x4 || type == GLB_MAT4x2 || type == GLB_MAT4x3);
}

bool glbTypeIsOpaque(int type)
{
    return (type == GLB_SAMPLER_1D || type == GLB_SAMPLER_2D || type == GLB_SAMPLER_3D ||
            type == GLB_SAMPLER_CUBE || type == GLB_SAMPLER_1D_SHADOW || 
            type == GLB_SAMPLER_2D_SHADOW || type == GLB_SAMPLER_CUBE_SHADOW);
}

//specific vector types
bool glbTypeIsUIVec(int type)
{
    return type == GLB_UIVEC2 || type == GLB_UIVEC3 || type == GLB_UIVEC4;
}
bool glbTypeIsVec(int type)
{
    return type == GLB_VEC2 || type == GLB_VEC3 || type == GLB_VEC4;
}

bool glbTypeIsDVec(int type)
{
    return type == GLB_DVEC2 || type == GLB_DVEC3 || type == GLB_DVEC4;
}

bool glbTypeIsIVec(int type)
{
    return type == GLB_IVEC2 || type == GLB_IVEC3 || type == GLB_IVEC4;
}

bool glbTypeIsBVec(int type)
{
    return type == GLB_BVEC2 || type == GLB_BVEC3 || type == GLB_BVEC4;
}

int glbTypeToScalar(int type)
{
    switch(type)
    {
        case GLB_VEC2:
        case GLB_VEC3:
        case GLB_VEC4:
            return GLB_FLOAT;

        case GLB_IVEC2:
        case GLB_IVEC3:
        case GLB_IVEC4:
            return GLB_INT;

        case GLB_UIVEC2:
        case GLB_UIVEC3:
        case GLB_UIVEC4:
            return GLB_UINT;

        case GLB_DVEC2:
        case GLB_DVEC3:
        case GLB_DVEC4:
            return GLB_DOUBLE;

        case GLB_BVEC2:
        case GLB_BVEC3:
        case GLB_BVEC4:
            return GLB_BOOL;
        default:
            return 0;
    }
}

int glbTypeToUnsigned(int type)
{
    switch(type)
    {
        case GLB_BYTE:
        case GLB_BOOL:
            return GLB_UNSIGNED_BYTE;
        case GLB_SHORT:
            return GLB_UNSIGNED_SHORT;
        case GLB_INT:
            return GLB_INT;
        case GLB_IVEC2:
            return GLB_UIVEC2;
        case GLB_IVEC3:
            return GLB_UIVEC3;
        case GLB_IVEC4:
            return GLB_UIVEC4;
    }
    return type; // if type not unsign-able, return self
}

int glbTypeToSigned(int type)
{
    switch(type)
    {
        case GLB_UNSIGNED_BYTE:
            return GLB_BYTE;
        case GLB_UNSIGNED_SHORT:
            return GLB_SHORT;
        case GLB_UNSIGNED_INT:
            return GLB_INT;
        case GLB_UIVEC2:
            return GLB_IVEC2;
        case GLB_UIVEC3:
            return GLB_IVEC3;
        case GLB_UIVEC4:
            return GLB_IVEC4;
    }
    return type; // if type not signable, return self
}/*}}}*/

/*{{{ Error info*/
const char *const glbErrorString(int error)
{
    switch(error)
    {
        case GLB_NO_ERROR:
            return "no error";
        case GLB_FILE_NOT_FOUND:
            return "file not found";
        case GLB_READ_ERROR:
            return "read error";
        case GLB_OUT_OF_MEMORY:
            return "out of memory";
        case GLB_INVALID_ARGUMENT:
            return "invalid argument";
        case GLB_COMPILE_ERROR:
            return "compile error";
        case GLB_UNKNOWN_ERROR:
            return "unknown error";
        case GLB_UNIMPLEMENTED:
            return "unimplemented feature";
        case GLB_GL_TOO_OLD:
            return "OpenGL too old for feature";
        default:
        return NULL;
    }
}/*}}}*/
