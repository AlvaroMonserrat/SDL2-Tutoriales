#ifndef TUTORIAL_6_H_INCLUDED
#define TUTORIAL_6_H_INCLUDED

#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
/*
    Tutorial 6: Cargando textura y renderizando
*/

using namespace std;

/*Constantes*/
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

//Key press surfaces constants
enum KeyPressSurfaces
{
    KEY_PRESS_SURFACE_DEFAULT,
    KEY_PRESS_SURFACE_UP,
    KEY_PRESS_SURFACE_DOWN,
    KEY_PRESS_SURFACE_LEFT,
    KEY_PRESS_SURFACE_RIGHT,
    KEY_PRESS_SURFACE_TOTAL
};

/*Variables Globales*/
SDL_Window* gWindow = NULL;
//SDL_Surface* gScreenSurface = NULL;
//SDL_Surface* gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];
//SDL_Surface* gCurrentSurface = NULL;

//Cargar una imagen individual como textura
SDL_Texture* loadTexture(string path);
SDL_Renderer* gRenderer = NULL;
SDL_Texture* gTexture = NULL;

//Event Handler
SDL_Event eventHandler;

//Flags
bool quit = false;


//Iniciar SDL y crear ventana
bool init();
//Loads media
bool loadMedia();
//Frees media and shuts down SDL
void close();

//Cargan Imagen Individual
SDL_Surface* loadSurface(std::string path);


bool init(){
    //Bandera de inicialización
    bool success = true;

    if( SDL_Init(SDL_INIT_VIDEO) < 0){
        cout << "Error al iniciar SDL library: " << SDL_GetError() << endl;
        success = false;
    }else{
        //Crear la ventana
        gWindow = SDL_CreateWindow("Tutorial 6", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if( gWindow == NULL){
           cout << "La ventana no pudo ser creada: " << SDL_GetError() << endl;
           success = false;
        }else{

            //Crear un render para la ventana
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if(gRenderer == NULL){
                cout << "Renderer no pudo ser creado: " << SDL_GetError() << endl;
                success = false;
            }else{

                //Iniciar renderer Color
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                //Iniciar PNG Loading
                int imgFlags = IMG_INIT_PNG;
                if(!( IMG_Init(imgFlags) & imgFlags)){
                    cout << "SDL_image no se pudo iniciar " << SDL_GetError() << endl;
                    success = false;
                }
            }



        }
    }

    return success;
}

bool loadMedia(){
    //Bandera de inicialización
    bool success = true;

    //Cargar PNG texture
    gTexture = loadTexture("images/up-arrow.png");
    if(gTexture == NULL){
        cout << "Error al cargar la imagen texture: " << SDL_GetError() << endl;
        success = false;
    }else{

    }
    return success;
}

void close(){
    // Deallocate surface
    SDL_DestroyTexture(gTexture);
    gTexture = NULL;

    // Destruir ventana
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    //Cerrar SDL subsistemas
    IMG_Quit();
    SDL_Quit();
}

/*
SDL_Surface* loadSurface(std::string path){

    //Imagen final optimizada
    SDL_Surface* optimizedSurface = NULL;

    // Cargar imagen en la superficie desde un path especifica
    SDL_Surface* loadSurface = IMG_Load(path.c_str());
    if(loadSurface == NULL){
        cout << "Error al cargar la imagen en la ruta: " << path.c_str() << endl;
    }else{
        //Convertir superficio al formato de la pantalla (Se crea una copia de loadSurface)
        optimizedSurface = SDL_ConvertSurface(loadSurface, gScreenSurface->format, 0);
        if(optimizedSurface == NULL){
            cout << "Error al optimizar la imagen" << endl;
        }
        //Eliminar superficie de carga
        SDL_FreeSurface(loadSurface);

    }

    return optimizedSurface;
}
*/

SDL_Texture* loadTexture(string path){

    //Final Texture
    SDL_Texture* newTexture = NULL;

    //Cargar la imagen desde una ruta especifica
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if(loadedSurface == NULL){
        cout << "Error al cargar la imagen en la ruta: " << path.c_str() << endl;
    }else{
        //Crear texture desde superficie de pixeles
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if(newTexture == NULL){
            cout << "Error al crear textura: " << SDL_GetError() << endl;
        }
        //Liberar superficie
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}

void gameRun(){
    if(!init()){
        cout << "Ha fallado la inicialización" << endl;
    }else{
        if(!loadMedia()){
            cout << "Ha fallado cargar la imagen" << endl;
        }else{

            while(!quit)
            {
                while(SDL_PollEvent(&eventHandler) != 0)
                {
                    if(eventHandler.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                }

                //Clear screen
                SDL_RenderClear(gRenderer);

                //Render texture a la screen
                SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);

                //Update
                SDL_RenderPresent(gRenderer);
            }

        }
    }

    // Libera recursos y cerrar SDL
    close();
}


#endif // TUTORIAL_6_H_INCLUDED
