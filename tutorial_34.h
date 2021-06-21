#ifndef TUTORIAL_34_H_INCLUDED
#define TUTORIAL_34_H_INCLUDED


#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include <sstream>
#include <string>
#include <vector>
#include <SDL2/SDL_ttf.h>

/*
    Tutorial 34: Multiples Ventanas
*/

/*Constantes*/
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const int TOTAL_WINDOWS = 3;


SDL_Renderer* gRenderer =NULL;

//Clase
class LWindow
{
    public:
        //Iniciación internas
        LWindow();

        //Crear Ventana
        bool init();

        //Crear renderer de ventana interna
        SDL_Renderer* createRenderer();

        //Focuses in window
        void focus();

        //Shows windows contents
        void render();

        //Handles window events
        void handleEvent(SDL_Event& e);

        //Free memory
        void free();

        //Window dimensions
        int getWidth();
        int getHeight();

        //Window focus
        bool hasMouseFocus();
        bool hasKeyboardFocus();
        bool isMinimized();
        bool isShown();

    private:
        //Window data
        SDL_Window* mWindow;
        SDL_Renderer* mRenderer;
        int mWindowID;

        //Window dimensions
        int mWidth;
        int mHeight;

        //Window Focus
        bool mMouseFocus;
        bool mKeyboardFocus;
        bool mFullScreen;
        bool mMinimized;
        bool mShown;
};

LWindow::LWindow()
{
    //Initialize non-existant window
    mWindow = NULL;
    mMouseFocus = false;
    mKeyboardFocus = false;
    mFullScreen = false;
    mMinimized = false;
    mWidth = 0;
    mHeight = 0;
}

bool LWindow::init()
{
    mWindow = SDL_CreateWindow(
        "Tutorial 34", //Titulo
        SDL_WINDOWPOS_UNDEFINED, // pos x
        SDL_WINDOWPOS_UNDEFINED, // pos y
        SCREEN_WIDTH,   // Ancho
        SCREEN_HEIGHT,  // Alto
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE  //Flags
    );

    if(mWindow != NULL)
    {
        mMouseFocus = true;
        mKeyboardFocus = true;
        mWidth = SCREEN_WIDTH;
        mHeight = SCREEN_HEIGHT;

        //Crear el renderer para la ventana
        mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if(mRenderer == NULL)
        {
            std::cout << "Error al crear renderer para la ventana" << std::endl;
            mWindow = NULL;
        }
        else{

            //Inizializar
            SDL_SetRenderDrawColor(mRenderer, 0x00, 0xFF, 0xFF, 0xFF);

            //Grab window identifier
            mWindowID = SDL_GetWindowID(mWindow);

            //Flag as openned
            mShown = true;
        }
    }
    else{
        std::cout << "Error al crear la ventana" << std::endl;
    }

    return mWindow != NULL && mRenderer != NULL;

}

