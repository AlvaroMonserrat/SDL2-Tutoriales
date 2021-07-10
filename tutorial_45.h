#ifndef TUTORIAL_45_H_INCLUDED
#define TUTORIAL_45_H_INCLUDED

#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_thread.h>
#include <sstream>
#include <vector>
#include <fstream>

/*
    Tutorial 45: Semaphores
*/

/*Constantes*/
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

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

    //Creates blank texture
    bool createBlank(int width, int height, SDL_TextureAccess = SDL_TEXTUREACCESS_STREAMING);

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

    //Set self as render target
    void setAsRenderTarget();

    //Obtener dimensioens de la imagen
    int getWidth();
    int getHeight();

    //Manipulación de pixeles
    bool lockTexture();
    bool unlockTexture();
    void* getPixels();
    void copyPixels(void* pixels);
    int getPitch();
    Uint32 getPixel32(unsigned int x, unsigned int y);

private:
    //La textura de hardware actual
    SDL_Texture* mTexture;
    void* mPixels;
    int mPitch;

    //Dimensiones de la imagen
    int mWidth;
    int mHeight;
};

//A test animation stream
class DataStream
{
    public:
        //Initializes internals
        DataStream();

        //Loads initial data
        bool loadMedia();

        //Deallocator
        void free();

        //Gets current frame data
        void* getBuffer();
    private:
        //Internal data
        SDL_Surface* mImages[4];
        int mCurrentImage;
        int mDelayFrames;
};

//Our bitmap font
class LBitmapFont
{
    public:
        //The default constructor
        LBitmapFont();

        //Generates the font
        bool buildFont(LTexture *bitmap);

        //Shows the text
        void renderText(int x, int y, std::string text);

    private:
        //The font texture
        LTexture* mBitmap;

        //The individual characters in the surface
        SDL_Rect mChars[256];

        //Spacing Variables
        int mNewLine, mSpace;
};

/*Variables Globales*/
SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer =NULL;

//Scene textures
LTexture gCallBackTexture;

//Cargar una imagen individual como textura
SDL_Texture* loadTexture(std::string path);

//Event Handler
SDL_Event eventHandler;

//Data access semaphore
SDL_sem* gDataLock = NULL;

//The "data buffer"
int gData = -1;

//Flags
bool quit = false;

//Our test callback function
//Uint32 callback( Uint32 interval, void* param );

//Our test thread function
//int threadFunction(void *data);

//Our worker thread function
int worker( void* data );

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

        //Convertir la superficie a un formato display
        SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0);
        if( formattedSurface == NULL)
        {
             std::cout << "No es posible convertir la superficie en formato displat SDL Error: " << SDL_GetError() << std::endl;
        }
        else
        {
            //Crear Textura fluida en blanco
            newTexture = SDL_CreateTexture(
                gRenderer,
                SDL_PIXELFORMAT_RGBA8888,
                SDL_TEXTUREACCESS_STREAMING,
                formattedSurface->w,
                formattedSurface->h
                );

            if(newTexture == NULL){
                std::cout << "No se pudo crear la texture error: " << SDL_GetError() << std::endl;
            }else{

                //Enable blending on texture
                SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND);

                //Lock texture para manipulación
                SDL_LockTexture(newTexture, NULL, &mPixels, &mPitch);

                //Copiar superficie de pixeles cargada/formateada
                memcpy(mPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h);

                //Obtener dimensiones de la imagen
                mWidth = formattedSurface->w;
                mHeight = formattedSurface->h;

                //Get pixel data in editable format
                Uint32* pixels = (Uint32*)mPixels;
                int pixelCount = ( mPitch / 4 ) * mHeight;

                //Map colors
                Uint32 colorKey = SDL_MapRGB( formattedSurface->format, 0, 0xFF, 0xFF );
                Uint32 transparent = SDL_MapRGBA( formattedSurface->format, 0x00, 0xFF, 0xFF, 0x00 );

                //Color key pixels
                for( int i = 0; i < pixelCount; ++i )
                {
                    if( pixels[ i ] == colorKey )
                    {
                        pixels[ i ] = transparent;
                    }
                }

                //Unlock texture to update
                SDL_UnlockTexture(newTexture);
                mPixels = NULL;


            }
            //liberar puntero de superficie formateada
            SDL_FreeSurface(formattedSurface);
        }
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

