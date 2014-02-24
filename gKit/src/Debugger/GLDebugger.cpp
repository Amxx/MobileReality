
#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cstdarg>

#include "GL/glew.h"
#include <GL/gl.h>

#include "Debugger/GLDebugger.h"


namespace gk {

namespace debug {

//! classe utilitaire : permet de construire une chaine de caracteres formatee. cf sprintf.
struct Format
{
    char text[1024];
    
    Format( const char *_format, ... )
    {
        text[0]= 0;     // chaine vide
        
        // recupere la liste d'arguments supplementaires
        va_list args;
        va_start(args, _format);
        vsnprintf(text, sizeof(text), _format, args);
        va_end(args);
    }
    
    ~Format( ) {}
    
    // conversion implicite de l'objet en chaine de caracteres stantard
    operator const char *( ) { return text; }
};


const char *modeString( GLenum value )
{
#define mode(v) case v: return #v;
    
    switch(value)
    {
        mode(GL_POINTS)
        mode(GL_LINE_STRIP)
        mode(GL_LINE_LOOP)
        mode(GL_LINES)
        mode(GL_LINE_STRIP_ADJACENCY)
        mode(GL_LINES_ADJACENCY)
        mode(GL_TRIANGLE_STRIP)
        mode(GL_TRIANGLE_FAN)
        mode(GL_TRIANGLES)
        mode(GL_TRIANGLE_STRIP_ADJACENCY)
        mode(GL_TRIANGLES_ADJACENCY)
        mode(GL_PATCHES)
        default: return "invalid enum";
    }
    
#undef mode
}

const char *typeString( GLenum value )
{
#define type(v) case v: return #v;
    
    switch(value)
    {
        type(GL_UNSIGNED_BYTE)
        type(GL_UNSIGNED_SHORT)
        type(GL_UNSIGNED_INT)
        default: return "invalid type";
    }
    
#undef type
}

std::string intString( unsigned int value )
{
    char tmp[1024];
    sprintf(tmp, "%u", value);
    return tmp;
}

std::string offsetString( unsigned long int value )
{
    char tmp[1024];
    sprintf(tmp, "%lu", value);
    return tmp;
}


//
int gl_sizeof( const int size, const GLenum type, const int stride= 0 )
{
    if(stride != 0)
        return stride;
    
    int length= 0;
    switch(type)
    {
        case GL_UNSIGNED_BYTE:
        case GL_BYTE:
            length= 1;
            break;
        case GL_UNSIGNED_SHORT:
        case GL_SHORT:
        case GL_HALF_FLOAT:
            length= 2;
            break;
        case GL_FIXED:
        case GL_INT_2_10_10_10_REV:
        case GL_UNSIGNED_INT_2_10_10_10_REV:
        case GL_UNSIGNED_INT:
        case GL_INT:
            length= 4;
            break;
        case GL_FLOAT:
            length= 4;
            break;
        case GL_DOUBLE:
            length= 8;
            break;
        
        default:
            //~ ERROR("unknown type");
            return 0;
    }
    
    return size * length;
}


// vertex state
VertexState::VertexState( ) {}
VertexState::~VertexState( ) {}

int VertexState::build( Draw *draw )
{
    vertex_buffer= 0;
    index_buffer= 0;
    vertex_array= 0;
    buffers.clear();
    
    if(draw == NULL || draw->program_state.name == 0)
        return -1;
    if(draw == NULL || draw->attribute_state.required_attributes == 0)
        return -1;
    
    int attribute_count= draw->attribute_state.required_attributes;
    GLint active_program= draw->program_state.name;
    
    //~ WARNING("active buffers:\n");
    
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vertex_buffer);
    //~ if(vertex_buffer == 0)
        //~ WARNING("  no vertex buffer object\n");
    //~ else
        //~ WARNING("  vertex buffer object %d\n", active_vertex_buffer);
    
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &index_buffer);
    if(index_buffer == 0 && draw->require_index)
        draw->output.append("  no index buffer object\n");
    //~ else
        //~ WARNING("  index buffer object %d\n", active_index_buffer);
    
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vertex_array);
    if(vertex_array == 0)
        draw->output.append("  no vertex array object. can't draw anything\n");
    //~ else
        //~ WARNING("  vertex array object %d:\n", vertex_array);
    
    buffers.resize(attribute_count);
    
    bool failed= false;
    for(int i= 0; i < attribute_count; i++)
    {
        GLint attribute_buffer= 0;
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &attribute_buffer);
        if(attribute_buffer == 0)
        {
            draw->output.append(Format("no vertex buffer bound to attribute %d '%s'.\n", 
                i, &draw->attribute_state.attributes[i].name.front()));
            //~ failed= true;
            
            continue;
        }
        
        GLint size, type, stride;
        GLint enabled, normalized;
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &normalized);
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_SIZE, &size);
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_TYPE, &type);
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride);
        if(stride == 0)
            stride= gl_sizeof(size, type);
        
        // determine glsl type from gl (size, type)
        GLint glsl_type= type;
        if(size > 1)
        {
            switch(type)
            {
                case GL_FLOAT:
                    glsl_type= GL_FLOAT_VEC2 + size-2;
                    break;
                case GL_DOUBLE:
                    glsl_type= GL_DOUBLE_VEC2 + size-2;
                    break;
                case GL_INT:
                    glsl_type= GL_INT_VEC2 + size-2;
                    break;
                case GL_UNSIGNED_INT:
                    glsl_type= GL_UNSIGNED_INT_VEC2 + size-2;
                    break;
                //!\todo more types
            }
        }
        
        GLvoid *offset= NULL;
        glGetVertexAttribPointerv(i, GL_VERTEX_ATTRIB_ARRAY_POINTER, &offset);
        
        GLint64 length= 0;
        glBindBuffer(GL_ARRAY_BUFFER, attribute_buffer);
        glGetBufferParameteri64v(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &length);
        
        buffers[i].name= attribute_buffer;
        buffers[i].enabled= enabled;
        buffers[i].size= size;
        buffers[i].type= type;
        buffers[i].glsl_type= glsl_type;
        buffers[i].normalized= normalized;
        buffers[i].stride= stride;
        buffers[i].length= length;
        buffers[i].offset= (GLint64) offset;
        
        //~ if(attribute_buffer != 0 /* && enabled != 0 */)
        //~ {
            //~ if(active_program != 0)
                //~ WARNING("    attribute %d '%s': vertex buffer object %d, enabled %d, item size %d, item type 0x%x, glsl type 0x%x, stride %d, offset %lu\n", 
                    //~ i, &draw->attribute->attributes[i].name.front(), 
                    //~ attribute_buffer, enabled, size, type, glsl_type, stride, offset);
            //~ else
                //~ WARNING("    attribute %d: vertex buffer object %d, enabled %d, item size %d, item type 0x%x, glsl type 0x%0x, stride %d, offset %lu\n", 
                    //~ i, attribute_buffer, 
                    //~ enabled, size, type, glsl_type, stride, offset);
        //~ }
        if(enabled == GL_FALSE)
            draw->output.append(Format("attribute %d '%s': vertex buffer object %d, not enabled.\n",  i, &draw->attribute_state.attributes[i].name.front()));
    }
    
    // restore state
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    return failed ? -1 : 0;
}

