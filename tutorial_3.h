#ifndef TUTORIAL_3_H_INCLUDED
#define TUTORIAL_3_H_INCLUDED

/*
    Tutorial 3: Evento - Presionar Teclas
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
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];
SDL_Surface* gCurrentSurface = NULL;

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
        gWindow = SDL_CreateWindow("Tutorial 3", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

    //Cargar imagen por defecto
    gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] = loadSurface("images/default-image.bmp");
    if( gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] == NULL)
    {
        cout << "Se ha fallado en cargar la imagen Default" << endl;
        success = false;
    }
    //Cargar imagen up
    gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] = loadSurface("images/up-arrow.bmp");
    if( gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] == NULL)
    {
        cout << "Se ha fallado en cargar la imagen up" << endl;
        success = false;
    }

    //Cargar imagen down
    gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] = loadSurface("images/down-arrow.bmp");
    if( gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] == NULL)
    {
        cout << "Se ha fallado en cargar la imagen down" << endl;
        success = false;
    }

    //Cargar imagen left
    gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] = loadSurface("images/left-arrow.bmp");
    if( gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] == NULL)
    {
        cout << "Se ha fallado en cargar la imagen left" << endl;
        success = false;
    }

    //Cargar imagen right
    gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] = loadSurface("images/right-arrow.bmp");
    if( gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] == NULL)
    {
        cout << "Se ha fallado en cargar la imagen right" << endl;
        success = false;
    }

    return success;
}

void close(){
    // Deallocate surface
    SDL_FreeSurface(gCurrentSurface);
    gCurrentSurface = NULL;

    // Destruir ventana
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    //Cerrar SDL subsistemas
    SDL_Quit();
}

SDL_Surface* loadSurface(std::string path){
    // Cargar imagen en la superficie desde un path especifica
    SDL_Surface* loadSurface = SDL_LoadBMP(path.c_str());
    if(loadSurface == NULL){
        cout << "Error al cargar la imagen en la ruta: " << path.c_str() << endl;
    }

    return loadSurface;
}


#endif // TUTORIAL_3_H_INCLUDED
