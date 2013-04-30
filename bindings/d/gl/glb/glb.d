module gl.glb.glb;

import c.gl.glb.glb;

public import c.gl.glb.glb_types;
public import gl.glb.buffer;
public import gl.glb.framebuffer;
public import gl.glb.sampler;
public import gl.glb.texture;
public import gl.glb.shader;
public import gl.glb.program;

alias glbTypeString typeString;
alias glbStringType stringType;
alias glbTypeSizeof typeSizeof;
alias glbTypeLength typeLength;
alias glbTypeIsInt typeIsInt;
alias glbTypeIsFloat typeIsFloat;
alias glbTypeIsDouble typeIsDouble;
alias glbTypeIsUnsigned typeIsUnsigned;
alias glbTypeIsScalar typeIsScalar;
alias glbTypeIsVector typeIsVector;
alias glbTypeIsMatrix typeIsMatrix;
alias glbTypeIsOpaque typeIsOpaque;

//specific vector types
alias glbTypeIsUIVec typeIsUIVec;
alias glbTypeIsVec typeIsVec;
alias glbTypeIsDVec typeIsDVec;
alias glbTypeIsIVec typeIsIVec;
alias glbTypeIsBVec typeIsBVec;

alias glbTypeToScalar typeToScalar;
alias glbTypeToUnsigned typeToUnsigned;
alias glbTypeToSigned typeToSigned;

// features
alias glbCanUseFeature canUseFeature;

// errors
alias glbErrorString errorString;

alias NO_DRAW_OPTIONS = GLB_NO_DRAW_OPTIONS;
alias OPTIONS_RESET   = GLB_OPTIONS_RESET  ;
alias POLYGON_MODE    = GLB_POLYGON_MODE   ;
alias CULL_MODE       = GLB_CULL_MODE      ;
alias PRIMATIVE_MODE  = GLB_PRIMATIVE_MODE ; ///< uses enum GLBPrimativeTypes
alias FRONT_FACE_MODE = GLB_FRONT_FACE_MODE; // TODO: create enum
alias LINE_WIDTH      = GLB_LINE_WIDTH     ;
alias POINT_SIZE      = GLB_POINT_SIZE     ;
alias PROVOKING_VERTEX = GLB_PROVOKING_VERTEX; // TODO: create enum

///TODO use primative type option
alias POINTS                      = GLB_POINTS                     ;
alias LINE_STRIP                  = GLB_LINE_STRIP                 ;
alias LINE_LOOP                   = GLB_LINE_LOOP                  ;
alias LINES                       = GLB_LINES                      ;
alias TRIANGLES                   = GLB_TRIANGLES                  ;
alias TRIANGLE_STRIP              = GLB_TRIANGLE_STRIP             ;
alias TRIANGLE_FAN                = GLB_TRIANGLE_FAN               ;
alias LINES_ADJACENCY             = GLB_LINES_ADJACENCY            ;
alias LINE_STRIP_ADJACENCY        = GLB_LINE_STRIP_ADJACENCY       ;
alias TRIANGLES_ADJACENCY         = GLB_TRIANGLES_ADJACENCY        ;
alias TRIANGLE_STRIP_ADJACENCY    = GLB_TRIANGLE_STRIP_ADJACENCY   ;
alias PATCHES                     = GLB_PATCHES                    ;

//ERRORS
alias SUCCESS = GLB_SUCCESS; ///< guarenteed to be zero. A function completed without error
alias FILE_NOT_FOUND = GLB_FILE_NOT_FOUND; ///< a file string parameter does not refer to an existing file
alias READ_ERROR = GLB_READ_ERROR;
alias WRITE_ERROR = GLB_WRITE_ERROR;
alias OUT_OF_MEMORY = GLB_OUT_OF_MEMORY;
alias INVALID_ARGUMENT = GLB_INVALID_ARGUMENT;
alias COMPILE_ERROR = GLB_COMPILE_ERROR; ///< a GLSL shader failed to compile.
alias UNKNOWN_ERROR = GLB_UNKNOWN_ERROR; ///< something strange happened