// attribute state
AttributeState::AttributeState( ) {}
AttributeState::~AttributeState( ) {}

int AttributeState::build( Draw *draw )
{
    attributes.clear();

    if(draw == NULL || draw->program_state.name == 0)
        return -1;      // no program
    
    GLint active_program= draw->program_state.name;
    glGetProgramiv(active_program, GL_ACTIVE_ATTRIBUTES, &required_attributes);
    attributes.resize(required_attributes);
    
    //~ WARNING("%d required attributes:\n", active_attribute_count);
    
    GLint attribute_length= 0;
    glGetProgramiv(active_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attribute_length);
    
    GLint array_size;
    GLenum glsl_type;
    for(int i= 0; i < required_attributes; i++)
    {
        attributes[i].name.clear();
        attributes[i].name.resize(attribute_length);
        glGetActiveAttrib(active_program, i, attribute_length, NULL, &array_size, &glsl_type, &attributes[i].name.front());
        //~ WARNING("  attribute %d '%s': array size %d, glsl type 0x%x\n", i, 
            //~ &attributes[i].name.front(), array_size, glsl_type);
        
        attributes[i].array_size= array_size;
        attributes[i].glsl_type= glsl_type;
    }
    
    return 0;
}

// program state
ProgramState::ProgramState( ) {}
ProgramState::~ProgramState( ) {}

int ProgramState::build( Draw *draw )
{
    if(draw == NULL)
        return -1;
    
    glGetIntegerv(GL_CURRENT_PROGRAM, &draw->program_state.name);
    if(draw->program_state.name == 0)
    {
        draw->output.append("no shader program. can't draw anything.\n");
        return -1;      // no program
    }
    
    //! \todo add support for program pipelines. 
    
    GLint active_program= draw->program_state.name;
    
    GLint linked;
    glGetProgramiv(active_program, GL_LINK_STATUS, &linked);
    if(linked == GL_FALSE)
    {
        draw->output.append("shader program is not linked. can't draw anything.\n");
        return -1;      // program can't run
    }
    
    return 0;
}


int Draw::buildState( )
{
    if(program_state.build(this) < 0)
        return -1;
    if(attribute_state.build(this) < 0)
        return -1;
    if(vertex_state.build(this) < 0)
        return -1;
    
    return 0;
}


// draw arrays
DrawArrays::DrawArrays( GLenum _mode, GLint _first, GLsizei _count )
    :
    Draw("glDrawArrays", ARRAYS)
{
    mode= _mode;
    first= _first;
    count= _count;
    require_index= false;
}

std::string DrawArrays::parameters( )
{
    std::string tmp;
    tmp.append(call).append("(");
    tmp.append("mode= ");
    tmp.append(modeString(mode));
    tmp.append(", first= ");
    tmp.append(intString(first));
    tmp.append(", count= ");
    tmp.append(intString(count));
    tmp.append(")");
    
    return tmp;
}

int DrawArrays::draw( )
{
    if(call_id != ARRAYS)
        return  -1;
    
    glDrawArrays(mode, first, count);
    return 0;
}


// draw elements
DrawElements::DrawElements( GLenum _mode, GLsizei _count, GLenum _type, GLvoid * _indices )
    :
    Draw("glDrawElements", ELEMENTS)
{
    mode= _mode;
    count= _count;
    type= _type;
    indices= (unsigned long int) _indices;
    require_index= true;
}

std::string DrawElements::parameters( )
{
    std::string tmp;
    tmp.append(call).append("(");
    tmp.append("mode= ");
    tmp.append(modeString(mode));
    tmp.append(", count= ");
    tmp.append(intString(count));
    tmp.append(", type= ");
    tmp.append(typeString(type));
    tmp.append(", indices= ");
    tmp.append(offsetString(indices));
    tmp.append(")");
    
    return tmp;
}

int DrawElements::draw( )
{
    if(call_id != ELEMENTS)
        return -1;
    
    glDrawElements(mode, count, type, (const GLvoid *) indices);
    return 0;
}

}       // gkd namespace


GLDebugger::GLDebugger( ) : m_break(true), m_break_on_frame(true), m_break_on_draw(false), frame_id(0)
{}

}       // namespace

