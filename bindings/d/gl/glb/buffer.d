module gl.glb.buffer;

import std.conv;
import c.gl.glb.glb_types;
import c.gl.glb.buffer;

struct Buffer
{
    private:
        GLBBuffer *_buffer;

    public:
        alias STREAM_DRAW = GLB_STREAM_DRAW;
        alias STREAM_READ = GLB_STREAM_READ;
        alias STREAM_COPY = GLB_STREAM_COPY;
        alias STATIC_DRAW = GLB_STATIC_DRAW;
        alias STATIC_READ = GLB_STATIC_READ;
        alias STATIC_COPY = GLB_STATIC_COPY;
        alias DYNAMIC_DRAW = GLB_DYNAMIC_DRAW;
        alias DYNAMIC_READ = GLB_DYNAMIC_READ;
        alias DYNAMIC_COPY = GLB_DYNAMIC_COPY;

        this(size_t nmemb, size_t sz, const(void) *ptr, int usage)
        {
            _buffer = glbCreateBuffer(nmemb, sz, ptr, usage, null); 
        }

        /+
         = Create index buffer
         +/
        this(size_t nmemb, size_t sz, const(void) *ptr, int type, int usage)
        {
            _buffer = glbCreateIndexBuffer(nmemb, sz, ptr, type, usage, null); 
        }

        /+
         = Create vertex buffer
         +/
        this(size_t nmemb, size_t sz, const(void) *ptr, GLBVertexLayout desc[], int usage)
        {
            _buffer = glbCreateVertexBuffer(nmemb, sz, ptr, to!int(desc.length), desc.ptr, usage, null);
        }

        ~this()
        {
            glbDeleteBuffer(_buffer);
        }

        int write(size_t offset, size_t sz, void *ptr)
        {
            return glbWriteBuffer(_buffer, offset, sz, ptr);
        }

        int read(size_t offset, size_t sz, void *ptr)
        {
            return glbReadBuffer(_buffer, offset, sz, ptr);
        }

        int fill(const(void) *pattern, size_t pattern_sz, size_t offset, size_t sz)
        {
            return glbFillBuffer(_buffer, pattern, pattern_sz, offset, sz);
        }

        int copy(ref Buffer src, size_t src_offset, size_t dst_offset, size_t sz)
        {
            return glbCopyBuffer(src._buffer, _buffer, src_offset, dst_offset, sz);
        }

        void *map(int access)
        {
            return glbMapBuffer(_buffer, access);
        }

        int unmap()
        {
            return glbUnmapBuffer(_buffer);
        }

        int vertexFormat(GLBVertexLayout desc[])
        {
            return glbVertexBufferFormat(_buffer, to!int(desc.length), desc.ptr);
        }

        int indexFormat(int offset, int count, int type)
        {
            return glbIndexBufferFormat(_buffer, offset, count, type);
        }

        GLBBuffer *getBuffer()
        {
            return _buffer;
        }
}
