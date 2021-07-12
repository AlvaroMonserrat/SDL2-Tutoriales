#ifndef TUTORIAL_48_H_INCLUDED
#define TUTORIAL_48_H_INCLUDED

#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <sstream>
#include <vector>
#include <fstream>

/*
    Tutorial 48: SDL2 and OpenGL
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

//OpenGL context
SDL_GLContext gContext;

//Render flag
bool gRenderQuad = true;

//Scene textures
LTexture gCallBackTexture;

//Cargar una imagen individual como textura
SDL_Texture* loadTexture(std::string path);

//Event Handler
SDL_Event eventHandler;

//Flags
bool quit = false;

//Iniciar SDL y crear ventana
bool init();

//Iniciar matrices y clear color
bool initGL();

//Input hanlder
void handlerKeys(unsigned char key, int x, int y);

//Per Frame update
void update();

//Renders quad to the screen
void render();

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

    if( SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "Error al iniciar SDL library: " << SDL_GetError() << std::endl;
        success = false;
    }else{

        //Use OpenGL 2.1
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

        //Crear la ventana
        gWindow = SDL_CreateWindow("Tutorial 48", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

        if( gWindow == NULL){
           std::cout << "La ventana no pudo ser creada: " << SDL_GetError() << std::endl;
           success = false;
        }else{

            //Crear contexto
            gContext = SDL_GL_CreateContext(gWindow);

            if(gContext == NULL)
            {
                std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
                success = false;
            }
            else{
                //Use Vsync
                if(SDL_GL_SetSwapInterval(1) < 0)
                {
                    std::cout << "Warning: Unable to set VSync! SDL Error: "  << SDL_GetError() << std::endl;
                }

                //Initialize OpenGL
                if(!initGL())
                {
                     std::cout << "Unable to initialize OpenGL!" << std::endl;
                    success = false;
                }

            }

        }
    }

    return success;
}

bool initGL()
{
    bool success = true;
    GLenum error = GL_NO_ERROR;

    //Initialize Projection Matrix
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    error = glGetError();

    if(error != GL_NO_ERROR)
    {
        std::cout << "Error al iniciar OpenGL Error: " << gluErrorString(error) << std::endl;
        success = false;
    }

    //Initialize Modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

     if(error != GL_NO_ERROR)
    {
        std::cout << "Error al iniciar OpenGL Error: " << gluErrorString(error) << std::endl;
        success = false;
    }

    glClearColor(0.f, 0.f, 0.f, 1.f);

    if(error != GL_NO_ERROR)
    {
        std::cout << "Error al iniciar OpenGL Error: " << gluErrorString(error) << std::endl;
        success = false;
    }

    return success;
}

void handlerKeys(unsigned char key, int x, int y)
{
    //Toggle quad
    if(key == 'q')
    {
        gRenderQuad = !gRenderQuad;
    }
}

void update()
{
    //No per frame update needed
}

void render()
{
    //Clear color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    //Render quad
    if(gRenderQuad)
    {
        glBegin(GL_QUADS);
            glVertex2f(-0.5f, -0.5f);
            glVertex2f(0.5f, -0.5f);
            glVertex2f(0.5f, 0.5f);
            glVertex2f(-0.5f, 0.5f);
        glEnd();
    }

}

bool loadMedia(){

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
    // Destruir ventana
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    //Cerrar SDL subsistemas
    SDL_Quit();
}


void gameRun(){

    if(!init()){
        std::cout << "Ha fallado la inicialización" << std::endl;
    }
    else
    {

        //Enable text input
        SDL_StartTextInput();

        while(!quit)
        {
            while(SDL_PollEvent(&eventHandler) != 0)
            {
                if(eventHandler.type == SDL_QUIT)
                {
                    quit = true;
                }
                else if(eventHandler.type == SDL_TEXTINPUT)
                {
                    int x = 0, y = 0;
                    SDL_GetMouseState(&x, &y);
                    handlerKeys(eventHandler.text.text[0], x, y);
                }
            }

            //Render quad
            render();

            //Update screen
            SDL_GL_SwapWindow(gWindow);
        }

        //disable text input
        SDL_StopTextInput();

    }
    // Libera recursos y cerrar SDL
    close();


}

#endif // TUTORIAL_48_H_INCLUDED
