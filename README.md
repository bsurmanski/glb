glb
===

##An OpenGL Abstraction layer

GLB is an abstraction layer built upon OpenGL created to reduce the little annoyances of working with vanilla OpenGL. GLB can be thought of as a redesign of the OpenGL interface, designed to clean up and modernize the OpenGL API. There is no new functionality in GLB, but what it does have is easier to learn than OpenGL and is easier to work with. So far the development of GLB has been focused on cleaning up the Shader, Program, Texture, and Buffer interfaces.

GLB was not created to be a standalone game library, the only new functionality above OpenGL is the convenience functions for loading TGAs into textures, and for loading a shader object directly from a provided filename. GLB was create to be a cleaner equivalent to OpenGL, that uses OpenGL as a backend.

##Whats so bad about just using OpenGL?
###Inconsistencies:
There are many inconsistencies in the interface to OpenGL that GLB tries to solve. One example is the difference in semantics between glTexImage*D, glTexSubImage*D, glGetTexImage, (the non-existance of) glGetTexSubImage and glBufferData, glBufferSubData, (the non-existence of) glGetBufferData, glGetBufferSubData. With the different methods for each texture dimension, and non-matching functions for reading, its easy to get confused with vanilla OpenGL. In GLB, all textures act the same, regardless of dimensionality, and glbReadBuffer/glbWriteBuffer matches up nicely with glbReadTexture/glbWriteTexture. Don't even get me started on all those different functions for Uniforms...

###All those different functions for uniforms:
in GLB, binding uniforms happens all in one functions: glbProgramUniform, which acts similar to glProgramUniform\* except it works in OpenGL before version 4.0.

###Binding Textures:
One thing that acts strangely in OpenGL is the binding of textures to programs. In vanilla OpenGL, a texture must be bound to a texture unit and a uniform must be updated correspondingly. In GLB, this step is merged and instead one call to glbProgramTexture handles both managing texture units, and uniform variables. 

###Necessity of retrieving uniform locations:
In OpenGL, uniform variable locations do not have a consistent number scheme. The spec claims that one *MUST* get the location from a call to glUniformLocation to update the uniform variable. This is especially noticeable when developing on a OpenGL implementation that purposely jumbles the association between declaration order and location order, such as the Open Source implementation Mesa. So, in a GLSL file, if 2 uniforms are declared, a and b, their locations are not guaranteed to be 0 and 1 respectively. GLB handles the ordering of uniforms, and instead of a need to query a location, you supply the shader the variable was declared in, and the order it was declared. This makes handling uniform variables much cleaner.

###Design Philosophies of GLB:
####Keep all functionality of OpenGL
####Make GLB as externally consistent as possible
####Make sure OpenGL stays fast through GLB
####Make GLB as accessible as possible (this means ANSI C)

##Conclusion:
due to the requirement of backwards compatibility that OpenGL maintains, there is a lot of legacy and badly designed functions. Yes, there was the deprecation that came with OpenGL 3.0, but that left all remaining functions as is. GLB tries to reimagine OpenGL as a cleaner interface, and hopefully it's useful to you.
