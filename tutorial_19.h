#ifndef TUTORIAL_19_H_INCLUDED
#define TUTORIAL_19_H_INCLUDED

/*
    Tutorial 19: Musica y effectos de sonido
*/

#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_mixer.h>
#include<cmath>

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

//Music
Mix_Music *gMusic = NULL;

//Efectos de Sonido
Mix_Chunk *gScratch = NULL;
Mix_Chunk *gHigh = NULL;
Mix_Chunk *gMedium = NULL;
Mix_Chunk *gLow = NULL;

//Scene texture
LTexture gPromptTexture;


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

    if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0){
        cout << "Error al iniciar SDL library: " << SDL_GetError() << endl;
        success = false;
    }else{

        if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")){
            cout << "Warning: Linear texture filtering not enabled!" << endl;
        }

        //Crear la ventana
        gWindow = SDL_CreateWindow("Tutorial 19", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

                //Iniciar SDL_mixer
                if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
                    cout << "SDL_mixer no pudo iniciar" << Mix_GetError << endl;
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
    if(!gPromptTexture.loadFromFile("images/prompt.png")){
        cout << "Error al cargar la imagen prompt.png" << endl;
        success = false;
    }

    //Cargar musica
    gMusic = Mix_LoadMUS("audios/beat2.mp3");
    if(gMusic == NULL){
        cout << "Error al cargar el audio beat2.mp3" << endl;
        success = false;
    }

    gScratch = Mix_LoadWAV("audios/scratch.wav");
    if(gScratch == NULL){
        cout << "Error al cargar el audio scratch.wav" << endl;
        success = false;
    }

    gHigh = Mix_LoadWAV("audios/high.wav");
    if(gHigh == NULL){
        cout << "Error al cargar el audio high.wav" << endl;
        success = false;
    }

    gMedium = Mix_LoadWAV("audios/medium.wav");
    if(gMedium == NULL){
        cout << "Error al cargar el audio medium.wav" << endl;
        success = false;
    }

    gLow = Mix_LoadWAV("audios/low.wav");
    if(gLow == NULL){
        cout << "Error al cargar el audio low.wav" << endl;
        success = false;
    }


    return success;
}

void close(){

    gPromptTexture.free();

    Mix_FreeChunk(gScratch);
    Mix_FreeChunk(gHigh);
    Mix_FreeChunk(gMedium);
    Mix_FreeChunk(gLow);
    gScratch = NULL;
    gHigh = NULL;
    gMedium = NULL;
    gLow = NULL;

    //Free music
    Mix_FreeMusic(gMusic);
    gMusic = NULL;

    // Destruir ventana
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    //Cerrar SDL subsistemas
    Mix_Quit();
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
                    //Handle key press
                    else if(eventHandler.type == SDL_KEYDOWN)
                    {
                        switch(eventHandler.key.keysym.sym)
                        {
                            //Play high sound effect
                            case SDLK_1:
                                Mix_PlayChannel(-1, gHigh, 0);
                                break;
                            case SDLK_2:
                                Mix_PlayChannel(-1, gMedium, 0);
                                break;
                            case SDLK_3:
                                Mix_PlayChannel(-1, gLow, 0);
                                break;
                            case SDLK_4:
                                Mix_PlayChannel(-1, gScratch, 0);
                                break;
                            case SDLK_9:
                                //Si no hay musica reproduciendose
                                if(Mix_PlayingMusic() == 0)
                                {
                                    //Play music
                                    Mix_PlayMusic(gMusic, -1);
                                }else
                                {
                                    //Musica en pausa
                                    if(Mix_PausedMusic() == 1)
                                    {
                                        //Reproducir
                                        Mix_ResumeMusic();
                                    }
                                    //Si musica está reproduciendose
                                    else
                                    {
                                        Mix_PausedMusic();
                                    }
                                }

                                break;
                            case SDLK_0:
                                //Stop Music
                                Mix_HaltMusic();
                                break;

                         }
                    }

                }

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

                //Render prompt
                gPromptTexture.render(0, 0);

                //Update
                SDL_RenderPresent(gRenderer);

            }

        }
    }

    // Libera recursos y cerrar SDL
    close();
}

#endif // TUTORIAL_19_H_INCLUDED
