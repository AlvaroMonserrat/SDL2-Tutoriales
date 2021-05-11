#include<iostream>
#include<SDL2/SDL.h>

using namespace std;

/*Constantes*/
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

/*Variables Globales*/
SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gSurfaceHelloWorld = NULL;

//Iniciar SDL y crear ventana
bool init();
//Loads media
bool load();
//Frees media and shuts down SDL
void close();

int main(){

    //La ventana donde se llevará el renderizado
    SDL_Window* window = NULL;

    //La superficie contenida por la ventana
    SDL_Surface* screenSurface = NULL;

    if( SDL_Init(SDL_INIT_VIDEO) < 0){
        cout << "Error al iniciar SDL library: " << SDL_GetError() << endl;
    }else{
        //Crear la ventana
        window = SDL_CreateWindow("Tutorial 1", 0, 0, SCREEN_WIDTH, SCREEN_WIDTH, SDL_WINDOW_SHOWN);

        if( window == NULL){
           cout << "La ventana no pudo ser creada: " << SDL_GetError() << endl;
        }else{

            //Obtener la superficie de la ventana
            screenSurface = SDL_GetWindowSurface(window);

            // Llenar con un rectangulo la superficie
            SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

            //Actualizar la superficie
            SDL_UpdateWindowSurface(window);

            //Esperar
            SDL_Delay(5000);
        }
    }

    //Destroy window
    SDL_DestroyWindow( window );

    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}


bool init(){
    //Bandera de inicialización
    bool success = true;

    if( SDL_Init(SDL_INIT_VIDEO) < 0){
        cout << "Error al iniciar SDL library: " << SDL_GetError() << endl;
        success = false;
    }else{
        //Crear la ventana
        gWindow = SDL_CreateWindow("Tutorial 1", 0, 0, SCREEN_WIDTH, SCREEN_WIDTH, SDL_WINDOW_SHOWN);
        if( gWindow == NULL){
           cout << "La ventana no pudo ser creada: " << SDL_GetError() << endl;
           success = false;
        }else{
            //Obtener la superficie de la ventana
            gScreenSurface = SDL_GetWindowSurface(window);
        }
    }

    return success;
}

