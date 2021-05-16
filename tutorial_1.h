#ifndef TUTORIAL_1_H_INCLUDED
#define TUTORIAL_1_H_INCLUDED

/*
    Tutorial 1: Mostar una imagen .bmp en la Ventana.
*/

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
bool loadMedia();
//Frees media and shuts down SDL
void close();


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
            gScreenSurface = SDL_GetWindowSurface(gWindow);
        }
    }

    return success;
}

bool loadMedia(){
    //Bandera de inicialización
    bool success = true;

    gSurfaceHelloWorld = SDL_LoadBMP("sample.bmp");

    if(gSurfaceHelloWorld == NULL){
        cout << "Error al intentar cargar la Imagen: " << SDL_GetError()<< endl;
        success = false;
    }

    return success;
}

void close(){
    // Deallocate surface
    SDL_FreeSurface(gSurfaceHelloWorld);
    gSurfaceHelloWorld = NULL;

    // Destruir ventana
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    //Cerrar SDL subsistemas
    SDL_Quit();
}


#endif // TUTORIAL_1_H_INCLUDED


/*MAIN

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

*/
