
#include "ProgramTweaks.h"

#include "Widgets/nvWidgets.h"
#include "GL/GLProgramUniforms.h"


namespace gk {

ProgramTweaks::ProgramTweaks( ) : programs() {}
ProgramTweaks::~ProgramTweaks( ) {}
    
int ProgramTweaks::tweak( const GLuint program ) { return -1; }
int ProgramTweaks::doTweaks( nv::UIContext *widgets, const GLuint program ) { return -1; }

}       // namespace