/// GLB was unable to correctly map or unmap a buffer; due to a GL error
alias MAP_ERROR = GLB_MAP_ERROR; 

/// a feature is currently not implemented; and may be in the future
alias UNIMPLEMENTED = GLB_UNIMPLEMENTED; 

/// a feature depends on an OpenGL version newer than the one in use
alias GL_TOO_OLD = GLB_GL_TOO_OLD; 


alias BOOL    = GLB_BOOL   ;
alias BYTE    = GLB_BYTE   ;
alias SHORT   = GLB_SHORT  ;
alias INT     = GLB_INT    ;
alias FLOAT   = GLB_FLOAT  ;
alias DOUBLE  = GLB_DOUBLE ;
alias HALF_FLOAT      = GLB_HALF_FLOAT     ;
alias UNSIGNED_BYTE   = GLB_UNSIGNED_BYTE  ;
alias UNSIGNED_SHORT  = GLB_UNSIGNED_SHORT ;
alias UNSIGNED_INT    = GLB_UNSIGNED_INT   ;
alias UBYTE   = GLB_UBYTE  ;
alias USHORT  = GLB_USHORT ;
alias UINT = GLB_UINT     ;

alias VEC2    = GLB_VEC2   ;
alias VEC3    = GLB_VEC3   ;
alias VEC4    = GLB_VEC4   ;
alias FLOAT_VEC2    = GLB_FLOAT_VEC2   ;
alias FLOAT_VEC3    = GLB_FLOAT_VEC3   ;
alias FLOAT_VEC4    = GLB_FLOAT_VEC4   ;

alias IVEC2    = GLB_IVEC2   ;
alias IVEC3    = GLB_IVEC3   ;
alias IVEC4    = GLB_IVEC4   ;
alias INT_VEC2 = GLB_INT_VEC2;
alias INT_VEC3 = GLB_INT_VEC3;
alias INT_VEC4 = GLB_INT_VEC4;

alias UIVEC2    = GLB_UIVEC2   ;
alias UIVEC3    = GLB_UIVEC3   ;
alias UIVEC4    = GLB_UIVEC4   ;
alias UNSIGNED_INT_VEC2 = GLB_UNSIGNED_INT_VEC2;
alias UNSIGNED_INT_VEC3 = GLB_UNSIGNED_INT_VEC3;
alias UNSIGNED_INT_VEC4 = GLB_UNSIGNED_INT_VEC4;

alias DVEC2   = GLB_DVEC2  ;
alias DVEC3   = GLB_DVEC3  ;
alias DVEC4   = GLB_DVEC4  ;
alias DOUBLE_VEC2   = GLB_DOUBLE_VEC2  ;
alias DOUBLE_VEC3   = GLB_DOUBLE_VEC3  ;
alias DOUBLE_VEC4   = GLB_DOUBLE_VEC4  ;

alias BVEC2 = GLB_BVEC2;
alias BVEC3 = GLB_BVEC3;
alias BVEC4 = GLB_BVEC4;
alias BOOL_VEC2 = GLB_BOOL_VEC2;
alias BOOL_VEC3 = GLB_BOOL_VEC3;
alias BOOL_VEC4 = GLB_BOOL_VEC4;


alias MAT2    = GLB_MAT2   ;
alias MAT3    = GLB_MAT3   ;
alias MAT4    = GLB_MAT4   ;
alias FLOAT_MAT2    = GLB_FLOAT_MAT2   ;
alias FLOAT_MAT3    = GLB_FLOAT_MAT3   ;
alias FLOAT_MAT4    = GLB_FLOAT_MAT4   ;
alias MAT2x3  = GLB_MAT2x3 ;
alias MAT2x4  = GLB_MAT2x4 ;
alias MAT3x2  = GLB_MAT3x2 ;
alias MAT3x4  = GLB_MAT3x4 ;
alias MAT4x2  = GLB_MAT4x2 ;
alias MAT4x3  = GLB_MAT4x3 ;
alias FLOAT_MAT2x3  = GLB_FLOAT_MAT2x3 ;
alias FLOAT_MAT2x4  = GLB_FLOAT_MAT2x4 ;
alias FLOAT_MAT3x2  = GLB_FLOAT_MAT3x2 ;
alias FLOAT_MAT3x4  = GLB_FLOAT_MAT3x4 ;
alias FLOAT_MAT4x2  = GLB_FLOAT_MAT4x2 ;
alias FLOAT_MAT4x3  = GLB_FLOAT_MAT4x3 ;


