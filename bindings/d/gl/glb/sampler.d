
module gl.glb.sampler;

import c.gl.glb.glb_types;
import c.gl.glb.sampler;

struct Sampler
{
    private:
        GLBSampler *_sampler;

    public:
        alias GLB_NEAREST  NEAREST;
        alias GLB_LINEAR  LINEAR;
        alias GLB_NEAREST_MIPMAP_NEAREST   NEAREST_MIPMAP_NEAREST;
        alias GLB_LINEAR_MIPMAP_NEAREST    LINEAR_MIPMAP_NEAREST;
        alias GLB_NEAREST_MIPMAP_LINEAR    NEAREST_MIPMAP_LINEAR;
        alias GLB_LINEAR_MIPMAP_LINEAR     LINEAR_MIPMAP_LINEAR;

        alias GLB_CLAMP_TO_EDGE   CLAMP_TO_EDGE;
        alias GLB_CLAMP           CLAMP;
        alias GLB_MIRRORED_REPEAT MIRRORED_REPEAT;
        alias GLB_MIRRORED        MIRRORED;
        alias GLB_REPEAT          REPEAT;
        alias GLB_CLAMP_TO_BORDER CLAMP_TO_BORDER;

        alias GLB_COMPARE_REF_TO_TEXTURE COMPARE_REF_TO_TEXTURE;
        alias GLB_NONE                   NONE;

        alias GLB_LEQUAL   LEQUAL;
        alias GLB_GEQUAL   GEQUAL;
        alias GLB_LESS     LESS;
        alias GLB_GREATER  GREATER;
        alias GLB_EQUAL    EQUAL;
        alias GLB_NOTEQUAL NOTEQUAL;
        alias GLB_ALWAYS   ALWAYS;
        alias GLB_NEVER    NEVER;

        static typeof(this) opCall()
        {
            Sampler s = Sampler.init;
            s._sampler = glbCreateSampler(null); 
            return s;
        }

        ~this()
        {
            glbReleaseSampler(_sampler);
        }

        void setFilter(int min, int max)
        {
            glbSamplerFilter(_sampler, min, max);
        }

        void setLOD(float minlod, float maxlod)
        {
            glbSamplerLOD(_sampler, minlod, maxlod);
        }

        void setWrap(int s, int t, int r)
        {
            glbSamplerWrap(_sampler, s, t, r);
        }

        void setCompare(int m, int f)
        {
            glbSamplerCompare(_sampler, m, f);
        }

        GLBSampler *getSampler()
        {
            return _sampler;
        }
}
