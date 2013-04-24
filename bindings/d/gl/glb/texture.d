module gl.glb.texture;

import c.gl.glb.glb_types;
import c.gl.glb.texture;
import gl.glb.sampler;

import std.string;

struct Texture
{
    private:
        GLBTexture *_texture = null;
        Sampler *_sampler = null;

    public:
        alias RGBA = GLB_RGBA;
        alias RGB = GLB_RGB;
        alias DEPTH = GLB_DEPTH;
        alias STENCIL = GLB_STENCIL;
        alias DEPTH_STENCIL = GLB_DEPTH_STENCIL;
        alias INT8 = GLB_INT8;
        alias INT16 = GLB_INT16;
        alias INT32 = GLB_INT32;
        alias _2INT16 = GLB_2INT16;

        alias READ_ONLY = GLB_READ_ONLY;
        alias WRITE_ONLY = GLB_WRITE_ONLY;
        alias READ_WRITE = GLB_READ_WRITE;
        alias TEXTURE_ARRAY = GLB_TEXTURE_ARRAY;

        this(int flags, int format, int x, int y, int z, 
                void *ptr = null, int *errcode_ret = null)
        {
            _texture = glbCreateTexture(flags, format, x, y, z, ptr, errcode_ret);
        }

        this(int flags, string filenm, int *errcode_ret = null)
        {
            _texture = glbCreateTextureWithTGA(flags, toStringz(filenm), errcode_ret);
        }

        ~this()
        {
            glbDeleteTexture(_texture);
        }

        int fill(int level, int origin[3], int region[3], int fillfmt, void *fill_color)
        {
            return glbFillTexture(_texture, level, origin, region, fillfmt, fill_color); 
        }

        int write(int level, int origin[3], int region[3], int writefmt, int sz, void *ptr)
        {
            return glbWriteTexture(_texture, level, origin, region, writefmt, sz, ptr); 
        }

        int write(int level, int origin[3], int region[3], string filenm)
        {
            return glbWriteTextureWithTGA(_texture, level, origin, region, toStringz(filenm)); 
        }

        int read(int level, int origin[3], int region[3], int readfmt, int sz, void *ptr)
        {
            return glbReadTexture(_texture, level, origin, region, readfmt, sz, ptr);
        }

        int copy(ref Texture dst, int srclvl, int dstlvl, 
                 int srcorigin[3], int dstorigin[3], int region[3])
        {
            return glbCopyTexture(_texture, dst._texture, srclvl, dstlvl,
                                  srcorigin, dstorigin, region);
        }

        const(int) *size()
        {
            return glbTextureSize(_texture);
        }

        int setSampler(Sampler *sampler)
        {
            _sampler = sampler;
            return glbTextureSampler(_texture, sampler.getSampler()); 
        }

        GLBTexture *getTexture()
        {
            return _texture;
        }
}


