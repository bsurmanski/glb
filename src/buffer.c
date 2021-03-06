/**
 * buffer.c
 * @file buffer.h
 * GLB
 * @date March 01, 2013
 * @author Brandon Surmanski
 *
 * @brief definition of the GLBBuffer object interface
 */

#include "glb_private.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// sz is the size for each element (sz/nmemb)
static int guessType(size_t sz)
{
    switch(sz)
    {
        case 1:
            return GLB_UBYTE;
        case 2:
            return GLB_USHORT;
        case 4:
            return GLB_UINT;
        default:
            return 0;
    }
}

GLBBuffer* glbCreateBuffer (size_t nmemb, size_t sz, const void *const ptr, int usage, int *errcode_ret)
{
    int errcode;

    GLB_ASSERT(nmemb * sz, GLB_INVALID_ARGUMENT, ERROR_BUFFER);

    GLBBuffer *buffer = malloc(sizeof(GLBBuffer));
    GLB_ASSERT(buffer, GLB_OUT_OF_MEMORY, ERROR_BUFFER);
    buffer->refcount = 1;
    glGenBuffers(1, &buffer->globj);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->globj);
    glBufferData(GL_ARRAY_BUFFER, nmemb * sz, ptr, usage);
    //TODO: detect errors

    buffer->nmemb = nmemb;
    buffer->sz = sz;
    buffer->vdata.count = 0;
    buffer->vdata.layout = NULL;
    buffer->idata.type = guessType(sz / nmemb); // guess index buffer info
    buffer->idata.count = nmemb; // guess index buffer info

    GLB_SET_ERROR(GLB_SUCCESS);
    return buffer;

ERROR_BUFFER:
    GLB_SET_ERROR(errcode);

#ifdef DEBUG
    printf("GLBCreateBuffer Error: %s\n", glbErrorString(errcode));
#endif

    return NULL;
}

GLBBuffer* glbCreateIndexBuffer  (size_t nmemb, size_t sz, const void *const ptr, int type,
                                  int usage, int *errcode_ret)
{
    GLBBuffer *buf = glbCreateBuffer(nmemb, sz, ptr, usage, errcode_ret);

    if(!buf)
    {
        return NULL;
    }

    glbIndexBufferFormat(buf, 0, (nmemb * sz) / glbTypeSizeof(type), type);

    return buf;
}

GLBBuffer* glbCreateVertexBuffer (size_t nmemb, size_t sz, const void *const ptr,
                                  int ndesc, GLBVertexLayout *desc,
                                  int usage, int *errcode_ret)
{
    GLBBuffer *buf = glbCreateBuffer(nmemb, sz, ptr, usage, errcode_ret);

    if(!buf)
    {
        return NULL;
    }

    glbVertexBufferFormat(buf, ndesc, desc);

    return buf;
}

void glbDeleteBuffer (GLBBuffer *buffer)
{
    if(!buffer) return;
    free(buffer->vdata.layout);
    glDeleteBuffers(1, &buffer->globj);
}

void glbRetainBuffer (GLBBuffer *buffer)
{
    if(!buffer) return;
    buffer->refcount++;
}

void glbReleaseBuffer(GLBBuffer *buffer)
{
    if(!buffer) return;
    buffer->refcount--;
    if(buffer->refcount <= 0)
    {
        glbDeleteBuffer(buffer);
    }
}

int glbWriteBuffer (GLBBuffer *buffer, size_t offset, size_t sz, void *ptr)
{
    if(!buffer) return 0;
    glBufferSubData(buffer->globj, offset, sz, ptr);
    return 0;
}

int glbReadBuffer (GLBBuffer *buffer, size_t offset, size_t sz, void *ptr)
{
    if(!buffer) return 0;
    glGetBufferSubData(buffer->globj, offset, sz, ptr);
    return 0;
}

int glbFillBuffer (GLBBuffer *buffer,
                    const void *pattern,
                    size_t pattern_size,
                    size_t offset,
                    size_t size)
{
    if(!buffer) return 0;
    //TODO
    int complete = 0;
    int try = 3; // try to fill at most this many times if it fails
    while (!complete && try--) // should only run once unless theres a write error
    {
        uint8_t *mapped = glbMapBuffer(buffer, GLB_WRITE_ONLY);
        if(!mapped) return GLB_MAP_ERROR;

        int i;
        for(i = 0; i < size / pattern_size; i++)
        {
            memcpy(&mapped[offset + i * pattern_size], pattern, pattern_size);
        }
        complete = glbUnmapBuffer(buffer);
    }

    return complete ? GLB_SUCCESS : GLB_WRITE_ERROR;
}

int glbCopyBuffer (GLBBuffer *src,
                    GLBBuffer *dst,
                    size_t src_offset,
                    size_t dst_offset,
                    size_t size)
{
    if(!src || !dst) return 0;
    glCopyBufferSubData(src->globj, dst->globj, src_offset, dst_offset, size);
    return 0;
}

void* glbMapBuffer (GLBBuffer *buffer, int access)
{
    if(!buffer) return 0;
    return glMapBuffer(buffer->globj, access);
}

int glbUnmapBuffer (GLBBuffer *buffer)
{
    if(!buffer) return 0;
    int err = glUnmapBuffer(buffer->globj);
    return err; //unfortunately there is no way to gaurd against this error
}

int glbVertexBufferFormat (GLBBuffer *buffer, int ndesc, GLBVertexLayout *desc)
{
    if(!buffer) return 0;

    // if zero passed, clear format
    if(!ndesc || !desc)
    {
        free(buffer->vdata.layout);
        buffer->vdata.count = 0;
        buffer->vdata.layout = NULL;
        return 0;
    }

    int max_attribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);

    if(ndesc > max_attribs)
    {
        return GLB_INVALID_ARGUMENT;
    }

    if(buffer->vdata.layout)
    {
        free(buffer->vdata.layout);
    }


    // validate the descriptors
    int i;
    for(i = 0; i < ndesc; i++)
    {
        if(desc[i].size > 4 ||
           !glbTypeIsScalar(desc[i].type))
        {
            return GLB_INVALID_ARGUMENT;
        }
    }

    buffer->vdata.count = ndesc;
    buffer->vdata.layout = malloc(sizeof(struct GLBVertexLayout) * ndesc);
    memcpy(buffer->vdata.layout, desc, sizeof(struct GLBVertexLayout) * ndesc);
    return 0;
}

int glbIndexBufferFormat (GLBBuffer *buffer, int offset, int count, int type)
{
    if(!buffer) return 0;
    // must be scalar and not float
    if(!glbTypeIsScalar(type) || glbTypeIsFloat(type))
    {
        return GLB_INVALID_ARGUMENT;
    }

    buffer->idata.count = count;
    buffer->idata.offset = offset;
    buffer->idata.type = glbTypeToUnsigned(type);
    return 0;
}
