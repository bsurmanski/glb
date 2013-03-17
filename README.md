#GLB

##An OpenGL Abstraction layer

GLB is an abstraction layer built upon OpenGL created to reduce the little
annoyances of working with vanilla OpenGL. GLB can be thought of as a redesign
of the OpenGL interface, designed to clean up and modernize the OpenGL API.
There is no new functionality in GLB, but what it does have is easier to learn
than OpenGL and is easier to work with. So far the development of GLB has been
focused on cleaning up the Shader, Program, Texture, and Buffer interfaces.

GLB was not created to be a standalone game library, the only new functionality
above OpenGL is the convenience functions for loading TGAs into textures, and
for loading a shader object directly from a provided filename. GLB was create to
    be a cleaner equivalent to OpenGL, that uses OpenGL as a backend.

##Whats so bad about just using OpenGL?  
###Inconsistencies: 
There are many
inconsistencies in the interface to OpenGL that GLB tries to solve. One example
is the difference in semantics between glTexImage*D, glTexSubImage*D,
glGetTexImage, (the non-existance of) glGetTexSubImage and glBufferData,
glBufferSubData, (the non-existence of) glGetBufferData, glGetBufferSubData.
With the different methods for each texture dimension, and non-matching
functions for reading, its easy to get confused with vanilla OpenGL. In GLB, all
textures act the same, regardless of dimensionality, and
glbReadBuffer/glbWriteBuffer matches up nicely with
glbReadTexture/glbWriteTexture. Don't even get me started on all those different
functions for Uniforms...

###All those different functions for uniforms: 
In GLB, binding uniforms happens
all in one functions: glbProgramUniform, which acts similar to
glProgramUniform\* except it works in OpenGL before version 4.0.

```c
/**
 * program: the program object to set the uniform for
 * shader: an enumerated constant for which shader (eg GLB_VERTEX_SHADER)
 * i: the order the uniform was defined in the GLSL file
 * sz: the size of the uniform in bytes
 * val: the value to set the uniform to
 */
glbProgramUniform(GLBProgram *program, int shader, int i, int sz, void *val);
```

###Binding Textures: 
One thing that acts strangely in OpenGL is the binding of
textures to programs. In vanilla OpenGL, a texture must be bound to a texture
unit and a uniform must be updated correspondingly. In GLB, this step is merged
and instead one call to glbProgramTexture handles both managing texture units,
and uniform variables. 

###Necessity of retrieving uniform locations: 
In OpenGL, uniform variable
locations do not have a consistent number scheme. The spec claims that one
*MUST* get the location from a call to glUniformLocation to update the uniform
variable. This is especially noticeable when developing on a OpenGL
implementation that purposely jumbles the association between declaration order
and location order, such as the Open Source implementation Mesa. So, in a GLSL
file, if 2 uniforms are declared, a and b, their locations are not guaranteed to
be 0 and 1 respectively. GLB handles the ordering of uniforms, and instead of a
need to query a location, you supply the shader the variable was declared in,
and the order it was declared. This makes handling uniform variables much
cleaner.

###Design Philosophies of GLB: 
####Keep all functionality of OpenGL 
####Make GLB as externally consistent as possible 
####Make sure OpenGL stays fast through GLB
####Make GLB as accessible as possible (this means ANSI C)

##Example:
The following is an example of using GLB to draw a model.