bool LTexture::createBlank(int width, int height, SDL_TextureAccess access)
{
    //Create uninitialized texture
    mTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, access, width, height);
    if(mTexture == NULL)
    {
        std::cout << "Error al crear la textura blanca" << std::endl;
    }
    else
    {
        mWidth = width;
        mHeight = height;
    }

    return mTexture != NULL;
}

void LTexture::setAsRenderTarget()
{
    //Make self render target
    SDL_SetRenderTarget(gRenderer, mTexture);
}

void LTexture::free(){
    //Liberar textura si existe
    if(mTexture != NULL){
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
        mPixels = NULL;
        mPitch = 0;
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

bool LTexture::lockTexture()
{
    bool success = true;

    //Texture is already locked
    if(mPixels != NULL)
    {
        std::cout << "Texture is already locked!" << std::endl;
        success = false;
    }
    //Lock texture
    else
    {
        if(SDL_LockTexture(mTexture, NULL, &mPixels, &mPitch) != 0)
        {
            std::cout << "Unable to lock texture!" << SDL_GetError() << std::endl;
            success = false;
        }
    }

    return success;

}

bool LTexture::unlockTexture()
{
    bool success = true;

    if(mPixels == NULL)
    {
        std::cout << "Texture is not locked!" << std::endl;
        success = false;
    }
    //Unlock texture
    else
    {
        SDL_UnlockTexture(mTexture);
        mPixels = NULL;
        mPitch = 0;
    }

    return success;
}

void* LTexture::getPixels()
{
    return mPixels;
}

void LTexture::copyPixels(void* pixels)
{
    //Texture is locked
    if(mPixels != NULL)
    {
        //Copy to locked pixels
        memcpy(mPixels, pixels, mPitch * mHeight);
    }
}

int LTexture::getPitch()
{
    return mPitch;
}

Uint32 LTexture::getPixel32(unsigned int x, unsigned int y)
{
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32*)mPixels;

    //Get the pixel request
    return pixels[(y * (mPitch / 4)) + x];
}

LBitmapFont::LBitmapFont()
{
    //Initialize variables
    mBitmap = NULL;
    mNewLine = 0;
    mSpace = 0;
}

bool LBitmapFont::buildFont(LTexture* bitmap)
{
    bool success = true;

    //Lock pixels for access
    if(!bitmap->lockTexture())
    {
        std::cout << "Unable to lock bitmap font texture!" << std::endl;
        success = false;
    }
    else
    {
        //Set the background color
        Uint32 bgColor = bitmap->getPixel32(0, 0);

        //Set the cell dimensions
        int cellW = bitmap->getWidth() / 16;
        int cellH = bitmap->getHeight() /16;

        //New line variables
        int top = cellH;
        int baseA = cellH;

        //The current character we're setting
        int currentChar = 0;

        //Go through the cell rows
        for(int rows=0; rows < 16; ++rows)
        {
            //Go through the cell columns
            for(int cols = 0; cols < 16; ++cols)
            {
                //Set the character offset
                mChars[currentChar].x = cellW * cols;
                mChars[currentChar].y = cellH * rows;

                //Set the dimensions of the character
                mChars[currentChar].w = cellW;
                mChars[currentChar].h = cellH;

                //Find Left Side
                //Go through pixel columns
                for(int pCol = 0; pCol < cellW; ++pCol)
                {
                    //Go through pixel rows
                    for(int pRow = 0; pRow < cellH; ++pRow)
                    {
                        //Get the pixel offsets
                        int pX = (cellW * cols) + pCol;
                        int pY = (cellH * rows) + pRow;

                        //If a non colorkey pixel is found
                        if(bitmap->getPixel32(pX, pY) != bgColor)
                        {
                            //Set the x offset
                            mChars[currentChar].x = pX;

                            //Break the loops
                            pCol = cellW;
                            pRow = cellH;
                        }

                    }

                }

                //Find Right Side
                //Go through pixel columns
                for(int pColW = cellW -1; pColW >= 0; --pColW)
                {
                    //Go through pixel rows
                    for(int pRowW = 0; pRowW < cellH; ++pRowW)
                    {
                        //Get the pixel offsets
                        int pX = (cellW * cols) + pColW;
                        int pY = (cellH * rows) + pRowW;

                        //If a non colorkey pixel is found
                        if(bitmap->getPixel32(pX, pY) != bgColor)
                        {
                            //Set the width
                            mChars[currentChar].w = (pX - mChars[currentChar].x) + 1;

                            //Break the loops
                            pColW = -1;
                            pRowW = cellH;
                        }

                    }

                }

                //Find Top
                //Go through pixel rows
                for(int pRow = 0; pRow < cellH; ++pRow)
                {
                    //Go through pixel columns
                    for(int pCol = 0; pCol < cellW; ++pCol)
                    {
                        //Get the pixel offset
                        int pX = (cellW * cols) + pCol;
                        int pY = (cellH * rows) + pRow;

                        //If a non colorkey pixel is found
                        if(bitmap->getPixel32(pX, pY) != bgColor)
                        {
                            if(pRow < top)
                            {
                                top = pRow;
                            }

                            //Break the looś
                            pCol = cellW;
                            pRow = cellH;
                        }
                    }

                }

                //Find Bottom of A
                if(currentChar == 'A')
                {
                    for(int pRow = cellH - 1; pRow >= 0; --pRow)
                    {
                        //Go through pixel columns
                        for(int pCol = 0; pCol < cellW; ++pCol)
                        {

                            //Get the pixel offset
                            int pX = (cellW * cols) + pCol;
                            int pY = (cellH * rows) + pRow;

                            if(bitmap->getPixel32(pX, pY) != bgColor)
                            {
                                //Bottom of a is found
                                baseA = pRow;

                                //Break the loops
                                pCol = cellW;
                                pRow = -1;
                            }
                        }
                    }
                }

                //Go to the next character
                ++currentChar;
            }

        }

        //Calculate space
        mSpace = cellW / 2;

        //Calculate new line
        mNewLine = baseA - top;

        //
        for(int i = 0; i < 256; ++i)
        {
            mChars[i].y += top;
            mChars[i].h -= top;
        }

        bitmap->unlockTexture();
        mBitmap = bitmap;

    }


    return success;

}

void LBitmapFont::renderText(int x, int y, std::string text)
{
    //If the font has been built
    if(mBitmap != NULL)
    {
        //Temp offsets
        int curX = x, curY = y;

        //Go through the text
        for(int i = 0; i < text.length(); ++i)
        {
            //If the current character is a space
            if(text[i] == ' ')
            {
                //Mover over
                curX += mSpace;
            }
            //If the current character is a newLine
            else if(text[i] == '\n')
            {
                //Move down
                curY += mNewLine;

                //Move back
                curX = x;
            }
            else
            {
                //Get the ASCII value of the character
                int ascii = (unsigned char)text[i];

                //Show the character
                mBitmap->render(curX, curY, &mChars[ascii]);

                //Move over the width of the character with one pixel of padding
                curX += mChars[ascii].w + 1;
            }
        }
    }
}


DataStream::DataStream()
{
    mImages[0] = NULL;
    mImages[1] = NULL;
    mImages[2] = NULL;
    mImages[3] = NULL;

    mCurrentImage = 0;
    mDelayFrames = 4;
}

bool DataStream::loadMedia()
{
    bool success = true;

    for(int i = 0; i < 4; ++i)
    {
        char path[64] = "";
        sprintf(path, "images/foo_walk_%d.png", i);

        SDL_Surface* loadedSurface = IMG_Load(path);
        if(loadedSurface == NULL)
        {
            success = false;
        }
        else
        {
            mImages[i] = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0);
        }

        SDL_FreeSurface(loadedSurface);
    }

    return success;
}

