
#ifndef _PROGRAM_UNIFORMS_H
#define _PROGRAM_UNIFORMS_H

#include "GL/GLPlatform.h"

namespace gk {

struct GLProgramUniforms
{
    std::vector<unsigned char> uniforms;
    std::vector<unsigned int> offsets;
    std::vector<GLenum> types;
    
    GLuint program;
    
    GLProgramUniforms( const GLuint _program ) : uniforms(), offsets(), types(), program(_program) {}
    
    int update( )
    {
        return -1;
    }
};

}       // namespace
#endif
