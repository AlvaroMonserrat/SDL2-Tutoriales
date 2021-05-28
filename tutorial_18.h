#ifndef TUTORIAL_18_H_INCLUDED
#define TUTORIAL_18_H_INCLUDED

#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<cmath>
/*
    Tutorial 18: Vibración Joystick
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

    //Set modulación de colores
    void setColor(Uint8 red, Uint8 green, Uint8 blue);

    //Set blending
    void setBlendMode(SDL_BlendMode blending);

    //Set alpha modulation
    void setAlpha(Uint8 alpha);

    //Renders textura en un punto dado
    void render(int x, int y, SDL_Rect* clip = NULL,double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
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

/*Variables Globales*/
SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer =NULL;

//Cargar una imagen individual como textura
SDL_Texture* loadTexture(string path);

//Game Controller 1 handler
SDL_Joystick* gGameController = NULL;
SDL_Haptic* gControllerHaptic = NULL;

//Scene texture
LTexture gSplashTexture;


//Event Handler
SDL_Event eventHandler;

//Flags
bool quit = false;

//Angle of rotarion
double degrees = 0;

//Flip type
SDL_RendererFlip flipType = SDL_FLIP_NONE;

//Iniciar SDL y crear ventana
bool init();
//Loads media
bool loadMedia();
//Frees media and shuts down SDL
void close();

//Cargan Imagen Individual
SDL_Surface* loadSurface(std::string path);


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

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue){
    //Modular textura
    SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending){

    SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha){
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip){
    //Set rendering space and render to screen
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};

    //Set clip rendering dimensions
    if( clip != NULL){
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    //Render to screen
    SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
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

    if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC) < 0){
        cout << "Error al iniciar SDL library: " << SDL_GetError() << endl;
        success = false;
    }else{

        if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")){
            cout << "Warning: Linear texture filtering not enabled!" << endl;
        }

        //Check for joysticks
        if(SDL_NumJoysticks() < 1){
            cout << "Warning: No hay joystick conectado" << endl;
        }else{
            //Load joystick
            gGameController = SDL_JoystickOpen(0);
            if(gGameController == NULL){
                cout << "Warning: No habilitado para abrir el controlador! SDL Error: " << SDL_GetError();
            }else{

                //Obtener el dispositivo haptic
                gControllerHaptic = SDL_HapticOpenFromJoystick(gGameController);
                if(gControllerHaptic == NULL){
                    cout << "Warning: Controller no soporta Haptic" << endl;
                }else{
                    // Init rumble
                    if(SDL_HapticRumbleInit(gControllerHaptic) < 0){
                        cout << "Warning: No se ha iniciado rumble!"<< endl;
                    }
                }

            }

        }

        //Crear la ventana
        gWindow = SDL_CreateWindow("Tutorial 18", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if( gWindow == NULL){
           cout << "La ventana no pudo ser creada: " << SDL_GetError() << endl;
           success = false;
        }else{
            //Crear un render para la ventana
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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
    if(!gSplashTexture.loadFromFile("images/splash.png")){
        cout << "La imagen splash no pudo ser cargada: " << SDL_GetError() << endl;
        success=false;
    }
    return success;
}

void close(){

    gSplashTexture.free();

    //Close game controller
    SDL_HapticClose(gControllerHaptic);
    SDL_JoystickClose(gGameController);
    gGameController = NULL;
    gControllerHaptic = NULL;

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

            int xDir = 0;
            int yDir = 0;

            while(!quit)
            {
                while(SDL_PollEvent(&eventHandler) != 0)
                {
                    if(eventHandler.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                    /*
                    else if(eventHandler.type == SDL_JOYBUTTONDOWN){

                        //Play rumble en 75% de fuerza por 500 milisegundos
                        if(SDL_HapticRumblePlay(gControllerHaptic, 0.75, 500) != 0){
                            cout << "Warning: No se puede habilitar el rumble" << endl;
                        }
                    }
                    */
                }

                if(SDL_JoystickGetButton(gGameController, 0) != 0){
                        //Play rumble en 75% de fuerza por 500 milisegundos
                        if(SDL_HapticRumblePlay(gControllerHaptic, 0.75, 500) != 0){
                            cout << "Warning: No se puede habilitar el rumble" << endl;
                        }
                }


				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

                //Render splash
                gSplashTexture.render(0, 0);

                //Update
                SDL_RenderPresent(gRenderer);

            }

        }
    }

    // Libera recursos y cerrar SDL
    close();
}

#endif // TUTORIAL_18_H_INCLUDED
