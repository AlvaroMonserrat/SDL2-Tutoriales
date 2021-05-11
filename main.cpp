#include<iostream>
#include<SDL2/SDL.h>
#include "tutorial_1.h"

using namespace std;


int main(){

    if(!init()){
        cout << "Ha fallado la inicialización" << endl;
    }else{

        if(!loadMedia()){

        }else{
            //
            SDL_BlitSurface(gSurfaceHelloWorld, NULL, gScreenSurface, NULL);

            //Actualizar superficie
            SDL_UpdateWindowSurface(gWindow);

            //Esperar 5 segundos
            SDL_Delay(5000);
        }

    }


    // Libera recursos y cerrar SDL
    close();

    return 0;
}




