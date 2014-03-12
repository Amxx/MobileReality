
#include <cstring>

#include "GL/GLProgram.h"


namespace gk {

GLenum GLProgram::types[]= {
    GL_VERTEX_SHADER,
    GL_FRAGMENT_SHADER,
    GL_GEOMETRY_SHADER,
    GL_TESS_CONTROL_SHADER,
    GL_TESS_EVALUATION_SHADER,
    GL_COMPUTE_SHADER,
    0
};

const char *GLProgram::labels[]= {
    "vertex",
    "fragment",
    "geometry",
    "control",
    "evaluation",
    "compute",
    ""
};


namespace glsl {
    
bool is_sampler( const GLenum type )
{
    switch(type)
    {
        case  GL_SAMPLER_1D:
        case  GL_SAMPLER_2D:
        case  GL_SAMPLER_3D:
        case  GL_SAMPLER_CUBE:
        case  GL_SAMPLER_1D_SHADOW:
        case  GL_SAMPLER_2D_SHADOW:
        
    #ifdef GL_VERSION_3_0
        case  GL_SAMPLER_1D_ARRAY:
        case  GL_SAMPLER_2D_ARRAY:
        case  GL_SAMPLER_1D_ARRAY_SHADOW:
        case  GL_SAMPLER_2D_ARRAY_SHADOW:
        case  GL_SAMPLER_CUBE_SHADOW:
        case  GL_INT_SAMPLER_1D:
        case  GL_INT_SAMPLER_2D:
        case  GL_INT_SAMPLER_3D:
        case  GL_INT_SAMPLER_CUBE:
        case  GL_INT_SAMPLER_1D_ARRAY:
        case  GL_INT_SAMPLER_2D_ARRAY:
        case  GL_UNSIGNED_INT_SAMPLER_1D:
        case  GL_UNSIGNED_INT_SAMPLER_2D:
        case  GL_UNSIGNED_INT_SAMPLER_3D:
        case  GL_UNSIGNED_INT_SAMPLER_CUBE:
        case  GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
        case  GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
    #endif
    
    #ifdef GL_VERSION_3_1
        case  GL_SAMPLER_2D_RECT:
        case  GL_SAMPLER_2D_RECT_SHADOW:
        case GL_SAMPLER_BUFFER:
        case GL_INT_SAMPLER_2D_RECT:
        case GL_INT_SAMPLER_BUFFER:
        case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
        case GL_UNSIGNED_INT_SAMPLER_BUFFER:
    #endif
    
    #ifdef GL_VERSION_4_0
        case GL_SAMPLER_CUBE_MAP_ARRAY:
        case GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW:
        case GL_INT_SAMPLER_CUBE_MAP_ARRAY:
        case GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY:
    #endif
    
    #ifdef GL_ARB_texture_multisample
        case GL_SAMPLER_2D_MULTISAMPLE:
        case GL_INT_SAMPLER_2D_MULTISAMPLE:
        case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
        case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
        case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
        case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    #endif
            return true;

        default:
            return false;
    }
}

bool is_image( const GLenum type )
{
    switch(type)
    {
        #ifdef GL_ARB_shader_image_load_store
        case  GL_IMAGE_1D:
        case  GL_IMAGE_2D:
        case  GL_IMAGE_3D:
        case  GL_IMAGE_2D_RECT:
        case  GL_IMAGE_CUBE:
        case  GL_IMAGE_BUFFER:
        case  GL_IMAGE_1D_ARRAY:
        case  GL_IMAGE_2D_ARRAY:
        case  GL_IMAGE_CUBE_MAP_ARRAY:
        case  GL_IMAGE_2D_MULTISAMPLE:
        case  GL_IMAGE_2D_MULTISAMPLE_ARRAY:
        case  GL_INT_IMAGE_1D:
        case  GL_INT_IMAGE_2D:
        case  GL_INT_IMAGE_3D:
        case  GL_INT_IMAGE_2D_RECT:
        case  GL_INT_IMAGE_CUBE:
        case  GL_INT_IMAGE_BUFFER:
        case  GL_INT_IMAGE_1D_ARRAY:
        case  GL_INT_IMAGE_2D_ARRAY:
        case  GL_INT_IMAGE_CUBE_MAP_ARRAY:
        case  GL_INT_IMAGE_2D_MULTISAMPLE:
        case  GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
        case  GL_UNSIGNED_INT_IMAGE_1D:
        case  GL_UNSIGNED_INT_IMAGE_2D:
        case  GL_UNSIGNED_INT_IMAGE_3D:
        case  GL_UNSIGNED_INT_IMAGE_2D_RECT:
        case  GL_UNSIGNED_INT_IMAGE_CUBE:
        case  GL_UNSIGNED_INT_IMAGE_BUFFER:
        case  GL_UNSIGNED_INT_IMAGE_1D_ARRAY:
        case  GL_UNSIGNED_INT_IMAGE_2D_ARRAY:
        case  GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY:
        case  GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE:
        case  GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
            return true;
        #endif
        
        default: 
            return false;
    }
}


bool is_integer( const GLenum type )
{
    switch(type)
    {
        case GL_BOOL:
        case GL_BOOL_VEC2:
        case GL_BOOL_VEC3:
        case GL_BOOL_VEC4:
        case GL_INT:
        case GL_INT_VEC2:
        case GL_INT_VEC3:
        case GL_INT_VEC4:
        case GL_UNSIGNED_INT:
        case GL_UNSIGNED_INT_VEC2:
        case GL_UNSIGNED_INT_VEC3: 
        case GL_UNSIGNED_INT_VEC4:
            return true;
        
        default:
            return false;
    }
}

bool is_matrix( const GLenum type )
{
    switch(type)
    {
        case GL_FLOAT_MAT2:
        case GL_FLOAT_MAT2x3:
        case GL_FLOAT_MAT2x4:
        case GL_FLOAT_MAT3:
        case GL_FLOAT_MAT3x2:
        case GL_FLOAT_MAT3x4:
        case GL_FLOAT_MAT4:
        case GL_FLOAT_MAT4x2:
        case GL_FLOAT_MAT4x3:
            return true;
        
        default:
            return false;
    }    
}


const char *type_string( const GLenum value )
{
#define type(v, s) case v: return s;
    
    switch(value)
    {
        type(GL_FLOAT, "float")
        type(GL_FLOAT_VEC2, "vec2")
        type(GL_FLOAT_VEC3, "vec3")
        type(GL_FLOAT_VEC4, "vec4")
        
        type(GL_UNSIGNED_INT, "uint")
        type(GL_UNSIGNED_INT_VEC2, "uvec2")
        type(GL_UNSIGNED_INT_VEC3, "uvec3")
        type(GL_UNSIGNED_INT_VEC4, "uvec4")
        
        type(GL_INT, "int")
        type(GL_INT_VEC2, "ivec2")
        type(GL_INT_VEC3, "ivec3")
        type(GL_INT_VEC4, "ivec4")
        
        type(GL_BOOL, "bool")
        type(GL_BOOL_VEC2, "bvec2")
        type(GL_BOOL_VEC3, "bvec3")
        type(GL_BOOL_VEC4, "bvec4")
        
        type(GL_FLOAT_MAT2, "mat2")
        type(GL_FLOAT_MAT3, "mat3")
        type(GL_FLOAT_MAT4, "mat4")
        
        default: return NULL;
    }
    
#undef type
}

}       // namespace


int GLProgram::resources( )
{
    //
    m_feedbacks.clear();
    m_attributes.clear();
    m_uniforms.clear();
    m_samplers.clear();
    m_images.clear();
    m_storage_buffers.clear();
    m_uniform_buffers.clear();
    
    // recupere les uniform standard
    GLint uniform_count= 0;
    glGetProgramiv(name, GL_ACTIVE_UNIFORMS, &uniform_count);
    if(uniform_count > 0)
    {
        GLint max_length= 0;
        glGetProgramiv(name, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_length);
        char *uname= new char [max_length +1];
        
        int unit_id= 0;
        int image_id= 0;
        for(int i= 0; i < uniform_count; i++)
        {
            GLint size= 0;
            GLenum type= 0;
            uname[0]= 0;
            glGetActiveUniform(name, i, max_length +1, NULL, &size, &type, uname);
            GLint location= glGetUniformLocation(name, uname);
            
            if(glsl::is_sampler(type))
            {
                MESSAGE("  sampler '%s' location %d, index %d, size %d, type 0x%x\n", 
                    uname, location, unit_id, size, type );
                
                m_samplers.push_back( 
                    parameter(uname, location, unit_id, size, type, 1 << ProgramName::SAMPLER) );
                unit_id++;
            }
            else if(glsl::is_image(type))
            {
                MESSAGE("  image '%s' location %d, index %d, size %d, type 0x%x\n", 
                    uname, location, unit_id, size, type );
                
                m_images.push_back( 
                    parameter(uname, location, image_id, size, type, 1 << ProgramName::IMAGE) );
                image_id++;
            }
            else
            {
                if(location >= 0)
                {
                    const char *string= glsl::type_string(type);
                    if(string != NULL)
                        // ne pas afficher les uniforms non modifiables par glProgramUniform(); / program->uniform("xxx")= xxx;
                        MESSAGE("  uniform '%s' location %d, index %d, size %d, type '%s'\n", 
                            uname, location, i, size, string);
                    else
                        MESSAGE("  uniform '%s' location %d, index %d, size %d, type 0x%x\n", 
                            uname, location, i, size, type);
                        
                }
                
                //! \todo si location < 0, pas la peine de conserver l'uniform, sa valeur ne peut pas etre fixee par glProgramUniform, il fait parti du pipeline fixe, ou d'un buffer...
                //! mais ils sont references par les uniforms buffers...
                m_uniforms.push_back( 
                    parameter(uname, location, m_uniforms.size(), size, type, 
                        glsl::is_integer(type) ? 1 << ProgramName::INTEGER : 1 << ProgramName::UNIFORM) );
            }
        }
        
        delete [] uname;
    }
    
    // recupere les attributs
    GLint attribute_count= 0;
    glGetProgramiv(name, GL_ACTIVE_ATTRIBUTES, &attribute_count);
    if(attribute_count > 0)
    {
        GLint max_length= 0;
        glGetProgramiv(name, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_length);
        char *aname= new char [max_length +1];
        
        for(int i= 0; i < attribute_count; i++)
        {
            GLint size= 0;
            GLenum type= 0;
            aname[0]= 0;
            glGetActiveAttrib(name, i, max_length +1, NULL, &size, &type, aname);
            GLint location= glGetAttribLocation(name, aname);
            
            if(location >= 0)
            {
                const char *string= glsl::type_string(type);
                if(string != NULL)
                    MESSAGE("  attribute '%s' location %d, index %d, size %d, type '%s'\n", 
                        aname, location, i, size, string);
                else
                    MESSAGE("  attribute '%s' location %d, index %d, size %d, type 0x%x\n", 
                        aname, location, i, size, type);
            }
            
            m_attributes.push_back( 
                parameter(aname, location, i, size, type, glsl::is_integer(type)) );
        }
        
        delete [] aname;
    }

    // recupere les feedbacks
    GLint feedback_count= 0;
    glGetProgramiv(name, GL_TRANSFORM_FEEDBACK_VARYINGS, &feedback_count);
    if(feedback_count > 0)
    {
        GLint max_length= 0;
        glGetProgramiv(name, GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH, &max_length);
        char *fname= new char [max_length +1];
        
        GLint buffer_mode= 0;
        glGetProgramiv(name, GL_TRANSFORM_FEEDBACK_BUFFER_MODE, &buffer_mode);
        MESSAGE("  feedback mode '%s'\n", 
            (buffer_mode == GL_SEPARATE_ATTRIBS) ? "separate" :
            (buffer_mode == GL_INTERLEAVED_ATTRIBS) ? "interleaved" : "??");
        
        int buffer_id= 0;
        for(int i= 0; i < feedback_count; i++)
        {
            GLint size;
            GLenum type;
            fname[0]= 0;            
            glGetTransformFeedbackVarying(name, i, max_length +1, NULL, &size, &type, fname);
            
            const char *string= glsl::type_string(type);
            if(string != NULL)
                MESSAGE("  feedback '%s' index %d, size %d, type '%s', buffer %d\n", fname, i, size, string, buffer_id);
            else
                MESSAGE("  feedback '%s' index %d, size %d, type 0x%x, buffer %d\n", fname, i, size, type, buffer_id);
            
            m_feedbacks.push_back( 
                parameter(fname, -1, buffer_id, size, type, glsl::is_integer(type)) );
            
            // determiner dans quel buffer le feedback sera stocke
            if(buffer_mode == GL_SEPARATE_ATTRIBS)
                buffer_id++;
            else if(buffer_mode == GL_INTERLEAVED_ATTRIBS && type == GL_NONE 
            && strcmp(fname, "gl_NextBuffer") == 0)      // extension arb_feedback_transform3
                buffer_id++;
        }
        
        delete [] fname;
    }
    
#if 1
    // recupere les uniform buffers
    GLint block_count= 0;
    glGetProgramiv(name, GL_ACTIVE_UNIFORM_BLOCKS, &block_count);
    if(block_count > 0)
    {
        GLint max_length= 0;
        glGetProgramiv(name, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &max_length);
        char *bname= new char [max_length +1];
        
        for(int i= 0; i < block_count; i++)
        {
            glGetActiveUniformBlockName(name, i, max_length, NULL, bname);
            GLint size= 0;
            glGetActiveUniformBlockiv(name, i, GL_UNIFORM_BLOCK_DATA_SIZE, &size);
            MESSAGE("  uniform buffer '%s', index %d, length %dB\n", bname, i, size);
            
            m_uniform_buffers.push_back( 
                parameter(bname, -1, i, 0, GL_UNIFORM_BUFFER, size) );
            
            GLint count= 0;
            glGetActiveUniformBlockiv(name, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &count);
            GLint *uniforms= new GLint [count];
            glGetActiveUniformBlockiv(name, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, uniforms);
            //~ for(int k= 0; k < count; k++)
            //~ {
                //~ printf("  uniform %d\n", uniforms[k]);
            //~ }
            
            delete [] uniforms;
        }
        
        delete [] bname;
    }
    
    #ifdef GK_OPENGL4
    // recupere les shader storage buffers, opengl >= 4.2 + arb_shader_storage_buffer
    // introspection opengl >= 4.3
    GLint buffer_count= 0;
    glGetProgramInterfaceiv(name, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &buffer_count);
    if(buffer_count > 0)
    {
        GLint max_length= 0;
        glGetProgramInterfaceiv(name, GL_SHADER_STORAGE_BLOCK, GL_MAX_NAME_LENGTH, &max_length);
        char *bname= new char [max_length +1];
        for(int i= 0; i < buffer_count; i++)
        {
            glGetProgramResourceName(name, GL_SHADER_STORAGE_BLOCK, i, max_length, NULL, bname);
            MESSAGE("  buffer '%s' index %d\n", bname, i);
            
            m_storage_buffers.push_back( 
                parameter(bname, -1, i, GL_SHADER_STORAGE_BLOCK) );
    
            GLint vcount= 0;
            {
                GLenum prop[]= { GL_NUM_ACTIVE_VARIABLES };
                glGetProgramResourceiv(name, GL_SHADER_STORAGE_BLOCK, i, 1, prop, 1, NULL, &vcount);
            }
        
            // GLint variables[vcount];
			std::vector<GLint> variables(vcount);
            {
                GLenum prop[]= { GL_ACTIVE_VARIABLES };
                glGetProgramResourceiv(name, GL_SHADER_STORAGE_BLOCK, i, 1, prop, vcount, NULL, &variables.front());
            }
            
            GLchar vname[128]= { 0 };
            for(int k= 0; k < vcount; k++)
            {
                GLenum props[]= { GL_OFFSET, GL_TYPE, GL_ARRAY_STRIDE, GL_MATRIX_STRIDE, GL_IS_ROW_MAJOR, GL_TOP_LEVEL_ARRAY_STRIDE };
                GLint params[sizeof(props) / sizeof(GLenum)];
                
                glGetProgramResourceiv(name, GL_BUFFER_VARIABLE, variables[k], sizeof(props) / sizeof(GLenum), props, sizeof(params) / sizeof(GLenum), NULL, params);
                glGetProgramResourceName(name, GL_BUFFER_VARIABLE, variables[k], sizeof(vname), NULL, vname);
                
                const char *string= glsl::type_string(params[1]);
                if(string != NULL)
                    MESSAGE("    '%s' offset %d type '%s', array stride %d, top level stride %d",
                        vname, params[0], string, params[2], params[5]);
                else
                    MESSAGE("    '%s' offset %d type 0x%x, array stride %d, top level stride %d",
                        vname, params[0], params[1], params[2], params[5]);
                    
                if(glsl::is_matrix(params[1]))
                    MESSAGE(" row major %d, matrix stride %d",params[4], params[3]);
                MESSAGE("\n");
            }
        }
    
        delete [] bname;
    }

    if(shaders[COMPUTE] != 0)
    {
        GLint group_size[3];
        glGetProgramiv(name, GL_COMPUTE_WORK_GROUP_SIZE, group_size);

        GLint group_max[3];
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &group_max[0]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &group_max[1]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &group_max[2]);

        GLint threads;
        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &threads);
        
