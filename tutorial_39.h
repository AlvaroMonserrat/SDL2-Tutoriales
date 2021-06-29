#ifndef TUTORIAL_39_H_INCLUDED
#define TUTORIAL_39_H_INCLUDED

#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include <sstream>
#include <vector>
#include <fstream>

/*
    Tutorial 39: Bitmap Fonts
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

    //Manipulación de pixeles
    bool lockTexture();
    bool unlockTexture();
    void* getPixels();
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
LTexture gBitmapTexture;
LBitmapFont gBitmapFont;

//Cargar una imagen individual como textura
SDL_Texture* loadTexture(std::string path);

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


bool init(){
    //Bandera de inicialización
    bool success = true;

    if( SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "Error al iniciar SDL library: " << SDL_GetError() << std::endl;
        success = false;
    }else{
        //Crear la ventana
        gWindow = SDL_CreateWindow("Tutorial 39", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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


    if(!gBitmapTexture.loadFromFile("images/lazyfont.png"))
    {
        std::cout << "Error al cargar la imagen lazyfont.png" << std::endl;
        success = false;
    }
    else
    {
        gBitmapFont.buildFont(&gBitmapTexture);
    }

    return success;
}


void close(){

    gBitmapTexture.free();
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
    }
    else
    {

        if(!loadMedia())
        {
            std::cout << "Error al cargar las ceramicas" << std::endl;
        }
        else
        {
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

                //Render
                gBitmapFont.renderText(0, 0, "Alvaro Monserrat");

                //Update
                SDL_RenderPresent(gRenderer);

            }

        }
        // Libera recursos y cerrar SDL
        close();

    }

}


#endif // TUTORIAL_39_H_INCLUDED
