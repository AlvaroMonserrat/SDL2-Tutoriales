#include<iostream>
#include<SDL2/SDL.h>
#include "tutorial_3.h"

using namespace std;


int main(){

        if(!init()){
            cout << "Ha fallado la inicialización" << endl;
        }else{
            if(!loadMedia()){
                cout << "Ha fallado cargar la imagen" << endl;
            }else{

                gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];

                while(!quit)
                {
                    while(SDL_PollEvent(&eventHandler) != 0)
                    {
                        if(eventHandler.type == SDL_QUIT)
                        {
                            quit = true;
                        }else if(eventHandler.type == SDL_KEYDOWN){
                            switch(eventHandler.key.keysym.sym)
                            {
                                case SDLK_UP:
                                    gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_UP];
                                    break;
                                case SDLK_DOWN:
                                    gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN];
                                    break;
                                case SDLK_LEFT:
                                    gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT];
                                    break;
                                case SDLK_RIGHT:
                                    gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT];
                                    break;
                            }

                        }
                    }

                    //
                    SDL_BlitSurface(gCurrentSurface, NULL, gScreenSurface, NULL);
                    //Actualizar superficie
                    SDL_UpdateWindowSurface(gWindow);
                }

            }
        }

    // Libera recursos y cerrar SDL
    close();

    return 0;
}




