#ifndef TUTORIAL_30_H_INCLUDED
#define TUTORIAL_30_H_INCLUDED

#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include <sstream>
#include <vector>
#include <SDL2/SDL_ttf.h>

/*
    Tutorial 30: Entrada de Texto y manipulación de clipBoard
*/

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

//Estructura
struct Circle
{
    int x, y;
    int r;
};

//Clase Dot
class Dot
{
public:
    //dimensiones del punto
    static const int DOT_WIDTH = 20;
    static const int DOT_HEIGHT = 20;

    //Maximum axis velocity
    static const int DOT_VEL = 5
    ;

    //Inicializar
    Dot();

    //Capturar key evento
    void handleEvent(SDL_Event& e);

    //Mover el punto
    void move2();

    //Mostrar el punto en la pantalla
    void render();

    //Gets collision circle
    Circle& getCollider();

    //Position accessors
    int getPosX();
    int getPoxY();

private:
    // x e y offsets
    int mPosX, mPosY;

    //velocidad
    int mVelX, mVelY;

    //Dot's collision circle
    Circle mCollider;

    //Moves the collision boxes relative to the dot's offset
    void shiftColliders();
};

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


//Dimeniones del level
const int LEVEL_WIDTH = 1920;
const int LEVEL_HEIGHT = 1080;

/*Constantes*/
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;


/*Variables Globales*/
SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer =NULL;

//Globally used fond
TTF_Font *gFont = NULL;

//Cargar una imagen individual como textura
SDL_Texture* loadTexture(std::string path);

//Scene texture
LTexture gPromptTextTexture;
LTexture gInputTextTexture;

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

//Box collision detector
bool checkCollision(std::vector<SDL_Rect>& a, std::vector<SDL_Rect>& b);

//Circle/Circle collision detector
bool checkCollision(Circle& a, Circle& b);

//Circle/Box collision detector
bool checkCollision(Circle& a, SDL_Rect& b);

