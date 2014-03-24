

#include "GL/GLProgram.h"
#include "GL/ProgramName.h"

#include "Logger.h"


namespace gk {  

static 
bool uniform_check( ProgramName& uniform, int size, GLenum type )
{
    if(uniform.isValid() == false)
        return false;

#ifndef NDEBUG
    if(uniform.size != size)
    {
        ERROR("setUniform('%s'): size %d != %d\n", uniform.program->uniformName(uniform), uniform.size, size);
        return false;
    }
    
    if(uniform.isSampler())
    {
        if(type != GL_INT)
        {
            ERROR("setSampler('%s'): not an int parameter\n", uniform.program->samplerName(uniform));
            return false;
        }
    }
    else
    {
        if(uniform.type != type)
        {
            ERROR("setUniform('%s'): type 0x%x != 0x%x\n", uniform.program->uniformName(uniform), uniform.type, type);
            return false;
        }
    }
#endif

#ifndef GK_OPENGL4
#ifndef NDEBUG
    {
        GLint current= 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &current);
        if(current == 0 || (GLuint) current != uniform.program->name)
        {
            ERROR("setUniform('%s'): program %d is not in use (current program %d).\n", uniform.program->uniformName(uniform), uniform.program->name, current);
            return false;
        }
    }
#endif
#endif
    
    return true;
}

ProgramName& ProgramName::operator= ( const bool x )
{
    if(uniform_check(*this, 1, GL_BOOL) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform1i(location, x);
#else
    glProgramUniform1i(program->name, location, x);
#endif
    return *this;
}

ProgramName& ProgramName::operator= ( const int x )
{
    if(uniform_check(*this, 1, GL_INT) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform1i(location, x);
#else
    glProgramUniform1i(program->name, location, x);
#endif
    return *this;
}

template <> ProgramName& ProgramName::operator= ( const TVec2<int>& v )
{
    if(uniform_check(*this, 1, GL_INT_VEC2) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform2iv(location, size, &v.x);
#else
    glProgramUniform2iv(program->name, location, size, &v.x);
#endif
    return *this;
}

template <> ProgramName& ProgramName::operator= ( const TVec3<int>& v )
{
    if(uniform_check(*this, 1, GL_INT_VEC3) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform3iv(location, size, &v.x);
#else
    glProgramUniform3iv(program->name, location, size, &v.x);
#endif
    return *this;
}

template <> ProgramName& ProgramName::operator= ( const TVec4<int>& v )
{
    if(uniform_check(*this, 1, GL_INT_VEC4) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform4iv(location, size, &v.x);
#else
    glProgramUniform4iv(program->name, location, size, &v.x);
#endif
    return *this;
}

ProgramName& ProgramName::operator= ( const unsigned int x )
{
    if(uniform_check(*this, 1, GL_UNSIGNED_INT) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform1ui(location, x);
#else
    glProgramUniform1ui(program->name, location, x);
#endif
    return *this;
}

template <> ProgramName& ProgramName::operator= ( const TVec2<unsigned int>& v )
{
    if(uniform_check(*this, 1, GL_UNSIGNED_INT_VEC2) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform2uiv(location, size, &v.x);
#else
    glProgramUniform2uiv(program->name, location, size, &v.x);
#endif
    return *this;
}

template <> ProgramName& ProgramName::operator= ( const TVec3<unsigned int>& v )
{
    if(uniform_check(*this, 1, GL_UNSIGNED_INT_VEC3) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform3uiv(location, size, &v.x);
#else
    glProgramUniform3uiv(program->name, location, size, &v.x);
#endif
    return *this;
}

template <> ProgramName& ProgramName::operator= ( const TVec4<unsigned int>& v )
{
    if(uniform_check(*this, 1, GL_UNSIGNED_INT_VEC4) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform4uiv(location, size, &v.x);
#else
    glProgramUniform4uiv(program->name, location, size, &v.x);
#endif
    return *this;
}


ProgramName& ProgramName::operator= ( const float x )
{
    if(uniform_check(*this, 1, GL_FLOAT) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform1f(location, x);
#else
    glProgramUniform1f(program->name, location, x);
#endif
    return *this;
}

template <> ProgramName& ProgramName::operator= ( const TVec2<float>& v )
{
    if(uniform_check(*this, 1, GL_FLOAT_VEC2) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform2fv(location, size, &v.x);
#else
    glProgramUniform2fv(program->name, location, size, &v.x);
#endif
    return *this;
}

template <> ProgramName& ProgramName::operator= ( const TVec3<float>& v )
{
    if(uniform_check(*this, 1, GL_FLOAT_VEC3) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform3fv(location, size, &v.x);
#else
    glProgramUniform3fv(program->name, location, size, &v.x);
#endif
    return *this;
}

template <> ProgramName& ProgramName::operator= ( const TVec4<float>& v )
{
    if(uniform_check(*this, 1, GL_FLOAT_VEC4) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform4fv(location, size, &v.x);
#else
    glProgramUniform4fv(program->name, location, size, &v.x);
#endif
    return *this;
}

ProgramName& ProgramName::operator= ( const VecColor& c )
{
    return (*this)= Vec4(c); 
}

ProgramName& ProgramName::operator= ( const Mat4& m )
{
    if(uniform_check(*this, 1, GL_FLOAT_MAT4) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniformMatrix4fv(location, size, GL_TRUE, &m.m[0][0]);
#else
    glProgramUniformMatrix4fv(program->name, location, size, GL_TRUE, &m.m[0][0]);
#endif
    return *this;
}

}
