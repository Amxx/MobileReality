
#ifndef _APP_DEBUGGER_H
#define _APP_DEBUGGER_H

#include "App.h"
#include "Widgets/nvSDLContext.h"
#include "Debugger/GLDebugger.h"


namespace gk {

//! client    
class AppDebugger : public GLDebugger, public App
{
protected:
    nv::SdlContext m_debug_widgets;
    unsigned char *m_debug_key_state;
    bool m_debug_stop;
    

public:
    AppDebugger( ) 
        : 
        GLDebugger(), App(),
        m_debug_key_state(NULL),
        m_debug_stop(false)
    {}

    AppDebugger( const int w, const int h, const AppSettings& flags= AppSettings() )
        :
        GLDebugger(), App(w, h, flags),
        m_debug_key_state(NULL),
        m_debug_stop(false)
    {}
    
    ~AppDebugger( )
    {
        delete m_debug_key_state;
    }
    
    int createWindow( const int w, const int h, const AppSettings& flags= AppSettings() )
    {
        int status= App::createWindow(w, h, flags);
        if(status < 0)
            return status;
        
        m_debug_widgets.init();
        
        // copie l'etat du clavier
        int keys;
        const unsigned char *state= SDL_GetKeyboardState(&keys);
        m_debug_key_state= new unsigned char[keys];
        for(int i= 0; i < keys; i++)
            m_debug_key_state[i]= state[i];
        
        return status;
    }
    
    void processEvents( )
    {
        if(m_break_on_frame == false)
        {
            App::processEvents();
            return;
        }
        
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_WINDOWEVENT:
                    if(event.window.event == SDL_WINDOWEVENT_RESIZED)
                    {
                        m_width= event.window.data1;
                        m_height= event.window.data2;
                        SDL_SetWindowSize(m_window, m_width, m_height);
                        
                        // prevenir l'application
                        processWindowResize(event.window);
                    }
                    break;
                
                case SDL_QUIT:
                    m_stop= 1;
                    break;
                    
                case SDL_KEYUP:
                    m_debug_key_state[event.key.keysym.scancode]= 0;
                    m_debug_widgets.processKeyboardEvent(event.key);
                    
                    processKeyboardEvent(event.key);
                    break;
                    
                case SDL_KEYDOWN:
                    m_debug_key_state[event.key.keysym.scancode]= 1;
                    m_debug_widgets.processKeyboardEvent(event.key);
                    
                    processKeyboardEvent(event.key);
                    break;
                
                case SDL_MOUSEMOTION:
                    m_debug_widgets.processMouseMotionEvent(event.motion);
                    
                    processMouseMotionEvent(event.motion);
                    break;
                
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEBUTTONDOWN:
                    m_debug_widgets.processMouseButtonEvent(event.button);
                    
                    processMouseButtonEvent(event.button);
                    break;
            }
        }
    }
    
    int run( )
    {
        if(isClosed())
        {
            printf("App::run( ): no window.\n");
            return -1;
        }
        
        // termine l'initialisation des classes derivees, chargement de donnees, etc.
        if(init() < 0)
        {
        #ifdef VERBOSE
            printf("App::init( ): failed.\n");
        #endif
            return -1;
        }

        Uint64 start= SDL_GetTicks();
        Uint64 last_frame= start;
        while(!isClosed())
        {
            // traitement des evenements : clavier, souris, fenetre, etc.
            processEvents();
            
            // mise a jour de la scene
            Uint64 ticks= SDL_GetTicks();
            Uint64 frame= ticks - start;
            Uint64 delta= ticks - last_frame;
            
            if(update(frame, delta) == 0)
                break;
            
            // affiche la scene
            if(draw() == 0)
                break;
            
            last_frame= frame;
        }
        
        // destruction des ressources chargees par les classes derivees.
        if(quit() < 0)
            return -1;

        return 0;        
    }
    
    void present( )
    {
        if(m_break_on_frame == false)
        {
            App::present();
            return;
        }
        
        static bool debug_unfold= 0;
        static bool profile_unfold= 0;
        static nv::Rect debug_panel;
        static nv::Rect profile_panel;
        
        //~ while(m_break_on_frame == true)
        {
            processEvents();
        
            glViewport(0, 0, windowWidth(), windowHeight());
            //~ glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            m_debug_widgets.reshape(windowWidth(), windowHeight());                

            // draw gui
            m_debug_widgets.begin();
            m_debug_widgets.beginGroup(nv::GroupFlags_GrowDownFromLeft);
            if(m_debug_widgets.beginPanel(debug_panel, "debug", &debug_unfold))
            {
                static bool shader= 0, buffer= 0;
                
                m_debug_widgets.doButton(nv::Rect(), "inspect shader", &shader);
                m_debug_widgets.doButton(nv::Rect(), "inspect buffer", &buffer);
                m_debug_widgets.endPanel();
            }
            
            if(m_debug_widgets.beginPanel(profile_panel, "profile", &profile_unfold))
            {
                static bool shader= 0, buffer= 0, texture= 0, draw= 0;

                m_debug_widgets.doButton(nv::Rect(), "change shader", &shader);
                m_debug_widgets.doButton(nv::Rect(), "change buffer", &buffer);
                m_debug_widgets.doButton(nv::Rect(), "change texture", &texture);
                m_debug_widgets.doButton(nv::Rect(), "draw degenerate triangles", &draw);
                m_debug_widgets.endPanel();
            }
            
            m_debug_widgets.endGroup();
            m_debug_widgets.end();

            SDL_GL_SwapWindow(m_window);
        }
    }
};
    
}       // namespace


#endif
