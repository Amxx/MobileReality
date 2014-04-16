
#ifndef _PROGRAM_TWEAKS_H
#define _PROGRAM_TWEAKS_H

#include <map>

#include "GL/GLProgram.h"
#include "Widgets/nvWidgets.h"
#include "Widgets/nvSDLContext.h"


namespace gk {
    
class GLProgramUniforms;

class ProgramTweaks
{
    // non copyable
    ProgramTweaks( const ProgramTweaks& );
    ProgramTweaks& operator= ( const ProgramTweaks& );
    
    ProgramTweaks( );
    ~ProgramTweaks( );
    
    std::map<GLuint, GLProgramUniforms *> programs;
    
public:
    int tweak( const GLuint program );
    int doTweaks( nv::UIContext *widgets, const GLuint program );
    
    static 
    ProgramTweaks& manager( ) 
    {
        static ProgramTweaks object;
        return object;
    }
};


inline
int tweakProgram( const GLuint program )
{
    return ProgramTweaks::manager().tweak(program);
}

inline
int tweakProgram( GLProgram *program )
{
    if(program == NULL || program == GLProgram::null()) return -1;
    return tweakProgram(program->name);
}

inline
int doTweaks( nv::UIContext *widgets, const GLuint program )
{
    if(widgets == NULL) return -1;
    return ProgramTweaks::manager().doTweaks(widgets, program);
}

inline
int doTweaks( nv::UIContext *widgets, GLProgram *program )
{
    if(program == NULL || program == GLProgram::null()) return -1;
    return doTweaks(widgets, program->name);
}

inline
int doTweaks( nv::SdlContext& widgets, GLProgram *program )
{
    if(program == NULL || program == GLProgram::null()) return -1;
    return doTweaks(&widgets, program->name);
}

} // namespace

#endif
