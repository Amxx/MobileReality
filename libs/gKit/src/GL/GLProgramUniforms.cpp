
#include "GL/GLProgramUniforms.h"

namespace gk {
    

UniformValue *UniformValue::create( const ProgramName& uniform )
{
    if(uniform.isInteger()) return new UniformInt(uniform);
    if(uniform.isVec2()) return new UniformVec2(uniform);
    if(uniform.isVec3()) return new UniformVec3(uniform);
    if(uniform.isVec4()) return new UniformVec4(uniform);

    return new UniformFloat(uniform);
}


UniformInt& UniformInt::operator= ( const std::string& _value )
{ 
    int x= 0;
    if(sscanf(_value.c_str(), "%d", &x) != 1)
        ERROR("UniformInt('%s') : wrong value '%s', using %d\n", uniform.program->uniformName(uniform), _value.c_str(), x);
    
    uniform= x;
    return *this; 
}

void UniformInt::update( )
{
    int values[4];
    glGetUniformiv(uniform.program->name, uniform.location, values);
    
    char tmp[1024];
    sprintf(tmp, "%d", values[0]);
    //~ printf("update int '%s': %s\n", uniform.program->uniformName(uniform), tmp);
    value= tmp;
}


UniformFloat& UniformFloat::operator= ( const std::string& _value )
{ 
    float x= 0;
    if(sscanf(_value.c_str(), "%f", &x) != 1)
        ERROR("UniformFloat('%s') : wrong value '%s', using %f\n", uniform.program->uniformName(uniform), _value.c_str(), x);
    
    uniform= x;
    return *this; 
}

void UniformFloat::update( )
{
    float values[4];
    glGetUniformfv(uniform.program->name, uniform.location, values);
    
    char tmp[1024];
    sprintf(tmp, "%f", values[0]);
    //~ printf("update float '%s': %s\n", uniform.program->uniformName(uniform), tmp);
    value= tmp;
}


UniformVec2& UniformVec2::operator= ( const std::string& _value )
{ 
    Vec2 v;
    if(sscanf(_value.c_str(), "%f%*[ f,] %f", &v.x, &v.y) != 2)
        ERROR("UniformVec2('%s') : wrong value '%s', using %f %f %f %f\n", uniform.program->uniformName(uniform), _value.c_str(), v.x, v.y);
    
    uniform= v;
    return *this; 
}

void UniformVec2::update( )
{
    float values[4];
    glGetUniformfv(uniform.program->name, uniform.location, values);
    
    char tmp[1024];
    sprintf(tmp, "%f %f", values[0], values[1]);
    //~ printf("update vec2 '%s': %s\n", uniform.program->uniformName(uniform), tmp);
    value= tmp;
}


UniformVec3& UniformVec3::operator= ( const std::string& _value )
{ 
    Vec3 v;
    if(sscanf(_value.c_str(), "%f%*[ f,] %f%*[ f,] %f", &v.x, &v.y, &v.z) != 3)
        ERROR("UniformVec3('%s') : wrong value '%s', using %f %f %f %f\n", uniform.program->uniformName(uniform), _value.c_str(), v.x, v.y, v.z);
    
    uniform= v;
    return *this; 
}

void UniformVec3::update( )
{
    float values[4];
    glGetUniformfv(uniform.program->name, uniform.location, values);
    
    char tmp[1024];
    sprintf(tmp, "%f %f %f", values[0], values[1], values[2]);
    //~ printf("update vec3 '%s': %s\n", uniform.program->uniformName(uniform), tmp);
    value= tmp;
}


UniformVec4& UniformVec4::operator= ( const std::string& _value )
{ 
    Vec4 v;
    if(sscanf(_value.c_str(), "%f%*[ f,] %f%*[ f,] %f%*[ f,] %f", &v.x, &v.y, &v.z, &v.w) != 4)
        ERROR("UniformVec4('%s') : wrong value '%s', using %f %f %f %f\n", uniform.program->uniformName(uniform), _value.c_str(), v.x, v.y, v.z, v.w);
    
    uniform= v;
    return *this; 
}

void UniformVec4::update( )
{
    float values[4];
    glGetUniformfv(uniform.program->name, uniform.location, values);
    
    char tmp[1024];
    sprintf(tmp, "%f %f %f %f", values[0], values[1], values[2], values[3]);
    //~ printf("update vec4 '%s': %s\n", uniform.program->uniformName(uniform), tmp);
    value= tmp;
}


int GLProgramUniforms::find( const std::string& name )
{
    for(unsigned int i= 0; i < names.size(); i++)
        if(names[i] == name)
            return i;
    
    return -1;
}

GLProgramUniforms::GLProgramUniforms( GLProgram *_program ) : names(), values(), program(GLProgram::null()), program_changes(0)
{
    if(_program == NULL || _program == GLProgram::null())
        return;
    
    program= _program;
    program_changes= program->changes;
    
    unsigned int n= program->uniformCount();
    for(unsigned int i= 0; i < n; i++)
    {
        ProgramUniform uniform= program->uniform(i);
        
        names.push_back( program->uniformName(uniform) );
        values.push_back( UniformValue::create(uniform) );
    }
    
    //~ printf("%lu uniforms\n", values.size());
}

GLProgramUniforms::~GLProgramUniforms( )
{
    for(unsigned int i= 0; i < values.size(); i++)
        delete values[i];
}

void GLProgramUniforms::update( )
{
    if(program_changes != program->changes)
    {
        // le programme a change, enumere les nouveaux uniforms
        unsigned int n= program->uniformCount();
        for(unsigned int i= 0; i < n; i++)
        {
            ProgramUniform uniform= program->uniform(i);
            std::string name= program->uniformName(uniform);

            UniformValue *value= UniformValue::create(uniform);
            
            int u= find(name);
            if(u < 0)
            {
                // nouvel uniform
                names.push_back( name );
                values.push_back( value );
            }
            else
            {
                // remplacer l'uniform 
                std::string tmp= values[u]->value;
                std::string edit_tmp= values[u]->edit_value;
                bool edit= values[u]->edit;
                
                delete values[u];
                values[u]= value;
                
                // propage les valeurs modifiees
                values[u]->value= tmp;
                values[u]->edit= edit;
                values[u]->edit_value= edit_tmp;
            }
        }
        
        for(unsigned int i= 0; i < values.size(); i++)
            if(values[i]->uniform.isValid() == false)
            {
                // remplacer l'uniform par une constante
                std::string tmp= values[i]->value;
                std::string edit_tmp= values[i]->edit_value;
                bool edit= values[i]->edit;
                
                UniformValue *value= new UniformConst(values[i]->uniform);
                delete values[i];
                values[i]= value;
                
                // propage les valeurs modifiees
                values[i]->value= tmp;
                values[i]->edit= edit;
                values[i]->edit_value= edit_tmp;
            }
    }
    
    for(unsigned int i= 0; i < values.size(); i++)
        if(values[i]->uniform.isValid())
            values[i]->update();
}

void GLProgramUniforms::assign( )
{
    for(unsigned int i= 0; i < values.size(); i++)
        if(values[i]->edit)
            *values[i]= values[i]->edit_value;
}

std::string GLProgramUniforms::summary( )
{
    std::string tmp;
    for(unsigned int i= 0; i < values.size(); i++)
    {
        tmp.append(names[i]).append("= ");
        tmp.append(values[i]->value).append(" ");
        if(values[i]->edit)
            tmp.append(values[i]->edit_value);
        tmp.append("\n");
    }
    
    return tmp;
}

}       // namespace
