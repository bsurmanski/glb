module gl.glb.program;

import c.gl.glb.glb_types;
import c.gl.glb.program;
import gl.glb.buffer;

struct Program
{
    private:
        GLBProgram *_program;

    public:
    this(int *errcode_ret = null)
    {
        _program = glbCreateProgram(errcode_ret); 
    }

    ~this()
    {
        glbDeleteProgram(_program);
    }

    int source(const(string) filenm, int stage)
    {
        return glbProgramAttachNewShaderSourceFile(_program, filenm, stage);
    }

    int source(int len, const void *mem, int stage)
    {
        return glbProgramAttachNewShaderSource(_program, len, cast(char*) mem, stage); 
    }

    void attach(GLBShader *shader)
    {
        glbProgramAttachShader(_program, shader);
    }

    int detach(GLBShader *shader)
    {
        return glbProgramDetachShader(_program, shader);
    }

    int detach(int stage)
    {
        return glbProgramDetachShaderStage(_program, stage);
    } 

    int uniform(int shader, int i, int sz, void *val)
    {
        return glbProgramUniform(_program, shader, i, sz, val);
    }

    int uniform(int shader, int i, int sz, bool transpose, void *val)
    {
        return glbProgramUniformMatrix(_program, shader, i, sz, transpose, val);
    }

    int texture(int shader, int i, GLBTexture *texture)
    {
        return glbProgramTexture(_program, shader, i, texture);
    }

    int draw(ref Buffer array)
    {
        return glbProgramDraw(_program, array.getBuffer());
    }

    int draw(ref Buffer array, ref Buffer index)
    {
        return glbProgramDrawIndexed(_program, array.getBuffer(), index.getBuffer());
    }

    int draw(ref Buffer array, int offset, int count)
    {
        return glbProgramDrawRange(_program, array.getBuffer(), offset, count);
    }

    int draw(ref Buffer array, ref Buffer index, int offset, int count)
    {
        return glbProgramDrawIndexedRange(_program, array.getBuffer(), index.getBuffer(), offset, count);
    }

    GLBProgram *getProgram()
    {
        return _program;
    }
}
