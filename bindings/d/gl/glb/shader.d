
module gl.glb.shader;

import std.string;
import c.gl.glb.glb_types;
import c.gl.glb.shader;

struct Shader
{
    private:
        GLBShader *_shader;

    public:
        alias VERTEX_SHADER = GLB_VERTEX_SHADER;
        alias TESS_CONTROL_SHADER = GLB_TESS_CONTROL_SHADER;
        alias TESS_EVALUATION_SHADER = GLB_TESS_EVALUATION_SHADER;
        alias GEOMETRY_SHADER = GLB_GEOMETRY_SHADER;
        alias FRAGMENT_SHADER = GLB_FRAGMENT_SHADER;

        this(string filenm, int stage, int *errcode_ret = null)
        {
            _shader = glbCreateShaderWithSourceFile(toStringz(filenm), stage, errcode_ret);
        }

        this(int len, void *mem, int stage, int *errcode_ret = null)
        {
            _shader = glbCreateShaderWithSource(len, mem, stage, errcode_ret);  
        }

        ~this()
        {
            glbDeleteShader(_shader);
        }
}