SDL_Renderer* LWindow::createRenderer()
{
    return SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void LWindow::handleEvent(SDL_Event& event)
{
    //Window event occured
    if(event.type == SDL_WINDOWEVENT && event.window.windowID == mWindowID)
    {
        //Caption update flag
        bool updateCaption = false;

        switch(event.window.event)
        {
            //Window appeared
            case SDL_WINDOWEVENT_SHOWN:
                mShown = true;
            break;

            //Window disappeared
            case SDL_WINDOWEVENT_HIDDEN:
                mShown = false;
            break;

            //Get new dimensions and repaint on window size change
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                mWidth = event.window.data1;
                mHeight = event.window.data2;
                SDL_RenderPresent(mRenderer);
            break;

            //Repaint on exposure
            case SDL_WINDOWEVENT_EXPOSED:
                SDL_RenderPresent(mRenderer);
            break;

            //Mouse entered window
            case SDL_WINDOWEVENT_ENTER:
                mMouseFocus = true;
                updateCaption = true;
            break;

            //Mouse left window
            case SDL_WINDOWEVENT_LEAVE:
                mMouseFocus = false;
                updateCaption = true;
            break;

            //Window has keyboard focus
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                mKeyboardFocus = true;
                updateCaption = true;
            break;

            //Window lost keyboard focus
            case SDL_WINDOWEVENT_FOCUS_LOST:
                mKeyboardFocus = false;
                updateCaption = true;
            break;

            //Window minimized
            case SDL_WINDOWEVENT_MINIMIZED:
                mMinimized = true;
            break;

            //Window maxized
            case SDL_WINDOWEVENT_MAXIMIZED:
                mMinimized = false;
            break;

            //Window restored
            case SDL_WINDOWEVENT_RESTORED:
                mMinimized = false;
            break;

            //Hide on close
            case SDL_WINDOWEVENT_CLOSE:
                SDL_HideWindow(mWindow);
            break;
        }

        //Update window caption with new data
        if(updateCaption)
        {
            std::stringstream caption;
            caption << "SDL Tutorial - ID: "<< mWindowID <<" MouseFocus:" << ((mMouseFocus) ? "On" : "Off") << " KeyboardFocus:" << ((mKeyboardFocus) ? "On" : "Off");
            SDL_SetWindowTitle(mWindow, caption.str().c_str());
        }

    }
}

void LWindow::focus()
{
    //Restore window if needed
    if(!mShown)
    {
        SDL_ShowWindow(mWindow);
    }

    //Move window forward
    SDL_RaiseWindow(mWindow);
}

void LWindow::render()
{
    if(!mMinimized)
    {
        //Clear screen
        SDL_SetRenderDrawColor(mRenderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderClear(mRenderer);

        //Update screen
        SDL_RenderPresent(mRenderer);

    }
}

void LWindow::free()
{
	if( mWindow != NULL )
	{
		SDL_DestroyWindow( mWindow );
	}

	mMouseFocus = false;
	mKeyboardFocus = false;
	mWidth = 0;
	mHeight = 0;
}

int LWindow::getWidth()
{
    return mWidth;
}

int LWindow::getHeight()
{
    return mHeight;
}

bool LWindow::hasMouseFocus()
{
    return mMouseFocus;
}

bool LWindow::hasKeyboardFocus()
{
    return mKeyboardFocus;
}

bool LWindow::isMinimized()
{
    return mMinimized;
}

bool LWindow::isShown()
{
	return mShown;
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

/*Variables Globales*/
LWindow gWindows[TOTAL_WINDOWS];

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
        std::cout << "Error al iniciar SDL library: " << SDL_GetError() << std::endl;
        success = false;
    }else{
        //Set texture filtering to linear
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
        {
            std::cout <<  "Warning: Linear texture filtering not enabled!" << std::endl;
        }

        //Crear window
        if(!gWindows[0].init())
        {
            std::cout << "Window 0 no puedo ser creada!" << std::endl;
            success == false;
        }

    }

    return success;
}

bool loadMedia(){
    //Bandera de inicialización
    bool success = true;

    //Load scene Texture
//    if(!gSceneTexture.loadFromFile("images/window.png"))
//    {
//        std::cout << "Ha fallado cargar la ventana" << std::endl;
//        success = false;
//    }

    return success;
}

void close(){

    //Free loaded images
    for(int i=0; i < TOTAL_WINDOWS; ++i)
    {
        gWindows[i].free();
    }

    //Cerrar SDL subsistemas
    SDL_Quit();
}


void gameRun(){

    if(!init()){
        std::cout << "Ha fallado la inicialización" << std::endl;
    }else{

        for(int i = 1; i < TOTAL_WINDOWS; ++i)
        {
            gWindows[i].init();
        }

        while(!quit)
        {
            while(SDL_PollEvent(&eventHandler) != 0)
            {
                if(eventHandler.type == SDL_QUIT)
                {
                    quit = true;
                }

                //Handle window events
                for(int i = 0; i < TOTAL_WINDOWS; ++i)
                {
                    gWindows[i].handleEvent(eventHandler);
                }

                if(eventHandler.type == SDL_KEYDOWN)
                {
                    switch(eventHandler.key.keysym.sym)
                    {
                        case SDLK_1:
                            gWindows[0].focus();
                        break;

                        case SDLK_2:
                            gWindows[2].focus();
                        break;

                        case SDLK_3:
                            gWindows[3].focus();
                        break;

                    }
                }

            }

            //Update all windows
            for(int i = 0; i < TOTAL_WINDOWS; ++i)
            {
                gWindows[i].render();
            }


            //check all windows
            bool allWindowsClosed = true;
            for(int i = 0; i < TOTAL_WINDOWS; ++i)
            {
                if(gWindows[i].isShown())
                {
                    allWindowsClosed = false;
                    break;
                }

            }

            if(allWindowsClosed)
            {
                quit = true;
            }

        }


    }

    // Libera recursos y cerrar SDL
    close();
}

#endif // TUTORIAL_34_H_INCLUDED
