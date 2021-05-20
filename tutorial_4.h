#ifndef TUTORIAL_4_H_INCLUDED
#define TUTORIAL_4_H_INCLUDED

/*
    Tutorial 4: Carga de la superficie optimizada y strech imagen
    Hacer más rápido el blit
    (blit: Una operación lógica en la que un bloque de datos se mueve o se copia
    rápidamente en la memoria, que se usa más comúnmente para animar gráficos bidimensionales.)
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
        gWindow = SDL_CreateWindow("Tutorial 4", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

    //Imagen final optimizada
    SDL_Surface* optimizedSurface = NULL;

    // Cargar imagen en la superficie desde un path especifica
    SDL_Surface* loadSurface = SDL_LoadBMP(path.c_str());
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

void gameRun(){
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
                //Aplicar ajuste de imagen
                SDL_Rect stretchRect;
                stretchRect.x = 0;
                stretchRect.y = 0;
                stretchRect.w = SCREEN_WIDTH;
                stretchRect.h = SCREEN_HEIGHT;

                SDL_BlitScaled(gCurrentSurface, NULL, gScreenSurface, NULL);
                //SDL_BlitSurface(gCurrentSurface, NULL, gScreenSurface, NULL);
                //Actualizar superficie
                SDL_UpdateWindowSurface(gWindow);
            }

        }
    }

    // Libera recursos y cerrar SDL
    close();
}

#endif // TUTORIAL_4_H_INCLUDED