///TODO: add all opaque types from GLSL to GLBOpaque enum

alias SAMPLER_1D = GLB_SAMPLER_1D;
alias SAMPLER_2D = GLB_SAMPLER_2D;
alias SAMPLER_3D = GLB_SAMPLER_3D;
alias SAMPLER_1D_ARRAY = GLB_SAMPLER_1D_ARRAY;
alias SAMPLER_2D_ARRAY = GLB_SAMPLER_2D_ARRAY;
alias SAMPLER_CUBE = GLB_SAMPLER_CUBE;
alias SAMPLER_1D_SHADOW = GLB_SAMPLER_1D_SHADOW;
alias SAMPLER_2D_SHADOW = GLB_SAMPLER_2D_SHADOW;
alias SAMPLER_CUBE_SHADOW = GLB_SAMPLER_CUBE_SHADOW;


// buffer object features
alias BUFFER_OBJECT_FEATURE = GLB_BUFFER_OBJECT_FEATURE;
alias ARRAY_BUFFER_FEATURE = GLB_ARRAY_BUFFER_FEATURE;
alias ATOMIC_COUNTER_BUFFER_FEATURE = GLB_ATOMIC_COUNTER_BUFFER_FEATURE;
alias COPY_READ_BUFFER_FEATURE = GLB_COPY_READ_BUFFER_FEATURE;
alias COPY_WRITE_BUFFER_FEATURE = GLB_COPY_WRITE_BUFFER_FEATURE;
alias DRAW_INDIRECT_BUFFER_FEATURE = GLB_DRAW_INDIRECT_BUFFER_FEATURE;
alias DISPATCH_INDIRECT_BUFFER_FEATURE = GLB_DISPATCH_INDIRECT_BUFFER_FEATURE;
alias ELEMENT_ARRAY_BUFFER_FEATURE = GLB_ELEMENT_ARRAY_BUFFER_FEATURE;
alias PIXEL_PACK_BUFFER_FEATURE = GLB_PIXEL_PACK_BUFFER_FEATURE;
alias PIXEL_UNPACK_BUFFER_FEATURE = GLB_PIXEL_UNPACK_BUFFER_FEATURE;
alias SHADER_STORAGE_BUFFER_FEATURE = GLB_SHADER_STORAGE_BUFFER_FEATURE;
alias TEXTURE_BUFFER_FEATURE = GLB_TEXTURE_BUFFER_FEATURE;
alias TRANSFORM_FEEDBACK_BUFFER_FEATURE = GLB_TRANSFORM_FEEDBACK_BUFFER_FEATURE;
alias UNIFORM_BUFFER_FEATURE = GLB_UNIFORM_BUFFER_FEATURE;

// shader object features
alias SHADER_OBJECT_FEATURE = GLB_SHADER_OBJECT_FEATURE;
alias VERTEX_SHADER_FEATURE = GLB_VERTEX_SHADER_FEATURE;
alias TESS_CONTROL_SHADER_FEATURE = GLB_TESS_CONTROL_SHADER_FEATURE;
alias TESS_EVALUATION_SHADER_FEATURE = GLB_TESS_EVALUATION_SHADER_FEATURE;
alias GEOMETRY_SHADER_FEATURE = GLB_GEOMETRY_SHADER_FEATURE;
alias FRAGMENT_SHADER_FEATURE = GLB_FRAGMENT_SHADER_FEATURE;