        MESSAGE("  compute shader group size %d %d %d, %d (max %d %d %d, %d)\n", 
            group_size[0], group_size[1], group_size[2], group_size[0] * group_size[1] * group_size[2], 
            group_max[0], group_max[1], group_max[2], threads);
    }
    #endif

#endif

    return 0;
}

ProgramUniform GLProgram::uniform( const char *uniform ) const
{
    if(name == 0)
        return ProgramUniform();

    for(unsigned int i= 0; i < m_uniforms.size(); i++)
        if(m_uniforms[i].name == uniform)
            return ProgramUniform( this, m_uniforms[i].location, m_uniforms[i].index, 
                m_uniforms[i].size, m_uniforms[i].type, m_uniforms[i].flags );

    WARNING("ProgramUniform('%s'): not found.\n", uniform);
    return ProgramUniform();
}

ProgramSampler GLProgram::sampler( const char *sampler ) const
{
    if(name == 0)
        return ProgramSampler();

    for(unsigned int i= 0; i < m_samplers.size(); i++)
        if(m_samplers[i].name == sampler)
            return ProgramSampler( this, m_samplers[i].location, m_samplers[i].index,
                m_samplers[i].size, m_samplers[i].type, m_samplers[i].flags );
    
    WARNING("ProgramSampler('%s'): not found.\n", sampler);
    return ProgramSampler();
}

