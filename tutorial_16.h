#ifndef TUTORIAL_16_H_INCLUDED
#define TUTORIAL_16_H_INCLUDED


#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
/*
    Tutorial 16: Evento de Mouse
*/

using namespace std;

/*Constantes*/
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const int BUTTON_WIDTH = 300;
const int BUTTON_HEIGHT = 200;
const int TOTAL_BUTTONS = 4;

//Key press surfaces constants
enum LButtonSprite
{
    BUTTON_SPRITE_MOUSE_OUT = 0,
    BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
    BUTTON_SPRITE_MOUSE_DOWN = 2,
    BUTTON_SPRITE_MOUSE_UP = 3,
    BUTTON_SPRITE_TOTAL = 4
};

//Clase
class LTexture{
public:
    //Iniciar variables
    LTexture();

    //Desasignar memoria
    ~LTexture();

    //Cargar imagen desde una ruta especifica
    bool loadFromFile(string path);

    #if defined(SDL_TTF_MAJOR_VERSION)
    //Crear imagen desde un font string
    bool loadFromRenderedText(string textureText, SDL_Color textColor);
    #endif // defined

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

//The mouse button
class LButton{

public:
    //Inicializar variables internas
    LButton();

    //Sets top left position
    void setPosition(int x, int y);

    //Manipular evento del mouse
    void handleEvent(SDL_Event* event);

    //Show button sprite
    void render();

private:
    //Top left position
    SDL_Point mPosition;

    //Actual sprite global usado
    LButtonSprite mCurrentSprite;
};


/*Variables Globales*/
SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer =NULL;

//Cargar una imagen individual como textura
SDL_Texture* loadTexture(string path);

//Scene texture
SDL_Rect gSpriteClips[BUTTON_SPRITE_TOTAL];
LTexture gButtonSpriteSheetTexture;

//Buttons objects
LButton gButtons[TOTAL_BUTTONS];

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

//Scene sprites
//SDL_Rect gSprinteClips[4];
//LTexture gSprinteSheetTexture;
LTexture gModulatedTexture;


//Scenes texture
LTexture gFooTexture;
LTexture gBackgroundTexture;

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

#if defined(SDL_TTF_MAJOR_VERSION)
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
#endif // defined

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

LButton::LButton(){

    mPosition.x = 0;
    mPosition.y = 0;

    mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
}

void LButton::setPosition(int x, int y){
    mPosition.x = x;
    mPosition.y = y;
}

void LButton::handleEvent(SDL_Event* event){
    if(event->type == SDL_MOUSEMOTION || event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP)
    {
        //Get mouse position
        int x, y;
        SDL_GetMouseState(&x, &y);

        //Check if mouse is in button
        bool inside = true;

        //Mouse is left of the button
        if(x < mPosition.x){
            inside = false;
        }
        //Mouse is right of the button
        else if(x > mPosition.x + BUTTON_WIDTH){
            inside = false;
        }
        //Mouse is above the button
        else if( y < mPosition.y){
            inside = false;
        }
        //Mouse below the button
        else if( y > mPosition.y + BUTTON_HEIGHT){
            inside = false;
        }

        //Mouse is outside button
        if(!inside){
            mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
        }
        //Mouse is inside button
        else{
            //Set mouse over sprite
            switch(event->type){
            case SDL_MOUSEMOTION:
                mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
                break;
            case SDL_MOUSEBUTTONDOWN:
                mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
                break;
            case SDL_MOUSEBUTTONUP:
                mCurrentSprite = BUTTON_SPRITE_MOUSE_UP;
                break;
            }
        }

    }
}

void LButton::render(){
    //Show current button sprite
    gButtonSpriteSheetTexture.render(mPosition.x, mPosition.y, &gSpriteClips[mCurrentSprite]);
}


bool init(){
    //Bandera de inicialización
    bool success = true;

    if( SDL_Init(SDL_INIT_VIDEO) < 0){
        cout << "Error al iniciar SDL library: " << SDL_GetError() << endl;
        success = false;
    }else{
        //Crear la ventana
        gWindow = SDL_CreateWindow("Tutorial 16", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
                /*
                //Iniciar SDL ttf
                if(TTF_Init() == -1){
                    cout << "SDL_ttf no se pudo iniciar " << TTF_GetError() << endl;
                    success = false;
                }
                */

            }



        }
    }

    return success;
}

bool loadMedia(){
    //Bandera de inicialización
    bool success = true;

    //Load sprites
    if(!gButtonSpriteSheetTexture.loadFromFile("images/button.png")){
        cout << "Error al cargar la imagen button" << endl;
        success = false;
    }
    else
    {
        //Set sprites
        for(int i = 0; i < BUTTON_SPRITE_TOTAL; ++i){
            gSpriteClips[i].x = 0;
            gSpriteClips[i].y = i * 200;
            gSpriteClips[i].w = BUTTON_WIDTH;
            gSpriteClips[i].h = BUTTON_HEIGHT;
        }

        //Set buttons in corners
        gButtons[0].setPosition(0, 0);
        gButtons[1].setPosition(SCREEN_WIDTH - BUTTON_WIDTH, 0);
        gButtons[2].setPosition(0, SCREEN_HEIGHT - BUTTON_HEIGHT);
        gButtons[3].setPosition(SCREEN_WIDTH - BUTTON_WIDTH, SCREEN_HEIGHT - BUTTON_HEIGHT);
    }


    return success;
}

void close(){

    gButtonSpriteSheetTexture.free();

    // Destruir ventana
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    //Cerrar SDL subsistemas
    //TTF_Quit();
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

            while(!quit)
            {
                while(SDL_PollEvent(&eventHandler) != 0)
                {
                    if(eventHandler.type == SDL_QUIT)
                    {
                        quit = true;
                    }

                    //Handle button events
                    for(int i = 0; i < TOTAL_BUTTONS; ++i){
                        gButtons[i].handleEvent(&eventHandler);
                    }
                }


				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Render buttons
				for(int i = 0; i < TOTAL_BUTTONS; ++i){
                    gButtons[i].render();
				}

                //Update
                SDL_RenderPresent(gRenderer);

            }

        }
    }

    // Libera recursos y cerrar SDL
    close();
}



#endif // TUTORIAL_16_H_INCLUDED
