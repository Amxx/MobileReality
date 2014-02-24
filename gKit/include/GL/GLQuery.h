
#ifndef _GL_QUERY_H
#define _GL_QUERY_H

#include "GL/GLPlatform.h"
#include "GLResource.h"


namespace gk {

//! representation d'une requete.
class GLQuery : public GLResource
{
    // non copyable
    GLQuery( const GLQuery& );
    GLQuery& operator= ( const GLQuery& );
    
public:
    GLenum target;      //!< type de la requete.
    
    //! constructeur par defaut.
    GLQuery( ) : GLResource() {}
    //! constructeur d'un objet opengl nomme, cf. utilisation de khr_debug.
    GLQuery( const char *_label ) : GLResource(_label) {}
    
    //! creation d'une requete de type target.
    GLQuery *create( const GLenum _target )
    {
        glGenQueries(1, &name);
        if(name == 0)
            return this;
        
        target= _target;
        manage();
        return this;
    }
    
    //! destruction de la requete.
    void release( )
    {
        if(name != 0)
            glDeleteQueries(1, &name);
        name= 0;
    }
    
    //! destructeur de l'objet c++, l'objet opengl doit etre deja detruit par release( ).
    ~GLQuery( ) {}
    
    //! debut de la requete.
    void begin( )
    {
        glBeginQuery(target, name);
    }
    
    //! fin de la requete.
    void end( )
    {
        glEndQuery(target);
    }
    
    //! renvoie vrai si le resultat de la requete est immediatement disponible.
    bool available( )
    {
        GLuint status= 0;
        glGetQueryObjectuiv(name, GL_QUERY_RESULT_AVAILABLE, &status);
        return (status == GL_TRUE);
    }
    
    //! renvoie le resultat de la requete, si available() == false, attends que le resultat soit disponible, de l'ordre de quelques millisecondes...
    GLuint result( )
    {
        GLuint count;
        glGetQueryObjectuiv(name, GL_QUERY_RESULT, &count);
        return count;
    }
    
    //! renvoie le resultat de la requete, si available() == false, attends que le resultat soit disponible, de l'ordre de quelques millisecondes...
    GLuint64 result64( )
    {
        GLuint64 count;
        glGetQueryObjectui64v(name, GL_QUERY_RESULT, &count);
        return count;
    }
    
    //! renvoie un objet c++ associe a la requete 0.
    static 
    GLQuery *null( )
    {
        static GLQuery object("null query");
        return &object;
    }
};


//! fonction utilitaire : creation simplifiee d'une requete GL_TIME_ELAPSED.
inline
GLQuery *createTimeQuery( )
{
    return (new GLQuery())->create(GL_TIME_ELAPSED);
}

//! fonction utilitaire : creation simplifiee d'une requete GL_SAMPLES_PASSED.
inline
GLQuery *createOcclusionQuery( )
{
    return (new GLQuery())->create(GL_SAMPLES_PASSED);
}

//! fonction utilitaire : creation simplifiee d'une requete GL_PRIMITIVES_GENERATED.
inline
GLQuery *createPrimitiveQuery( )
{
    return (new GLQuery())->create(GL_PRIMITIVES_GENERATED);
}

//! fonction utilitaire : creation simplifiee d'une requete GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN.
inline
GLQuery *createFeedbackQuery( )
{
    return (new GLQuery())->create(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
}


}       // namespace 

#endif
