
#ifndef _GL_PROGRAM_H
#define _GL_PROGRAM_H

#include "GL/GLPlatform.h"
#include "GLResource.h"
#include "GL/ProgramName.h"

#include "Logger.h"


namespace gk {

//! representation d'un shader program.
//! \ingroup OpenGL.
class GLProgram : public GLResource
{
    // non copyable
    GLProgram( const GLProgram& );
    GLProgram& operator= ( const GLProgram& );
    
protected:
    struct parameter
    {
        std::string name;
        int location;
        int index;
        int size;
        GLenum type;
        unsigned int flags;
        
        parameter( )
            :
            name(),
            location(-1),
            index(-1),
            size(0),
            type(0),
            flags(0u)
        {}
        
        parameter( const char *_name, const int _location, const int _index, 
            const int _size= 0, const GLenum _type= 0, const unsigned int _flags= 0u)
            :
            name(_name),
            location(_location),
            index(_index),
            size(_size),
            type(_type),
            flags(_flags)
        {}
        
        ~parameter( ) {}
    };
    
    std::vector<parameter> m_feedbacks;
    std::vector<parameter> m_attributes;
    std::vector<parameter> m_uniforms;
    std::vector<parameter> m_samplers;
    std::vector<parameter> m_images;
    
    std::vector<parameter> m_storage_buffers;
    std::vector<parameter> m_uniform_buffers;
    
public:
    std::vector<GLuint> shaders;
    
    //! constructeur par defaut.
    GLProgram( )
        :
        GLResource(),
        shaders(SHADERTYPE_LAST, 0) 
    {}
    
    //! constructuer d'un programme nomme, cf khr_debug.
    GLProgram( const std::string& _label )
        :
        GLResource(_label),
        shaders(SHADERTYPE_LAST, 0)
    {}
    
    //! creation d'un shader program opengl.
    GLProgram *create( )
    {
        assert(name == 0 && "create progran error");
        name= glCreateProgram();
        if(name == 0)
            return this;
        
        manage();
        if(label.empty())
            DEBUGLOG("create program %d\n", name);
        else
            DEBUGLOG("create program %d, label '%s'\n", name, label.c_str());
        return this;
    }
    
    //! destruction du shader program opengl.
    virtual void release( )
    {
        if(name != 0)
            glDeleteProgram(name);
        name= 0;
    }
    
    //! desctructeur.
    virtual ~GLProgram( ) {}
    
    //! enumeration des ressources du programme. necessaire avant d'appeller attribute()/uniform()...
    int resources( );
    
    //! renvoie le nom d'un uniform.
    const char *uniformName( const ProgramUniform& uniform ) const
    {
        return m_uniforms[uniform.index].name.c_str();
    }

    //! renvoie le nom d'un attribut.
    const char *attributeName( const ProgramAttribute& attribute ) const
    {
        return m_attributes[attribute.index].name.c_str();
    }
    
    //! renvoie le nom d'un sampler (cf GLTexture).
    const char *samplerName( const ProgramSampler& sampler ) const
    {
        return m_samplers[sampler.index].name.c_str();
    }
    
    //! renvoie le nom d'une image.
    const char *imageName( const ProgramImage& image ) const
    {
        return m_images[image.index].name.c_str();
    }
    
    //! recherche un attribut.
    ProgramAttribute attribute( const char *name ) const;
    
    //! recherche un uniform.
    ProgramUniform uniform( const char *name ) const;
    //! recherche un smapler.
    ProgramUniform sampler( const char *name ) const;
    //! recherche une image.
    ProgramUniform image( const char *name ) const;
    //~ ProgramUniform subroutineUniform( const char *name );

    //! recherche un uniform buffer.
    ProgramBuffer uniformBuffer( const char *name ) const;
    //! recherche un shader storage buffer.
    ProgramBuffer storageBuffer( const char *name ) const;
    //~ ProgramBuffer atomicBuffer( const char *name );
    
    //~ ProgramIndex subroutine( const char *name );
    //! recherche un varying a enregistrer dans un buffer / feedback. cf glTransformFeedback.
    ProgramFeedback feedback( const char *name ) const;
    
    //! renvoie un objet c++ pour le program opengl 0.
    static
    GLProgram *null( )
    {
        static GLProgram object("null program");
        return &object;
    }
    
    //! types de shaders.
    enum
    {
        VERTEX= 0,      //!< type d'un vertex shader,
        FRAGMENT,       //!< type d'un fragment shader,
        GEOMETRY,       //!< type d'un geometry shader,
        CONTROL,        //!< type d'un control shader,
        EVALUATION,     //!< type d'un evaluation shader,
        COMPUTE,        //!< type d'un compute shader
        SHADERTYPE_LAST
    };
    
    static GLenum types[];      //!< type opengl.
    static const char *labels[];        //!< nom d'un shader en fonction de son type.
};


namespace glsl {
    
bool is_sampler( const GLenum type );
bool is_image( const GLenum type );
bool is_integer( const GLenum type );
bool is_matrix( const GLenum type );
const char *type_string( const GLenum value );

}       // namespace 

}       // namespace 

#endif
