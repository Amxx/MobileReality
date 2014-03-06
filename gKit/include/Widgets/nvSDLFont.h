
#ifndef NV_SDL2_FONT_H
#define NV_SDL2_FONT_H

#include "SDLttfPlatform.h"
#include "Widgets/nvWidgets.h"


namespace nv {

class SdlFont : public UIFont
{
    SDL_Surface *m_sdl_texture;
    int m_font_height;
    
    int init_cache( TTF_Font *font );
    
public:
    SdlFont( const char *filename= "verdana.ttf", const int size= 14 );
    //~ SdlFont( const char *filename= "ubuntu.ttf", const int size= 16 );
    //~ SdlFont( const char *filename= "liberation.ttf", const int size= 16 );
    ~SdlFont( );
    
    int init( );
};

}       // namespace
#endif