#ifndef TUTORIAL_2_H_INCLUDED
#define TUTORIAL_2_H_INCLUDED

/*
    Tutorial 2: Loop de Eventos y captura de Exit (Cerrar Ventana X)
*/

using namespace std;

/*Constantes*/
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

/*Variables Globales*/
SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gSurfaceHelloWorld = NULL;

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


bool init(){
    //Bandera de inicialización
    bool success = true;

    if( SDL_Init(SDL_INIT_VIDEO) < 0){
        cout << "Error al iniciar SDL library: " << SDL_GetError() << endl;
        success = false;
    }else{
        //Crear la ventana
        gWindow = SDL_CreateWindow("Tutorial 2", 0, 0, SCREEN_WIDTH, SCREEN_WIDTH, SDL_WINDOW_SHOWN);
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


#endif // TUTORIAL_2_H_INCLUDED


/*-----MAIN-----

int main(){

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

                    //
                    SDL_BlitSurface(gSurfaceHelloWorld, NULL, gScreenSurface, NULL);
                    //Actualizar superficie
                    SDL_UpdateWindowSurface(gWindow);
                }

            }
        }

    // Libera recursos y cerrar SDL
    close();

    return 0;
}








*/
