#ifndef TUTORIAL_10_H_INCLUDED
#define TUTORIAL_10_H_INCLUDED

#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
/*
    Tutorial 10: Representación de clips y hojas de Sprite
*/

using namespace std;

//Clase
class LTexture{
public:
    //Iniciar variables
    LTexture();

    //Desasignar memoria
    ~LTexture();

    //Cargar imagen desde una ruta especifica
    bool loadFromFile(string path);

    //Desasignar textura en memoria
    void free();

    //Renders textura en un punto dado
    void render(int x, int y, SDL_Rect* clip = NULL);
    //Obtener dimensioens de la imagen
    int getWidth();
    int getHeight();

private:
    //La textura de hardware actual
    SDL_Texture* mTexture;

    //Dimensiones de la imagen
    int mWidth;
    int mHeight;
};


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

//Scene sprites
SDL_Rect gSprinteClips[4];
LTexture gSprinteSheetTexture;


//Scenes texture
LTexture gFooTexture;
LTexture gBackgroundTexture;

LTexture::LTexture(){
    //Inicializar
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

LTexture::~LTexture(){
    //Dellocate
    free();
}

bool LTexture::loadFromFile(string path){
    //liberar texturas preexistente
    free();

    SDL_Texture* newTexture = NULL;

    //Cargar la imagen desde una ruta especifica
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if(loadedSurface == NULL){
        cout << "Error al cargar la imagen en la ruta: " << path.c_str() << endl;
    }else{
        //Color key image
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if(newTexture == NULL){
            cout << "No se pudo crear la texture error: " << SDL_GetError();
        }else{
            //Obtener dimensiones de la imagen
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }
        //liberar puntero de superficie cargada
        SDL_FreeSurface(loadedSurface);
    }

    mTexture = newTexture;
    return mTexture != NULL;
}

void LTexture::free(){
    //Liberar textura si existe
    if(mTexture != NULL){
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::render(int x, int y, SDL_Rect* clip){
    //Set rendering space and render to screen
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};

    //Set clip rendering dimensions
    if( clip != NULL){
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    //Render to screen
    SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
}

int LTexture::getWidth(){
    return mWidth;
}

int LTexture::getHeight(){
    return mHeight;
}




bool init(){
    //Bandera de inicialización
    bool success = true;

    if( SDL_Init(SDL_INIT_VIDEO) < 0){
        cout << "Error al iniciar SDL library: " << SDL_GetError() << endl;
        success = false;
    }else{
        //Crear la ventana
        gWindow = SDL_CreateWindow("Tutorial 10", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
    if(!gSprinteSheetTexture.loadFromFile("images/dots.png")){
        cout << "La imagen dots no pudo ser cargada: " << SDL_GetError() << endl;
        success=false;
    }else{
        //Set top left sprite
        gSprinteClips[0].x = 0;
        gSprinteClips[0].y = 0;
        gSprinteClips[0].w = 100;
        gSprinteClips[0].h = 100;

        gSprinteClips[1].x = 100;
        gSprinteClips[1].y = 0;
        gSprinteClips[1].w = 100;
        gSprinteClips[1].h = 100;

        gSprinteClips[2].x = 0;
        gSprinteClips[2].y = 100;
        gSprinteClips[2].w = 100;
        gSprinteClips[2].h = 100;

        gSprinteClips[3].x = 100;
        gSprinteClips[3].y = 100;
        gSprinteClips[3].w = 100;
        gSprinteClips[3].h = 100;

    }

    return success;
}

void close(){

    gFooTexture.free();
    gBackgroundTexture.free();

    // Destruir ventana
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    //Cerrar SDL subsistemas
    IMG_Quit();
    SDL_Quit();
}

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
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

                //Render top left sprite
                gSprinteSheetTexture.render(0, 0, &gSprinteClips[0]);

                //Render top right sprite
                gSprinteSheetTexture.render(SCREEN_WIDTH - gSprinteClips[1].w, 0, &gSprinteClips[1]);

                //Render bottom left sprite
                gSprinteSheetTexture.render(0, SCREEN_HEIGHT - gSprinteClips[2].h, &gSprinteClips[2]);

                //Render bottom right sprite
                gSprinteSheetTexture.render(SCREEN_WIDTH - gSprinteClips[3].w, SCREEN_HEIGHT - gSprinteClips[3].h, &gSprinteClips[3]);

                //Update
                SDL_RenderPresent(gRenderer);
            }

        }
    }

    // Libera recursos y cerrar SDL
    close();
}

#endif // TUTORIAL_10_H_INCLUDED
