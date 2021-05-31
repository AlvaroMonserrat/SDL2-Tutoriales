#ifndef TUTORIAL_21_H_INCLUDED
#define TUTORIAL_21_H_INCLUDED

#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include<cmath>
#include <sstream>

/*
    Tutorial 21: Temporizador avanzado
*/

using namespace std;

//Clase timer
class LTimer
{
public:
    //constructor
    LTimer();

    //clock actions
    void start();
    void stop();
    void pause();
    void unpause();

    //gets the timer's time
    Uint32 getTicks();

    //Checks status timer
    bool isStarted();
    bool isPaused();

private:
    //the clock time when the timer started
    Uint32 mStartTicks;

    //The ticks stored when the timer was paused
    Uint32 mPausedTicks;

    //The timer status
    bool mPaused;
    bool mStarted;
};

LTimer::LTimer()
{
    //Iniciar variables
    mStartTicks = 0;
    mPausedTicks = 0;

    mPaused = false;
    mStarted = false;
}

void LTimer::start()
{
    //Start the timer
    mStarted = true;
    mPaused = false;

    //Obtener actual clock timer
    mStartTicks = SDL_GetTicks();
    mPausedTicks = 0;
}

void LTimer::stop()
{
    //Stop the timer
    mStarted = false;

    //Unpause the timer
    mPaused = false;

    //Clear ticks variables
    mStartTicks = 0;
    mPausedTicks = 0;
}

void LTimer::pause(){


    if(mStarted && !mPaused)
    {
        mPaused = true;


        mPausedTicks = SDL_GetTicks() - mStartTicks;
        mStartTicks = 0;
    }

}

void LTimer::unpause()
{
    if(mStarted && mPaused)
    {
        mPaused = false;

        mStartTicks = SDL_GetTicks() - mPausedTicks;

        mPausedTicks = 0;

    }
}

Uint32 LTimer::getTicks()
{
    //The actual timer time
    Uint32 time = 0;

    //If the timer is running
    if(mStarted)
    {
        //If the timer is paused
        if(mPaused)
        {
            //Return the number of ticks when the timer was paused
            time = mPausedTicks;
        }
        else
        {
            //return the current time minus the start time
            time = SDL_GetTicks() - mStartTicks;
        }
    }

    return time;
}

bool LTimer::isStarted()
{
    return mStarted;
}

bool LTimer::isPaused()
{
    return mPaused && mStarted;
}

//Clase
class LTexture{
public:
    //Iniciar variables
    LTexture();

    //Desasignar memoria
    ~LTexture();

    //Cargar imagen desde una ruta especifica
    bool loadFromFile(string path);

    //Crear imagen desde un font string
    bool loadFromRenderedText(string textureText, SDL_Color textColor);

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

//Globally used font
TTF_Font *gFont = NULL;

//Scene texture
LTexture gTimeTextTexture;
LTexture gPausePromptTexture;
LTexture gStartPromptTexture;

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

bool LTexture::loadFromRenderedText(string textureText, SDL_Color textColor){

    //Liberar textura preexistente
    free();

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
    if(textSurface == NULL){
            cout << "Error, no se puedo crear la superficie: " << TTF_GetError() << endl;
    }else{
        //Crear textura desde superficie de pixeles
        mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
        if(mTexture == NULL){
            cout << "Error, no se puedo crear la textura: " << SDL_GetError() << endl;
        }else{
            //Obtener dimensiones de la imagen
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }
        //
        SDL_FreeSurface(textSurface);
    }

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

    if( SDL_Init(SDL_INIT_VIDEO) < 0){
        cout << "Error al iniciar SDL library: " << SDL_GetError() << endl;
        success = false;
    }else{
        //Crear la ventana
        //Set texture filtering to linear
        if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        {
            cout << "Warning: Filtro Linear texture no disponible" << endl;
        }

        gWindow = SDL_CreateWindow("Tutorial 21", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

                //Iniciar SDL ttf
                if(TTF_Init() == -1){
                    cout << "SDL_ttf no se pudo iniciar " << TTF_GetError() << endl;
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

    //Open the font
    gFont = TTF_OpenFont("fonts/lazy.ttf", 28);

    if(gFont == NULL){
            cout << "Error al abrir la Font: " << TTF_GetError() << endl;
        success = false;
    }else{

        //Render Text
        SDL_Color textColor = {0, 0, 0, 255};

        if(!gStartPromptTexture.loadFromRenderedText("Presione S para comenzar o detener el timer", textColor)){
            cout << "Falló la renderización de la textura con texto" << endl;
            success = false;
        }

        if(!gPausePromptTexture.loadFromRenderedText("Presiona P para pausar o despausar el timer", textColor))
        {
            cout << "Falló la renderización de la textura con texto" << endl;
            success = false;
        }
    }


    return success;
}

void close(){

    gTimeTextTexture.free();
    gStartPromptTexture.free();
    gPausePromptTexture.free();

    //Free global font
    TTF_CloseFont(gFont);
    gFont = NULL;

    // Destruir ventana
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    //Cerrar SDL subsistemas
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void gameRun(){
    if(!init()){
        cout << "Ha fallado la inicialización" << endl;
    }else{
        if(!loadMedia()){
            cout << "Ha fallado cargar la imagen" << endl;
        }else{

            SDL_Color textColor = {0, 0, 0, 255};

            //Actual tiempo
            Uint32 startTime = 0;

            //Timer
            LTimer timer;

            //In memory text stream
            stringstream timeText;

            while(!quit)
            {
                while(SDL_PollEvent(&eventHandler) != 0)
                {
                    if(eventHandler.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                    // Reset start time al presionar tecla return
                    else if(eventHandler.type == SDL_KEYDOWN)
                    {
                        //Start/Stop
                        if(eventHandler.key.keysym.sym == SDLK_s)
                        {
                            if(timer.isStarted())
                            {
                                timer.stop();
                            }
                            else
                            {
                                timer.start();
                            }
                        }
                        //Pause/unpause
                        else if(eventHandler.key.keysym.sym == SDLK_p)
                        {
                            if(timer.isPaused())
                            {
                                timer.unpause();
                            }
                            else{
                                timer.pause();
                            }
                        }
                    }
                }

                //Set text to be rendered
                timeText.str("");
                timeText << "Seconds since start time " << (timer.getTicks() / 1000.f);

                if(!gTimeTextTexture.loadFromRenderedText(timeText.str().c_str(), textColor))
                {
                    cout << "No es posibble render time texture" <<  endl;

                }


				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

				//Render frame actual
                gStartPromptTexture.render((SCREEN_WIDTH - gStartPromptTexture.getWidth()) / 2, 0);
                gPausePromptTexture.render((SCREEN_WIDTH - gPausePromptTexture.getWidth()) / 2, gStartPromptTexture.getHeight());
                gTimeTextTexture.render((SCREEN_WIDTH - gTimeTextTexture.getWidth()) / 2, (SCREEN_HEIGHT - gTimeTextTexture.getHeight()) / 2);

                //Update
                SDL_RenderPresent(gRenderer);

            }

        }
    }

    // Libera recursos y cerrar SDL
    close();
}


#endif // TUTORIAL_21_H_INCLUDED
