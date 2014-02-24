
#ifndef _IMAGE_H
#define _IMAGE_H

#include <cassert>
#include <cstring>

#include "Vec.h"
#include "IOResource.h"


namespace gk {

//! representation d'une image.
class Image : public IOResource
{
    Image( const Image & );
    Image& operator=( const Image& );

protected:
    //! modifie la valeur d'un pixel.
    void setPixel( const unsigned int offset, const VecColor& color )
    {
        switch(type)
        {
            case UNSIGNED_BYTE: 
            {
                unsigned char *p= (unsigned char *) (data + offset);
                p[0]= color.r * 255.f;
                if(channels > 1)
                    p[1]= color.g * 255.f;
                if(channels > 2)
                    p[2]= color.b * 255.f;
                if(channels > 3)
                    p[3]= color.a * 255.f;
            }
            break;
            
            case FLOAT:
            {
                float *p= (float *) (data + offset);
                p[0]= color.r;
                if(channels > 1)
                    p[1]= color.g;
                if(channels > 2)
                    p[2]= color.b;
                if(channels > 3)
                    p[3]= color.a;
            }
            break;
            
            default:
                assert(0 && "invalid image type");
        }
    }
    
    //! renvoie la valeur d'un pixel.
    VecColor pixel( const unsigned int offset )
    {
        VecColor color(0, 0, 0, 1);
        switch(type)
        {
            case UNSIGNED_BYTE:
            {
                unsigned char *p= (unsigned char *) (data + offset);
                color.r= (float) p[0] / 255.f;
                if(channels > 1)
                    color.g= (float) p[1] / 255.f;
                if(channels > 2)
                    color.b= (float) p[2] / 255.f;
                if(channels > 3)
                    color.a= (float) p[3] / 255.f;
            }
            break;
            
            case FLOAT:
            {
                float *p= (float *) (data + offset);
                color.r= (float) p[0];
                if(channels > 1)
                    color.g= (float) p[1];
                if(channels > 2)
                    color.b= (float) p[2];
                if(channels > 3)
                    color.a= (float) p[3];
            }
            break;
            
            default:
                assert(0 && "invalid image type");
        }
        
        return color;
    }
    
public:
    int width;
    int height;
    int depth;
    int channels;
    unsigned int type;
    int pixel_sizeof;
    unsigned char *data;
    bool reference_data;

    enum {
        UNSIGNED_BYTE= 0,
        FLOAT
    };
    
    //! constructeur par defaut.
    Image( ) 
        : 
        IOResource(), 
        width(0), height(0), depth(0), channels(0), type(0), pixel_sizeof(0), data(NULL), reference_data(false)
    {}
    
    //! constructeur d'une image nommee / associee a un fichier.
    Image( const std::string& filename ) 
        : 
        IOResource(filename), 
        width(0), height(0), depth(0), channels(0), type(0), pixel_sizeof(0), data(NULL), reference_data(false)
    {}
    
    //! destructeur.
    ~Image( )
    {
        if(reference_data == false)
            delete [] data;
    }
    
    //! construction d'une image de dimension wxh. 
    Image *create( const int _w, const int _h, const int _channels, const unsigned int _type, void *_data= NULL )
    {
        width= _w;
        height= _h;
        depth= 1;
        channels= _channels;
        type= _type;
        released= false;
        
        switch(type)
        {
            case UNSIGNED_BYTE: pixel_sizeof= sizeof(unsigned char); break;
            case FLOAT: pixel_sizeof= sizeof(float); break;
            default:
                assert(0 && "invalid image type");
        }
        
        int length= width * height * pixel_sizeof * channels;
        data= new unsigned char[length];
        if(_data != NULL)
            memcpy(data, _data, length);
        
        return this;
    }
    
    //! construction d'une image de dimension wxhxd. 
    Image *create( const int _w, const int _h, const int _d, const int _channels, const unsigned int _type, void *_data= NULL )
    {
        width= _w;
        height= _h;
        depth= _d;
        channels= _channels;
        type= _type;
        released= false;
        
        switch(type)
        {
            case UNSIGNED_BYTE: pixel_sizeof= sizeof(unsigned char); break;
            case FLOAT: pixel_sizeof= sizeof(float); break;
            default:
                assert(0 && "invalid image type");
        }
        
        int length= width * height * depth * pixel_sizeof * channels;
        data= new unsigned char[length];
        if(_data != NULL)
            memcpy(data, _data, length);
        
        return this;
    }
    
    Image *reference( Image *image, const unsigned int offset= 0u )
    {
        if(image == NULL)
            return this;
        
        width= image->width;
        height= image->height;
        depth= image->depth;
        channels= image->channels;
        type= image->type;
        pixel_sizeof= image->pixel_sizeof;
        data= image->data + offset;
        reference_data= true;
        released= false;
        
        return this;
    }
    
    //! destruction des donnees de l'image / de la ressource.
    void release( )
    {
        if(reference_data == false)
            delete [] data;
        data= NULL;
        released= true;
    }
    
    //! adressage lineaire d'un pixel (x,y).
    unsigned int offset( const unsigned int x, const unsigned int y ) const
    {
        assert(x < (unsigned int) width);
        assert(y < (unsigned int) height);
        // z= 0;
        return (y * width + x) * channels * pixel_sizeof;
    }
    
    //! adressage lineaire d'un pixel (x,y, z).
    unsigned int offset( const unsigned int x, const unsigned int y, const unsigned int z ) const
    {
        assert(x < (unsigned int) width);
        assert(y < (unsigned int) height);
        assert(z < (unsigned int) depth);
        return (z * width * height + y * width + x) * channels * pixel_sizeof;
    }
    
    //! modifie la valeur d'un pixel.
    void setPixel( const unsigned int x, const unsigned int y, const VecColor& color )
    {
        setPixel(offset(x,y), color);
    }
    
    //! modifie la valeur d'un pixel.
    void setPixel( const unsigned int x, const unsigned int y, const unsigned int z, const VecColor& color )
    {
        setPixel(offset(x, y, z), color);
    }
    
    //! renvoie la valeur d'un pixel.
    VecColor pixel( const unsigned int x, const unsigned int y )
    {
        return pixel(offset(x, y));
    }
    
    //! renvoie la valeur d'un pixel.
    VecColor pixel( const unsigned int x, const unsigned int y, const unsigned int z )
    {
        return pixel(offset(x, y, z));
    }    
};


//! fonction utilitaire : simplifie la creation d'une image "standard".
inline
Image *createImage( const int w, const int h, const int channels= 4, const unsigned int type= Image::FLOAT )
{
    return (new Image())->create(w, h, channels, type);
}

}       // namespace

#endif
