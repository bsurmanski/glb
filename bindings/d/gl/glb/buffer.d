module gl.glb.buffer;

import std.conv;
import core.exception;
import c.gl.glb.glb;
import c.gl.glb.glb_types;
import c.gl.glb.buffer;

struct Buffer
{
    //@disable this();

    private:
        GLBBuffer *_buffer = null;

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

        this(T)(T arr[], int usage = STATIC_DRAW)
        {

            int err;
            _buffer = glbCreateBuffer(arr.length, T.sizeof, arr.ptr, usage, &err); 

            if(err == GLB_OUT_OF_MEMORY)
            {
                throw new OutOfMemoryError(); 
            }

            assert(!err);
        }

        /**
         * Create index buffer
         */
        //TODO: conflicts with this(arr, usage)
        this(T)(T arr[], int type, int usage = STATIC_DRAW)
        {
            _buffer = glbCreateIndexBuffer(arr.length, T.sizeof, arr.ptr, type, usage, null); 
        } unittest{
            ushort buf[] = [1, 2, 3];
            Buffer(buf, GLB_USHORT);
        }

        /**
         * Create vertex buffer
         */
        this(T)(T arr[], GLBVertexLayout desc[], int usage = STATIC_DRAW)
        in
        {
            //assert(nmemb * sz); 
        }
        body
        {
            _buffer = glbCreateVertexBuffer(arr.length, T.sizeof, arr.ptr, to!int(desc.length), desc.ptr, usage, null);
        }

        ~this()
        {
            glbReleaseBuffer(_buffer);
        }

        int write(size_t offset, size_t sz, void *ptr)
        in
        {
            assert(ptr); 
        }
        body
        {
            return glbWriteBuffer(_buffer, offset, sz, ptr);
        }

        int read(size_t offset, size_t sz, void *ptr)
        in
        {
            assert(ptr); 
        }
        body
        {
            return glbReadBuffer(_buffer, offset, sz, ptr);
        }

        int fill(const(void) *pattern, size_t pattern_sz, size_t offset, size_t sz)
        in
        {
            assert(pattern);
        }
        body
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
