
#ifndef _GL_BUFFER_H
#define _GL_BUFFER_H

#include "GL/GLPlatform.h"
#include "GLResource.h"


namespace gk {

//! representation d'un buffer openGL.    
//! \ingroup OpenGL.
class GLBuffer : public GLResource
{
    // non copyable
    GLBuffer( const GLBuffer& );
    GLBuffer& operator= ( const GLBuffer& );

public:
    unsigned int length;        //!< taille du buffer (en octets)

    //! constructeur par defaut.
    GLBuffer( ) : GLResource() {}
    //! constructeur d'un objet opengl nomme, cf. utilisation de khr_debug.
    GLBuffer( const char *_label ) : GLResource(_label) {}

    //! creation d'un buffer, eventuellement initialise.
    GLBuffer *create( const GLenum target, const unsigned int _length, const void *data, 
        const GLenum usage= GL_STATIC_DRAW )
    {
        assert(name == 0 && "create buffer error");
        glGenBuffers(1, &name);
        if(name == 0) 
            return this;
        
        length= _length;
        manage();       // insere l'objet dans le manager
        glBindBuffer(target, name);
        glBufferData(target, length, data, usage);
        return this;
    }
    
    //! destruction du buffer.
    void release( )
    {
        if(name != 0)
            glDeleteBuffers(1, &name);
        name= 0;
    }
    
    //! destructeur de l'objet c++, l'objet opengl doit etre deja detruit par release( ).
    ~GLBuffer( ) {}
    
    //! renvoie un objet c++ associe au buffer 0.
    static
    GLBuffer *null( )
    {
        static GLBuffer object("null buffer");
        return &object;
    }
};    

//! fonction utilitaire : creation simplifiee d'un buffer initialise.
//! \ingroup OpenGL.
inline 
GLBuffer *createBuffer( const GLenum target, const unsigned int length, const void *data, 
    const GLenum usage= GL_STATIC_DRAW )
{
    return (new GLBuffer())->create(target, length, data, usage);
}

//! fonction utilitaire : creation simplifiee d'un buffer initialise.
//! \ingroup OpenGL.
template < typename T >
GLBuffer *createBuffer( const GLenum target, const std::vector<T>& data, 
    const GLenum usage= GL_STATIC_DRAW )
{
    return (new GLBuffer())->create(target, data.size() * sizeof(T), &data.front(), usage);
}

//! fonction utilitaire : creation simplifiee d'un buffer.
//! \ingroup OpenGL.
inline 
GLBuffer *createBuffer( const GLenum target, const unsigned int length, 
    const GLenum usage= GL_STATIC_DRAW )
{
    return (new GLBuffer())->create(target, length, NULL, usage);
}

}       // namespace

#endif