ProgramImage GLProgram::image( const char *image ) const
{
    if(name == 0)
        return ProgramImage();

    for(unsigned int i= 0; i < m_images.size(); i++)
        if(m_images[i].name == image)
            return ProgramImage( this, m_images[i].location, m_images[i].index,
                m_images[i].size, m_images[i].type, m_images[i].flags );
    
    WARNING("ProgramImage('%s'): not found.\n", image);
    return ProgramImage();
}


ProgramAttribute GLProgram::attribute( const char *attribute ) const
{
    if(name == 0)
        return ProgramAttribute();
    
    for(unsigned int i= 0; i < m_attributes.size(); i++)
        if(m_attributes[i].name == attribute)
            return ProgramAttribute( this, m_attributes[i].location, m_attributes[i].index, 
                m_attributes[i].size, m_attributes[i].type, m_attributes[i].flags );
    
    WARNING("ProgramAttribute('%s'): not found.\n", attribute);
    return ProgramAttribute();
}

ProgramFeedback GLProgram::feedback( const char *varying ) const
{
    if(name == 0)
        return ProgramFeedback();

    for(unsigned int i= 0; i < m_feedbacks.size(); i++)
        if(m_feedbacks[i].name == varying)
            return ProgramFeedback( this, -1, m_feedbacks[i].index, 
                m_feedbacks[i].size, m_feedbacks[i].type, m_feedbacks[i].flags);
    
    WARNING("ProgramFeedback('%s'): not found.\n", varying);
    return ProgramFeedback();
}

ProgramBuffer GLProgram::uniformBuffer( const char *buffer ) const
{
    if(name == 0)
        return ProgramBuffer();
    
    for(unsigned int i= 0; i < m_uniform_buffers.size(); i++)
        if(m_uniform_buffers[i].name == buffer)
            return ProgramBuffer( this, -1, m_uniform_buffers[i].index, m_uniform_buffers[i].size );
        
    WARNING("UniformBuffer('%s'): uniform block not found.\n", buffer);
    return ProgramBuffer();
}

ProgramBuffer GLProgram::storageBuffer( const char *buffer ) const
{
    if(name == 0)
        return ProgramBuffer();
    
    for(unsigned int i= 0; i < m_storage_buffers.size(); i++)
        if(m_storage_buffers[i].name == buffer)
            return ProgramBuffer( this, -1, m_storage_buffers[i].index );
        
    WARNING("ShaderBuffer('%s'): shader storage buffer not found.\n", buffer);
    return ProgramBuffer();
}

}       // namespace

