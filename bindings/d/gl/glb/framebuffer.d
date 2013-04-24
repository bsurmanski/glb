module gl.glb.framebuffer;

import c.gl.glb.glb_types;
import c.gl.glb.framebuffer;

import gl.glb.texture;

struct Framebuffer
{
    private:
        GLBFramebuffer *_framebuffer;

    public:

    this(int *errcode_ret = null)
    {
        _framebuffer = glbCreateFramebuffer(null); 
    }

    ~this()
    {
        glbDeleteFramebuffer(_framebuffer);
    }

    int addTexture(ref Texture texture)
    {
        return glbFramebufferTexture(_framebuffer, texture.getTexture());    
    }

    int addColor(int i, ref Texture texture)
    {
        return glbFramebufferColor(_framebuffer, i, texture.getTexture());
    }

    int addDepth(ref Texture texture)
    {
        return glbFramebufferDepth(_framebuffer, texture.getTexture());
    }

    int addStencil(ref Texture texture)
    {
        return glbFramebufferStencil(_framebuffer, texture.getTexture());
    }

    int addDepthStencil(ref Texture texture)
    {
        return glbFramebufferDepthStencil(_framebuffer, texture.getTexture());
    }

    GLBFramebuffer *getFramebuffer()
    {
        return _framebuffer;
    }
}