void DataStream::free()
{
    for(int i = 0; i < 4; ++i)
    {
        SDL_FreeSurface(mImages[i]);
    }
}

void* DataStream::getBuffer()
{
    --mDelayFrames;
    if(mDelayFrames == 0)
    {
        ++mCurrentImage;
        mDelayFrames = 4;
    }

    if(mCurrentImage == 4)
    {
        mCurrentImage = 0;
    }

    return mImages[mCurrentImage]->pixels;
}

bool init(){
    //Bandera de inicialización
    bool success = true;

    if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0){
        std::cout << "Error al iniciar SDL library: " << SDL_GetError() << std::endl;
        success = false;
    }else{

        //Seed random
		srand( SDL_GetTicks() );
        //Crear la ventana
        gWindow = SDL_CreateWindow("Tutorial 43", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

    //Initialize semaphore
    gDataLock = SDL_CreateSemaphore(1);

    //Bandera de inicialización
    bool success = true;

    //Load blank texture
    if(!gCallBackTexture.loadFromFile("images/splash.png"))
    {
        std::cout << "Error al crear la background texture" << std::endl;
        success = false;
    }

    return success;
}


void close(){

    gCallBackTexture.free();

    SDL_DestroySemaphore(gDataLock);
    gDataLock = NULL;

    // Destruir ventana
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    //Cerrar SDL subsistemas
    IMG_Quit();
    SDL_Quit();
}

/*
Uint32 callback(Uint32 interval, void* param)
{
    std::cout << interval << "Callback called call back with message: " << (char*)param << std::endl;
    return interval;
}
*/

/*
int threadFunction(void* data)
{
    printf( "Running thread with value = %d\n", (int*)data );
    return 0;
}
*/
//Our worker thread function
int worker(void* data)
{
    std::cout << "starting..." << (char *)data << std::endl;

    //Pre thread random seeding
    srand(SDL_GetTicks());

    for(int i = 0; i < 5; ++i)
    {
        // Wait randomly
        SDL_Delay(16 + rand() % 32);

        //Lock
        SDL_SemWait(gDataLock);

        //Show pre work data
        std::cout << (char *)data << " gets " << gData << std::endl;

        //Work
        gData = rand() % 256;

        //Show post work data
        std::cout << (char *)data << " sets " << gData << std::endl << std::endl;

        //Unlock
        SDL_SemPost(gDataLock);

        // Wait randomly
        SDL_Delay(16 + rand() % 640);
    }

    std::cout << (char *)data << " finished!\n "<< std::endl;

    return 0;
}

void gameRun(){

    if(!init()){
        std::cout << "Ha fallado la inicialización" << std::endl;
    }
    else
    {

        if(!loadMedia())
        {
            std::cout << "Error al cargar las ceramicas" << std::endl;
        }
        else
        {
            //Run the threads
            srand(SDL_GetTicks());
            SDL_Thread* threadA = SDL_CreateThread(worker, "Thread A", (void*)"Thread A");
            SDL_Delay(16 + rand() % 32);
            SDL_Thread* threadB = SDL_CreateThread(worker, "Thread B", (void*)"Thread B");
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

                gCallBackTexture.render(0, 0);

                //Update
                SDL_RenderPresent(gRenderer);

            }

            SDL_WaitThread(threadA, NULL);
            SDL_WaitThread(threadB, NULL);
        }
        // Libera recursos y cerrar SDL
        close();

    }

}


#endif // TUTORIAL_45_H_INCLUDED
