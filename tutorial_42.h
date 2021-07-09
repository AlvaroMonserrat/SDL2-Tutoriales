#ifndef TUTORIAL_42_H_INCLUDED
#define TUTORIAL_42_H_INCLUDED

#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include <sstream>
#include <vector>

/*
    Tutorial 42: Frame Independent Movement
*/

/*Constantes*/
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

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
    bool loadFromFile(std::string path);

    #if defined(SDL_TTF_MAJOR_VERSION)
    //Crear imagen desde un font string
    bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
    #endif
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


//Clase Dot
class Dot
{
public:
    //dimensiones del punto
    static const int DOT_WIDTH = 20;
    static const int DOT_HEIGHT = 20;

    //Maximum axis velocity
    static const int DOT_VEL = 640;

    //Inicializar
    Dot();

    //Capturar key evento
    void handleEvent(SDL_Event& e);

    //Mover el punto
    void move2(float timeStep);

    //Mostrar el punto en la pantalla
    void render();

private:
    // x e y offsets
    float mPosX, mPosY;

    //velocidad
    float mVelX, mVelY;

};



/*Variables Globales*/
SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer =NULL;

//Cargar una imagen individual como textura
SDL_Texture* loadTexture(std::string path);

//Scene texture
LTexture gDotTexture;

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

bool LTexture::loadFromFile(std::string path){
    //liberar texturas preexistente
    free();

    SDL_Texture* newTexture = NULL;

    //Cargar la imagen desde una ruta especifica
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if(loadedSurface == NULL){
        std::cout << "Error al cargar la imagen en la ruta: " << path.c_str() << std::endl;
    }else{
        //Color key image
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if(newTexture == NULL){
            std::cout << "No se pudo crear la texture error: " << SDL_GetError();
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

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor){

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
#endif

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



Dot::Dot()
{
    //Iniciar posición
    mPosX = 0;
    mPosY = 0;

    //Iniciar velocidad
    mVelX = 0;
    mVelY = 0;
}


void Dot::handleEvent(SDL_Event &e)
{

    //Si una tecla fue presionada
    if(e.type == SDL_KEYDOWN && e.key.repeat == 0)
    {
        switch(e.key.keysym.sym)
        {
            case SDLK_UP: mVelY -= DOT_VEL; break;
            case SDLK_DOWN: mVelY += DOT_VEL; break;
            case SDLK_LEFT: mVelX -= DOT_VEL; break;
            case SDLK_RIGHT: mVelX += DOT_VEL; break;
        }
    }else if(e.type == SDL_KEYUP && e.key.repeat == 0){
        switch(e.key.keysym.sym)
        {
            case SDLK_UP: mVelY += DOT_VEL; break;
            case SDLK_DOWN: mVelY -= DOT_VEL; break;
            case SDLK_LEFT: mVelX += DOT_VEL; break;
            case SDLK_RIGHT: mVelX -= DOT_VEL; break;
        }

    }

}

void Dot::move2(float timeStep)
{
    //Move the dot left or right
    mPosX += mVelX * timeStep;

    //If the dot went too far to the left or right
    if(mPosX < 0)
    {
        mPosX = 0;
    }
    else if(mPosX > SCREEN_WIDTH - DOT_WIDTH)
    {
        mPosX = SCREEN_WIDTH - DOT_WIDTH;
    }

    //Move the dot up or down
    mPosY += mVelY * timeStep;

    //If the dot went too far up or down
    if( mPosY < 0 )
    {
        mPosY = 0;
    }
    else if(mPosY > SCREEN_HEIGHT - DOT_HEIGHT)
    {
        mPosY = SCREEN_HEIGHT - DOT_HEIGHT;
    }
}


void Dot::render()
{
    //show dot
    gDotTexture.render((int)mPosX, (int)mPosY);
}


bool init(){
    //Bandera de inicialización
    bool success = true;

    if( SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "Error al iniciar SDL library: " << SDL_GetError() << std::endl;
        success = false;
    }else{
        //Crear la ventana
        gWindow = SDL_CreateWindow("Tutorial 42", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if( gWindow == NULL){
           std::cout << "La ventana no pudo ser creada: " << SDL_GetError() << std::endl;
           success = false;
        }else{

            //Crear un render para la ventana
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(gRenderer == NULL){
                std::cout << "Renderer no pudo ser creado: " << SDL_GetError() << std::endl;
                success = false;
            }else{

                //Iniciar renderer Color
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                //Iniciar PNG Loading
                int imgFlags = IMG_INIT_PNG;
                if(!( IMG_Init(imgFlags) & imgFlags)){
                    std::cout << "SDL_image no se pudo iniciar " << SDL_GetError() << std::endl;
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
    if(!gDotTexture.loadFromFile("images/dot.bmp"))
    {
        std::cout << "Error al cargar la imagen dot" << std::endl;
        success = false;
    }


    return success;
}

void close(){

    gDotTexture.free();

    // Destruir ventana
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    //Cerrar SDL subsistemas
    IMG_Quit();
    SDL_Quit();
}


void gameRun(){
    if(!init()){
        std::cout << "Ha fallado la inicialización" << std::endl;
    }else{
        if(!loadMedia()){
         }else{

            //The dot that will be moving around on the screen
            Dot dot;

            //Keeps track of time between steps
            LTimer stepTimer;

            while(!quit)
            {
                //start cap timer

                while(SDL_PollEvent(&eventHandler) != 0)
                {
                    if(eventHandler.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                    dot.handleEvent(eventHandler);
                }

                //Calculate time step
                float timeStep = stepTimer.getTicks() / 1000.f;

                std::cout << timeStep << std::endl;
				//Clear screen
				dot.move2(timeStep);

                //Restart step Timer
                stepTimer.start();

                //Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

				dot.render();
                //Update
                SDL_RenderPresent(gRenderer);

            }

        }
    }

    // Libera recursos y cerrar SDL
    close();
}

#endif // TUTORIAL_42_H_INCLUDED
