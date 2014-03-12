
#ifndef _BASIC_MESH_H
#define _BASIC_MESH_H

#include "Vec.h"
#include "GLPlatform.h"
#include "GL/GLBuffer.h"
#include "GL/GLVertexArray.h"


namespace gk {

namespace gl {
    template< typename T > inline GLenum type( ) { assert(0 && "gl::type<T>( ): not defined"); return 0; }
    
    template< > inline GLenum type<char>( ) {  return GL_BYTE; }
    template< > inline GLenum type<unsigned char>( ) {  return GL_UNSIGNED_BYTE; }
    
    template< > inline GLenum type<short>( ) {  return GL_SHORT; }
    template< > inline GLenum type<unsigned short>( ) {  return GL_UNSIGNED_SHORT; }
    
    template< > inline GLenum type<int>( ) {  return GL_INT; }
    template< > inline GLenum type<unsigned int>( ) {  return GL_UNSIGNED_INT; }
    
    template< > inline GLenum type<float>( ) {  return GL_FLOAT; }
}
    
class GLBasicMesh
{
    GLBasicMesh( const GLBasicMesh& );
    GLBasicMesh& operator= ( const GLBasicMesh& );
    
public:
    GLenum primitive;                   //!< type de primitive, GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.
    int count;                          //!< nombre de vertex / d'indices a dessiner.
    
    GLVertexArray *vao;                 //!< vertex array object.
    std::vector<GLBuffer *> buffers;    //!< attributs : position, texcoord, normal.
    GLBuffer *index_buffer;             //!< index buffer.
    GLenum index_type;                  //!< type des indices.
    unsigned int index_sizeof;                  //!< sizeof(type des indices).
    
    GLBasicMesh( const GLenum _primitive, const int _count ) 
        : 
        primitive(_primitive), count(_count), vao(createVertexArray()), buffers(), index_buffer(GLBuffer::null()), index_type(0), index_sizeof(0)
    {}

    virtual ~GLBasicMesh( ) {}
    
    // creation d'un buffer d'attribut de sommet.
    GLBasicMesh& createBuffer( const int index, const int item_size, const GLenum item_type, 
        const unsigned int length, const void *data= NULL, const GLenum usage= GL_STATIC_DRAW );
    
    template < typename T > GLBasicMesh& createBuffer( const int index, 
        const std::vector< TVec2<T> >& data, const GLenum usage= GL_STATIC_DRAW )
    {
        return createBuffer(index, 2, gl::type<T>(), data.size() * sizeof(TVec2<T>), &data.front(), usage);
    }
    
    template < typename T > GLBasicMesh& createBuffer( const int index, 
        const std::vector< TVec3<T> >& data, const GLenum usage= GL_STATIC_DRAW )
    {
        return createBuffer(index, 3, gl::type<T>(), data.size() * sizeof(TVec3<T>), &data.front(), usage);
    }
    
    template < typename T > GLBasicMesh& createBuffer( const int index, 
        const std::vector< TVec4<T> >& data, const GLenum usage= GL_STATIC_DRAW )
    {
        return createBuffer(index, 4, gl::type<T>(), data.size() * sizeof(TVec4<T>), &data.front(), usage);
    }
    
    
    GLBasicMesh& createIndexBuffer( const GLenum item_type, 
        const unsigned int length, const void *data, const GLenum usage= GL_STATIC_DRAW );
    
    template < typename T > GLBasicMesh& createIndexBuffer( 
        const std::vector<T>& data, const GLenum usage= GL_STATIC_DRAW )
    {
        return createIndexBuffer(gl::type<T>(), data.size() * sizeof(T), &data.front(), usage);
    }
    
    GLBasicMesh& bindBuffer( const int index, const int item_size, const GLenum item_type, GLBuffer *buffer )
    {
        if(index < 0 || buffer == NULL || buffer->length == 0)
            return *this;
        
        if(index >= (int) buffers.size())
            buffers.resize(index +1, GLBuffer::null());
        buffers[index]= buffer;

    #ifndef NDEBUG
        {
            GLint current;
            glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &current);
            if((GLuint) current != vao->name)
                ERROR("invalid vertex array %d, basic mesh %d\n", current, vao->name);
        }
    #endif
        
        glVertexAttribPointer(index, item_size, item_type, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(index);
        return *this;
    }
    
    int draw( );
    int draw( const int base ); // base vertex
    int drawGroup( const unsigned int begin, const unsigned int end );  // base index
    int drawInstanced( const int n );   // instance
};

}       // namespace

#endif