//Calcular distancia entre dos puntos
double distanceSquare(int x1, int y1, int x2, int y2);

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
            std::cout << "Error, no se puedo crear la superficie: " << TTF_GetError() << std::endl;
    }else{
        //Crear textura desde superficie de pixeles
        mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
        if(mTexture == NULL){
            std::cout << "Error, no se puedo crear la textura: " << SDL_GetError() << std::endl;
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

    //Iniciar colliders relative to position
    //shiftColliders();

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

void Dot::move2()
{
    //Move the dot left or right
    mPosX += mVelX;

    std::cout << mPosX << std::endl;

    if((mPosX < 0) || (mPosX + DOT_WIDTH > LEVEL_WIDTH))
    {
        //move back
        mPosX -= mVelX;

    }
    //Move the dot up or down
    mPosY += mVelY;

    if((mPosY < 0) || (mPosY + DOT_HEIGHT > LEVEL_HEIGHT))
    {
        //move back
        mPosY -= mVelY;
    }
}


void Dot::render()
{
    //show the dot relative to the camera
    //gDotTexture.render(mPosX, mPosY);
}


Circle& Dot::getCollider()
{
    return mCollider;
}


void Dot::shiftColliders()
{
    mCollider.x = mPosX;
    mCollider.y = mPosY;
}

int Dot::getPosX()
{
    return mPosX;
}

int Dot::getPoxY()
{
    return mPosY;
}

bool init(){
    //Bandera de inicialización
    bool success = true;

    if( SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "Error al iniciar SDL library: " << SDL_GetError() << std::endl;
        success = false;
    }else{
        //Crear la ventana
        gWindow = SDL_CreateWindow("Tutorial 30", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

                //Iniciar SDL ttf
                if(TTF_Init() == -1){
                    std::cout << "SDL_ttf no se pudo iniciar " << TTF_GetError() << std::endl;
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

    if(gFont == NULL)
    {
        std::cout << "Error al cargar la Font" << TTF_GetError()<< std::endl;
        success = false;
    }
    else
    {
        //Render the prompt
        SDL_Color textColor = {0, 0, 0, 0xFF};
        if(!gPromptTextTexture.loadFromRenderedText("Enter Text:", textColor))
        {
            std::cout << "Error al renderizar prompt" << std::endl;
            success = false;
        }
    }

    return success;
}

void close(){

    gPromptTextTexture.free();
    gInputTextTexture.free();

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

bool checkCollision(std::vector<SDL_Rect> &a, std::vector<SDL_Rect> &b)
{
    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Go through the A boxes
    for(int Abox = 0; Abox < a.size(); Abox++)
    {
        //  Calculate the sides of rect A
        leftA = a[Abox].x;
        rightA = a[Abox].x + a[Abox].w;

        topA = a[Abox].y;
        bottomA = a[Abox].y + a[Abox].h;


        //Go through the B boxes
        for(int Bbox = 0; Bbox < b.size(); Bbox++)
        {
            // Caculate the sides of rect B
            leftB = b[Bbox].x;
            rightB = b[Bbox].x + b[Bbox].w;

            topB = b[Bbox].y;
            bottomB = b[Bbox].y + b[Bbox].h;

            if( ((bottomA <= topB) || (topA >= bottomB) || (rightA <= leftB) || (leftA >= rightB)) == false)
            {
                //A collision is detected
                return true;
            }
        }
    }

    return false;

}

bool checkCollision(Circle & a, Circle& b)
{
    int totalRadiusSquared = a.r + b.r;
    totalRadiusSquared = totalRadiusSquared * totalRadiusSquared;

    if( distanceSquare(a.x, a.y, b.x, b.y) < (totalRadiusSquared))
    {
        return true;
    }

    return false;
}

bool checkCollision(Circle &a, SDL_Rect &b)
{
    int cX, cY;

    //Find closest x offset
    if(a.x < b.x)
    {
        cX = b.x;
    }
    else if(a.x > b.x + b.w)
    {
        cX = b.x + b.w;
    }
    else{
        cX = a.x;
    }

    //Find closest y offset
    if(a.y < b.y)
    {
        cY = b.y;
    }
    else if(a.y > b.y + b.h)
    {
        cY = b.y + b.h;
    }
    else{
        cY = a.y;
    }

    //If the closest point is inside the circle
    if(distanceSquare(a.x, a.y, cX, cY) < a.r * a.r)
    {
        //Collision Detected
        return true;
    }

    //If the shapes have not collided
    return false;
}

double distanceSquare(int x1, int y1, int x2, int y2)
{
    int deltaX = x2 - x1;
    int deltaY = y2 -y1;
    return deltaX*deltaX + deltaY*deltaY;
}


void gameRun(){
    if(!init()){
        std::cout << "Ha fallado la inicialización" << std::endl;
    }else{
        if(!loadMedia()){
         }else{

            //Set color del texto negro
            SDL_Color textColor = {0, 0, 0, 0xFF};

            //Actual texto de entrada
            std::string inputText = "Some Text";
            gInputTextTexture.loadFromRenderedText(inputText.c_str(), textColor);

            //Habilitar entrada de texto
            SDL_StartTextInput();


            while(!quit)
            {
                //start cap timer
                bool renderText = false;

                while(SDL_PollEvent(&eventHandler) != 0)
                {
                    if(eventHandler.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                    else if(eventHandler.type == SDL_KEYDOWN)
                    {
                        if(eventHandler.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0)
                        {
                            //lop off character
                            inputText.pop_back();
                            renderText = true;
                        }
                        //Handle copy
                        else if(eventHandler.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
                        {
                            SDL_SetClipboardText(inputText.c_str());
                        }
                        //Handle paste
                        else if(eventHandler.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
                        {
                            inputText = SDL_GetClipboardText();
                            renderText = true;
                        }
                    }
                    //Special text input event
                    else if(eventHandler.type == SDL_TEXTINPUT)
                    {
                        //Not copy or paste
                        if(!(SDL_GetModState() & KMOD_CTRL &&
                          (eventHandler.text.text[0] == 'c' || eventHandler.text.text[0] == 'C'
                           || eventHandler.text.text[0] == 'v' || eventHandler.text.text[0] == 'V')))
                        {
                            inputText += eventHandler.text.text;
                            renderText = true;
                        }
                    }
                }

                //Render text if needed
                if(renderText)
                {
                    //Text is no empty
                    if(inputText != "")
                    {
                        //Render new text
                        gInputTextTexture.loadFromRenderedText(inputText.c_str(), textColor);
                    }
                    //Text is empty
                    else
                    {
                        //Render space texture
                        gInputTextTexture.loadFromRenderedText(" ", textColor);
                    }
                }

                //Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

                //Render text texture
                gPromptTextTexture.render((SCREEN_WIDTH - gPromptTextTexture.getWidth()) / 2, 0);
                gInputTextTexture.render( (SCREEN_WIDTH - gInputTextTexture.getWidth()) / 2, gPromptTextTexture.getHeight());

                //Update
                SDL_RenderPresent(gRenderer);

            }

            //Disable text input
            SDL_StopTextInput();

        }
    }

    // Libera recursos y cerrar SDL
    close();
}



#endif // TUTORIAL_30_H_INCLUDED
