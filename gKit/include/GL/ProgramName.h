
#ifndef _PROGRAM_NAME_H
#define _PROGRAM_NAME_H

#include "GL/GLPlatform.h"
#include "GL/GLSLUniforms.h"
#include "Vec.h"


namespace gk {
    
class GLProgram;

//! utilisation interne. identifiant generique d'une variable d'un GLProgram.
//! \ingroup OpenGL.
class ProgramName
{
    ProgramName& operator= ( const ProgramName& );
    
public:
    enum {
        UNIFORM= 0,
        INTEGER,
        SAMPLER,
        IMAGE
    };
    
    const GLProgram *program;
    GLint location;
    GLint index;
    int size;
    GLenum type;
    unsigned int flags;

    ProgramName( )
        :
        program(NULL),
        location(-1),
        index(-1),
        size(0),
        type(0),
        flags(0u)
    {}

    ProgramName( const ProgramName& name ) 
        :
        program(name.program),
        location(name.location),
        index(name.index),
        size(name.size),
        type(name.type),
        flags(name.flags)
    {}
        
    ProgramName( const GLProgram *_program, const GLint _location, const GLint _index, 
        const int _size= 0, const GLenum _type= 0, const unsigned int _flags= UNIFORM ) 
        :
        program(_program),
        location(_location),
        index(_index),
        size(_size),
        type(_type),
        flags(_flags)
    {}
    
    ~ProgramName( ) {}
    
    ProgramName& operator= ( const int x );
    ProgramName& operator= ( const unsigned int x );
    ProgramName& operator= ( const float x );
    
    template < typename T > ProgramName& operator= ( const TVec2<T>& x );
    template < typename T > ProgramName& operator= ( const TVec3<T>& x );
    template < typename T > ProgramName& operator= ( const TVec4<T>& x );
    ProgramName& operator= ( const VecColor& x );
    
    ProgramName& operator= ( const Mat4& v );
    
    bool isValid( ) const
    {
        return (program != NULL && location >= 0);
    }
    
    operator GLint ( ) const
    {
        return location;
    }
    
    bool isInteger( ) const
    {
        return flags & (1 << INTEGER);
    }
    
    bool isSampler( ) const
    {
        return flags & (1 << SAMPLER);
    }
    
    bool isImage( ) const
    {
        return flags & (1 << IMAGE);
    }
    
    bool isArray( ) const
    {
        return (size > 1);
    }
};

template <> ProgramName& ProgramName::operator= ( const TVec2<int>& v );
template <> ProgramName& ProgramName::operator= ( const TVec3<int>& v );
template <> ProgramName& ProgramName::operator= ( const TVec4<int>& v );
template <> ProgramName& ProgramName::operator= ( const TVec2<unsigned int>& v );
template <> ProgramName& ProgramName::operator= ( const TVec3<unsigned int>& v );
template <> ProgramName& ProgramName::operator= ( const TVec4<unsigned int>& v );
template <> ProgramName& ProgramName::operator= ( const TVec2<float>& v );
template <> ProgramName& ProgramName::operator= ( const TVec3<float>& v );
template <> ProgramName& ProgramName::operator= ( const TVec4<float>& v );

//! identifiant d'un uniform.
typedef ProgramName ProgramUniform;
//! identifiant d'un attribut.
typedef ProgramName ProgramAttribute;
//! identifiant d'un transform feedback varying.
typedef ProgramName ProgramFeedback;
//! identifiant d'un sampler.
typedef ProgramName ProgramSampler;
//! identifiant d'une image.
typedef ProgramName ProgramImage;

//! identifiant d'un uniform block / shader storage buffer.
typedef ProgramName ProgramBuffer;


//! modifier la valeur d'un sampler du shader program actif : index de l'unite de texture.
int setSamplerUniform( const ProgramSampler& sampler, const int unit );

}

#endif
