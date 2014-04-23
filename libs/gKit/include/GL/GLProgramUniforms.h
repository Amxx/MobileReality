
#ifndef _PROGRAM_UNIFORMS_H
#define _PROGRAM_UNIFORMS_H

#include "GL/GLProgram.h"
#include "GL/ProgramName.h"


namespace gk {

//! representation de la valeur d'un uniform. classe de base a deriver en fonction du type de l'uniform.
struct UniformValue
{
    ProgramName uniform;
    std::string value;
    std::string edit_value;
    bool edit;
    
    UniformValue( const ProgramName& _uniform ) : uniform(_uniform), value(), edit_value(), edit(false) {}
    virtual ~UniformValue( ) {}
    
    //! affecte une valeur a l'uniform.
    virtual UniformValue& operator= ( const std::string& _value )= 0;
    
    //! renvoie la valeur actuelle de l'uniform.
    virtual operator std::string( ) { return value; }
    
    //! recupere la valeur de l'uniform.
    virtual void update( )= 0;
    
    //! utilisation interne, creation d'une classe derivee en fonction du type de l'uniform.
    static UniformValue *create( const ProgramName& uniform );
};


//! representation d'un uniform non modifiable / constant.
struct UniformConst : public UniformValue
{
    UniformConst( const ProgramName& _uniform ) : UniformValue(_uniform) {}
    UniformConst& operator= ( const std::string& _value ) { return *this; }
    void update( ) { return; }
};

//! representation d'un uniform de type int.
struct UniformInt : public UniformValue
{
    UniformInt( const ProgramName& _uniform ) : UniformValue(_uniform) {}
    UniformInt& operator= ( const std::string& _value );
    void update( );
};

//! representation d'un uniform de type float.
struct UniformFloat : public UniformValue
{
    UniformFloat( const ProgramName& _uniform ) : UniformValue(_uniform) {}
    UniformFloat& operator= ( const std::string& _value );
    void update( );
};

//! representation d'un uniform de type vec2.
struct UniformVec2 : public UniformValue
{
    UniformVec2( const ProgramName& _uniform ) : UniformValue(_uniform) {}
    UniformVec2& operator= ( const std::string& _value );
    void update( );
};

//! representation d'un uniform de type vec3.
struct UniformVec3 : public UniformValue
{
    UniformVec3( const ProgramName& _uniform ) : UniformValue(_uniform) {}
    UniformVec3& operator= ( const std::string& _value );
    void update( );
};

//! representation d'un uniform de type vec4.
struct UniformVec4 : public UniformValue
{
    UniformVec4( const ProgramName& _uniform ) : UniformValue(_uniform) {}
    UniformVec4& operator= ( const std::string& _value );
    void update( );
};


//! representation de l'ensemble d'uniforms d'un shader program.
struct GLProgramUniforms
{
    std::vector<std::string> names;
    std::vector<UniformValue *> values;
    
    GLProgram *program;
    unsigned int program_changes;
    
    int find( const std::string& name );
    
    GLProgramUniforms(  ) : names(), values(), program(GLProgram::null()), program_changes(0) {}
    GLProgramUniforms( GLProgram *_program );
    ~GLProgramUniforms( );
    
    void update( );
    void assign( );
    
    std::string summary( );
};

}       // namespace
#endif
