module gl.glb.texture;

import std.string;

import c.gl.glb.glb_types;
import c.gl.glb.texture;
import gl.glb.sampler;

struct Texture
{
    @disable this();

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

        this(Texture t)
        {
            glbRetainTexture(t._texture);
            this = t;
        }

        this(int flags, int format, int x, int y, int z, 
                void *ptr = null)
        {
            _texture = glbCreateTexture(flags, format, x, y, z, ptr, null); //TODO: errorcodes
        }

        this(int flags, string filenm)
        {
            _texture = glbCreateTextureWithTGA(flags, toStringz(filenm), null); //TODO: errorcodes
        }

        this(int format, int x, int y, int z, string filenms[])
        {
            int origin[3] = [0, 0, 0];
            int region[3] = [x, y, 1];

            _texture = glbCreateTexture(GLB_TEXTURE_ARRAY, format, x, y, z, null, null); 

            foreach(int i, string filenm; filenms)
            {
                origin[2] = i; 
                glbWriteTextureWithTGA(_texture, 0, origin.ptr, region.ptr, filenm.toStringz());
            }
        }

        ~this()
        {
            glbReleaseTexture(_texture);
        }

        int fill(int level, int origin[3], int region[3], int fillfmt, void *fill_color)
        {
            return glbFillTexture(_texture, level, origin.ptr, region.ptr, fillfmt, fill_color); 
        }

        int write(int level, int origin[3], int region[3], int writefmt, int sz, void *ptr)
        {
            return glbWriteTexture(_texture, level, origin.ptr, region.ptr, writefmt, sz, ptr); 
        }

        int write(int level, int origin[3], int region[3], string filenm)
        {
            return glbWriteTextureWithTGA(_texture, level, origin.ptr, region.ptr, toStringz(filenm)); 
        }

        int read(int level, int origin[3], int region[3], int readfmt, int sz, void *ptr)
        {
            return glbReadTexture(_texture, level, origin.ptr, region.ptr, readfmt, sz, ptr);
        }

        int copy(ref Texture src, int srclvl, int dstlvl, 
                 int srcorigin[3], int dstorigin[3], int region[3])
        {
            return glbCopyTexture(src._texture, _texture, srclvl, dstlvl,
                                  srcorigin.ptr, dstorigin.ptr, region.ptr);
        }

        const(int) *size()
        {
            return glbTextureSize(_texture);
        }

        @property int width()
        {
            return glbTextureSize(_texture)[0];
        }

        @property int height()
        {
            return glbTextureSize(_texture)[1];
        }

        @property int depth()
        {
            return glbTextureSize(_texture)[2];
        }

        @property Texture dup()
        {
            glbRetainTexture(_texture);
            Texture ret = this;
            return ret;
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


