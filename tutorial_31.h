#ifndef TUTORIAL_31_H_INCLUDED
#define TUTORIAL_31_H_INCLUDED


#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include <sstream>
#include <string>
#include <vector>
#include <SDL2/SDL_ttf.h>

/*
    Tutorial 31: Leer y escribir archivos
*/


/*Constantes*/
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

//
const int TOTAL_DATA = 10;

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
SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer =NULL;

//Globally used fond
TTF_Font *gFont = NULL;

//Cargar una imagen individual como textura
SDL_Texture* loadTexture(std::string path);

//Scene texture
LTexture gPromptTextTexture;
LTexture gDataTextures[TOTAL_DATA];


//Data points
Sint32 gData[TOTAL_DATA];

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


bool init(){
    //Bandera de inicialización
    bool success = true;

    if( SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "Error al iniciar SDL library: " << SDL_GetError() << std::endl;
        success = false;
    }else{
        //Crear la ventana
        gWindow = SDL_CreateWindow("Tutorial 31", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

    SDL_Color textColor = {0, 0, 0, 0xFF};
    SDL_Color highlightColor = { 0xFF, 0, 0, 0xFF };

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
        if(!gPromptTextTexture.loadFromRenderedText("Enter Text:", textColor))
        {
            std::cout << "Error al renderizar prompt" << std::endl;
            success = false;
        }
    }

    //Abrir Archivo para leer en binario.
    SDL_RWops* file = SDL_RWFromFile("data/nums.bin", "r+b");

    //Archivo no existe
    if(file == NULL)
    {
        //Crear archivo
        file = SDL_RWFromFile("data/nums.bin", "w+b");

        if(file != NULL)
        {
            std::cout << "Nuevo Archivo se ha creado" << std::endl;

            //Iniciar Data
            for(int i = 0; i < TOTAL_DATA; ++i)
            {
                gData[i] = 0;
                SDL_RWwrite(file, &gData[i], sizeof(Sint32), 1);
            }

            SDL_RWclose(file);
        }else{
            std::cout << "Error al crear el archivo nums.bin" << std::endl;
            success = false;
        }
    }
    //archivo existe
    else
    {
        //Cargar datos
        std::cout << "Leyendo archivo..." << std::endl;

        for(int i = 0; i < TOTAL_DATA; ++i)
        {
            SDL_RWread(file, &gData[i], sizeof(Sint32), 1);
        }

        SDL_RWclose(file);

    }

    //Inicializar data textures
    gDataTextures[0].loadFromRenderedText(std::to_string(gData[0]), highlightColor);
    for(int i = 1; i < TOTAL_DATA; ++i)
    {
        gDataTextures[i].loadFromRenderedText(std::to_string(gData[i]), textColor);
    }

    return success;
}

void close(){

    //Abrir archivo para guardar data
    SDL_RWops *file = SDL_RWFromFile("data/nums.bin", "w+b");
    if(file != NULL)
    {
        for(int i = 0; i < TOTAL_DATA; ++i)
        {
            SDL_RWwrite(file, &gData[i], sizeof(Sint32), 1);
        }

        //Close file handler
        SDL_RWclose(file);
    }

    gPromptTextTexture.free();
    for(int i = 0; i < TOTAL_DATA; ++i)
    {
        gDataTextures[i].free();
    }

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
        std::cout << "Ha fallado la inicialización" << std::endl;
    }else{
        if(!loadMedia()){
         }else{

            //Set color del texto negro
            SDL_Color textColor = {0, 0, 0, 0xFF};
            SDL_Color highlightColor = { 0xFF, 0, 0, 0xFF };

            //Current input point
            int currentData = 0;

            while(!quit)
            {
                while(SDL_PollEvent(&eventHandler) != 0)
                {
                    if(eventHandler.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                    else if(eventHandler.type == SDL_KEYDOWN)
                    {
                        switch(eventHandler.key.keysym.sym)
                        {

                            case SDLK_UP:
                                //Render previous entry input point
                                gDataTextures[currentData].loadFromRenderedText(std::to_string(gData[currentData]), textColor);
                                --currentData;
                                if(currentData < 0)
                                {
                                    currentData = TOTAL_DATA - 1;
                                }

                                //Renderer current entry input point
                                gDataTextures[currentData].loadFromRenderedText(std::to_string(gData[currentData]), highlightColor);
                                break;

                            case SDLK_DOWN:
                                //Render previous entry input point
                                gDataTextures[currentData].loadFromRenderedText(std::to_string(gData[currentData]), textColor);
                                ++currentData;
                                if(currentData == TOTAL_DATA)
                                {
                                    currentData = 0;
                                }

                                //Renderer current entry input point
                                gDataTextures[currentData].loadFromRenderedText(std::to_string(gData[currentData]), highlightColor);
                                break;

                            //Decrement input point
                            case SDLK_LEFT:
                                --gData[currentData];
                                gDataTextures[currentData].loadFromRenderedText(std::to_string(gData[currentData]), highlightColor);
                                break;

                            //Increment input point
                            case SDLK_RIGHT:
                                ++gData[currentData];
                                gDataTextures[currentData].loadFromRenderedText(std::to_string(gData[currentData]), highlightColor);
                                break;

                        }
                    }

                }

                //Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

                //Render text texture
                gPromptTextTexture.render((SCREEN_WIDTH - gPromptTextTexture.getWidth()) / 2, 0);
                for(int i = 0; i < TOTAL_DATA; ++i)
                {
                    gDataTextures[i].render( (SCREEN_WIDTH - gDataTextures[i].getWidth())/ 2, gPromptTextTexture.getHeight() + gDataTextures[0].getHeight() * i);
                }
                //Update
                SDL_RenderPresent(gRenderer);

            }

        }
    }

    // Libera recursos y cerrar SDL
    close();
}


#endif // TUTORIAL_31_H_INCLUDED