The Vertex Shader (drawmodel.vs):
```glsl
#version 130

#define EPSILON 0.01

uniform bool bones_enable = false;

uniform mat4 t_matrix; // p_matrix * v_matrix * m_matrix
uniform mat4 bones[255]; 

in vec3 position;
in vec3 normal;
in vec2 uv;
in ivec2 boneids;
in vec2 boneweights;

smooth out vec3 f_normal;
smooth out vec2 f_uv;

void main()
{
    mat4 b_matrix = mat4(1);
    if(bones_enable && (boneweights[0] + boneweights[1]) > EPSILON)
    {
        b_matrix = ((bones[int(boneids[0])] * boneweights[0]) +
                   (bones[int(boneids[1])] * boneweights[1]));
    }
    gl_Position = t_matrix * b_matrix * vec4(position, 1.0f);
    f_normal = (t_matrix * vec4(normal, 0.0f)).xyz;
    f_normal.z = -f_normal.z;
    f_uv = uv;
}
```
The Fragment Shader (drawmodel.fs):
```glsl
#version 130

uniform sampler2D inColor; 

smooth in vec4 f_pos;
smooth in vec3 f_normal;
smooth in vec2 f_uv;

out vec4 outColor;
out vec4 outNormal;

void main()
{
    outColor = texture(inColor, f_uv);
    outNormal = vec4(normalize(f_normal), 1.0f);
}

```
And finally the C code using GLB:
```c
#include<assert.h>

 int running = 1;
 extern void *vertices;
 extern void *indices;
 extern int vsize;
 extern int nverts;
 extern int nindices;
 int main(int argc, char **argv)
 {
    int err;
    //create and initialize program and shaders
    GLBProgram *program = glbCreateProgram(&err);
    assert(glbdrawmodel && !err); //optional error checking
    err |= glbProgramAttachNewShaderSourceFile(program,          //pointer to program 
                                              "drawmodel.vs",    //shader filename
                                               GL_VERTEX_SHADER);//type of shader
    err |= glbProgramAttachNewShaderSourceFile(program,
                                              "drawmodel.fs", 
                                               GL_FRAGMENT_SHADER);
    assert(!err); // make sure attaching shaders worked

     struct GLBVertexLayout vlayout[] = 
         {
         {3, GL_FLOAT,           false,  32, 0},
         {3, GL_SHORT,           true,   32, 12},
         {2, GL_UNSIGNED_SHORT,  true,   32, 18},
         {1, GL_UNSIGNED_SHORT,  false,  32, 22},
         {2, GL_UNSIGNED_BYTE,   false,  32, 24},
         {2, GL_UNSIGNED_BYTE,   true,   32, 26},
         };

     GLBBuffer *vbuffer = glbCreateVertexBuffer(nverts,   // number of vertices
                                          vsize,           // size of each vertex in bytes
                                          vertices,        // pointer to vertices
                                          6,               // number of layout entries
                                          vlayout,         // layout pointer
                                          GLB_STATIC_DRAW, //buffer usage
                                          &err);           // optional error code
    assert(!err);

     GLBBuffer *ibuffer = glbCreateIndexBuffer(m->nfaces,     // number of faces 
                                          sizeof(Mesh_face), // size of each face
                                          m->faces,          // pointer to indices
                                          GLB_UNSIGNED_SHORT,//type of indices
                                          GLB_STATIC_DRAW,   // buffer usage
                                          &err);             //optional error code
    assert(!err);

    GLBTexture *texture = glbCreateTextureWithTGA("color.tga", &err);
    assert(!err);


    while(running)
    {
        glbProgramUniform(program, GLB_VERTEX_SHADER, 0, sizeof(int), &FALSE);
        glbProgramUniform(program, GLB_VERTEX_SHADER, 1, sizeof(float[16]), t_matrix);
        glbProgramNamedTexture(program, GLB_FRAGMENT_SHADER, 0, color);
        glbProgramDrawIndexed(program, vbuffer, ibuffer);
    }
 }
```

##TODO:
The following is currently unimplemented and needs to be finished:
texture functionality,
program options,
uniform buffers

##Conclusion: 
Due to the requirement of backwards compatibility that OpenGL
maintains, there is a lot of legacy and badly designed functions. Yes, there was
the deprecation that came with OpenGL 3.0, but that left all remaining functions
as is. GLB tries to reimagine OpenGL as a cleaner interface, and hopefully it's
useful to you.

##License:
This project is licensed under the MIT Open Source license, which
means that you can do pretty much whatever you want with this source code. view
the LICENSE file for full details
