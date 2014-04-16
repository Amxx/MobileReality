
#ifndef _GL_DEBUG_H
#define _GL_DEBUG_H

#include <string>
#include <vector>
#include <cstdio>
#include <cstring>

#include "GL/glew.h"
#include <GL/gl.h>


namespace gk {

namespace debug {

const char *modeString( GLenum value );
const char *typeString( GLenum value );
std::string intString( unsigned int value );
std::string offsetString( unsigned long int value );

    
struct Draw;

struct BufferBinding
{
    GLint name;
    GLint enabled;
    GLint size;         // GL size : 1, 2, 3
    GLint type;         // GL type : GL_FLOAT
    GLint stride;
    GLint glsl_type;    // GLSL : GL_FLOAT_VEC3
    GLint normalized;
    GLint integer;
    GLint divisor;
    GLint64 length;
    GLint64 offset;
};

struct VertexState
{
    std::vector<BufferBinding> buffers;
    GLint vertex_array;
    GLint vertex_buffer;
    GLint index_buffer;
    
    VertexState( );
    ~VertexState( );
    int build( Draw *draw );
};

struct Attribute
{
    std::vector<GLchar> name;
    GLint array_size;
    GLint glsl_type;
};

struct AttributeState
{
    std::vector<Attribute> attributes;
    int required_attributes;
    
    AttributeState( );
    ~AttributeState( );
    int build( Draw *draw );
};

struct ProgramState
{
    GLint name;
    std::vector<GLuint> shaders;
    
    ProgramState( );
    ~ProgramState( );
    int build( Draw *draw );
};


struct Draw
{
    std::string call;
    int call_id;
    
    enum {
        undef= 0,
        ARRAYS= 1,
        ELEMENTS
    };
    
    GLenum mode;
    GLint first;
    GLsizei count;
    GLenum type;
    unsigned long int indices;
    
    bool require_index;
    //~ bool require_instance;
    //~ bool require_indirect;
    
    VertexState vertex_state;
    AttributeState attribute_state;
    ProgramState program_state;
    std::string output;
    
    Draw( const char *_call, unsigned int _id )
        :
        call(_call), call_id(_id),
        mode(0), first(0), count(0), type(0), indices(0),
        require_index(false),
        vertex_state(), attribute_state(), program_state()
    {}
    
    int buildState( );
    
    virtual ~Draw( ) { printf("error: debug::Draw::~Draw( ) called.\n"); }
    virtual int draw( ) { printf("error: debug::Draw::draw( ) called.\n"); return -1; }
    virtual std::string parameters( ) { printf("error: debug::Draw::parameters() called.\n"); return ""; }
};

struct DrawArrays : public Draw
{
    DrawArrays( GLenum _mode, GLint _first, GLsizei _count );
    std::string parameters( );
    int draw( );
};

struct DrawElements : public Draw
{
    DrawElements( GLenum _mode, GLsizei _count, GLenum _type, GLvoid * _indices );
    std::string parameters( );
    int draw( );
};

};

//! serveur
class GLDebugger
{
    // non copyable
    GLDebugger( const GLDebugger& );
    GLDebugger& operator= ( const GLDebugger& );

public:
    GLDebugger( );

    //~ virtual int run( ) = 0;
    
    virtual void break_frame( )
    {
        printf("frame %d:\n%s\n", frame_id, trace().c_str());
        //~ run();
    }
    
    virtual void break_call( debug::Draw *call )
    {
        printf("frame %d: call %lu:\n%s\n%s", frame_id, m_trace.size(), 
            call->parameters().c_str(), call->output.c_str());
        //~ run();
    }
    
protected:
    int pushDraw( debug::Draw *call )
    {
        if(call->buildState() < 0)
            call->output.append("internal error.\n");
        
        m_trace.push_back(call); 
        // + recuperer debug output
        // + temps cpu / gpu 
        call->draw();
        
        if(m_break_on_draw)
            break_call(call);
        
        return 0;
    }
    
    int beginFrame( ) { m_trace.clear(); return 0; }
    int endFrame() { frame_id++; return 0; }
    
    std::vector<debug::Draw *> m_trace;
    
    bool m_break;
    bool m_break_on_frame;
    bool m_break_on_draw;
    // bool m_break_on_error;
    
    int frame_id;
    
public:
    // controle de l'execution de l'application
    int nextFrame( ) { m_break= true; m_break_on_frame= true; m_break_on_draw= false; return 0; }
    int nextDraw( ) { m_break= true; m_break_on_frame= false; m_break_on_draw= true; return 0; }
    
    int play( ) { m_break= false; m_break_on_frame= false; m_break_on_draw= false; return 0; }
    int stop( ) { m_break= true; m_break_on_frame= true; m_break_on_draw= false; return 0; }
    
    // 
    std::string trace( )
    {
        std::string tmp;
        for(unsigned int i= 0; i < m_trace.size(); i++)
            tmp.append(m_trace[i]->parameters()).append("\n");
        
        return tmp;
    }
    
    // construction de la trace
    int swapBuffers( )
    {
        endFrame();
        
        // stop 
        if(m_break_on_frame)
            break_frame();        // appeler fonction virtuelle de la classe derivee
        
        //
        beginFrame();
        return 0;
    }
    
    int drawArrays( GLenum mode, GLint first, GLsizei count )
    {
        return pushDraw( new debug::DrawArrays(mode, first, count) );
    }
    
    int drawElements( GLenum mode, GLsizei count, GLenum type, GLvoid * indices )
    {
        return pushDraw( new debug::DrawElements(mode, count, type, indices) );
    }
};

}       // namespace

#endif

