#GLB

##An OpenGL Abstraction layer

GLB is an abstraction layer built upon OpenGL created to reduce the little
annoyances of working with vanilla OpenGL. GLB can be thought of as a redesign
of the OpenGL interface, designed to clean up and modernize the OpenGL API.
There is no new functionality in GLB, but what it does have is easier to learn
than OpenGL and is easier to work with. So far the development of GLB has been
focused on cleaning up the Shader, Program, Texture, and Buffer interfaces.
All GLB calls end up calling OpenGL code to function, so it should be just as 
portible as OpenGL itself.

GLB was not created to be a standalone game library, the only new functionality
above OpenGL is the convenience functions for loading TGAs into textures, and
for loading a shader object directly from a provided filename. GLB was create to
be a cleaner equivalent to OpenGL, that uses OpenGL as a backend. 

Since GLB has not been widely tested, and is mostly used for personal use, 
use GLB in production code with caution.

##Whats so bad about just using OpenGL?  
###Inconsistencies: 
There are many
inconsistencies in the interface to OpenGL that GLB tries to solve. One example
is the difference in semantics between the following sets: 

* glTexImage\*D, 
* glTexSubImage\*D,
* glGetTexImage, 
* glGetTexSubImage (does not actually exist) 

and 

* glBufferData,
* glBufferSubData, 
* glGetBufferData (does not actually exist)
* glGetBufferSubData

With the different methods for each texture dimension, and non-matching
functions for reading, its easy to get confused with vanilla OpenGL. In GLB, all
textures act the same, regardless of dimensionality, and
glbReadBuffer/glbWriteBuffer matches up nicely with
glbReadTexture/glbWriteTexture. Don't even get me started on all those different
functions for Uniforms...

###All those different functions for uniforms: 
In GLB, binding uniforms happens
all in one functions: glbProgramUniform, which acts similar to
glProgramUniform\* except it doesn't depend of OpenGL 4.0.

```c
/**
 * program: the program object to set the uniform for
 * shader: an enumerated constant for which shader the uniform object is in(eg GLB_VERTEX_SHADER)
 * i: the order the uniform was defined in the GLSL file
 * sz: the size of the uniform in bytes
 * val: the value to set the uniform to
 */
glbProgramUniform(GLBProgram *program, int shader, int i, int sz, void *val);
```

###Binding Textures: 
One thing that acts strangely in OpenGL is the binding of
textures to programs. In vanilla OpenGL, a texture must be bound to a texture
unit and a uniform must be updated correspondingly. This is contradictory to 
binding shaders, or other uniforms. Its just inconsistant. In GLB, texture binding 
is a single step that makes sense. Instead of multiple steps, one call to 
glbProgramTexture handles both managing texture units, and the uniform variables. 

###Sane Defaults
We've all done it. We think we've gotten everything working, but the textures
aren't showing up! Yeah, we forgot to set the TEXTURE_MIN_FILTER and TEXTURE_MAG_FILTER
parameters with glTexParameteri again. 
GLB uses sane defaults to allow you to get working as quickly as possible,
And if something unexpected happens, GLB will return an error so you know.

### State Machines
All that binding and unbind is such a pain, and the fact that OpenGL functions depend 
on persistant state leads to unexpected behaviour and bugs. GLB trys to be as functional 
as possible, and the result of each GLB function should only depends on the provided parameters.

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
cleaner and much more flexible for projects with many shaders. Functionality no
longer depends on a strict naming scheme. 

###Design Philosophies of GLB: 
####Keep all functionality of OpenGL 
####Make GLB as externally consistent as possible 
####Make sure OpenGL stays fast through GLB
####Make GLB as accessible as possible (this means ANSI C)
####Keep GLB functional, functions should not depend on persistant state

##Example:
The following is an example of using GLB to draw a textured and skinned model.

The Vertex Shader (drawmodel.vs):
```glsl
#version 130

#define EPSILON 0.01

uniform bool bones_enable = false;

uniform mat4 t_matrix; // p_matrix * v_matrix * m_matrix
uniform mat4 bones[255]; 

// this is the vertex layout. specified in vlayout
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

#include<glb/glb.h>

 int running = 1;
 extern void *vertices;
 extern void *indices;
 extern int vsize;
 extern int nverts;
 extern int nindices;
 int main(int argc, char **argv)
 {
      // Normal OpenGL Initialization (Through SDL, GLFW, GLEW, etc)

         ...
         ...

      // End Normal Opengl Initialization 

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

    GLBTexture *texture = glbCreateTextureWithTGA(0, "color.tga", &err);
    assert(!err);


    while(running)
    {
        glbProgramUniform(program, GLB_VERTEX_SHADER, 0, sizeof(int), &FALSE);
        glbProgramUniform(program, GLB_VERTEX_SHADER, 1, sizeof(float[16]), t_matrix);
        glbProgramTexture(program, GLB_FRAGMENT_SHADER, 0, color);
        glbProgramDrawIndexed(program, vbuffer, ibuffer);
    }
 }
```

##Further Documentation

API documentation is in the form of Doxygen. To create the documentation, simply run
`make docs` in the project root.

##Dependencies and Installation

GLB's only dependency is OpenGL and the OpenGL headers. The makefile uses gcc
for compilation, but any C99 compliant compiler should work. To install run

    make
    make install

This will install the package headers to /usr/local/include and the library to
/usr/local/lib. Simply include the `<glb/glb.h>` header and link with `-lglb` to
use GLB in your project.

##TODO:

The following is currently incomplete and needs to be finished or added:
* attach mipmap texture levels to framebuffer
* program options
* program input/output layout (?)
* uniform buffers (non-buffer uniforms should work, though)
* TGA color maps
* more robust GLSL parsing. (currently working, needs improvement(eg, multiple uniform definitions on one line))
* uniform array length verification variables
* handling of large amounts of inputs, outputs and uniforms

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
